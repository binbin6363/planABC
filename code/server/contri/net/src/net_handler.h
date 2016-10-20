/********************************************************************
	created:	2013/06/9
	filename: 	net_handler.h
	file base:	net_handler
	file ext:	h
	author:		pengshuai
	
	purpose:	Net_Event解析和功能分发
*********************************************************************/
#ifndef Net_Handler_h__
#define Net_Handler_h__
#include "net_event.h"
#include "net_manager.h"
#include "timeoutmanager.h"

class Net_Handler : public TimeoutEvent
{
public:
	Net_Handler(void);
	virtual ~Net_Handler(void);
	virtual int process_event(Net_Event &ev);
	virtual int handle_accept(uint32_t new_handle, const INET_Addr &remote_addr );
	virtual int handle_connect(uint32_t new_handle, const INET_Addr &remote_addr);
	virtual int handle_close(uint32_t handle);
	virtual int handle_message( Net_Packet &packet, const INET_Addr &addr);

	virtual void handle_timeout(int id,void *userData);

	//************************************
	// Method:    close
	// FullName:  Net_Handler::close
	// Access:    public 
	// Returns:   int
	// Qualifier: 关闭当前连接
	//************************************
	virtual int	 close();

	inline void handle(uint32_t handle){handle_ = handle;};
	inline void net_manager(Net_Manager *net_manager){net_manager_ = net_manager;};
	inline Net_Manager *net_manager(){return net_manager_;};
	inline uint32_t handle(){return handle_;};

private:
	uint32_t handle_;
	Net_Manager *net_manager_;
};


#endif // Net_Handler_h__
