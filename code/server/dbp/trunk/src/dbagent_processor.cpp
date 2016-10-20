/**
* file: dbagent_processor.cpp
* desc: dbagent_processor file. process cmd
* auth: bbwang
* date: 2015/11/3
*/
#include "dbagent_processor.h"
#include "client_msg.h"
#include "protocols.h"
#include "log.h"

using namespace common;


DbagentProcessor *DbagentProcessor::Instance()
{
    static DbagentProcessor inst;
    return &inst;
}

// handle request event from dbp server
int DbagentProcessor::ProcessData(const Param &param, Msg *msg) const
{
    LOG(INFO)("DbagentProcessor received data. [name:%s, remote:%s, netid:%u]"
        , param.service_name, FromAddrTostring(param.remote_addr).c_str(), param.net_id);
    switch (msg->GetCmd())
    {
        default:
            LOG(ERROR)("unsupported cmd:%u", msg->GetCmd());
        break;
    }

    return 0;
}


