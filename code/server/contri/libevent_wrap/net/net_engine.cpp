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
#include "net_engine.h"
#include "base_error.h"


NetEngine::NetEngine()
{
}

NetEngine::~NetEngine()
{
}


int NetEngine::Start()
{
    log_info("start service ...");
    while (!stop_service_)
    {
        Event *ev = GetEvent();
        if (NULL == ev)
        {
            usleep(10);
            continue;
        }
        Param param;
        param.net_id = ev->net_id;
//        param.task_id = 
        // 处理一个完整的任务
        BaseProcessor *processor = ev->processor;
        if (NULL == processor)
        {
            return ERR_NO_PROCESSOR;
        }
        processor->OnRecvRequest(param, data, len);
    }
    log_info("end service ...");
    return 0;
}

int NetEngine::Stop()
{
    stop_service_ = true;
}



