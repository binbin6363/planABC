//! @file epoll_reactor.h


#ifndef _EPOLL_REACTOR_H_
#define _EPOLL_REACTOR_H_


#include "reactor.h"
#include "cycle_buffer.h"
#include "thread_mutex.h"
#include <sys/epoll.h>
#include <map>

using namespace std;


//! @class Epoll_Reactor
//! @brief EPOLL版本的反应器
class Epoll_Reactor : public Reactor
{
public:
	Epoll_Reactor();
	virtual ~Epoll_Reactor();

public:
	//! 打开reactor
	//! @return 0:成功, <0:失败
	int open_reactor();

	//! 关闭reactor
	//! @return 0:成功, <0:失败
	int close_reactor();

	//! 事件主循环
	//! @return >=0:处理事件的个数, <0:反应器错误
	virtual int run_reactor_event_loop();

	//! 停止事件主循环并清理所有注册的处理器
	virtual int end_reactor_event_loop();

	//! 注册一个事件监听
	//! @param event_handler 事件处理器
	//! @param masks 要监听的事件
	//! @return 0:成功, <0:失败
	virtual int register_handler(Event_Handler *event_handler, uint32_t masks);

	//! 删除一个事件监听
	//! @param event_handler 事件处理器
	//! @param masks 要删除的事件
	//! @return 0:成功, <0:失败
	virtual int remove_handler(Event_Handler *event_handler, uint32_t masks);

	//! 删除一个sock上的所有监听
	//! @param event_handler 事件处理器
	//! @return 0:成功, <0:失败
	virtual int delete_handler(Event_Handler *event_handler);

	//! 设置超时
	//! @param id 网络通道
	//! @param timeout 超时时间(秒)
	//! @return 0:成功, <0:失败
	virtual int set_timeout(uint32_t id, int timeout);

	//! 取消超时
	//! @param id 网络通道
	//! @return 0:成功, <0:失败
	virtual int cancel_timeout(uint32_t id);

	//! 通知关闭
	//! @param id 通道id
	//! @return 0:成功, <0:失败
	virtual int notify_close(uint32_t id);

	//! 取通道事件处理器
	//! @param id 通道id
	//! @return 事件处理器
	virtual Event_Handler* get_event_handler(uint32_t id);

private:
	//! 最大句柄数(20万)
	static const int MAX_HANDLER = 200000;

	//! epoll描述符
	int m_epfd;

	//! epoll返回的事件
	epoll_event *m_events;


	//! 已注册通道信息
	class Event_Handler_Info
	{
	public:
		Event_Handler *event_handler;
		bool	read;
		bool	write;
		bool	notify_close;
		bool	have_check_send_task;
		time_t	timeout;		//<! 超时的时间, 0表示没有设置超时
	};

	//! 已注册通道信息map类型定义, [id -> Event_Handler_Info]
	typedef map<uint32_t, Event_Handler_Info> Event_Handler_Info_Map;

	//! 所有已注册通道信息map
	Event_Handler_Info_Map m_event_handler_info_map;

	//! 线程锁, 保护处理器map
	utils::Thread_Mutex m_map_mutex;
};


#endif // _EPOLL_REACTOR_H_
