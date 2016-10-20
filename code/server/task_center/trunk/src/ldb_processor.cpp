/**
* file: ldb_processor.cpp
* desc: ldb_processor file. process cmd
* auth: bbwang
* date: 2015/11/4
*/
#include "ldb_processor.h"
#include "client_msg.h"
#include "protocols.h"
#include "log.h"
#include "constants.h"

using namespace common;


LdbProcessor *LdbProcessor::Instance()
{
    static LdbProcessor inst;
    return &inst;
}

// handle request event from ldb server
int LdbProcessor::ProcessData(const Param &param, Msg *msg) const
{
    (void)param;
    switch (msg->GetCmd())
    {
        case CMD_KEEPALIVE:
        {
            LOG(INFO)("[beat] ldb service is alive. [%s:%s]", param.service_name, FromAddrTostring(param.remote_addr).c_str());
        }
        break;
        default:
            LOG(ERROR)("unsupported cmd:%u", msg->GetCmd());
        break;
    }

    return 0;
}

int LdbProcessor::ProcessClose(const Param &param) const
{
    int ret = 0;
    LOG(DEBUG)("ServerProcessor ProcessClose");
    return ret;
}

