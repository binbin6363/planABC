#include "log.h"
#include "net_event.h"


// class Net_Event
Net_Event::Net_Event(int packetLen /*= MAX_PACKET_LENGTH*/):
	packet(packetLen)
{
	clear();
}

Net_Event::~Net_Event()
{
}

void Net_Event::clear()
{
	net_event_type = TYPE_NULL;
	id = 0;
	new_id = 0;
	local_addr.clear();
	remote_addr.clear();
	packet.clear();
	handler = NULL;
}

void Net_Event::setBuff(const char* buff, int packet_len)
{
	if (packet_len <= MAX_PACKET_LENGTH) 
	{
		memcpy(packet.ptr(), buff, packet_len);
		packet.length(packet_len);
//			LOG_DEBUG("ORG packet_len:%d, maxlen:%d",
//					packet_len, packet.max_packet_length());
	}
	else
	{
		packet.resize(packet_len);		
		memcpy(packet.ptr(), buff, packet_len);
		packet.length(packet_len);
		LOG_WARN("Net_Event::setBuff RESIZED packet_len:%d, maxlen:%d",packet_len, packet.max_packet_length());
	}
}

int Net_Event::objectPoolSize()
{
	return m_pool_allocator.size();
}


IMPLEMENT_ALLOCATOR(Net_Event)

