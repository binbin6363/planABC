//! @file net_manager.h


#ifndef _NET_MANAGER_H_
#define _NET_MANAGER_H_


#include "types.h"
#include "net_thread.h"
#include "id_manager.h"
#include "event_handler.h"
#include "inet_addr.h"
#include "net_packet.h"
#include "net_event.h"
#include "net_event_queue.h"
#include "send_task.h"
#include "send_task_queue.h"
#include "packet_splitter.h"

#ifdef WIN32	// for windows
  #include "select_reactor.h"
  //! windows平台下使用select
  typedef Select_Reactor Net_Reactor;
#else			// for linux
  //! linux平台下使用epoll
  #include "epoll_reactor.h"
  typedef Epoll_Reactor Net_Reactor;
#endif

enum kCMD_LEVEL {
	CMD_LEVEL_1 = 1,
	CMD_LEVEL_2 ,
	CMD_LEVEL_3 ,
};


//! @class Net_Manager
//! @brief 网络管理器
class Net_Manager
{
public:
	Net_Manager();
	~Net_Manager();

public:
	//! 启动网络管理器线程
	//! @return 0:成功, -1失败
	int start();

	//! 停止网络管理器线程
	//! @return 0:成功, -1失败
	int stop();

	//! 创建一个upd通道
	//! @param local_addr upd本地绑定地址
	//! @param handler 该连接的事件处理函数指针
	//! @return 通道id, >0:成功, 0失败
	uint32_t create_udp(const INET_Addr& local_addr, pfnNetEventHandler handler);

	//! 创建一个upd通道
	//! @param local_ip upd本地绑定ip
	//! @param local_port upd本地绑定port
	//! @param handler 该连接的事件处理函数指针
	//! @return 通道id, >0:成功, 0失败
	uint32_t create_udp(const char *local_ip, int local_port, pfnNetEventHandler handler);

	//! 创建一个tcp client通道
	//! @param remote_addr tcp对端地址
	//! @param packet_splitter tcp拆包器
	//! @param handler 该连接的事件处理函数指针
	//! @param timeout 连接超时时间(秒)
	//! @return 通道id, >0:成功, 0失败
	uint32_t create_tcp_client(const INET_Addr& remote_addr, Packet_Splitter *packet_splitter, 
	                pfnNetEventHandler handler, int timeout,
	                size_t recv_buff_len = RECV_BUFFER_LENGTH);

	//! 创建一个tcp client通道
	//! @param remote_ip 对端ip
	//! @param remote_port 对端port
	//! @param packet_splitter tcp拆包器
	//! @param handler 该连接的事件处理函数指针
	//! @param timeout 连接超时时间(秒)
	//! @return 通道id, >0:成功, 0失败
	uint32_t create_tcp_client(const char *remote_ip, int remote_port, Packet_Splitter *packet_splitter, 
	                pfnNetEventHandler handler, int timeout, size_t recv_buff_len = RECV_BUFFER_LENGTH);

	//! 创建一个tcp server通道
	//! @param local_addr tcp本地监听地址
	//! @param packet_splitter tcp拆包器
	//! @param accpet_handler 该监听端口accpet的事件处理函数指针
	//! @param handler 该连接的事件处理函数指针
	//! @return 通道id, >0:成功, 0失败
	uint32_t create_tcp_server(const INET_Addr& local_addr, Packet_Splitter *packet_splitter, pfnNetEventHandler accept_handler, 
	            pfnNetEventHandler handler, size_t recv_buff_len = RECV_BUFFER_LENGTH);

	//! 创建一个tcp server通道
	//! @param local_ip tcp本地监听ip
	//! @param local_port tcp本地监听port
	//! @param packet_splitter tcp拆包器
	//! @param accpet_handler 该监听端口accpet的事件处理函数指针
	//! @param handler 该连接的事件处理函数指针
	//! @return 通道id, >0:成功, 0失败
	uint32_t create_tcp_server(const char *local_ip, int local_port, Packet_Splitter *packet_splitter, 
	                pfnNetEventHandler accept_handler, pfnNetEventHandler handler, 
	                        size_t recv_buff_len = RECV_BUFFER_LENGTH);

