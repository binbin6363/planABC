
#include "net_manager.h"




// �¼�����
const char* event_name(NET_EVENT_TYPE event_type);


// ��ʾ�¼���Ϣ
void show_event(Net_Event* event);

void null_event(Net_Event& event);


// 2���ư����(��ǰ���ֽڱ�ʾ���ĳ���)
class Easy_Packet_Splitter : public Packet_Splitter
{
public:
	virtual ~Easy_Packet_Splitter();
	virtual int split(const char *buf, int len, int& packet_begin, int& packet_len);
};


