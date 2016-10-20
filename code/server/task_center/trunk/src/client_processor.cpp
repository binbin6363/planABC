/**
* file: client_processor.h
* desc: client_processor file. process cmd
* auth: bbwang
* date: 2015/11/4
*/

#include "client_processor.h"
#include "cmd.h"
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
        case CMD_SYNC_DATA:
        {
            SyncTaskCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_REPORT_DATA:
        {
            ReportDataCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_SYNC_DATA_ACK:    // 2001, time
        {
            SyncDataAckCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_START_TASK:    // 4000
        {
            StartTaskCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;

        // taskmgr request
        case CMD_PUBLISH_TASK:
        {
            PublishTaskCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_QUERY_TASK_LIST:
        {
            QueryTaskListCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_QUERY_TASK_INFO:
        {
            QueryTaskInfoCmd cmd(this, param, msg);
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


