
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
	LOG_INIT("test_tcp_client", 50000000, utils::L_TRACE);
	LOG_OPEN();

	LOG(INFO)("test_tcp_client begin");

	int rc;

	Net_Manager net_manager;

	rc = net_manager.start();
	if (0 != rc) {
		LOG(INFO)("start net_manager error");
		return -1;
	}
	LOG(INFO)("start net_manager ok");

	INET_Addr tcp_server_addr;
	tcp_server_addr.set_addr(inet_addr("192.168.52.99"));
	tcp_server_addr.set_port(htons(7799));

	Easy_Packet_Splitter splitter;

	uint32_t id = net_manager.create_tcp_client(tcp_server_addr, &splitter, &null_event, 10);
	if (0 == id) {
		LOG(INFO)("create_tcp_client error");
		net_manager.stop();
		return -1;
	}
	LOG(INFO)("create_tcp_client ok, id:%d", id);

	// wait for connected
	while (true)
	{
		Net_Event* event = net_manager.get_event();
		if (event == NULL) {
			usleep(1000);
			continue;
		}

		show_event(event);
		if ((event->id = id) && (event->net_event_type == TYPE_CONNECT))
		{
			LOG(INFO)("connectd");
			delete event;
			break;
		}

		delete event;
	}

	// send request
	int count = 0;
	while (exit_flag != true)
	{
		// send a packet
		Net_Packet *packet = new Net_Packet;

		memset(packet->ptr(), '0', 1024);
		short *plen = (short*)packet->ptr();
		*plen = htons(1024);
		packet->length(1024);

		int rc = net_manager.send_packet(id, packet, tcp_server_addr);
		if (0 != rc) {
			delete packet;
		}

		while (true)
		{
			Net_Event* event = net_manager.get_event();
			if (event == NULL) {
				break;
			}

			if (event->net_event_type != TYPE_DATA) {
				show_event(event);
			}

			delete event;
		}
		
		count++;
		if (count % 1000 == 0)
		{
			LOG(INFO)("#");
		}
		usleep(1 * 0);
	}

	net_manager.delete_net(id);
	usleep(1 * 1000 * 1000);	// 1s

	while (true)
	{
		Net_Event* event = net_manager.get_event();
		if (NULL == event) {
			break;
		}
//		show_event(event);
		delete event;
	}

	rc = net_manager.stop();
	if (0 != rc) {
		LOG(INFO)("stop net_manager error");
		return -1;
	}
	LOG(INFO)("stop net_manager ok");

	LOG(INFO)("test_tcp_client end");

#ifdef WIN32
	WSACleanup();
#endif
	return 0;	
}

