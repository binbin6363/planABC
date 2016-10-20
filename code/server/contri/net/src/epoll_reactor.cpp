

#include "epoll_reactor.h"
#include "event_handler.h"
#include "log.h"
#include <assert.h>
#include <vector>


// class Epoll_Reactor
Epoll_Reactor::Epoll_Reactor()
{
	m_epfd = -1;
	m_events = new epoll_event[MAX_HANDLER * 2];
}

Epoll_Reactor::~Epoll_Reactor()
{
	delete [] m_events;
}

int Epoll_Reactor::open_reactor()
{
	m_epfd = epoll_create(MAX_HANDLER);
	if (-1 == m_epfd) {
		LOG(FATAL)("Epoll_Reactor::open_reactor error, epoll_create error, errno:%d", error_no());
		return -1;
	}

	return 0;
}

int Epoll_Reactor::close_reactor()
{
	if (-1 != m_epfd) {
		close(m_epfd);
		m_epfd = -1;
	}

	return 0;
}

int Epoll_Reactor::run_reactor_event_loop()
{
	if (-1 == m_epfd) {
		return -1;
	}

	// epoll事件
	int count = epoll_wait(m_epfd, m_events, MAX_HANDLER, 1);
	if (count < 0) {
		LOG(FATAL)("Epoll_Reactor::run_reactor_event_loop error, epoll_wait error, errno:%d", error_no());

		if(error_no() == EINTR)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}

	for (int i = 0; i < count; i++)
	{
		Event_Handler *event_handler = (Event_Handler*)m_events[i].data.ptr;
		uint32_t id = event_handler->get_id();

		// 异常
		if (m_events[i].events & EPOLLERR)
		{
			utils::Thread_Mutex_Guard guard(m_map_mutex);

			m_event_handler_info_map.erase(id);
			LOG(ERROR)("Epoll_Reactor::run_reactor_event_loop, got EPOLLERR for id:%d", id);
			event_handler->handle_exception();
			delete event_handler;
			continue;
		}

		// 关闭
		if (m_events[i].events & EPOLLHUP)
		{
			utils::Thread_Mutex_Guard guard(m_map_mutex);

			m_event_handler_info_map.erase(id);
			LOG(INFO)("Epoll_Reactor::run_reactor_event_loop, got EPOLLHUP for id:%d", id);
			event_handler->handle_close();
			delete event_handler;
			continue;
		}

		// 可读
		if (m_events[i].events & EPOLLIN)
		{
			int rc = event_handler->handle_input();
			//LOG(ERROR)("Epoll_Reactor::handle_input return:%d for id:%d", rc, id);
			if (-1 == rc)
			{
				// 连接关闭
				utils::Thread_Mutex_Guard guard(m_map_mutex);

				m_event_handler_info_map.erase(id);
				event_handler->handle_close();
				delete event_handler;
				continue;
			}
			if (-2 == rc)
			{
				// 连接异常
				utils::Thread_Mutex_Guard guard(m_map_mutex);

				m_event_handler_info_map.erase(id);
				event_handler->handle_exception();
				delete event_handler;
				continue;
			}
		}

		// 可写
		if (m_events[i].events & EPOLLOUT)
		{
			int rc = event_handler->handle_output();
			//LOG(ERROR)("Epoll_Reactor::handle_output return:%d for id:%d", rc, id);
			if (-1 == rc)
			{
				// 连接关闭
				utils::Thread_Mutex_Guard guard(m_map_mutex);

				m_event_handler_info_map.erase(id);
				event_handler->handle_close();
				delete event_handler;
				continue;
			}
			else if (-2 == rc)
			{
				// 连接异常
				utils::Thread_Mutex_Guard guard(m_map_mutex);

				m_event_handler_info_map.erase(id);
				event_handler->handle_exception();
				delete event_handler;
				continue;
			}
			else 
			{
				//LOG(WARN)("Epoll_reactor:: handleoutput return:%d", rc);				
			}
		}
	}

	// 处理用户通知关闭的处理器和超时的处理器
	vector<Event_Handler*> close_handler;
	vector<Event_Handler*> timeout_handler;
	//定时处理，避免每次都跑，影响系统性能
	static time_t last_scan_time = 0;
	time_t current_time = time(NULL);
	if(current_time - last_scan_time > 0)
	{ // lock section
		last_scan_time = current_time;
		utils::Thread_Mutex_Guard guard(m_map_mutex);
		for (Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.begin(); iter != m_event_handler_info_map.end();)
		{
			Event_Handler *event_handler = iter->second.event_handler;

			// 关闭通知
			if (true == iter->second.notify_close)
			{
				//LOG(WARN)("Epoll_reactor:: checker id:%d, write:%d, writecheck:%d", 
				//				iter->first, iter->second.write, iter->second.have_check_send_task);				
				if (iter->second.write == false) 
				{
					// 此通道上没有待发送数据
					if(iter->second.have_check_send_task)
					{
						// 如果已经检查了net_manager的发送队列，则可以删除
						m_event_handler_info_map.erase(iter++);
						close_handler.push_back(event_handler);
					}
					else
					{
						// 如果没有检查过net_manager的发送队列，则在此处退出，
						// 上层net_thread.svc 中会进行检查，故此处提前将标志置上
						iter->second.have_check_send_task = true;
						iter++;
					}
					continue;					
				}
				else {
					// 此通道上还有待发送数据, 如果没有设置超时则设置超时
					if (iter->second.timeout == 0) {
						iter->second.timeout = current_time + 30;
					}
				}
			}

			// 超时
			if ((iter->second.timeout != 0) && (current_time >= iter->second.timeout))
			{
				m_event_handler_info_map.erase(iter++);
				timeout_handler.push_back(event_handler);
				continue;
			}

			++iter;
		}
	} // end lock section

	for (size_t i = 0; i < close_handler.size(); i++)
	{
		Event_Handler *event_handler = close_handler[i];
		//LOG(ERROR)("Epoll_Reactor:: call handle_close for id:%d", event_handler->get_id());
		event_handler->handle_close();
		delete event_handler;
	}

	for (size_t i = 0; i < timeout_handler.size(); i++)
	{
		Event_Handler *event_handler = timeout_handler[i];
		//LOG(ERROR)("Epoll_Reactor:: call handle_timeout for id:%d", event_handler->get_id());
		event_handler->handle_timeout();
		delete event_handler;
	}

	return count;
}

