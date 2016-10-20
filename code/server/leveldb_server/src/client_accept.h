/********************************************************************
	created:	2013/06/13
	filename: 	client_accept.h
	file base:	client_accept
	file ext:	h
	author:		pengshuai
	
	purpose:	负责接入客户端需要发送消息的服务，并做过滤等工作
*********************************************************************/
#ifndef msg_accept_h__
#define msg_accept_h__
#include "net_acceptor.h"
#include "client_session.h"
#include "serverconfig.h"

class ClientAccept :
	public Net_Acceptor<ClientSession, TCP_SESSION>
{
	typedef Net_Acceptor<ClientSession, TCP_SESSION> super;
public:
	ClientAccept(void);
	virtual ~ClientAccept(void);

	virtual int make_svc_handler(ClientSession *&sh);
	virtual int activate_svc_handler(ClientSession *svc_handler, const INET_Addr &remote_addr);
};

#endif // msg_accept_h__

