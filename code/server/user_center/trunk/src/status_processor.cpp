/**
* file: status_processor.cpp
* desc: ban liao, status_processor file. process cmd
* auth: bbwang
* date: 2015/4/21
*/
#include "status_processor.h"
#include "client_msg.h"
#include "protocols.h"
#include "log.h"

using namespace common;


StatusProcessor *StatusProcessor::Instance()
{
    static StatusProcessor inst;
    return &inst;
}

// handle request event from dbp server
int StatusProcessor::ProcessData(const Param &param, Msg *msg) const
{
    switch (msg->GetCmd())
    {
        case CMD_KEEPALIVE:
        {
            LOG(INFO)("[beat] status service is alive. [%s:%s, handle:%u]"
                , param.service_name, FromAddrTostring(param.remote_addr).c_str(), param.net_id);
        }
        break;
        default:
            LOG(ERROR)("unsupported cmd:%u", msg->GetCmd());
        break;
    }

    return 0;
}

int StatusProcessor::ProcessClose(const Param &param) const
{
    int ret = 0;
    LOG(DEBUG)("StatusProcessor ProcessClose");
    return ret;
}



