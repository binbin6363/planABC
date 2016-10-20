#include "net_handle_manager.h"

Net_Handle_Manager::Net_Handle_Manager(void)
{
}

Net_Handle_Manager::~Net_Handle_Manager(void)
{
}

int Net_Handle_Manager::AddHandle( uint32_t netid, Net_Handler *handle )
{
	if (!handle)
	{
		return -1;
	}

	std::map<uint32_t, Net_Handler*>::iterator it = map_handle_.find(netid);
	if (it != map_handle_.end())
	{
		return -1;
	}

	map_handle_.insert(make_pair(netid, handle));
	return 0;
}

void Net_Handle_Manager::RemoveHandle( uint32_t netid )
{
	map_handle_.erase(netid);
}

Net_Handler * Net_Handle_Manager::GetHandle( uint32_t netid )
{
	std::map<uint32_t, Net_Handler*>::iterator it = map_handle_.find(netid);
	if (it != map_handle_.end())
	{
		return it->second;
	}

	return NULL;
}
