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
	//! 包缓存
	char m_packet[MAX_PACKET_LENGTH];	
	
	DECLARE_ALLOCATOR(Net_Buff1, NET_PACKET_POOL_COUNT)

	//! 得到当前对象池的数量
	//! @return 对象池的数量
	static int objectPoolSize();
};

	class Net_Buff2
	{
	public:
		//! 包缓存
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
//! @brief 网络包封装类
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
	//! 取包数据缓存指针
	//! @return 包数据缓存指针
	char* ptr();


	//! 得到当前对象池的数量
	//! @return 对象池的数量
	static int objectPoolSize();

	
	//! 取包长度
	//! @return 包长度
	int length();

	//! 设置包长度
	//! @param n 包长度
	void length(int n);

	//! 取最大包长度
	//! @return 最大包长度
	int max_packet_length();

	//! 清空包
	void clear();

private:
//		//! 包缓存
//		char m_packet[MAX_PACKET_LENGTH];
	Net_Buff_Type buff_type;
	//! 包长度
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
//		//! 包缓存
//		char m_packet[4*MAX_PACKET_LENGTH];	
//		DECLARE_ALLOCATOR(Net_Packet, (((NET_PACKET_POOL_COUNT/4)>0) ? (NET_PACKET_POOL_COUNT/4):1))
//	};
//	
//	class Net_Buff8
//	{
//		//! 包缓存
//		char m_packet[8*MAX_PACKET_LENGTH];	
//		DECLARE_ALLOCATOR(Net_Packet, (((NET_PACKET_POOL_COUNT/8)>0) ? (NET_PACKET_POOL_COUNT/8):1))
//	};
//	
//	class Net_Buff16
//	{
//		//! 包缓存
//		char m_packet[16*MAX_PACKET_LENGTH];	
//		DECLARE_ALLOCATOR(Net_Packet, (((NET_PACKET_POOL_COUNT/16)>0) ? (NET_PACKET_POOL_COUNT/16):1))
//	};
//	
//	class Net_Buff32
//	{
//		//! 包缓存
//		char m_packet[32*MAX_PACKET_LENGTH];	
//		DECLARE_ALLOCATOR(Net_Packet, (((NET_PACKET_POOL_COUNT/32)>0) ? (NET_PACKET_POOL_COUNT/32):1))
//	};


#endif // _NET_PACKET_H_
