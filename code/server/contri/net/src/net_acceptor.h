/********************************************************************
	created:	2013/06/09
	filename: 	NetAcceptor.h
	file base:	NetAcceptor
	file ext:	h
	author:		pengshuai
	
	purpose:	创建监听端口，实例化新建session
*********************************************************************/
#ifndef NetAcceptor_h__
#define NetAcceptor_h__
#include "net_handler.h"
#include "net_manager.h"
#include "net_session.h"
#include "net_handle_manager.h"
#include "log.h"
extern Net_Manager* g_netManager;

class Call_Back
{
public:
	static void NetAcceptHandler(Net_Event& ev);
	static void NetEventHandler(Net_Event& ev);
};

/************************************************************************

example:
class session : public Net_Session
{
	virtual int on_receive_message(char *ptr, int len)
	{
		log("%s, %d", ptr, len);
	}

	virtual int handle_close(uint32_t handle)
	{
		log("close.");
	}
}

Net_Acceptor<session> session_acc;
INET_Addr addr;
Packet_Splitter packet_splitter;
session_acc.open(addr, &packet_splitter);

************************************************************************/

template <class SVC_HANDLER, Session_Type TYPE=TCP_SESSION>
class Net_Acceptor : public Net_Handler
{
public:
	typedef SVC_HANDLER     handler_type;

	Net_Acceptor(void);
	virtual ~Net_Acceptor(void);
	int open(const INET_Addr &addr, Packet_Splitter *packet_splitter, size_t recv_buff_len = 20480);

	 //************************************
	 // Method:    make_svc_handler
	 // FullName:  NetAcceptor<SVC_HANDLER>::make_svc_handler
	 // Access:    virtual public 
	 // Returns:   int
	 // Qualifier: 实例化新建的连接对象
	 // Parameter: SVC_HANDLER * & sh
	 //************************************
	 virtual int make_svc_handler (SVC_HANDLER *&sh);

	 //************************************
	 // Method:    activate_svc_handler
	 // FullName:  NetAcceptor<SVC_HANDLER, TYPE>::activate_svc_handler
	 // Access:    virtual public 
	 // Returns:   int (0成功，其他失败)
	 // Qualifier: 初始化新建session数据
	 // Parameter: SVC_HANDLER * svc_handler
	 // Parameter: const INET_Addr &remote_addr
	 //************************************
	 virtual int activate_svc_handler (SVC_HANDLER *svc_handler, const INET_Addr &remote_addr);

private:
	virtual int handle_accept(uint32_t new_handle, const INET_Addr &remote_addr );
	
};

template <class SVC_HANDLER, Session_Type TYPE>
Net_Acceptor<SVC_HANDLER, TYPE>::Net_Acceptor(void)
{
}

template <class SVC_HANDLER, Session_Type TYPE>
Net_Acceptor<SVC_HANDLER, TYPE>::~Net_Acceptor(void)
{
}

template <class SVC_HANDLER, Session_Type TYPE>
int Net_Acceptor<SVC_HANDLER, TYPE>::open( const INET_Addr &addr, Packet_Splitter *packet_splitter, size_t recv_buff_len)
{
	if (TYPE == TCP_SESSION)
	{
		uint32_t handle = net_manager()->create_tcp_server(addr, packet_splitter, Call_Back::NetAcceptHandler, 
		            Call_Back::NetEventHandler, recv_buff_len);
		if (handle == 0)
		{
			//LOG(INFO)("create tcp server failed.");
			return -1;
		}

		if (Net_Handle_Manager::Instance()->AddHandle(handle, this) != 0)
		{
			LOG(ERROR)("Net_Acceptor::open add handle falied.");
			net_manager()->delete_net(handle);
			return -1;
		}
		LOG(INFO)("Net_Acceptor::open  open succ recv_buff_len:%zu", recv_buff_len);
	}
	else
	{
		uint32_t handle = net_manager()->create_udp(addr, Call_Back::NetEventHandler);
		if (handle == 0)
		{
			//LOG(INFO)("create udp failed.");
			return -1;
		}

		handle_accept(handle, addr);//模拟tcp创建session过程
	}

	return 0;
}


template <class SVC_HANDLER, Session_Type TYPE>
int Net_Acceptor<SVC_HANDLER, TYPE>::make_svc_handler( SVC_HANDLER *&sh )
{
	if (sh == NULL)
	{
		sh = new SVC_HANDLER();
	}

	return 0;
}

template <class SVC_HANDLER, Session_Type TYPE>
int Net_Acceptor<SVC_HANDLER, TYPE>::activate_svc_handler( SVC_HANDLER *svc_handler, const INET_Addr &remote_addr )
{
	if (svc_handler && svc_handler->open(this, remote_addr) != 0)
	{
		return -1;
	}

	return 0;
}

template <class SVC_HANDLER, Session_Type TYPE>
int Net_Acceptor<SVC_HANDLER, TYPE>::handle_accept( uint32_t new_handle, const INET_Addr &remote_addr )
{
	LOG(INFO)("NetAcceptor::handle_accept, new_net_id:%d", new_handle);
	SVC_HANDLER *handle = NULL;
	if (make_svc_handler(handle) != 0)
	{
		net_manager()->delete_net(new_handle);
		LOG(INFO)("NetAcceptor::handle_accept, make handler return failed");
		return -1;
	}

	handle->handle(new_handle);
	handle->net_manager(net_manager());
	handle->session_type(TYPE);
	if (Net_Handle_Manager::Instance()->AddHandle(new_handle, handle) != 0)
	{
		LOG(ERROR)("Net_Acceptor::handle_accept add handle falied.");
		net_manager()->delete_net(new_handle);
		return -1;
	}

	if (activate_svc_handler(handle, remote_addr) != 0)
	{
		net_manager()->delete_net(new_handle);
		LOG(INFO)("NetAcceptor::handle_accept, accept handler return failed");
		return -1;
	}

	return 0;
}

#endif // NetAcceptor_h__
