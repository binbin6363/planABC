
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


int main()
{
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);
#endif

	signal(SIGINT, exit_process);

	// set log
	LOG_INIT("test_tcp_server", 50000000, utils::L_TRACE);
	LOG_OPEN();


	LOG(INFO)("test_tcp_server begin");

	int rc;

	Net_Manager net_manager;

	rc = net_manager.start();
	if (0 != rc) {
		LOG(INFO)("start net_manager error");
		return -1;
	}
	LOG(INFO)("start net_manager ok");

	INET_Addr tcp_addr;
	tcp_addr.set_addr(inet_addr("0.0.0.0"));
	tcp_addr.set_port(htons(7799));

	Easy_Packet_Splitter splitter;

	uint32_t id = net_manager.create_tcp_server(tcp_addr, &splitter, &null_event, &null_event);
	if (0 == id) {
		LOG(INFO)("create_tcp_server error");
		net_manager.stop();
		return -1;
	}
	LOG(INFO)("create_tcp_server ok, id:%d", id);


	// 事件循环
	while (exit_flag != true)
	{
		Net_Event* event = net_manager.get_event();
		if (NULL == event)
		{
			usleep(100);
			continue;
		}

		if (event->net_event_type != TYPE_DATA)
		{
		//	show_event(event);
		}
		else
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
			}
		}

		delete event;
	}

	net_manager.delete_net(id);
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
	//	show_event(event);
		delete event;
	}

	LOG(INFO)("test_tcp_server end");

#ifdef WIN32
	WSACleanup();
#endif
	return 0;
}

