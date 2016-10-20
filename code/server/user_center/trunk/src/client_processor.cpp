/**
* file: client_processor.h
* desc: ban liao, client_processor file. process cmd
* auth: bbwang
* date: 2015/3/3
*/

#include "client_processor.h"
#include "cmd.h"
//#include "client_msg.h"
#include "protocols.h"
#include "log.h"

using namespace common;

ClientProcessor *ClientProcessor::Instance()
{
    static ClientProcessor inst;
    return &inst;
}


// handle request event from client
int ClientProcessor::ProcessData(const Param &param, Msg *msg) const
{
    LOG(INFO)("ClientProcessor received data. [name:%s, remote:%s, netid:%u]"
        , param.service_name, FromAddrTostring(param.remote_addr).c_str(), param.net_id);
    switch (msg->GetCmd())
    {
        case CMD_LOGIN:
        {
            LoginCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_USER_KEEPALIVE:    // 100, time
        {
            UserKeepAliveCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_WITHDRAW:    // 100, time
        {
            WithdrawCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        default:
            LOG(ERROR)("unsupported cmd:%u", msg->GetCmd());
        break;
    }

    return 0;
}

// 客户端上来的心跳请求
void ClientProcessor::KeepAlive(const Param &param, Msg *msg) const
{
    // 此处无需修改!!!!!
    BeatCmd cmd(this, param, msg);
    cmd.Execute();
}