int Epoll_Reactor::end_reactor_event_loop()
{
	utils::Thread_Mutex_Guard guard(m_map_mutex);

	for (Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.begin(); iter != m_event_handler_info_map.end();)
	{
		Event_Handler *event_handler = (Event_Handler*)(iter->second.event_handler);
		m_event_handler_info_map.erase(iter++);
		event_handler->handle_close();
		delete event_handler;
	}

	return 0;
}

int Epoll_Reactor::register_handler(Event_Handler *event_handler, uint32_t masks)
{
	if (-1 == m_epfd) {
		return -1;
	}

	event_handler->reactor(this);
	uint32_t id = event_handler->get_id();

	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end())
	{
		// add
		Event_Handler_Info ehi;
		ehi.event_handler = event_handler;
		ehi.notify_close = false;
		ehi.have_check_send_task = false;
		ehi.timeout = 0;

		epoll_event event;
		event.events = 0;
		event.data.ptr = (void*)event_handler;

		if (masks & MASK_READ) {
			ehi.read = true;
			event.events |= EPOLLIN|EPOLLET;
		}
		else {
			ehi.read = false;
		}

		if (masks & MASK_WRITE) {
			ehi.write = true;
			event.events |= EPOLLOUT|EPOLLET;
		}
		else {
			ehi.write = false;
		}

		int rc = epoll_ctl(m_epfd, EPOLL_CTL_ADD, event_handler->get_handle(), &event);
		if (0 != rc) {
			LOG(FATAL)("Epoll_Reactor::register_handler error, epoll_ctl error, errno:%d", error_no());
			return -1;
		}

		m_event_handler_info_map[id] = ehi;
		return 0;
	}
	else
	{
		// mod
		Event_Handler_Info ehi = iter->second;
		ehi.event_handler = event_handler;

		epoll_event event;
		event.events = 0;
		event.data.ptr = (void*)event_handler;

		if (masks & MASK_READ) ehi.read = true;
		if (masks & MASK_WRITE) ehi.write = true;

		if (ehi.read) event.events |= EPOLLIN;
		if (ehi.write) event.events |= EPOLLOUT;

		int rc = epoll_ctl(m_epfd, EPOLL_CTL_MOD, event_handler->get_handle(), &event);
		if (0 != rc) {
			LOG(FATAL)("Epoll_Reactor::register_handler error, epoll_ctl error, errno:%d", error_no());
			return -1;
		}

		m_event_handler_info_map[id] = ehi;
		return 0;
	}
}

