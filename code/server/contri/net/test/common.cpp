

#include "common.h"
#include "log.h"

#ifdef WIN32
  #include <windows.h>
  #pragma comment(lib, "Ws2_32.lib")
#else
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif


const char* event_name(NET_EVENT_TYPE event_type)
{
	switch (event_type)
	{
	case TYPE_NULL:
		return "TYPE_NULL";
	case TYPE_DATA:
		return "TYPE_DATA";
	case TYPE_CONNECT:
		return "TYPE_CONNECT";
	case TYPE_ACCEPT:
		return "TYPE_ACCEPT";
	case TYPE_CLOSE:
		return "TYPE_CLOSE";
	case TYPE_EXCEPTION:
		return "TYPE_EXCEPTION";
	case TYPE_TIMEOUT:
		return "TYPE_TIMEOUT";
	default:
		return "ERROR_TYPE";
	}
}

void null_event(Net_Event& event)
{
}

void show_event(Net_Event* event)
{
	LOG(INFO)("=========== event info ===========");
	LOG(INFO)("type:%s", event_name(event->net_event_type));
	LOG(INFO)("id:%d", event->id);
	LOG(INFO)("new_id:%d", event->new_id);

	in_addr addr;

	addr.s_addr = event->local_addr.get_addr();
	LOG(INFO)("local_addr ip:%s", inet_ntoa(addr));
	LOG(INFO)("local_addr port:%d", ntohs(event->local_addr.get_port()));

	addr.s_addr = event->remote_addr.get_addr();
	LOG(INFO)("remote_addr ip:%s", inet_ntoa(addr));
	LOG(INFO)("remote_addr port:%d", ntohs(event->remote_addr.get_port()));

	if (event->packet.length() > 0)
	{
		char *buf = new char [event->packet.length() + 1];
		memcpy(buf, event->packet.ptr(), event->packet.length());
		buf[event->packet.length()] = 0;
		LOG(INFO)("packet:%s", buf);
		delete [] buf;
	}
	LOG(INFO)("=============== end ==============");
}


//  class Easy_Packet_Splitter
Easy_Packet_Splitter::~Easy_Packet_Splitter()
{
}

int Easy_Packet_Splitter::split(const char *buf, int len, int& packet_begin, int& packet_len)
{
	if (len < 2) {
		return 0;
	}

	int pack_len = ntohs(*(short*)buf);
	if (pack_len < 2) {
		LOG(INFO)("Easy_Packet_Splitter::split, pack_len:%d", pack_len);
		return -1;
	}
	if (pack_len > 2048) {
		LOG(INFO)("Easy_Packet_Splitter::split, pack_len:%d", pack_len);
		return -1;
	}

	if (len >= pack_len)
	{
		packet_begin = 0;
		packet_len = pack_len;
		return 1;
	}

	return 0;
}

