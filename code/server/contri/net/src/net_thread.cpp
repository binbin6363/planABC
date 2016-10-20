#include "net_thread.h"
#include "net_manager.h"
#include "reactor.h"
#include "net_event.h"
#include "net_packet.h"
#include "sock_stream.h"
#include "log.h"



// class Net_Thread
StatusManager::StatusManager(Net_Manager *net_manager)
	:m_net_manager(net_manager)
{
	m_is_run = false;
	m_notify_stop = false;
	starttime = time(NULL);
}

StatusManager::~StatusManager()
{

}

int StatusManager::start()
{
	if (false != m_is_run) {
		return -1;
	}

	m_notify_stop = false;
	int rc = activate();
	if (rc != 0) {
		return -1;
	}
	m_is_run = true;
	return 0;
}

int StatusManager::stop()
{
	m_notify_stop = true;
	wait();

	m_is_run = false;
	return 0;
}


int StatusManager::svc()
{
	while (m_notify_stop != true)
	{
		time_t currentTimeval = time(NULL);
		if (currentTimeval - starttime < 5)
		{
			sleep(1);
			continue;
		}
		starttime = currentTimeval;
		StatusInfo statusInfo;
		statusInfo.m_pendingNetEvent = Net_Event::objectPoolSize();
		statusInfo.m_netPacketNums = Net_Packet::objectPoolSize();
		statusInfo.m_netBuffer1Nums = Net_Buff1::objectPoolSize();
		statusInfo.m_netBuffer2Nums = Net_Buff2::objectPoolSize();
		statusInfo.m_tcpClientNums = SOCK_Stream::objectPoolSize();
		LOG(INFO)("dump netmanager status info,pending netevent=%d,tcpclient nums=%d,netpacket nums=%d,netbuffer1 nums=%d,netbuffer2 nums=%d,event queue=%d,send task queue=%d",statusInfo.m_pendingNetEvent,statusInfo.m_tcpClientNums,statusInfo.m_netPacketNums,statusInfo.m_netBuffer1Nums,statusInfo.m_netBuffer2Nums,m_net_manager->get_recv_size(),m_net_manager->get_send_size());
	}
	return 0;
}











// class Net_Thread
Net_Thread::Net_Thread(Net_Manager *net_manager, Reactor *reactor)
{
	m_is_run = false;
	m_notify_stop = false;
	m_net_manager = net_manager;
	m_reactor = reactor;
}

Net_Thread::~Net_Thread()
{
}

int Net_Thread::start()
{
	if (false != m_is_run) {
		return -1;
	}

	m_notify_stop = false;
	int rc = activate();
	if (rc != 0) {
		return -1;
	}

	m_is_run = true;
	return 0;
}

int Net_Thread::stop()
{
	m_notify_stop = true;
	wait();

	m_is_run = false;
	return 0;
}

int Net_Thread::svc()
{
	while (m_notify_stop != true)
	{
		// dispatch task
		int task_count = 0;
		while (true)
		{
			//LOG(FATAL)("Net_Thread::svc check send task quene");
			Send_Task* send_task = m_net_manager->get_send_task();
			if (NULL == send_task) {
				break;
			}

			Event_Handler* event_handler = m_reactor->get_event_handler(send_task->id);
			if (event_handler == NULL) {
				delete send_task;
				continue;
			}

			int rc = event_handler->send_data(send_task);
			if(rc != 0)
			{
				delete event_handler;
				event_handler = NULL;
			}

			task_count++;
		}

		// network event loop
		int event_count = m_reactor->run_reactor_event_loop();
		if (event_count < 0)
		{
			// epoll错误, 线程退出
			m_is_run = false;
			m_net_manager->on_net_error();
			return -1;
		}
//		睡眠通过epoll_wait 的timeout来实现
//			if ((0 == task_count) && (0 == event_count))
//			{
//				// no task, sleep a while
//				int s = 1;
//				usleep(s);
//			}
	}

	return 0;
}

