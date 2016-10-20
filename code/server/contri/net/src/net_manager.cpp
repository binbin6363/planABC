

#include "net_manager.h"
#include "sock_dgram.h"
#include "sock_connector.h"
#include "sock_acceptor.h"
#include "log.h"
#include <assert.h>


// class Net_Manager
Net_Manager::Net_Manager()
{
	m_status = 0;
	m_net_thread = NULL;
	m_net_event_queue = new Net_Event_Queue;
	m_send_task_queue = new Net_Send_Task_Queue;
	m_net_statusManager = NULL;

	// 默认系数，可配置
	// 级别为1的请求，在总数据包多于100%时就要丢弃.
	lev1_water_percent = 1.0;
	// 级别为2的请求，在总数据包多于80%时就要丢弃
	lev2_water_percent = 0.9;
	// 级别为3的请求，在总数据包多于90%时就要丢弃
	lev3_water_percent = 0.8;
}

Net_Manager::~Net_Manager()
{
	delete m_net_event_queue;
	delete m_send_task_queue;
}

int Net_Manager::start()
{
#ifdef _WIN32
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;
    wVersionRequested = MAKEWORD( 2, 2 );
    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        return 1;
    }
    if ( LOBYTE( wsaData.wVersion ) != 2 ||
        HIBYTE( wsaData.wVersion ) != 2 ) {
            WSACleanup( );
            return -1; 
    }
#endif 

    if (0 != m_status) {
		LOG(ERROR)("Net_Manager::start error, alreay started");
		return -1;
	}

	int rc = m_reactor.open_reactor();
	if (0 != rc) {
		LOG(ERROR)("Net_Manager::start error, open reactor error");
		return -1;
	}

	m_net_thread = new Net_Thread(this, &m_reactor);

	rc = m_net_thread->start();
	if (0 != rc) {
		LOG(ERROR)("Net_Manager::start error, start thread error");
		delete m_net_thread;
		m_net_thread = NULL;
		m_reactor.close_reactor();
		return -1;
	}


	
	m_net_statusManager = new StatusManager(this);
	rc = m_net_statusManager->start();
	LOG(INFO)("start status manager thread ret=%d",rc);
	if (0 != rc) {
		LOG(ERROR)("Net_Manager::start error, start status manager thread error");
		delete m_net_statusManager;
		m_net_statusManager = NULL;
		m_reactor.close_reactor();
		return -1;
	}

	m_status = 1;
	return 0;
}

int Net_Manager::stop()
{
	if (0 == m_status) {
		LOG(ERROR)("Net_Manager::stop error, not started");
		return -1;
	}

	int rc = m_net_thread->stop();
	if (0 != rc) {
		LOG(ERROR)("Net_Manager::stop error, stop thread error");
		return -1;
	}

	delete m_net_thread;
	m_net_thread = NULL;


	rc = m_net_statusManager->stop();
	if (0 != rc) {
		LOG(ERROR)("Net_Manager::stop error, stop statusManager thread error");
		return -1;
	}

	delete m_net_statusManager;
	m_net_statusManager = NULL;



	m_reactor.end_reactor_event_loop();
	m_reactor.close_reactor();

	m_status = 0;
	return 0;
}

uint32_t Net_Manager::create_udp(const INET_Addr& local_addr, pfnNetEventHandler handler)
{
	if (1 != m_status) {
		LOG(WARN)("Net_Manager::create_udp error, not started");
		return 0;
	}

	int rc;

	SOCK_Dgram *dgram = new SOCK_Dgram(this, handler);
	uint32_t id = dgram->get_id();

	rc = dgram->create_udp(local_addr);
	if (0 != rc) {
		LOG(WARN)("Net_Manager::create_udp error, create_udp error");
		delete dgram;
		return 0;
	}

	rc = m_reactor.register_handler(dgram, MASK_READ);
	if (0 != rc) {
		LOG(WARN)("Net_Manager::create_udp error, register_handler error");
		dgram->close_udp();
		delete dgram;
		return 0;
	}

	return id;
}

uint32_t Net_Manager::create_udp(const char *local_ip, int local_port, pfnNetEventHandler handler)
{
	INET_Addr local_addr;
	local_addr.set_addr(inet_addr(local_ip));
	local_addr.set_port(htons(local_port));

	return create_udp(local_addr, handler);
}

uint32_t Net_Manager::create_tcp_client(const INET_Addr& remote_addr, Packet_Splitter *packet_splitter, 
                pfnNetEventHandler   handler, int timeout, size_t recv_buff_len)
{
	if (1 != m_status) {
		LOG(WARN)("Net_Manager::create_tcp_client error, not started");
		return 0;
	}

	int rc;
       LOG(DEBUG)("Net_Manager create_tcp_client recv_buff_len:%u", recv_buff_len);
	SOCK_Connector *connector = new SOCK_Connector(this, packet_splitter, handler, recv_buff_len);
	uint32_t id = connector->get_id();

	rc = connector->create_tcp_client();
	if (0 != rc) {
		LOG(WARN)("Net_Manager::create_tcp_client error, create_tcp_client error");
		delete connector;
		return 0;
	}

	rc = connector->connect_server(remote_addr, timeout);
	if (0 != rc) {
		LOG(WARN)("Net_Manager::create_tcp_client error, connect_server error");
		connector->close_tcp_client();
		delete connector;
		return 0;
	}

	rc = m_reactor.register_handler(connector, MASK_WRITE);
	if (0 != rc) {
		LOG(WARN)("Net_Manager::create_tcp_client error, register_handler error");
		connector->close_tcp_client();
		delete connector;
		return 0;
	}

	if (0 != timeout) {
		m_reactor.set_timeout(id, timeout);
	}

	return id;
}

