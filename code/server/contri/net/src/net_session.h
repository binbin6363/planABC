/********************************************************************
	created:	2013/06/9
	filename: 	net_session.h
	file base:	net_session
	file ext:	h
	author:		pengshuai
	
	purpose:	具体的连接处理类
*********************************************************************/
#ifndef Net_Session_h__
#define Net_Session_h__

#include "net_handler.h"

enum Session_Type
{
	TCP_SESSION,
	UDP_SESSION
};
class Net_Session :
	public Net_Handler
{
public:
	Net_Session(void);
	virtual ~Net_Session(void);

	virtual int open(void *arg, const INET_Addr &remote_addr);

	virtual int on_receive_message(char *ptr, int len);

	//************************************
	// Method:    get_buffer
	// FullName:  Net_Session::get_buffer
	// Access:    virtual public 
	// Returns:   char *
	// Qualifier: 子类可实现该函数以自己提供任意buff
	//************************************
	virtual char *get_buffer();//

	void session_type(Session_Type session_type);
	int send_msg(char *ptr, int len);

	inline const string &session_id(){return session_id_;};
	inline void set_session_id(string session_id_){this->session_id_ = session_id_;};
	inline const INET_Addr &remote_addr()const {return remote_addr_;}

protected:
	inline void remote_addr(const INET_Addr &remote_addr) {remote_addr_ = remote_addr;};

private:
	virtual int handle_connect(uint32_t new_handle, const INET_Addr &remote_addr);
	virtual int handle_message( Net_Packet &packet, const INET_Addr &addr);

private:
	
	INET_Addr remote_addr_;
	Session_Type session_type_;
	string session_id_;
};
#endif // Net_Session_h__
