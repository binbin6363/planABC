/**
 * @filedesc: 
 * 
 * @author: 
 *  bbwang
 * @date: 
 *  2016/8/4 20:02:59
 * @modify:
 *
**/
#include "base_app_processor.h"
#include "base_error.h"

BaseProcessor::BaseProcessor()
{

}

BaseProcessor::~BaseProcessor()
{

}

// default implements
int BaseProcessor::OnRecvRequest(const Param &param, char *data, int len)
{
    int net_id = param.net_id;
    int task_id = param.task_id;
    Session *session = GetSession(net_id);
    if (NULL == session)
    {
        return ERR_SESSION_NOT_EXIST;
    }
    // ��Ϊÿ�����󣬶�Ӧ���и�Э����֮��Ӧ��û�������
    Task *task = new Task(task_id);
    if (NULL == session)
    {
        return ERR_TASK_NOT_EXIST;
    }

    Translater *translater = param.translater;
    if (NULL == translater)
    {
        return ERR_TRANSLATER_NOT_EXIST;
    }

    Msg *request = translater->Translate(data, len);
    if (NULL == request)
    {
        return ERR_CAN_NOT_TRANSLATER_MSG;
    }

    int cmd = request->GetCmd();
    switch(cmd)
    {
        // ϵͳ����ڿ�ܴ���
        // 
        case -1:
            return 0;
    }

    return 0;
}


