/**
 * @filedesc: 
 * 
 * @author: 
 *  bbwang
 * @date: 
 *  2016/8/4 20:02:59
 * @modify:
 *
**/

#ifndef _NET_ENGINE_H_
#define _NET_ENGINE_H_

class PriorityList;
//class BaseProcessor;
class Event;

class NetEngine
{
public:
    NetEngine();
    ~NetEngine();

    int Start();
    int Stop();

	Event *GetEvent();

private:
	PriorityList      *event_list_;
	bool              stop_service_;
	// 每个服务只有一个处理器
    //BaseProcessor     *processor_;
};

#endif //_NET_ENGINE_H_
