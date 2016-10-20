/**
 * @filedesc: 
 * server_session.cpp
 * @author: 
 *  bbwang
 * @date: 
 *  2015/2/26 11:18:41
 * @modify:
 *
**/

#include "server_session.h"
#include "session_manager.h"
#include "msg.h"
#include "option.h"

uint32_t ServerSession::myself_service_id_ = 0;

static uint32_t seq_inc = 0;
ServerSession::ServerSession()
: node_id_(0)
, id_(0)
, score_(0)
{
    LOG(INFO)("new server session.");
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(60), NULL);
}

ServerSession::~ServerSession()
{
    LOG(INFO)("destroy server session.");
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
}

// 对于服务端session，成功连接才能将session放入cluster中
int ServerSession::open(void *arg, const INET_Addr &remote_addr)
{
    LOG(INFO)("ServerSession::open, ip:%s, handle:%u.", FromAddrTostring(remote_addr).c_str(), handle());

    LibcoNetSession::open(arg, remote_addr);
    b_connected_ = true;
    keepalive();//连接成功则直接发送心跳

    SessionManager::Instance().AddServerSession(this);
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.heart_beat_interval), NULL);
    return 0;
}

int ServerSession::handle_close(uint32_t handle)
{
    LOG(ERROR)("ServerSession::handle_close, ip:%s, handle:%u, reconnect interval:%u."
              , FromAddrTostring(remote_addr()).c_str(), handle, serv_info_.reconnect_interval);
    b_connected_ = false;
    last_time_ = time(NULL);
    SetScore(0); // 异常关闭也得0分
    //先取消之前的定时器
    SessionManager::Instance().RemoveServerSession(this);
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
    return 0;
}

void ServerSession::handle_timeout(int ,void *)
{
    time_t cur_time = time(NULL);
    if (!b_connected_ && ((cur_time - last_time_) >= serv_info_.reconnect_interval))
    {
        last_time_ = cur_time;
        LOG(INFO)("reconnect server:%s, ip:%s:%s"
            , serv_info_.service_name.c_str(), serv_info_.server_ip.c_str(), serv_info_.server_port.c_str());
        //重连服务端 ，重连失败则再次注册定时器
        if (SessionManager::Instance().ReConnectServer(this) != 0)
        {
            SetScore(0); // 重连失败依然0分
            LOG(WARN)("reconnect server:%s falied, ip:%s", serv_info_.service_name.c_str(), FromAddrTostring(remote_addr()).c_str());
            TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
        }
        // 无论成功与否，此时都是未连接状态
        b_connected_ = false;

        return ;
    }

    if (b_connected_ && (cur_time - last_time_ >= serv_info_.time_out))
    {
        LOG(WARN)("server:%s time out, time_out:%u, session_id:%s, curtime:%ld, last_time:%ld, difftime:%d"
            , serv_info_.service_name.c_str(), serv_info_.time_out, session_id().c_str()
            , cur_time, last_time_, (int)difftime(cur_time, last_time_));
        SetScore(0); // 超时就得0分
        this->close();
        b_connected_ = false;
        TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
        return;
    }

    if (b_connected_)
    {
        keepalive();
    }

    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.heart_beat_interval), NULL);
}

void ServerSession::keepalive()
{
    BeatMsg request_beat;
    request_beat.cmd = 0;
    request_beat.seq = ++seq_inc;
    request_beat.server_id = myself_service_id_;
    LOG(INFO)("send beat pkg to server:%s, addr:%s:%s"
        , serv_info_.service_name.c_str(), serv_info_.server_ip.c_str(), serv_info_.server_port.c_str());
    SendRequest(request_beat);
}


// 未调用
void ServerSession::BeatOK(const Msg *msg)
{
    (void)msg;
    Option *option = LibcoNetSession::option();
    LOG(INFO)("server is alive. name:%s, remote addr:%s"
        , option->service_name.c_str(), FromAddrTostring(remote_addr()).c_str());
}


void ServerSession::set_serv_info(const Server_Info &info)
{
    this->serv_info_ = info;
}

const Server_Info &ServerSession::serv_info()
{
    return serv_info_;
}

void ServerSession::SetId(uint32_t id)
{
    id_ = id;
}

uint32_t ServerSession::GetId()
{
    return id_;
}

uint32_t ServerSession::GetId() const
{ 
    return id_;
}


void ServerSession::SetScore(uint32_t score)
{
    score_ = score;
}

uint32_t ServerSession::GetScore()
{
    return score_;
}

uint32_t ServerSession::GetScore() const
{
    return score_;
}

uint32_t ServerSession::NodeId() // 虚拟节点id
{
    return node_id_;
}


void ServerSession::SetNodeId(uint32_t node_id)
{
    node_id_ = node_id;
}

void ServerSession::SetMyselfServiceId(uint32_t myself_id)
{
    myself_service_id_ = myself_id;
}


