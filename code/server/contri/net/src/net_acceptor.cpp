#include "net_acceptor.h"
#include "net_handle_manager.h"

void Call_Back::NetAcceptHandler(Net_Event& ev)
{
	Net_Handler *net_handle = Net_Handle_Manager::Instance()->GetHandle(ev.id);
	if (net_handle)
	{
		net_handle->process_event(ev);
	}
	else
	{
		LOG(WARN)("error net id, netid:%u.", ev.id);
	}
}

void Call_Back::NetEventHandler(Net_Event& ev)
{
	Net_Handler *net_handle = Net_Handle_Manager::Instance()->GetHandle(ev.id);
	LOG(INFO)("netid:%u, type:%u", ev.id, ev.net_event_type);
	if (net_handle)
	{
		net_handle->process_event(ev);
	}
	else
	{
		LOG(WARN)("error net id, netid:%u.", ev.id);
	}
}

