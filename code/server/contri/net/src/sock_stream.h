//! @file sock_stream.h


#ifndef _SOCK_STREAM_H_
#define _SOCK_STREAM_H_

#include "net_event.h"
#include "event_handler.h"
#include "inet_addr.h"
#include "send_task.h"
#include "send_task_queue.h"

#include "pool_allocator.h"
#include "config.h"
#include "block_buffer.h"

class Net_Manager;
class Packet_Splitter;
//class Block_Buffer_D;

//! @class SOCK_Stream
//! @brief tcp通道处理类
class SOCK_Stream : public Event_Handler
{
private:
	//! 构造函数
	//! @param id 通道id
	//! @param net_manager 网络管理器
	//! @param packet_splitter 拆包器
	//! @param socket 套接字
	//! @param local_addr 本地地址
	//! @param remote_addr 对端地址
	SOCK_Stream(uint32_t id, Net_Manager *net_manager, Packet_Splitter *packet_splitter, SOCKET socket, const INET_Addr& local_addr, const INET_Addr& remote_addr
	            , pfnNetEventHandler in_handler, size_t buff_len = RECV_BUFFER_LENGTH);

public:
	virtual ~SOCK_Stream();

	DECLARE_ALLOCATOR(SOCK_Stream, SOCK_STREAM_POOL_COUNT)

public:

	static int objectPoolSize();

	//! 关闭连接
	void close_stream();

	//! 获取通道id
	//! @return 通道id
	virtual uint32_t get_id();

	//! 获取socket句柄
	//! @return socket句柄
	virtual SOCKET get_handle();

	//! 读
	//! @return 处理结果 0:处理正常, -1: 连接被对方关闭, -2:连接异常
	virtual int handle_input();

	//! 写
	//! @return 处理结果 0:处理正常, -1: 连接异常
	virtual int handle_output();

	//! 连接断开
	virtual int handle_exception();

	//! 超时
	virtual int handle_timeout();

	//! 连接关闭
	virtual int handle_close();

	//! 提交发送任务
	virtual int send_data(Send_Task *send_task);

public:
	friend class SOCK_Acceptor;
	friend class SOCK_Connector;

private:
	//! 通道id
	uint32_t m_id;

	//! socket句柄
	SOCKET m_socket;

	//! 本地监听地址
	INET_Addr m_local_addr;

	//! 对端地址
	INET_Addr m_remote_addr;

	//! 网络管理器
	Net_Manager *m_net_manager;

	//! 拆包器
	Packet_Splitter *m_packet_splitter;

	//! 接受缓存
	//Block_Buffer_T<RECV_BUFFER_LENGTH> m_recv_buffer;
	
       //修改为动态buffer，由应用程序指定
	Block_Buffer_D m_recv_buffer;

	//! 待发送任务队列
	Socket_Send_Task_Queue m_send_task_queue;

	//! 当前正在发送的任务
	Send_Task *m_current_send_task;

	//! 当前正在发送的任务已经发送的字节数
	int m_current_send_length;
	
	//! 该连接的事件处理函数指针
	pfnNetEventHandler handler;
};


#endif // _SOCK_STREAM_H_
