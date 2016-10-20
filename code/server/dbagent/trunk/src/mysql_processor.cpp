/**
* file: dbp_processor.cpp
* desc: dbp_processor file. process cmd
* auth: bbwang
* date: 2015/11/3
*/
#include "mysql_processor.h"
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
    int ret = 0;
    switch (msg->GetCmd())
    {
        case CMD_KEEPALIVE:
        {
            BeatMsg *beat_msg = dynamic_cast<BeatMsg *>(msg);
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

int DbpProcessor::ProcessClose(const Param &param) const
{
    int ret = 0;
    LOG(DEBUG)("ServerProcessor ProcessClose");
    return ret;
}

