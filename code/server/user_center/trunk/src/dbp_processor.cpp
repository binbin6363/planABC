/**
* file: dbp_processor.cpp
* desc: ban liao, dbp_processor file. process cmd
* auth: bbwang
* date: 2015/3/3
*/
#include "dbp_processor.h"
#include "client_msg.h"
#include "protocols.h"
#include "log.h"

using namespace common;


DbpProcessor *DbpProcessor::Instance()
{
    static DbpProcessor inst;
    return &inst;
}

// handle request event from dbp server
int DbpProcessor::ProcessData(const Param &param, Msg *msg) const
{
    switch (msg->GetCmd())
    {
        case CMD_KEEPALIVE:
        {
            LOG(INFO)("[beat] dbp service is alive. [%s:%s, handle:%u]"
                , param.service_name, FromAddrTostring(param.remote_addr).c_str(), param.net_id);
        }
        break;
        default:
            LOG(ERROR)("unsupported cmd:%u", msg->GetCmd());
        break;
    }

    return 0;
}