int Epoll_Reactor::remove_handler(Event_Handler *event_handler, uint32_t masks)
{
	if (-1 == m_epfd) {
		return -1;
	}

	event_handler->reactor(this);
	uint32_t id = event_handler->get_id();

	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return 0;
	}

	// mod
	Event_Handler_Info ehi = iter->second;
	ehi.event_handler = event_handler;

	epoll_event event;
	event.events = 0;
	event.data.ptr = (void*)event_handler;

	if (masks & MASK_READ) ehi.read = false;
	if (masks & MASK_WRITE) ehi.write = false;

	if (ehi.read) event.events |= EPOLLIN;
	if (ehi.write) event.events |= EPOLLOUT;

	int rc = epoll_ctl(m_epfd, EPOLL_CTL_MOD, event_handler->get_handle(), &event);
	if (0 != rc) {
		LOG(FATAL)("Epoll_Reactor::remove_handler error, epoll_ctl error, errno:%d", error_no());
		return -1;
	}

	m_event_handler_info_map[id] = ehi;
	return 0;
}


int Epoll_Reactor::delete_handler(Event_Handler *event_handler)
{
	if (-1 == m_epfd) {
		return -1;
	}

	event_handler->reactor(this);
	uint32_t id = event_handler->get_id();

	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return 0;
	}
	Event_Handler_Info ehi = iter->second;
	ehi.event_handler = event_handler;

	epoll_event event;
	event.events = 0;
	event.data.ptr = (void*)event_handler;

	if (ehi.read) event.events |= EPOLLIN;
	if (ehi.write) event.events |= EPOLLOUT;

	m_event_handler_info_map.erase(id);
	int rc = epoll_ctl(m_epfd, EPOLL_CTL_DEL, event_handler->get_handle(), &event);
	if (0 != rc) {
		LOG(FATAL)("Epoll_Reactor::remove_handler error, epoll_ctl error, errno:%d", error_no());
		return -1;
	}
	return 0;
}

int Epoll_Reactor::set_timeout(uint32_t id, int timeout)
{
	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return -1;
	}

	iter->second.timeout = time(NULL) + timeout;
	return 0;
}

int Epoll_Reactor::cancel_timeout(uint32_t id)
{
	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return 0;
	}

	iter->second.timeout = 0;
	return 0;
}

int Epoll_Reactor::notify_close(uint32_t id)
{
	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return -1;
	}

	iter->second.notify_close = true;

	return 0;
}

Event_Handler* Epoll_Reactor::get_event_handler(uint32_t id)
{
	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		LOG(FATAL)("Epoll_Reactor::get_event_handler can't handler for :%d", id);
		return NULL;
	}
	
	//LOG(FATAL)("Epoll_Reactor::get_event_handler found handler for :%d", id);

	return iter->second.event_handler;
}

