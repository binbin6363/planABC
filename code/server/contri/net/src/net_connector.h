/********************************************************************
	created:	2013/06/13
	filename: 	Net_Connector.h
	file base:	Net_Connector
	file ext:	h
	author:		pengshuai
	
	purpose:	连接器主动连接服务
*********************************************************************/
#ifndef Net_Connector_h__
#define Net_Connector_h__
#include "net_manager.h"
#include "net_acceptor.h"

template <class SVC_HANDLER>
class Net_Connector : public Net_Handler
{
public:
	Net_Connector(void);
	~Net_Connector(void);
	int open(Net_Manager *net_manager);

	//************************************
	// Method:    connect
	// FullName:  Net_Connector<SVC_HANDLER>::connect
	// Access:    public 
	// Returns:   int 0成功 -1失败
	// Qualifier:
	// Parameter: SVC_HANDLER * & sh 连接session对象
	// Parameter: const INET_Addr & remote_addr 对端地址
	// Parameter: Packet_Splitter * packet_splitter 解包器
	// Parameter: int time_out
	//************************************
	int connect(SVC_HANDLER *&sh, const INET_Addr &remote_addr, Packet_Splitter *packet_splitter, 
	                int time_out,  size_t recv_buff_len = 20480);

private:
	Net_Manager *net_manager_;
};


template <class SVC_HANDLER>
int Net_Connector<SVC_HANDLER>::connect( SVC_HANDLER *&sh, const INET_Addr &remote_addr, Packet_Splitter *packet_splitter
										, int time_out, size_t recv_buff_len)
{
	if (net_manager_ == NULL)
	{
		LOG(ERROR)("connector is not initialized.");
		return -1;
	}
       LOG(DEBUG)("Net_Connector connect recv_buff_len:%zu", recv_buff_len);
	uint32_t handle = net_manager_->create_tcp_client(remote_addr, packet_splitter, 
	            Call_Back::NetEventHandler, time_out, recv_buff_len);
	if (handle == 0)
	{
		LOG(ERROR)("create tcp client failed.");
		return -1;
	}

// 	if (sh == NULL)
// 	{
// 		//make_svc_handler(sh);
// 		sh = new SVC_HANDLER;
// 	}

	sh->session_type(TCP_SESSION);
	sh->handle(handle);
	sh->net_manager(net_manager_);

//	printf("client id:%u\n", handle);
	if (Net_Handle_Manager::Instance()->AddHandle(handle, sh) != 0)
	{
		LOG(ERROR)("Net_Connector::connect add handle falied.");
		net_manager_->delete_net(handle);
		return -1;
	}

	return 0;
}

template <class SVC_HANDLER>
int Net_Connector<SVC_HANDLER>::open( Net_Manager *net_manager )
{
	net_manager_ = net_manager;
	return 0;
}

template <class SVC_HANDLER>
Net_Connector<SVC_HANDLER>::~Net_Connector( void )
{

}

template <class SVC_HANDLER>
Net_Connector<SVC_HANDLER>::Net_Connector( void )
{
	net_manager_ = NULL;
}

#endif // Net_Connector_h__
