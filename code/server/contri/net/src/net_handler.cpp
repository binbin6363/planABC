#include "net_handler.h"
#include "net_handle_manager.h"
#include "log.h"

Net_Handler::Net_Handler(void)
{
	net_manager_ = NULL;
	handle_ = 0;
}

Net_Handler::~Net_Handler(void)
{
}

int Net_Handler::process_event( Net_Event &ev )
{
	switch ( ev.net_event_type )
	{
	case TYPE_DATA:
		{
			handle_message(ev.packet, ev.remote_addr);
		}break;	
	case TYPE_ACCEPT:
		{
			handle_accept(ev.new_id, ev.remote_addr);
		}
		break;
	case TYPE_CONNECT:
		{
			handle_connect(ev.id, ev.remote_addr);
		}
		break;
	case TYPE_EXCEPTION:
	case TYPE_SYS_ERROR:
	case TYPE_CLOSE:
	case TYPE_TIMEOUT:
		{
			handle_close(ev.id);
			Net_Handle_Manager::Instance()->RemoveHandle(ev.id);
		}break;
	default:
		{
			LOG(WARN)("process error, unknown net_event_type:%d", ev.net_event_type);
		}break;
	}

	return 0;
}

int Net_Handler::handle_accept( uint32_t new_id, const INET_Addr &remote_addr )
{
	(void)new_id;
	(void)remote_addr;
	LOG(INFO)("Net_Handler::handle_accept");
	return 0;
}

int Net_Handler::handle_close( uint32_t handle )
{
	(void)handle;
	LOG(INFO)("Net_Handler::handle_close");
	return 0;
}

int Net_Handler::handle_message( Net_Packet &packet, const INET_Addr &addr )
{
	(void)packet;
	(void)addr;
	LOG(INFO)("Net_Handler::handle_massage");
	return 0;
}

int Net_Handler::handle_connect( uint32_t new_handle, const INET_Addr &remote_addr )
{
	(void)new_handle;
	(void)remote_addr;
	LOG(INFO)("Net_Handler::handle_connect, handle:%u\n", new_handle);
	return 0;
}

int Net_Handler::close()
{
	net_manager()->delete_net(handle());
	LOG(INFO)("Net_Handler::close, close handle:%u", handle());
	return 0;
}

void Net_Handler::handle_timeout( int id,void *userData )
{
	(void)userData;
	LOG(INFO)("Net_Handler::handle_time_out, handle:%u, time_id:%d", handle(), id);
}

