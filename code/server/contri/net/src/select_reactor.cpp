

#include "select_reactor.h"
#include "event_handler.h"
#include "log.h"
#include <assert.h>
#include <vector>
#include <time.h>


// class Select_Reactor
Select_Reactor::Select_Reactor()
{
}

Select_Reactor::~Select_Reactor()
{
}

int Select_Reactor::open_reactor()
{
	return 0;
}

int Select_Reactor::close_reactor()
{
	return 0;
}

int Select_Reactor::run_reactor_event_loop()
{
	//
	fd_set read_set;
	fd_set write_set;
	fd_set exception_set;

	FD_ZERO(&read_set);
	FD_ZERO(&write_set);
	FD_ZERO(&exception_set);

	SOCKET max_fd = 0;

	{
		utils::Thread_Mutex_Guard guard(m_map_mutex);
		if (0 == m_event_handler_info_map.size())
		{
			usleep(1000);
			return 0;
		}

		for (Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.begin(); iter != m_event_handler_info_map.end(); iter++)
		{
			Event_Handler_Info &si = iter->second;
			SOCKET fd = si.event_handler->get_handle();
			if (si.read) FD_SET(fd, &read_set);
			if (si.write) FD_SET(fd, &write_set);
			if (true) FD_SET(fd, &exception_set);
			if (fd > max_fd) max_fd = fd;
		}
	}

	max_fd++;

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 1000;

	// select事件
	int count = select((int)max_fd, &read_set, &write_set, &exception_set, &timeout);
	if (count < 0)
	{
		return -1;
	}

	for (Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.begin(); iter != m_event_handler_info_map.end();)
	{
		Event_Handler_Info &si = iter->second;
		Event_Handler *event_handler = si.event_handler;
		SOCKET fd = event_handler->get_handle();

		if (true)
		{
			if (FD_ISSET(fd, &exception_set))
			{
				utils::Thread_Mutex_Guard guard(m_map_mutex);

				m_event_handler_info_map.erase(iter++);
				event_handler->handle_exception();
				delete event_handler;
				continue;
			}
		}

		if (si.read)
		{
			if (FD_ISSET(fd, &read_set))
			{
				int rc = event_handler->handle_input();
				if (-1 == rc)
				{
					utils::Thread_Mutex_Guard guard(m_map_mutex);

					m_event_handler_info_map.erase(iter++);
					event_handler->handle_close();
					delete event_handler;
					continue;
				}
				if (-2 == rc)
				{
					utils::Thread_Mutex_Guard guard(m_map_mutex);

					m_event_handler_info_map.erase(iter++);
					event_handler->handle_exception();
					delete event_handler;
					continue;
				}
			}
		}

		if (si.write)
		{
			if (FD_ISSET(fd, &write_set))
			{
				int rc = event_handler->handle_output();
				if (-1 == rc)
				{
					// 连接关闭
					utils::Thread_Mutex_Guard guard(m_map_mutex);

					m_event_handler_info_map.erase(iter++);
					event_handler->handle_close();
					delete event_handler;
					continue;
				}
				if (-2 == rc)
				{
					// 连接异常
					utils::Thread_Mutex_Guard guard(m_map_mutex);

					m_event_handler_info_map.erase(iter++);
					event_handler->handle_exception();
					delete event_handler;
					continue;
				}
			}
		}

		++iter;
	}

	// 处理用户通知关闭的处理器和超时的处理器
	vector<Event_Handler*> close_handler;
	vector<Event_Handler*> timeout_handler;

	time_t current_time = time(NULL);

	{ // lock section

		utils::Thread_Mutex_Guard guard(m_map_mutex);
		for (Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.begin(); iter != m_event_handler_info_map.end();)
		{
			Event_Handler *event_handler = iter->second.event_handler;

			// 关闭通知
			if (true == iter->second.notify_close)
			{
				if (iter->second.write == false) {
					// 此通道上没有待发送数据
					m_event_handler_info_map.erase(iter++);
					close_handler.push_back(event_handler);
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
		event_handler->handle_close();
		delete event_handler;
	}

	for (size_t i = 0; i < timeout_handler.size(); i++)
	{
		Event_Handler *event_handler = timeout_handler[i];
		event_handler->handle_timeout();
		delete event_handler;
	}

	return count;
}

int Select_Reactor::end_reactor_event_loop()
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

int Select_Reactor::register_handler(Event_Handler *event_handler, uint32_t masks)
{
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
		ehi.timeout = 0;

		if (masks & MASK_READ) {
			ehi.read = true;
		}
		else {
			ehi.read = false;
		}

		if (masks & MASK_WRITE) {
			ehi.write = true;
		}
		else {
			ehi.write = false;
		}

		m_event_handler_info_map[id] = ehi;
		return 0;
	}
	else
	{
		// mod
		Event_Handler_Info ehi = iter->second;
		ehi.event_handler = event_handler;

		if (masks & MASK_READ) ehi.read = true;
		if (masks & MASK_WRITE) ehi.write = true;

		m_event_handler_info_map[id] = ehi;
		return 0;
	}
}

int Select_Reactor::remove_handler(Event_Handler *event_handler, uint32_t masks)
{
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

	if (masks & MASK_READ) ehi.read = false;
	if (masks & MASK_WRITE) ehi.write = false;

	m_event_handler_info_map[id] = ehi;
	return 0;
}

int Select_Reactor::delete_handler(Event_Handler *event_handler)
{
	event_handler->reactor(this);
	uint32_t id = event_handler->get_id();

	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return 0;
	}
	m_event_handler_info_map.erase(id);
	return 0;
}

int Select_Reactor::set_timeout(uint32_t id, int timeout)
{
	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return -1;
	}

	iter->second.timeout = time(NULL) + timeout;
	return 0;
}

int Select_Reactor::cancel_timeout(uint32_t id)
{
	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return 0;
	}

	iter->second.timeout = 0;
	return 0;
}

int Select_Reactor::notify_close(uint32_t id)
{
	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return -1;
	}

	iter->second.notify_close = true;

	return 0;
}

Event_Handler* Select_Reactor::get_event_handler(uint32_t id)
{
	utils::Thread_Mutex_Guard guard(m_map_mutex);

	Event_Handler_Info_Map::iterator iter = m_event_handler_info_map.find(id);
	if (iter == m_event_handler_info_map.end()) {
		return NULL;
	}

	return iter->second.event_handler;
}

