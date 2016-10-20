
#include "net_manager.h"




// 事件名称
const char* event_name(NET_EVENT_TYPE event_type);


// 显示事件信息
void show_event(Net_Event* event);

void null_event(Net_Event& event);


// 2进制包拆包(包前两字节表示包的长度)
class Easy_Packet_Splitter : public Packet_Splitter
{
public:
	virtual ~Easy_Packet_Splitter();
	virtual int split(const char *buf, int len, int& packet_begin, int& packet_len);
};


