

#include "sock_dgram.h"
#include "net_manager.h"
#include "log.h"
#include <assert.h>


// class SOCK_Dgram
SOCK_Dgram::SOCK_Dgram(Net_Manager *net_manager, pfnNetEventHandler in_handler)
{
	m_id = net_manager->m_id_manager.acquire(ID_UDP);
	assert(m_id != 0);

	m_socket = INVALID_SOCKET;
	m_net_manager = net_manager;
	m_current_send_task = NULL;
	handler = in_handler;
}

SOCK_Dgram::~SOCK_Dgram()
{
	if (0 != m_id) {
		m_net_manager->m_id_manager.release(m_id);
	}
}

int SOCK_Dgram::create_udp(const INET_Addr& local_addr)
{
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket) {
		LOG(ERROR)("SOCK_Dgram::create_udp error, socket error, errno:%d", error_no());
		return -1;
	}

	set_socket_nonblocking(m_socket);

	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = local_addr.get_addr();
	local.sin_port = local_addr.get_port();

	int rc = bind(m_socket, (sockaddr*)&local, sizeof(local));
	if (0 != rc) {
		LOG(ERROR)("SOCK_Dgram::create_udp error, bind error, errno:%d", error_no());
		close(m_socket);
		m_socket = INVALID_SOCKET;
		return -1;
	}

	m_local_addr = local_addr;
	return 0;
}

void SOCK_Dgram::close_udp()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

uint32_t SOCK_Dgram::get_id()
{
	return m_id;
}

SOCKET SOCK_Dgram::get_handle()
{
	return m_socket;
}

int SOCK_Dgram::handle_input()
{
	while (true)
	{
		char recv_buf[MAX_PACKET_LENGTH];
		sockaddr_in from;
		socklen_t fromlen = sizeof(from);

		int rc = recvfrom(m_socket, recv_buf, MAX_PACKET_LENGTH, 0, (sockaddr*)&from, &fromlen);
		if (rc > 0)
		{
			
			Net_Event *net_event = new Net_Event;

			net_event->net_event_type = TYPE_DATA;
			net_event->id = m_id;
			net_event->new_id = 0;
			net_event->local_addr = m_local_addr;
			net_event->handler = handler;
			net_event->remote_addr.set_addr(from.sin_addr.s_addr);
			net_event->remote_addr.set_port(from.sin_port);
			memcpy(net_event->packet.ptr(), recv_buf, rc);
			net_event->packet.length(rc);

			m_net_manager->put_event(net_event);
		}
		else if (rc == 0)
		{
			// close
			LOG(WARN)("SOCK_Dgram::handle_input, socket close by peer");
			return 0;
		}
		else
		{
			if (EAGAIN == error_no()) {
				return 0;
			}
			else {
				// exception
				LOG(WARN)("SOCK_Dgram::handle_input error, recvfrom error, errno:%d", error_no());
				return 0;
			}
		}
	}

	return 0;
}

