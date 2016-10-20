
#include "net_manager.h"
#include "common.h"
#include "os.h"
#include "log.h"
#include <signal.h>

#ifdef WIN32
  #include <windows.h>
  #pragma comment(lib, "Ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif


bool exit_flag;
void exit_process(int)
{
	exit_flag = true;
}

// global var
Net_Manager net_manager;
uint32_t tcp_id;
uint32_t udp_id;
void handle_event(Net_Event* event);
void handle_request(Net_Event* event);


int main()
{
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

	signal(SIGINT, exit_process);

	// set log
	LOG_INIT("test_server", 50000000, utils::L_TRACE);
	LOG_OPEN();


	LOG(INFO)("test_server begin");

	int rc;

	

	rc = net_manager.start();
	if (0 != rc) {
		LOG(INFO)("start net_manager error");
		return -1;
	}
	LOG(INFO)("start net_manager ok");

	INET_Addr server_addr;
	server_addr.set_addr(inet_addr("0.0.0.0"));
	server_addr.set_port(htons(7799));

	Easy_Packet_Splitter splitter;

	tcp_id = net_manager.create_tcp_server(server_addr, &splitter, &null_event, &null_event);
	if (0 == tcp_id) {
		LOG(INFO)("create_tcp_server error");
		net_manager.stop();
		return -1;
	}
	LOG(INFO)("create_tcp_server ok, id:%d", tcp_id);

	udp_id = net_manager.create_udp(server_addr, &null_event);
	if (0 == udp_id) {
		LOG(INFO)("create_udp error");
		net_manager.stop();
		return -1;
	}
	LOG(INFO)("create_udp ok, id:%d", udp_id);


	// 事件循环
	while (exit_flag != true)
	{
		Net_Event* event = net_manager.get_event();
		if (NULL == event) {
			usleep(10);
			continue;
		}

		handle_event(event);
		delete event;
	}

	net_manager.delete_net(tcp_id);
	net_manager.delete_net(udp_id);
	usleep(1 * 1000 * 1000);	// 1s

	while (true)
	{
		Net_Event* event = net_manager.get_event();
		if (NULL == event) {
			break;
		}
		show_event(event);
		delete event;
	}

	rc = net_manager.stop();
	if (0 != rc) {
		LOG(INFO)("stop net_manager error");
		return -1;
	}
	LOG(INFO)("stop net_manager ok");

	usleep(1 * 1000 * 1000);	// 1s

	while (true)
	{
		Net_Event* event = net_manager.get_event();
		if (NULL == event) {
			break;
		}
		show_event(event);
		delete event;
	}

	LOG(INFO)("test_tcp_server end");

#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}


void handle_event(Net_Event* event)
{
	if (event->net_event_type == TYPE_NULL) {
		assert(1 == 2);
	}
	else if (event->net_event_type == TYPE_DATA) {
		handle_request(event);
	}
	else if (event->net_event_type == TYPE_CONNECT) {
		assert(1 == 3);
	}
	else if (event->net_event_type == TYPE_ACCEPT) {
	}
	else if (event->net_event_type == TYPE_CLOSE) {
	}
	else if (event->net_event_type == TYPE_EXCEPTION) {
		LOG(INFO)("handle_event TYPE_EXCEPTION");
	}
	else if (event->net_event_type == TYPE_TIMEOUT) {
		LOG(INFO)("handle_event TYPE_TIMEOUT");
	}
	else if (event->net_event_type == TYPE_SYS_ERROR) {
		assert(1 == 4);
	}
}

void handle_request(Net_Event* event)
{
	char *recv_data = event->packet.ptr();
	int recv_len = event->packet.length();

	// 原样发回
	Net_Packet *packet = new Net_Packet;
	memcpy(packet->ptr(), recv_data, recv_len);
	packet->length(recv_len);

	int rc = net_manager.send_packet(event->id, packet, event->remote_addr);
	if (0 != rc) {
		delete packet;
		LOG(INFO)("net_manager.send_packet error");
	}
}

