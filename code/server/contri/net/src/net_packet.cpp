
#include "net_packet.h"





// check memory leak
uint32_t netpacket_cnt = 0;
uint32_t GetNetpacketCnt()
{
    return netpacket_cnt;
}

int Net_Buff1::objectPoolSize()
{
	return m_pool_allocator.size();
}


int Net_Buff2::objectPoolSize()
{
	return m_pool_allocator.size();
}

// class Net_Packet

void Net_Packet::allocate_buff(int size)
{
	buff_type = Net_Buff_Type_NONE;
	p_net_buff1 = NULL;
	p_net_buff2 = NULL;
	if(size <= SMALL_PACKET_LENGTH)
	{
		p_net_buff2 = new Net_Buff2;
		m_packet = p_net_buff2->m_packet;
		max_length = SMALL_PACKET_LENGTH;
		buff_type = Net_Buff_Type_Buff2;
	}
	else if(size > SMALL_PACKET_LENGTH && size <= MAX_PACKET_LENGTH)
	{
		p_net_buff1 = new Net_Buff1;		
		m_packet = p_net_buff1->m_packet;
		max_length = MAX_PACKET_LENGTH;
		buff_type = Net_Buff_Type_Buff1;
	}
	else
	{
		m_packet = new char[size];
		max_length = size;
		buff_type = Net_Buff_Type_Buff_ARRAY;
	}
	
	memset(m_packet, 0, sizeof(m_packet));
	m_length = 0;	
}

void Net_Packet::release_buff()
{
	switch(buff_type)
	{
		case Net_Buff_Type_Buff1:
			delete p_net_buff1;
			p_net_buff1 = NULL;
			break;
		case Net_Buff_Type_Buff2:
			delete p_net_buff2;
			p_net_buff2 = NULL;
			break;
		case Net_Buff_Type_Buff_ARRAY:
			delete[] m_packet;
			break;
	}	
	
	m_packet = NULL;
	max_length = 0;
	m_length = 0;
	return;
}

Net_Packet::Net_Packet()
{
	allocate_buff(MAX_PACKET_LENGTH);	
    ++netpacket_cnt;
}

Net_Packet::Net_Packet(Net_Packet & p)
{
	allocate_buff(p.max_packet_length());
	m_length = p.m_length;
	memcpy(m_packet, p.m_packet, m_length);
    ++netpacket_cnt;
}

Net_Packet::Net_Packet(int size)
{
	allocate_buff(size);
    ++netpacket_cnt;
}

Net_Packet::~Net_Packet()
{
	release_buff();	
    --netpacket_cnt;
}

int Net_Packet::objectPoolSize()
{
	return m_pool_allocator.size();
}


void Net_Packet::resize(int size)
{
	release_buff();
	allocate_buff(size);
}

char* Net_Packet::ptr()
{
	return m_packet;
}

int Net_Packet::length()
{
	return m_length;
}

void Net_Packet::length(int n)
{
	m_length = n;
}

int Net_Packet::max_packet_length()
{
	return max_length;
}

void Net_Packet::clear()
{
	memset(m_packet, 0, max_length);
	m_length = 0;
}


IMPLEMENT_ALLOCATOR(Net_Packet)
IMPLEMENT_ALLOCATOR(Net_Buff1)
IMPLEMENT_ALLOCATOR(Net_Buff2)



