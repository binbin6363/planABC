//! @file sock_dgram.h


#ifndef _SOCK_DGRAM_H_
#define _SOCK_DGRAM_H_

#include "net_event.h"
#include "event_handler.h"
#include "inet_addr.h"
#include "send_task.h"
#include "send_task_queue.h"


class Net_Manager;


//! @class SOCK_Dgram
//! @brief upd通道处理类
class SOCK_Dgram : public Event_Handler
{
public:
	//! 构造函数
	//! @param net_manager 网络管理器
	SOCK_Dgram(Net_Manager *net_manager, pfnNetEventHandler in_handler);

	//! 析构函数
	virtual ~SOCK_Dgram();

public:
	//! 创建upd通道
	//! @param local_addr upd本地绑定地址
	//! @param handler 该连接的事件处理函数指针
	//! @return 结果, 0:成功, -1失败
	int create_udp(const INET_Addr& local_addr);

	//! 关闭通道
	void close_udp();

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

	//! 本地绑定地址
	INET_Addr m_local_addr;

	//! 网络管理器
	Net_Manager *m_net_manager;

	//! 待发送任务队列
	Socket_Send_Task_Queue m_send_task_queue;

	//! 当前正在发送的任务
	Send_Task *m_current_send_task;
	
	//! 该连接的事件处理函数指针
	pfnNetEventHandler handler;
};


#endif // _SOCK_DGRAM_H_
