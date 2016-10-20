/**
* file: redis_proxy_processor.cpp
* desc: redis_proxy_processor file. process cmd
* auth: bbwang
* date: 2015/11/3
*/
#include "redis_proxy_processor.h"
#include "client_msg.h"
#include "protocols.h"
#include "log.h"

using namespace common;


RedisProxyProcessor *RedisProxyProcessor::Instance()
{
    static RedisProxyProcessor inst;
    return &inst;
}

// handle request event from redis proxy server
int RedisProxyProcessor::ProcessData(const Param &param, Msg *msg) const
{
    LOG(INFO)("RedisProxyProcessor received data. [name:%s, remote:%s, netid:%u]"
        , param.service_name, FromAddrTostring(param.remote_addr).c_str(), param.net_id);
    switch (msg->GetCmd())
    {
        default:
            LOG(ERROR)("unsupported cmd:%u", msg->GetCmd());
        break;
    }

    return 0;
}


