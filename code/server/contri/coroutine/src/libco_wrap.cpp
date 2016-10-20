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

// ��ܵ�id,ȡ8λ��
uint16_t libco_src::service_id = 0;

uint16_t make_default_random_id()
{
    struct timeval start;
    gettimeofday( &start, NULL );
    // �������:ȡ���ʱ���16��Ϊ���ӵĸ�16λ��ȡ΢���ʱ���16��Ϊ���ӵĵ�16λ
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


// �����ṩ�Ľӿ�
int co_run_service(Net_Manager *net_manager)
{
    if (!net_manager)
    {
        LOG(WARN)("[frame] app is not initialized.");
        return -1;
    }

    // ������û�����ÿ��id���˴�Ĭ������
    // ��id��������coroutine id����ֹ��������ܲ�����id��ͻ
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



