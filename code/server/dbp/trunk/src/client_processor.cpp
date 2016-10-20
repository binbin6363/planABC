/**
* file: client_processor.h
* desc: client_processor file. process cmd
* auth: bbwang
* date: 2015/11/3
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
        case CMD_INNER_CHECK_USER:
        {
            CheckUserCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_INNER_INSERT_USER:
        {
            InsertUserCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_SYNC_DATA:
        {
            SyncTaskCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_INNER_GET_INCOME:
        {
            GetIncomeCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        // ��mysql�ĸ��������б��в���
        case CMD_SAVE_TASK_INFO:
        {
            SaveTaskCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_THIRD_PARTY_LOGIN:
        {
            ThirdPartyLoginCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_INNER_CHECK_TASK:
        {
            CheckTaskValid cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_INNER_WRITE_TASK:
        {
            WriteTaskCmd cmd(this, param, msg);
            cmd.Execute();
        }
        case CMD_INNER_CLOSE_TASK:
        {
            CloseTaskCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_INNER_REPORT_TASK_INFO:
        {
            ReportDataCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        case CMD_WITHDRAW:
        {
            WithdrawCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;

        // ��������id��ȡ������Ϣ
        case CMD_QUERY_TASK_INFO:
        {
            QueryTaskInfoCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        /*
        ������ע��
        case CMD_INNER_REWARD_INFO:
        {
            RewardUserCmd cmd(this, param, msg);
            cmd.Execute();
        }
        break;
        */

        default:
            LOG(ERROR)("unsupported cmd:%u", msg->GetCmd());
        break;
    }

    return 0;
}


// �ͻ�����������������
void ClientProcessor::KeepAlive(const Param &param, Msg *msg) const
{
    // �˴������޸�!!!!!
    BeatCmd cmd(this, param, msg);
    cmd.Execute();
}


