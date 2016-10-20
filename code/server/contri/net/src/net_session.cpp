#include "net_session.h"
#include "log.h"

Net_Session::Net_Session(void)
{
}

Net_Session::~Net_Session(void)
{
}

void Net_Session::session_type( Session_Type session_type )
{
	session_type_ = session_type;
}

int Net_Session::open( void *arg, const INET_Addr &remote_addr )
{
	(void)arg;
	remote_addr_ = remote_addr;
	return 0;
}

int Net_Session::on_receive_message( char *ptr, int len )
{
	(void)ptr;
	LOG(INFO)("Net_Session::on_receive_message");
	return 0;
}

int Net_Session::handle_message( Net_Packet &packet, const INET_Addr &addr)
{
	if (session_type_ != TCP_SESSION)//为udp保存对端地址
	{
		remote_addr(addr);
	}
	
	return on_receive_message(packet.ptr(), packet.length());
}

int Net_Session::send_msg( char *ptr, int len )
{
	if (net_manager() == NULL)
	{
		LOG(WARN)("send msg failed, not initialized.");
		return -1;
	}

	Net_Packet *packet = new Net_Packet(len);

	memcpy(packet->ptr(), ptr, len);
	packet->length(len);
	int ret = 0;
	if (session_type_ == TCP_SESSION)
	{
		ret = net_manager()->send_packet(handle(), packet);
	}
	else
	{
		ret = net_manager()->send_packet(handle(), packet, remote_addr());
	}

	if (ret == 0)
	{
		LOG(DEBUG)("send buf handle:%u, len:%d", handle(), len);
	}
	else//发送失败则删除packet
	{
		delete packet;
	}

	return ret;
}

char * Net_Session::get_buffer()
{
	return NULL;
}

int Net_Session::handle_connect( uint32_t new_handle, const INET_Addr &remote_addr )
{
	if (open(this, remote_addr) != 0)//调用open通知上层
	{
		this->close();
	}
	
	return 0;
}
