#ifndef Net_Handle_Manager_h__
#define Net_Handle_Manager_h__
#include "net_handler.h"
#include <map>
using namespace std;

class Net_Handle_Manager
{
public:
	Net_Handle_Manager(void);
	~Net_Handle_Manager(void);

	static Net_Handle_Manager* Instance()
	{
		static  Net_Handle_Manager instance;
		return &instance;
	}

	int AddHandle(uint32_t netid, Net_Handler *handle);
	Net_Handler *GetHandle(uint32_t netid);
	void RemoveHandle(uint32_t netid);
	
private:
	std::map<uint32_t, Net_Handler*> map_handle_;
};
#endif // Net_Manager_h__