	//! 通知删除网络通道
	//! @param id 通道id
	//! @return 0:成功 <0:失败
	int delete_net(uint32_t id);

	//! 发送数据包
	//! @warning 如果此调用成功, packet类由网络组件负责释放; 如果此调用失败, packet类由调用方负责释放
	//! @param id 通道id
	//! @param packet 数据包
	//! @param remote_addr 对端地址(udp需要使用此地址, tcp忽略此地址)
	//! @return 发送结果, 0:成功, -1:失败(通道不存在), -2:失败(缓冲区满)
	int send_packet(uint32_t id, Net_Packet* packet, const INET_Addr& remote_addr);

	//! 发送数据包
	//! @warning 如果此调用成功, packet类由网络组件负责释放; 如果此调用失败, packet类由调用方负责释放
	//! @param id 通道id
	//! @param packet 数据包
	//! @param remote_ip 对端ip
	//! @param remote_port 对端port
	//! @return 发送结果, 0:成功, -1:失败(通道不存在), -2:失败(缓冲区满)
	int send_packet(uint32_t id, Net_Packet* packet, const char *remote_ip, int remote_port);

	//! 发送数据包(只能用于tcp)
	//! @warning 如果此调用成功, packet类由网络组件负责释放; 如果此调用失败, packet类由调用方负责释放
	//! @param id 通道id
	//! @param packet 数据包
	//! @return 发送结果, 0:成功, -1:失败(通道不存在), -2:失败(缓冲区满)
	int send_packet(uint32_t id, Net_Packet* packet);

	//! 取网络事件
	//! @warning 客户端应循环调用此函数获取网络事件, 并在处理完成每个网络事件后将其释放
	//! @return 网络事件, 没有网络事件时返回NULL
	Net_Event* get_event();

	//! 获取未处理的发送事件的数量
	//! @return 发送事件的数量
	int get_send_size();

	//! 获取未处理的接收事件的数量
	//! @return 接收事件的数量
	int get_recv_size();

	int get_net_event_size();
	int set_water(float water1_p, float water2_p, float water3_p);

	float l1_water() {return lev1_water_percent;}
	float l2_water() {return lev2_water_percent;}
	float l3_water() {return lev3_water_percent;}

	int register_cmd_level(uint32_t cmd, uint32_t lev);
	int level(uint32_t cmd);

public:
	friend class Net_Thread;
	friend class SOCK_Dgram;
	friend class SOCK_Acceptor;
	friend class SOCK_Connector;
	friend class SOCK_Stream;

private:
	//! 放入一个网络事件
	//! @param net_event 网络事件
	void put_event(Net_Event* net_event);

	//! 取发送任务
	//! @return 发送任务, 没有发送任务时返回NULL
	Send_Task* get_send_task();

	//! 网络故障(reactor故障)
	void on_net_error();

private:
	//! 运行状态 0:停止, 1:运行, 2:异常
	int m_status;

	//! 网络管理器线程
	Net_Thread* m_net_thread;


	StatusManager* m_net_statusManager;

	//! 通道id管理器
	Id_Manager m_id_manager;

	//! 反应器
	Net_Reactor m_reactor;

	//! 网络事件队列
	Net_Event_Queue *m_net_event_queue;

	//! 待发送任务队列
	Net_Send_Task_Queue *m_send_task_queue;

	// 压力过大时丢包水位比例
	// lev3_water_percent <= lev2_water_percent <= lev1_water_percent
	float lev1_water_percent; // 在处理网络事件时控制
	float lev2_water_percent; // 在处理网络事件时控制
	float lev3_water_percent; // 在分包时控制

	map<uint32_t, uint32_t> cmd_level_map; // cmd -> level
};


#endif // _NET_MANAGER_H_