uint32_t Net_Manager::create_tcp_client(const char *remote_ip, int remote_port, 
Packet_Splitter          *packet_splitter, pfnNetEventHandler handler, int timeout, size_t recv_buff_len)
{
	INET_Addr remote_addr;
	remote_addr.set_addr(inet_addr(remote_ip));
	remote_addr.set_port(htons(remote_port));

	return create_tcp_client(remote_addr, packet_splitter, handler, timeout, recv_buff_len);
}

uint32_t Net_Manager::create_tcp_server(const INET_Addr& local_addr, Packet_Splitter *packet_splitter,
                    pfnNetEventHandler accept_handler, pfnNetEventHandler handler, size_t recv_buff_len)
{
	if (1 != m_status) {
		LOG(WARN)("Net_Manager::create_tcp_server error, not started");
		return 0;
	}

	int rc;

	SOCK_Acceptor *acceptor = new SOCK_Acceptor(this, packet_splitter, accept_handler, handler, recv_buff_len);
	uint32_t id = acceptor->get_id();

	rc = acceptor->create_tcp_server(local_addr);
	if (0 != rc) {
		LOG(WARN)("Net_Manager::create_tcp_server error, create_tcp_server error");
		delete acceptor;
		return 0;
	}

	rc = m_reactor.register_handler(acceptor, MASK_READ);
	if (0 != rc) {
		LOG(WARN)("Net_Manager::create_tcp_server error, register_handler error");
		acceptor->close_tcp_server();
		delete acceptor;
		return 0;
	}

	return id;
}

uint32_t Net_Manager::create_tcp_server(const char *local_ip, int local_port, Packet_Splitter *packet_splitter, 
                pfnNetEventHandler  accept_handler, pfnNetEventHandler handler, size_t recv_buff_len)
{
	INET_Addr local_addr;
	local_addr.set_addr(inet_addr(local_ip));
	local_addr.set_port(htons(local_port));

	return create_tcp_server(local_addr, packet_splitter, accept_handler, handler, recv_buff_len);
}

int Net_Manager::delete_net(uint32_t id)
{
	int rc = m_reactor.notify_close(id);
	if (0 != rc) {
		LOG(ERROR)("Net_Manager::delete_net error, notify_close error");
		return -1;
	}

	return 0;
}

int Net_Manager::send_packet(uint32_t id, Net_Packet* packet, const INET_Addr& remote_addr)
{
	if (1 != m_status) {
		LOG(WARN)("Net_Manager::send_packet error, not started");
		return -1;
	}

	if (0 == packet->length()) {
		LOG(WARN)("Net_Manager::send_packet error, length is 0");
		return -1;
	}

	Event_Handler* event_handler = m_reactor.get_event_handler(id);
	if (event_handler == NULL) {
		// 通道不存在
		LOG(WARN)("Net_Manager::send_packet error, net no exist");
		return -1;
	}

	Send_Task *send_task = new Send_Task(id, remote_addr, packet);

	bool rc = m_send_task_queue->write(send_task);
	if (rc != true)	// 队列用尽
	{
		LOG(WARN)("Net_Manager::send_packet error, send task queue is full");
		send_task->packet = NULL;	// 此处不释放packet, packet由调用方释放
		delete send_task;
		return -2;
	}

	return 0;
}

int Net_Manager::send_packet(uint32_t id, Net_Packet* packet, const char *remote_ip, int remote_port)
{
	INET_Addr remote_addr;
	remote_addr.set_addr(inet_addr(remote_ip));
	remote_addr.set_port(htons(remote_port));

	return send_packet(id, packet, remote_addr);
}

int Net_Manager::send_packet(uint32_t id, Net_Packet* packet)
{
	INET_Addr remote_addr;
	return send_packet(id, packet, remote_addr);
}

Net_Event* Net_Manager::get_event()
{
	Net_Event *net_event = NULL;

	bool rc = m_net_event_queue->read(net_event);
	if (true != rc) {
		return NULL;
	}

	return net_event;
}

void Net_Manager::put_event(Net_Event* net_event)
{
	bool rc = m_net_event_queue->write(net_event);
	if (true != rc) {
		// 队列用尽
		LOG(ERROR)("Net_Manager::put_event error, net event queue is full, event_type:%d, netid:%d",
					net_event->net_event_type, net_event->id);
		delete net_event;
	}
}

int Net_Manager::get_send_size()
{
	return m_send_task_queue->size();
}

int Net_Manager::get_recv_size()
{
	return m_net_event_queue->size();
}

int Net_Manager::get_net_event_size()
{
	return Net_Event::objectPoolSize();
}

Send_Task* Net_Manager::get_send_task()
{
	Send_Task *send_task = NULL;

	bool rc = m_send_task_queue->read(send_task);
	if (true != rc) {
		return NULL;
	}

	return send_task;
}

void Net_Manager::on_net_error()
{
	m_status = 2;

	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_SYS_ERROR;
	net_event->id = 0;
	net_event->new_id = 0;

	put_event(net_event);
}

int Net_Manager::set_water(float water1_p, float water2_p, float water3_p)
{
	if (lev1_water_percent < 1.0)
	{
		LOG(WARN)("invalid water percent, lev1_water_percent must >= 1.0");
		return -1;
	}
	lev1_water_percent = water1_p;
	lev2_water_percent = water2_p;
	lev3_water_percent = water3_p;
	return 0;
}


int Net_Manager::register_cmd_level(uint32_t cmd, uint32_t lev)
{
	cmd_level_map[cmd] = lev;
	return 0;
}

int Net_Manager::level(uint32_t cmd)
{
	int lev = 0;
	if (cmd_level_map.find(cmd) != cmd_level_map.end()) {
		lev = cmd_level_map[cmd];
	}
	return lev;
}


