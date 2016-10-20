/**
 * @filedesc: 
 * processor.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/2/14 10:02:41
 * @modify:
 *
**/
#include "processor.h"

#include "msg.h"
#include "net_handle_manager.h"
#include "libco_routine_manager.h"
#include "session_manager.h"
#include "comm.h"


LibcoNetSession *Processor::GetSessionByNetid(uint32_t netid) const
{
    Net_Handler *handle = Net_Handle_Manager::Instance()->GetHandle(netid);
    LibcoNetSession *session = dynamic_cast<LibcoNetSession *>(handle);
    return session;
}

void Processor::KeepAlive(const Param &param, Msg *) const
{
    // 此处无需修改!!!!!
    LOG(INFO)("Processor. service is alive. name:%s, net id:%u, remote:%s"
        , param.service_name, param.net_id, FromAddrTostring(param.remote_addr).c_str());
}

LibcoNetSession *Processor::GetSession(const std::string &service_name, Msg &msg, int strategy) const
{
    LibcoNetSession *session = NULL;
    session = SessionManager::Instance().GetSession(service_name, msg, strategy);
    return session;
}

int Processor::GetResult(const std::string &service_name, Msg &request, Msg *&result, int strategy) const
{
    int ret = 0;
    // make sure result must not be null!!!
    result = SystemErrorMsg::Instance();
    result->set_err_msg(string("unknown error!"));

    LibcoNetSession *session = GetSession(service_name, request, strategy);
    if (NULL == session) 
    {
        ret = SESSION_NOT_FOUND_ERROR;
        // singleton, no need delete
        result = SessionNotFoundErrorMsg::Instance();
        LOG(ERROR)("%s not exist. GetResult failed, err code:%d.", service_name.c_str(), ret);
        return ret;
    }
    
    // 1. 发送请求
    ret = session->SendRequest(request);
	if (0 != ret)
	{
		LOG(WARN)("send request failed, err code:%d", ret);
        // singleton, no need delete
        result = SystemErrorMsg::Instance();
        result->set_err_msg(string("send request error!"));
		return ret;
	}

    // 2. 挂起
    uint32_t coid = CoroutineMgr::Instance().GetCurCoId();
    LOG(INFO)("[yield] send cmd:%u, msg:%s, task id:%u", request.GetCmd(), request.GetTypeName().c_str(), coid);
    if (0 == coid)
    {
        LOG(ERROR)("not in coroutine, can not yeild!");
        // singleton, not to delete, because not in system msg list
        result = SystemErrorMsg::Instance();
        result->set_err_msg(string("not in coroutine error!"));
        return NOT_COROUTINE_ERROR;
    }
    CoroutineMgr::Instance().CoroutineYieldCt();

    // 3. 唤醒之后从当前协程获取数据
    uint32_t cur_coid = CoroutineMgr::Instance().GetCurCoId();
    LOG(DEBUG)("[resume] current task id:%u, done GetResult.", cur_coid);
    result = CoroutineMgr::Instance().GetCurMsg();
    // 4. 对收到的消息做校验，空消息/系统出错/超时/错乱
    // check, result must not be null
    if (NULL == result)
    {
        LOG(ERROR)("resume, but msg is null.");
        // singleton, not to delete, because not in system msg list
        result = SystemErrorMsg::Instance();
        result->set_err_msg(string("unknown error!"));
        return UNKNOWN_ERROR;
    }
    else if (result->IsSysError())
    {
        LOG(ERROR)("[frame] msg create failed.");
        return MSG_CREATE_ERROR;
    }
    else if (result->IsTimeout())
    {
        LOG(ERROR)("[frame] msg timeout. task id:%u", result->receiver_coid);
		return TIME_OUT_ERROR;
    }
    else if (result->receiver_coid != cur_coid)
    {
        LOG(ERROR)("[frame] coroutine id not match. task id:%u", result->receiver_coid);
		return COID_ERROR;
    }
    LOG(INFO)("[resume] recv cmd:%u, msg:%s, task id:%u", result->GetCmd(), result->GetTypeName().c_str(), cur_coid);
    return ret;
}

int Processor::SendData(const std::string &service_name, Msg &msg, int strategy) const
{
    LibcoNetSession *session = GetSession(service_name, msg, strategy);
    if (NULL == session)
    {
        LOG(WARN)("session is null.");
        return -1;
    }
    return session->SendRequest(msg);
}

int Processor::Reply(const Param &param, const Msg &request, Msg &result) const
{
    uint32_t netid = param.net_id;
    LibcoNetSession *session = GetSessionByNetid(netid);
    if (NULL == session)
    {
        LOG(ERROR)("get session failed. netid:%u", netid);
        return -1;
    }
    return session->SendResponse(request, result);
}


int Processor::ProcessData(const Param &, Msg *) const 
{
    LOG(WARN)("call base class function ProcessData.");
    return -1;
}

int Processor::ProcessConnect(const Param &param) const
{
    uint32_t netid = param.net_id;
    Net_Handler *handle = Net_Handle_Manager::Instance()->GetHandle(netid);
    if (NULL == handle)
    {
        LOG(ERROR)("get handle failed. netid:%u", netid);
        return -1;
    }
    const INET_Addr &remote_addr = param.remote_addr;
    handle->handle_connect(netid, remote_addr);
	LOG(INFO)("open connect succeed. netid:%u", netid);
    return 0;
}

int Processor::ProcessAccept(const Param &param) const
{
    uint32_t netid = param.net_id;
    LibcoNetSession *session = GetSessionByNetid(netid);
    if (NULL == session)
    {
        LOG(ERROR)("cast session failed. netid:%u", netid);
        return -1;
    }
    session->handle_accept(netid, param.remote_addr);
    return 0;
}

int Processor::ProcessClose(const Param &param) const
{
    uint32_t netid = param.net_id;
    LibcoNetSession *session = GetSessionByNetid(netid);
    if (NULL == session)
    {
        LOG(ERROR)("cast session failed. netid:%u", netid);
        return -1;
    }
    session->handle_close(netid);
    return 0;
}


Processor::Processor()
{
    LOG(DEBUG)("Processor");
}

Processor::~Processor()
{
    LOG(DEBUG)("~Processor");
}


