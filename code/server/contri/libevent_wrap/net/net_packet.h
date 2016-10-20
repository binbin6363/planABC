//! @file net_packet.h


#ifndef _NET_PACKET_H_
#define _NET_PACKET_H_


#include "types.h"
#include "config.h"
#include "pool_allocator.h"
#include <string.h>

uint32_t GetNetpacketCnt();

class Net_Buff1
{
	public:
	//! ������
	char m_packet[MAX_PACKET_LENGTH];	
	
	DECLARE_ALLOCATOR(Net_Buff1, NET_PACKET_POOL_COUNT)

	//! �õ���ǰ����ص�����
	//! @return ����ص�����
	static int objectPoolSize();
};

	class Net_Buff2
	{
	public:
		//! ������
		char m_packet[SMALL_PACKET_LENGTH];	
		
		DECLARE_ALLOCATOR(Net_Buff2,NET_SMALL_PACKET_POOL_COUNT)

		static int objectPoolSize();
	};

typedef enum
{
	Net_Buff_Type_NONE = 0,
	Net_Buff_Type_Buff1 = 1,
	Net_Buff_Type_Buff_ARRAY = 2,
	Net_Buff_Type_Buff2 = 3,
}Net_Buff_Type;



//! @class Net_Packet
//! @brief �������װ��
class Net_Packet
{
public:
	Net_Packet();
	Net_Packet(int size);

    Net_Packet(Net_Packet & p);

	~Net_Packet();


	void resize(int size);
	
	DECLARE_ALLOCATOR(Net_Packet, NET_PACKET_POOL_COUNT)

public:
	//! ȡ�����ݻ���ָ��
	//! @return �����ݻ���ָ��
	char* ptr();


	//! �õ���ǰ����ص�����
	//! @return ����ص�����
	static int objectPoolSize();

	
	//! ȡ������
	//! @return ������
	int length();

	//! ���ð�����
	//! @param n ������
	void length(int n);

	//! ȡ��������
	//! @return ��������
	int max_packet_length();

	//! ��հ�
	void clear();

private:
//		//! ������
//		char m_packet[MAX_PACKET_LENGTH];
	Net_Buff_Type buff_type;
	//! ������
	int m_length;

	int max_length;
	
	Net_Buff1* p_net_buff1;

	Net_Buff2* p_net_buff2;
	char* m_packet;

private:	
	void allocate_buff(int size);
	
	void release_buff();
};


//	class Net_Buff4
//	{
//		//! ������
//		char m_packet[4*MAX_PACKET_LENGTH];	
//		DECLARE_ALLOCATOR(Net_Packet, (((NET_PACKET_POOL_COUNT/4)>0) ? (NET_PACKET_POOL_COUNT/4):1))
//	};
//	
//	class Net_Buff8
//	{
//		//! ������
//		char m_packet[8*MAX_PACKET_LENGTH];	
//		DECLARE_ALLOCATOR(Net_Packet, (((NET_PACKET_POOL_COUNT/8)>0) ? (NET_PACKET_POOL_COUNT/8):1))
//	};
//	
//	class Net_Buff16
//	{
//		//! ������
//		char m_packet[16*MAX_PACKET_LENGTH];	
//		DECLARE_ALLOCATOR(Net_Packet, (((NET_PACKET_POOL_COUNT/16)>0) ? (NET_PACKET_POOL_COUNT/16):1))
//	};
//	
//	class Net_Buff32
//	{
//		//! ������
//		char m_packet[32*MAX_PACKET_LENGTH];	
//		DECLARE_ALLOCATOR(Net_Packet, (((NET_PACKET_POOL_COUNT/32)>0) ? (NET_PACKET_POOL_COUNT/32):1))
//	};


#endif // _NET_PACKET_H_
