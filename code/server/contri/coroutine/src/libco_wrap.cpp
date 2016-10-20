/**
 * @filedesc: 
 * libco_wrap.cpp
 * @author: 
 *  bbwang
 * @date: 
 *  2015/1/6 16:02:41
 * @modify:
 *
**/
#include <stdlib.h>
#include "libco_wrap.h"
#include "timecounter.h"
#include "session_manager.h"

// 框架的id,取8位吧
uint16_t libco_src::service_id = 0;

uint16_t make_default_random_id()
{
    struct timeval start;
    gettimeofday( &start, NULL );
    // 种子组成:取秒的时间低16作为种子的高16位，取微秒的时间低16作为种子的低16位
    uint32_t seed = (((start.tv_sec & 0xFFFF) << 16) | (start.tv_usec & 0xFFFF));
    
    srand(seed);
    uint16_t rand_id = rand() & 0xFF;
    
    return rand_id;
}

int set_service_id(uint16_t serviceid)
{
    if (0 == serviceid) 
    {
        LOG(WARN)("[frame] set service id, id can not be 0! use make_default_random_id");
        serviceid = make_default_random_id();
    }
    libco_src::service_id = serviceid;
    LOG(INFO)("[frame] set frame id:%u, ok, used for generate coroutine id.", serviceid);
    return 0;
}

int set_max_co_num(uint32_t max_co)
{
    if (0 == max_co) 
    {
        LOG(INFO)("[frame] set max co num, use default value:%u", max_co_num);
    }
    else
    {
        max_co_num = max_co;
        LOG(INFO)("[frame] set max co num:%u.", max_co_num);
    }
    return 0;
}


// 对外提供的接口
int co_run_service(Net_Manager *net_manager)
{
    if (!net_manager)
    {
        LOG(WARN)("[frame] app is not initialized.");
        return -1;
    }

    // 如果外界没有设置框架id，此处默认设置
    // 该id用于生成coroutine id，防止与其他框架产生的id冲突
    if (0 == libco_src::service_id)
    {
        set_service_id(make_default_random_id());
    }
    SessionManager::Instance().BindId(libco_src::service_id);

    while(1)
    {
//        TimeCounterAssistant tc("once run_service cost time");
        TIME_SPEC nowTime = gettimeofday();
        TimeoutManager::Instance()->UpdateTimeout(nowTime);
        Net_Event*  ev = net_manager->get_event();
        if (ev)
        {
            LOG(DEBUG)("[frame] receive data, handle ...");
            CoroutineMgr::Instance().Run((void *)ev);
            CoroutineMgr::Instance().CheckDelCurTask();
            LOG(DEBUG)("[frame] done handle data.");
        }
        else
        {
            usleep(10);
        }
    }
}