int SOCK_Dgram::handle_output()
{
	// 如果当前正在发送的任务非空则先发送
	if (m_current_send_task != NULL)
	{
		sockaddr_in to;
		to.sin_family = AF_INET;
		to.sin_addr.s_addr = m_current_send_task->remote_addr.get_addr();
		to.sin_port = m_current_send_task->remote_addr.get_port();

		socklen_t tolen = sizeof(to);

		int rc = sendto(m_socket, m_current_send_task->packet->ptr(), m_current_send_task->packet->length(), 0, (sockaddr*)&to, tolen);
		if (rc > 0) {
			// 发送成功
			delete m_current_send_task;
			m_current_send_task = NULL;
		}
		else if (rc == 0) {
			// close
			LOG(WARN)("SOCK_Dgram::handle_output, socket close by peer");
			delete m_current_send_task;
			m_current_send_task = NULL;
		}
		else {
			if (EAGAIN == error_no()) {
				return 0;
			}
			else {
				// exception
				LOG(WARN)("SOCK_Dgram::handle_output error, sendto error, errno:%d", error_no());
				delete m_current_send_task;
				m_current_send_task = NULL;
			}
		}
	}

	assert(m_current_send_task == NULL);

	while (true)
	{
		bool brc = m_send_task_queue.read(m_current_send_task);
		if (false == brc) {
			// 发送队列已空
			break;
		}

		sockaddr_in to;
		to.sin_family = AF_INET;
		to.sin_addr.s_addr = m_current_send_task->remote_addr.get_addr();
		to.sin_port = m_current_send_task->remote_addr.get_port();

		socklen_t tolen = sizeof(to);

		int rc = sendto(m_socket, m_current_send_task->packet->ptr(), m_current_send_task->packet->length(), 0, (sockaddr*)&to, tolen);
		if (rc > 0) {
			delete m_current_send_task;
			m_current_send_task = NULL;
		}
		else if (rc == 0) {
			// close
			LOG(WARN)("SOCK_Dgram::handle_output, socket close by peer");
			delete m_current_send_task;
			m_current_send_task = NULL;
		}
		else {
			if (EAGAIN == error_no()) {
				return 0;
			}
			else {
				LOG(WARN)("SOCK_Dgram::handle_output error, sendto error, errno:%d", error_no());
				delete m_current_send_task;
				m_current_send_task = NULL;
			}
		}
	}

	assert(m_current_send_task == NULL);

	int rc = reactor()->remove_handler(this, MASK_WRITE);
	if (0 != rc) {
		// 设置reactor失败,认为socket异常
		LOG(ERROR)("SOCK_Dgram::handle_output error, remove_handler error");
		return -2;
	}

	return 0;
}

int SOCK_Dgram::handle_exception()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}

	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_EXCEPTION;
	net_event->id = m_id;
	net_event->new_id = 0;
	net_event->local_addr = m_local_addr;
	net_event->handler = handler;

	m_net_manager->put_event(net_event);

	if (m_current_send_task != NULL) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}
	while (true == m_send_task_queue.read(m_current_send_task)) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}

	return 0;
}

int SOCK_Dgram::handle_close()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}

	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_CLOSE;
	net_event->id = m_id;
	net_event->new_id = 0;
	net_event->local_addr = m_local_addr;
	net_event->handler = handler;

	m_net_manager->put_event(net_event);

	if (m_current_send_task != NULL) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}
	while (true == m_send_task_queue.read(m_current_send_task)) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}

	return 0;
}

int SOCK_Dgram::handle_timeout()
{
	return -1;
}

int SOCK_Dgram::send_data(Send_Task *send_task)
{
	// 当前没有待发任务, 直接发送
	if (NULL == m_current_send_task)
	{
		// assert send_task_queue is empty
		Send_Task *st;
		assert(false == m_send_task_queue.read(st));

		sockaddr_in to;
		to.sin_family = AF_INET;
		to.sin_addr.s_addr = send_task->remote_addr.get_addr();
		to.sin_port = send_task->remote_addr.get_port();

		socklen_t tolen = sizeof(to);

		int rc = sendto(m_socket, send_task->packet->ptr(), send_task->packet->length(), 0, (sockaddr*)&to, tolen);
		if (rc > 0)
		{
			// 发送成功
			delete send_task;
			return 0;
		}
		else if (rc == 0)
		{
			// close
			LOG(WARN)("SOCK_Dgram::send_data, socket close by peer");
			delete send_task;
			return 0;
		}
		else
		{
			if (EAGAIN == error_no())
			{
				// 网络忙, 入队列待发
			}
			else
			{
				// exception
				LOG(WARN)("SOCK_Dgram::send_data error, sendto error, errno:%d", error_no());
				delete send_task;
				return 0;
			}
		}
	}

	// 网络忙, 入队列待发
	if (NULL == m_current_send_task)
	{
		m_current_send_task = send_task;
	}
	else
	{
		bool brc = m_send_task_queue.write(send_task);
		if (true != brc)
		{
			// 队列用尽
			LOG(ERROR)("SOCK_Dgram::send_data error, send task queue is full, packet lost");
			delete send_task;
			return 0;
		}
	}

	assert(NULL != m_current_send_task);

	// 监控可写事件
	int rc = reactor()->register_handler(this, MASK_WRITE);
	if (0 != rc) {
		// 设置reactor失败, 认为socket异常
		LOG(ERROR)("SOCK_Dgram::send_data error, register_handler error");
		reactor()->notify_close(m_id);
		return 0;
	}
	return 0;
}

