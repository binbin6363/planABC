/**
 * @filedesc: 
 * client_session.cpp
 * @author: 
 *  bbwang
 * @date: 
 *  2015/2/26 11:18:41
 * @modify:
 *
**/
#include "client_session.h"
#include "libco_net_session.h"
#include "msg.h"
#include "option.h"

ClientSession::ClientSession()
    : node_id_(0)
    , id_(0)
    , last_cli_alive_time_(0)
{
    last_time_ = time(NULL);
    // 手动添加一个timer，防止连上来啥也不干，就耗着资源，需要检测干掉，10s钟超时
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(10), NULL);
    LOG(DEBUG)("new client session. handle:%u, connecting ...", handle());
}

ClientSession::~ClientSession()
{
    LOG(DEBUG)("del client session. handle:%u, session id:%s, disconnected.", handle(), session_id().c_str());
    Net_Handler::close();
    Net_Handle_Manager::Instance()->RemoveHandle(handle());
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
}


int ClientSession::open( void *arg, const INET_Addr &remote_addr )
{
    LOG(INFO)("ClientSession::open, address:%s, handle:%u.", FromAddrTostring(remote_addr).c_str(), handle());

    last_time_ = time(NULL);
    last_cli_alive_time_ = last_time_;
    LibcoNetSession::open(arg, remote_addr);
    b_connected_ = true;
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(2), NULL);

    //MobileApp::Instance()->add_first_pkg_list(this);
    return 0;
}

int ClientSession::handle_close(uint32_t handle)
{
    LOG(WARN)("ClientSession::handle_close, handle:%u, address:%s"
        , handle, FromAddrTostring(remote_addr()).c_str());
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    b_connected_ = false;
    // 此处不delete，不销毁ClientSession实例
    // 在session manager的void SessionManager::handle_timeout(int id,void *userData)中处理
    // delete this;
    return 0;
}

// 此处仅对连接做超时检测，不delete实例
void ClientSession::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;

    if (session_type() == UDP_SESSION)
    {
        return ;
    }

    time_t cur_time = time(NULL);
    if (difftime(cur_time, last_time_) >= serv_info_.time_out)
    {
        LOG(WARN)("client session time out, session_id:%s, curtime:%ld, last_time:%ld, difftime:%d, ttl:%u, ip:%s"
            , session_id().c_str(), cur_time, last_time_, (int)difftime(cur_time, last_time_)
            , serv_info_.heart_beat_interval, FromAddrTostring(remote_addr()).c_str());
        this->close();
        b_connected_ = false;
    }
    else
    {
        TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.time_out), NULL);
    }

}

void ClientSession::SetGroupId(uint32_t group_id)
{
    serv_info_.group_id = group_id;
}

void ClientSession::SetId(uint32_t id)
{
    id_ = id;
//    if (serv_info_.group_id == 0 && id_ != 0) {
//        serv_info_.group_id = id_;
//    }
}

uint32_t ClientSession::GetId()
{
    return id_;
}

uint32_t ClientSession::GetId() const
{
    return id_;
}

uint32_t ClientSession::NodeId() // 虚拟节点id
{
    return node_id_;
}


void ClientSession::SetNodeId(uint32_t node_id)
{
    node_id_ = node_id;
}

void ClientSession::set_serv_info(const Server_Info &serv_info)
{
    serv_info_ = serv_info;
}

Server_Info &ClientSession::serv_info()
{
    return serv_info_;
}

uint32_t ClientSession::GetSessionStrategy()
{
    return serv_info_.session_strategy;
}


