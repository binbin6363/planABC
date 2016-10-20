/**
* file: push_processor.cpp
* desc: ban liao, push_processor file. process cmd
* auth: bbwang
* date: 2015/3/3
*/
#include "push_processor.h"
#include "client_msg.h"
#include "protocols.h"
#include "log.h"

using namespace common;


PushProcessor *PushProcessor::Instance()
{
    static PushProcessor inst;
    return &inst;
}

// handle request event from push server
int PushProcessor::ProcessData(const Param &param, Msg *msg) const
{
    switch (msg->GetCmd())
    {
        case CMD_KEEPALIVE:
        {
            LOG(INFO)("[beat] push service is alive. [%s:%s, handle:%u]"
                , param.service_name, FromAddrTostring(param.remote_addr).c_str(), param.net_id);
        }
        break;
        default:
            LOG(ERROR)("unsupported cmd:%u", msg->GetCmd());
        break;
    }

    return 0;
}


