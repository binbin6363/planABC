

#include "sock_connector.h"
#include "sock_stream.h"
#include "net_manager.h"
#include "packet_splitter.h"
#include "log.h"
#include <assert.h>


// class SOCK_Connector
SOCK_Connector::SOCK_Connector(Net_Manager *net_manager, Packet_Splitter *packet_splitter, 
                pfnNetEventHandler   in_handler, size_t recv_buff_len)
{
	m_id = net_manager->m_id_manager.acquire(ID_TCP_CONNECTOR);
	assert(m_id != 0);

	m_socket = INVALID_SOCKET;
	m_net_manager = net_manager;
	m_packet_splitter = packet_splitter;
	handler = in_handler;
	m_recv_buff_len = recv_buff_len;
	LOG(DEBUG)("SOCK_Connector SOCK_Connector recv_buff_len:%u", recv_buff_len);
}

SOCK_Connector::~SOCK_Connector()
{
	if (0 != m_id) {
		m_net_manager->m_id_manager.release(m_id);
	}
}

int SOCK_Connector::create_tcp_client()
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket) {
		LOG(ERROR)("SOCK_Connector::create_tcp_client error, socket error, errno:%d", error_no());
		return -1;
	}

	set_socket_nonblocking(m_socket);

	return 0;
}

int SOCK_Connector::connect_server(const INET_Addr& remote_addr, int timeout)
{
	sockaddr_in remote;
	remote.sin_family = AF_INET;
	remote.sin_addr.s_addr = remote_addr.get_addr();
	remote.sin_port = remote_addr.get_port();

	int rc = connect(m_socket, (sockaddr*)&remote, sizeof(remote));
	if ((rc != 0) && (error_no() != EINPROGRESS)) {
		LOG(ERROR)("SOCK_Connector::connect_server error, connect error, errno:%d", error_no());
		return -1;
	}

	m_remote_addr = remote_addr;
	
	return 0;
}

void SOCK_Connector::close_tcp_client()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

uint32_t SOCK_Connector::get_id()
{
	return m_id;
}

SOCKET SOCK_Connector::get_handle()
{
	return m_socket;
}

int SOCK_Connector::handle_input()
{
	assert(true == false);
	return -1;
}

int SOCK_Connector::handle_output()
{
	int sock_error;
	socklen_t len = sizeof(sock_error);

	int rc = getsockopt(m_socket, SOL_SOCKET, SO_ERROR, (char*)&sock_error, &len);
	if (0 != rc) {
		LOG(ERROR)("SOCK_Connector::handle_output error, getsockopt error, errno:%d", error_no());
		return -2;
	}

	if (0 != sock_error) {
		LOG(ERROR)("SOCK_Connector::handle_output error, sock_error:%d", sock_error);
		return -2;
	}

	// connected

	rc = reactor()->remove_handler(this, MASK_WRITE);
	if (0 != rc) {
		LOG(ERROR)("SOCK_Connector::handle_output error, remove_handler error");
		return -2;
	}

	rc = reactor()->cancel_timeout(m_id);
	if (0 != rc) {
		LOG(ERROR)("SOCK_Connector::handle_output error, cancel_timeout error");
		return -2;
	}

	// get socket local addr
	INET_Addr local_addr;
	sockaddr_in name;
	socklen_t namelen = sizeof(name);
	rc = getsockname(m_socket, (sockaddr*)&name, &namelen);
	if (0 == rc) {
		local_addr.set_addr(name.sin_addr.s_addr);
		local_addr.set_port(name.sin_port);
	}
        LOG(DEBUG)("SOCK_Connector handle_output recv_buff_len:%u", m_recv_buff_len);
	// create stream
	SOCK_Stream *stream = new SOCK_Stream(m_id, m_net_manager, m_packet_splitter, m_socket,
	                local_addr, m_remote_addr, handler, m_recv_buff_len);

	// NOTICE SOCK_Stream创建成功后, m_id, m_socket的生命周期由SOCK_Stream管理
	m_id = 0;
	m_socket = INVALID_SOCKET;

	rc = reactor()->register_handler(stream, MASK_READ);
	if (0 != rc) {
		LOG(ERROR)("SOCK_Connector::handle_output error, register_handler error");
		stream->close_stream();
		delete stream;
		return -2;
	}

	// throw a net event
	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_CONNECT;
	net_event->id = stream->get_id();
	net_event->new_id = 0;
	net_event->local_addr = local_addr;
	net_event->remote_addr = m_remote_addr;
	net_event->handler = handler;

	m_net_manager->put_event(net_event);

	// NOTICE
	delete this;
	return 0;
}

int SOCK_Connector::handle_exception()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}

	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_EXCEPTION;
	net_event->id = m_id;
	net_event->new_id = 0;
	net_event->remote_addr = m_remote_addr;
	net_event->handler = handler;

	m_net_manager->put_event(net_event);

	return 0;
}

int SOCK_Connector::handle_close()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}

	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_CLOSE;
	net_event->id = m_id;
	net_event->new_id = 0;
	net_event->remote_addr = m_remote_addr;
	net_event->handler = handler;

	m_net_manager->put_event(net_event);

	return 0;
}

int SOCK_Connector::handle_timeout()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}

	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_TIMEOUT;
	net_event->id = m_id;
	net_event->new_id = 0;
	net_event->remote_addr = m_remote_addr;
	net_event->handler = handler;

	m_net_manager->put_event(net_event);

	return 0;
}

int SOCK_Connector::send_data(Send_Task *send_task)
{
	LOG(ERROR)("SOCK_Connector::send_data error, can't send on this socket");
	delete send_task;
	return 0;
}

