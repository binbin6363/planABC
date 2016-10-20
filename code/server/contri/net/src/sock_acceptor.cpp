

#include "sock_acceptor.h"
#include "sock_stream.h"
#include "net_manager.h"
#include "packet_splitter.h"
#include "log.h"
#include <assert.h>


// class SOCK_Acceptor
SOCK_Acceptor::SOCK_Acceptor(Net_Manager *net_manager, Packet_Splitter *packet_splitter, 
        pfnNetEventHandler   in_accept_handler, pfnNetEventHandler in_handler,
            size_t recv_buff_len)
{
	m_id = net_manager->m_id_manager.acquire(ID_TCP_ACCEPTOR);
	assert(m_id != 0);

	m_socket = INVALID_SOCKET;
	m_net_manager = net_manager;
	m_packet_splitter = packet_splitter;
	accept_handler = in_accept_handler;
	handler = in_handler;
	m_recv_buff_len = recv_buff_len;
}

SOCK_Acceptor::~SOCK_Acceptor()
{
	if (0 != m_id) {
		m_net_manager->m_id_manager.release(m_id);
	}
}

int SOCK_Acceptor::create_tcp_server(const INET_Addr& local_addr)
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket) {
		LOG(ERROR)("SOCK_Acceptor::create_tcp_server error, socket error, errno:%d", error_no());
		return -1;
	}

	set_socket_nonblocking(m_socket);

	int val = 1;
	setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&val, sizeof(val));

	sockaddr_in local;
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = local_addr.get_addr();
	local.sin_port = local_addr.get_port();

	int rc = bind(m_socket, (sockaddr*)&local, sizeof(local));
	if (0 != rc) {
		LOG(ERROR)("SOCK_Acceptor::create_tcp_server error, bind error, errno:%d", error_no());
		close(m_socket);
		m_socket = INVALID_SOCKET;
		return -1;
	}

	rc = listen(m_socket, 10240);
	if (0 != rc) {
		LOG(ERROR)("SOCK_Acceptor::create_tcp_server error, listen error, errno:%d", error_no());
		close(m_socket);
		m_socket = INVALID_SOCKET;
		return -1;
	}

	m_local_addr = local_addr;
	return 0;
}

void SOCK_Acceptor::close_tcp_server()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

uint32_t SOCK_Acceptor::get_id()
{
	return m_id;
}

SOCKET SOCK_Acceptor::get_handle()
{
	return m_socket;
}

int SOCK_Acceptor::handle_input()
{
	while (true)
	{
		sockaddr_in addr;
		socklen_t addrlen = sizeof(addr);

		SOCKET accept_socket = accept(m_socket, (sockaddr*)&addr, &addrlen);
		if (-1 == accept_socket) {
			if (EAGAIN == error_no() || ECONNABORTED == error_no()) {
				return 0;
			}
			else {
				// exception
				LOG(WARN)("SOCK_Acceptor::handle_input error, accept error, errno:%d", error_no());
				return -2;
			}
		}

		// got a new connection
		int rc;

		set_socket_nonblocking(accept_socket);

		uint32_t new_id = m_net_manager->m_id_manager.acquire(ID_TCP_STREAM);
		assert(new_id != 0);

		INET_Addr remote_addr;
		remote_addr.set_addr(addr.sin_addr.s_addr);
		remote_addr.set_port(addr.sin_port);

		SOCK_Stream *stream = new SOCK_Stream(new_id, m_net_manager, m_packet_splitter, accept_socket, 
		                m_local_addr, remote_addr, handler, m_recv_buff_len);

		rc = reactor()->register_handler(stream, MASK_READ);
		if (0 != rc) {
			LOG(ERROR)("SOCK_Acceptor::handle_input error, register_handler MASK_READ error");
			stream->close_stream();
			delete stream;
			return 0;
		}

		//参照IMD 的epoll框架，则accept时即置位写监听标记
		rc = reactor()->register_handler(stream, MASK_WRITE);
		if (0 != rc) {
			// 设置reactor失败,认为socket异常
			LOG(ERROR)("SOCK_Acceptor::handle_input error, register_handler MASK_WRITE error");
			stream->close_stream();
			delete stream;
			return 0;
		}

		// throw a net event
		Net_Event *net_event = new Net_Event;

		net_event->net_event_type = TYPE_ACCEPT;
		net_event->id = m_id;
		net_event->new_id = new_id;
		net_event->local_addr = m_local_addr;
		net_event->remote_addr = remote_addr;
		net_event->handler = accept_handler;

		m_net_manager->put_event(net_event);
	}

	return 0;
}

int SOCK_Acceptor::handle_output()
{
	assert(true == false);
	return -1;
}

int SOCK_Acceptor::handle_exception()
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
	net_event->handler = accept_handler;

	m_net_manager->put_event(net_event);
	
	return 0;
}

int SOCK_Acceptor::handle_close()
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
	net_event->handler = accept_handler;

	m_net_manager->put_event(net_event);

	return 0;
}

int SOCK_Acceptor::handle_timeout()
{
	return -1;
}

int SOCK_Acceptor::send_data(Send_Task *send_task)
{
	LOG(ERROR)("SOCK_Acceptor::send_data error, can't send at this socket");
	delete send_task;
	return 0;
}

