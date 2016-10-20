//! @file sock_acceptor.h


#ifndef _SOCK_ACCEPTOR_H_
#define _SOCK_ACCEPTOR_H_

#include "net_event.h"
#include "event_handler.h"
#include "inet_addr.h"
#include "send_task.h"

class Net_Manager;
class Packet_Splitter;


//! @class SOCK_Acceptor
//! @brief tcp监听处理类
class SOCK_Acceptor : public Event_Handler
{
public:
	//! 构造函数
	//! @param net_manager 网络管理器
	//! @param packet_splitter 拆包器
	SOCK_Acceptor(Net_Manager *net_manager, Packet_Splitter *packet_splitter,
	            pfnNetEventHandler in_accept_handler, pfnNetEventHandler in_handler,
	                size_t recv_buff_len = RECV_BUFFER_LENGTH);

	//! 析构函数
	virtual ~SOCK_Acceptor();

public:
	//! 创建tcp监听器
	//! @param local_addr tcp本地监听地址
	//! @return 结果, 0:成功, -1失败
	int create_tcp_server(const INET_Addr& local_addr);

	//! 关闭通道
	void close_tcp_server();

	//! 获取通道id
	//! @return 通道id
	virtual uint32_t get_id();

	//! 获取socket句柄
	//! @return socket句柄
	virtual SOCKET get_handle();

	//! 处理读
	//! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
	virtual int handle_input();

	//! 处理写
	//! @return 处理结果 0:处理正常, -1: 连接被关闭, -2:连接异常
	virtual int handle_output();

	//! 连接异常
	virtual int handle_exception();

	//! 连接关闭
	virtual int handle_close();

	//! 超时
	virtual int handle_timeout();

	//! 提交发送任务
	//! @param send_task 待发送的任务
	virtual int send_data(Send_Task *send_task);

private:
	//! 通道id
	uint32_t m_id;

	//! socket句柄
	SOCKET m_socket;

	//! 本地监听地址
	INET_Addr m_local_addr;

	//! 网络管理器
	Net_Manager *m_net_manager;

	//! 拆包器
	Packet_Splitter *m_packet_splitter;
	
	//! 该监听端口accpet的事件处理函数指针
	pfnNetEventHandler accept_handler;
	
	//! 该连接的事件处理函数指针
	pfnNetEventHandler handler;

	size_t m_recv_buff_len ;
};


#endif // _SOCK_ACCEPTOR_H_
