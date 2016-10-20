/**
 * @filedesc: 
 * session_manager.cpp
 * @author: 
 *  bbwang
 * @date: 
 *  2015/2/14 11:02:41
 * @modify:
 *
**/
#include "session_manager.h"

#include "comm.h"
#include "client_session.h"
#include "server_session.h"
#include "option.h"
#include "translater.h"
#include "msg.h"

static const int TMP_BUF_LEN = 512;

ClientAcceptor::ClientAcceptor(void)
    : option_(NULL)
{
}

ClientAcceptor::~ClientAcceptor(void)
{
    option_ = NULL;
}

void ClientAcceptor::set_option(Option *option)
{
    this->option_ = option;
}

int ClientAcceptor::make_svc_handler(ClientSession *&sh)
{
    sh = new ClientSession;
    sh->set_option(option_);
    return 0;
}

int ClientAcceptor::activate_svc_handler(ClientSession *svc_handler, const INET_Addr &remote_addr)
{
    LOG(INFO)("[frame] new session connected, handle:%u, clientip:%s."
        , svc_handler->handle(), FromAddrTostring(remote_addr).c_str());
    return super::activate_svc_handler(svc_handler, remote_addr);
}

// udp acceptor
UdpAcceptor::UdpAcceptor(void)
    : option_(NULL)
{
}

UdpAcceptor::~UdpAcceptor(void)
{
    option_ = NULL;
}

void UdpAcceptor::set_option(Option *option)
{
    this->option_ = option;
}

int UdpAcceptor::make_svc_handler(ClientSession *&sh)
{
    sh = new ClientSession;
    sh->set_option(option_);
    return 0;
}

int UdpAcceptor::activate_svc_handler(ClientSession *svc_handler, const INET_Addr &remote_addr)
{
    LOG(INFO)("[frame] new udp session connected, handle:%u, clientip:%s."
        , svc_handler->handle(), FromAddrTostring(remote_addr).c_str());
    return super::activate_svc_handler(svc_handler, remote_addr);
}



// spliter
int ClientSplitter::split(const char *buf, int len, int &packet_begin, int &packet_len)
{
    int ret = 1;
    char info[TMP_BUF_LEN] = {0};
    do {
        snprintf(info, TMP_BUF_LEN, "[frame] receive client data, splite pkg. len:%d,", len );
        if ((size_t) len < 5*sizeof(int32_t) )
        {
            ret = 0;
            break;
        }

        int32_t unLen = ntohl(*(int32_t *)buf);
        if( unLen > (int32_t)CLIENT_MAX_PKG_SIZE || unLen < (int32_t)sizeof(uint32_t))
        {
            LOG(ERROR) ("ClientSplitter split error, pkg too long or too short. data len:%u, pkg len:%u, limited len:%u",
                len, unLen, CLIENT_MAX_PKG_SIZE);
            ret = -1;
            break;
        }

        if(unLen > len)
        {
            ret = 0;
            break;
        }
        if (len >= (int) unLen)
        {
            packet_begin = 0;
            packet_len = unLen;
            ret = 1;
            break;
        } 
    } while (0);
    LOG(DEBUG)("%s pkg_begin:%d, pkg_len:%d, ret:%d", info, packet_begin, packet_len, ret);
    LOG_HEX(buf, packet_len, utils::L_DEBUG);
    return ret; 
}

int ServerSplitter::split(const char *buf, int len, int &packet_begin, int &packet_len)
{
    int ret = 1;
    char info[TMP_BUF_LEN] = {0};
    do {
        snprintf(info, TMP_BUF_LEN, "[frame] receive server data, splite pkg. len:%d,", len );
        if ((size_t) len < 5*sizeof(int32_t) )
        {
            ret = 0;
            break;
        }

        int32_t unLen = ntohl(*(int32_t *)buf);
        if( unLen > (int32_t)SERVER_MAX_PKG_SIZE || unLen < (int32_t)sizeof(uint32_t))
        {
            LOG(ERROR) ("ServerSplitter split error, pkg too long or too short. data len:%u, pkg len:%u, limited len:%u",
                len, unLen, SERVER_MAX_PKG_SIZE);
            ret = -1;
            break;
        }

        if(unLen > len)
        {
            ret = 0;
            break;
        }
        if (len >= (int) unLen)
        {
            packet_begin = 0;
            packet_len = unLen;
            ret = 1;
            break;
        } 
    } while (0);
    LOG(DEBUG)("%s pkg_begin:%d, pkg_len:%d, ret:%d", info, packet_begin, packet_len, ret);
    LOG_HEX(buf, packet_len, utils::L_DEBUG);
    return ret; 
}

// session manager 


SessionManager &SessionManager::Instance()
{
    static SessionManager inst;
    return inst;
}


SessionManager::SessionManager()
    : net_manager_(NULL)
{
//    srv_sessions_vec_map_.clear();
    session_clusters_.clear();
    TimeoutManager::Instance()->RegisterTimer(this, 5000, NULL);
}

SessionManager::~SessionManager()
{
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
}

void SessionManager::net_manager(Net_Manager *net_manager)
{
    this->net_manager_ = net_manager;
    client_acceptor_.net_manager(net_manager);
    udp_acceptor_.net_manager(net_manager);
    server_connector_.open(net_manager);
}

int SessionManager::ConnectServer(const Server_Info &server_info, Option *option)
{
    if (NULL == net_manager_)
    {
        LOG(ERROR)("[frame] ConnectServer, net manager is not init.");
        return -1;
    }
    ServerSession *server_session = new ServerSession;
    server_session->set_option(option);
    server_session->set_serv_info(server_info);
    
    INET_Addr addr;
    string strAddr = server_info.server_ip + ":" + server_info.server_port;
    LOG(DEBUG)("[frame] connect server, address info:%s", strAddr.c_str());
    if(FromStringToAddr(strAddr.c_str(), addr) == -1){
        LOG(ERROR)("[frame] connect failed. parse addr error,service address=%s", strAddr.c_str());
        return -1;
    }

    if (server_connector_.connect(server_session, addr, &server_splitter_
        , server_info.conn_time_out, server_info.max_buf_size) != 0)
    {
        LOG(WARN)("[frame] connect server:%s failed.", server_info.print());
        //连接失败，则重连
        server_session->handle_close(0);
        return -1;
    }
    // 所有server先放分组
    server_session->SetId((uint32_t)atol(server_info.server_id.c_str()));
//    int ret_value = AddToCluster(server_info, server_session);
//    if (0 == ret_value)
//    {
//        LOG(INFO)("add session to session group ok");
//    }
//    else
//    {
//        LOG(ERROR)("add session to session group failed, ret:%d", ret_value);
//    }
    return 0;
}

int SessionManager::ReConnectServer(ServerSession *server_session)
{
    const Server_Info &server_info = server_session->serv_info();
    INET_Addr addr;
    string strAddr = server_info.server_ip + ":" + server_info.server_port;
    LOG(DEBUG)("[frame] server address info:%s", strAddr.c_str());
    if(FromStringToAddr(strAddr.c_str(), addr) == -1){
        LOG(ERROR)("[frame] parse addr error,service address=%s", strAddr.c_str());
        return -1;
    }

    if (server_connector_.connect(server_session, addr, &server_splitter_
        , server_info.conn_time_out, server_info.max_buf_size) != 0)
    {
        LOG(WARN)("[frame] connect server:%s failed.", server_info.print());
        //连接失败，则重连
        //server_session->handle_close(0);
        return -1;
    }
    // 所有server先放分组
//    int ret_value = AddToCluster(server_info, server_session);
//    if (0 == ret_value)
//    {
//        LOG(INFO)("add session to session group ok");
//    }
//    else
//    {
//        LOG(ERROR)("add session to session group failed, ret:%d", ret_value);
//    }
    return 0;
}


int SessionManager::OpenAcceptor(const Server_Info &server_info, Option *option)
{
    if (NULL == net_manager_)
    {
        LOG(ERROR)("[frame] OpenAcceptor, net manager is not init.");
        return -1;
    }
    INET_Addr inetAddr;
    std::string strAddr = server_info.server_ip + ":" + server_info.server_port;
    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1) {
        LOG(ERROR)("[frame] parse addr error,service address=%s", strAddr.c_str());
        return -1;
    }
    client_acceptor_.set_option(option);
    LOG(INFO)("[frame] init system service, system buf size:%u, service addr:%s, server info:%s"
        , server_info.max_buf_size, strAddr.c_str(), server_info.print());
    if (client_acceptor_.open(inetAddr, &client_splitter_, server_info.max_buf_size) != 0)
    {
        LOG(ERROR)("[frame] system service open failed.");
        return -1;
    }
    return 0;
}

int SessionManager::OpenUdpAcceptor(const Server_Info &server_info, Option *option)
{
    if (NULL == net_manager_)
    {
        LOG(ERROR)("[frame] OpenUdpAcceptor, net manager is not init.");
        return -1;
    }
    INET_Addr inetAddr;
    std::string strAddr = server_info.server_ip + ":" + server_info.server_port;
    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1) {
        LOG(ERROR)("[frame] parse addr error,service address=%s", strAddr.c_str());
        return -1;
    }
    udp_acceptor_.set_option(option);
    LOG(INFO)("[frame] init system udp service, system buf size:%u, service addr:%s, server info:%s"
        , server_info.max_buf_size, strAddr.c_str(), server_info.print());
    if (udp_acceptor_.open(inetAddr, NULL) != 0)
    {
        LOG(ERROR)("[frame] system udp service open failed.");
        return -1;
    }
    return 0;

}


int SessionManager::AddServerSession(ServerSession *server_session)
{
    if (NULL == server_session)
    {
        LOG(WARN)("server session is null. can not add server session!");
        return -1;
    }
    return AddToCluster(server_session->serv_info(), server_session);
}


int SessionManager::AddToCluster(const Server_Info &server_info, LibcoNetSession *session)
{
    int ret = 0;
    if (NULL == session)
    {
        LOG(ERROR)("session is null.");
        ret = -1;
        return ret;
    }

    // 一个Cluster里面只能有一个selector，一个SessionGroup里面只能有一个selector
    Cluster *cluster = NULL;
    SessionGroup *session_group = NULL;
    const char *cluster_name = server_info.service_name.c_str();
    uint32_t group_id = server_info.group_id;
    int group_strategy = server_info.group_strategy;
    int session_strategy = (int)server_info.session_strategy;
    ClustersIter cluster_iter = session_clusters_.find(cluster_name);
    if (cluster_iter == session_clusters_.end())
    {
        LOG(INFO)("cluster not exist with name:%s, new add cluster.", cluster_name);
        cluster = new Cluster;
        ret = cluster->CreateGroupSelector(group_strategy);
        if (0 != ret)
        {
            LOG(ERROR)("create group selector failed, add group failed.");
            SAFE_DELETE(cluster);
            return ret;
        }
        LOG(INFO)("create group selector ok, new add group, cluster_name:%s.", cluster_name);
        session_clusters_[cluster_name] = cluster;
    }
    else
    {
        cluster = cluster_iter->second;
    }

    session_group = cluster->GetGroup(group_id);
    if (NULL == session_group)
    {
        session_group = new SessionGroup;
        session_group->SetId(group_id);
        ret = cluster->AddGroup(session_group);
        ret |= session_group->CreateSessionSelector(session_strategy);
        if (0 != ret)
        {
            LOG(ERROR)("add group failed.");
            SAFE_DELETE(session_group);
            return ret;
        }
    }

    if (0 != ret)
    {
        LOG(ERROR)("AddToCluster, add session failed.");
        return ret;
    }
    return session_group->AddSession(session);
}

// 仅仅是将session从分组中摘除，不析构session本身
int SessionManager::RemoveServerSession(ServerSession *session)
{
    if (NULL == session)
    {
        LOG(ERROR)("session is null.");
        return 0;
    }
    const utils::Server_Info &server_info = session->serv_info();
    uint32_t session_id = session->GetId();
    uint32_t group_id = server_info.group_id;
    const char *service_name = server_info.service_name.c_str();
    LOG(INFO)("remove session from cluster. cluster name:%s, group id:%u, session id:%u"
        , service_name, group_id, session_id);
    
    ClustersIter cluster_iter = session_clusters_.find(service_name);
    if (cluster_iter == session_clusters_.end())
    {
        LOG(WARN)("remove session from cluster, cluster not exist with name:%s", service_name);
        return 0;
    }
    Cluster *cluster = cluster_iter->second;
    // crash bug source
    //cluster->SetHashId(group_id);
    //SessionGroup *session_group = cluster->GetGroup();
    SessionGroup *session_group = cluster->GetGroup(group_id);
    if (NULL == session_group)
    {
        LOG(DEBUG)("remove session from cluster, group not exist. cluster name:%s, group id:%u, session id:%u"
            , service_name, group_id, session_id);
        return 0;
    }
    // 从分组中删掉session
    session_group->RemoveSession(session_id);
 
    // 如果分组也为空，就把分组从cluster中删除
    if (session_group->Sessions().empty())
    {
        cluster->RemoveGroup(group_id);
        SAFE_DELETE(session_group);
    }
    return 0;
}

bool SessionManager::IsExistSession(const char *cluster_name, uint32_t group_id, uint32_t session_id)
{
    bool ret = true;
    if (NULL == cluster_name)
    {
        LOG(WARN)("cluster name is null. can not check session, set it not exist!");
        return false;
    }
    ClustersIter cluster_iter = session_clusters_.find(cluster_name);
    if (cluster_iter == session_clusters_.end())
    {
        LOG(WARN)("cluster not exist with name:%s", cluster_name);
        return false;
    }
    Cluster *cluster = cluster_iter->second;
    SessionGroup *session_group = cluster->GetGroup(group_id);
    if (NULL == session_group)
    {
        LOG(DEBUG)("session group not exist. cluster name:%s, group id:%u, session id:%u"
            , cluster_name, group_id, session_id);
        return false;
    }
    LibcoNetSession *session = session_group->GetSession(session_id);
    if (NULL == session)
    {
        LOG(DEBUG)("session not exist. cluster name:%s, group id:%u, session id:%u"
            , cluster_name, group_id, session_id);
        ret = false;
    }
    else
    {
        LOG(DEBUG)("session exist. cluster name:%s, group id:%u, session id:%u"
            , cluster_name, group_id, session_id);
        ret = true;
    }
    return ret;
}

int SessionManager::AddClientSession(ClientSession *client_session)
{
    if (NULL == client_session)
    {
        LOG(WARN)("client session is null. can not add client session!");
        return -1;
    }
    return AddToCluster(client_session->serv_info(), client_session);
}


int SessionManager::RemoveAndCloseClientSession(const char *cluster_name, const std::string &server_id)
{
    if (NULL == cluster_name)
    {
        LOG(WARN)("cluster name is null. can not remove and close client session!");
        return -1;
    }
    Cluster *cluster = NULL;
    ClustersIter cluster_iter = session_clusters_.find(cluster_name);
    if (cluster_iter == session_clusters_.end())
    {
        LOG(WARN)("cluster not exist with name:%s.", cluster_name);
        return -1;
    }
    uint32_t hash_id = (uint32_t)atol(server_id.c_str());
    cluster = cluster_iter->second;
    cluster->SetHashId(hash_id);
    
    SessionGroup *session_group = cluster->GetGroup();
    if (NULL == session_group)
    {
        LOG(WARN)("session_group not exist with hash_id:%u.", hash_id);
        return -1;
    }
    LibcoNetSession *session = session_group->GetSession();
    if (NULL == session)
    {
        LOG(WARN)("session not exist with hash_id:%u.", hash_id);
        return -1;
    }

    session_group->RemoveSession(session->GetId());
    SAFE_DELETE(session);

    // 如果分组也为空，就把分组从cluster中删除
    if (session_group->Sessions().empty())
    {
        cluster->RemoveGroup(session_group->GetId());
        SAFE_DELETE(session_group);
    }
    LOG(INFO)("[frame] remove and close client session, server id:%s", server_id.c_str());
    return 0;
}


ClientSession *SessionManager::GetClientSession(const std::string &server_id)
{
    Cluster *cluster = NULL;
    ClientSession *session = NULL;
    ClustersIter cluster_iter = session_clusters_.find(CLIENT_CLUSTER_NAME);
    if (cluster_iter == session_clusters_.end())
    {
        LOG(WARN)("cluster not exist with name:%s.", CLIENT_CLUSTER_NAME);
        return session;
    }
    uint32_t hash_id = (uint32_t)atol(server_id.c_str());
    cluster = cluster_iter->second;
    cluster->SetHashId(hash_id);
    
    SessionGroup *session_group = cluster->GetGroup();
    if (NULL == session_group)
    {
        LOG(ERROR)("get client session failed. session group is null.");
    }
    else
    {
        session = dynamic_cast<ClientSession *>(session_group->GetSession());
    }
    return session;
}


/*   S E S S I O N   M A N A G E R .   G E T   S E S S I O N   */
/*-------------------------------------------------------------------------
    对外提供的选取session的接口，含有策略，但是strategy字段已不使用
-------------------------------------------------------------------------*/
LibcoNetSession *SessionManager::GetSession(const std::string &name, Msg &msg, int strategy)
{
    (void)strategy;
    ClustersCIter cluster_iter = session_clusters_.find(name);
    if (cluster_iter == session_clusters_.end())
    {
        LOG(ERROR)("not found session with name:%s", name.c_str());
        return NULL;
    }

    // 按照策略选取
    Cluster *cluster = cluster_iter->second;
    uint32_t hash_id = msg.hashid();
    cluster->SetHashId(hash_id);
    SessionGroup *session_group = cluster->GetGroup();
    if (NULL == session_group)
    {
        LOG(ERROR)("not found session with hash_id:%u", hash_id);
        return NULL;
    }

    // 本次的选择是按照综合得分
//    LOG(DEBUG)("GetGroup ok, group id:%u, start to get session.", session_group->GetId());
    return session_group->GetSession();
}



// 仅仅检测连接是否可用，不可用就从集群中删除
void SessionManager::handle_timeout(int ,void *)
{
    LOG(INFO)("[frame] enter session manager timer.");

    ClustersIter cluster_iter = session_clusters_.begin();
    ClustersIter cluster_end_iter = session_clusters_.end();
    for (; cluster_iter != cluster_end_iter; )
    {
        Cluster *cluster = cluster_iter->second;
        const char *cluster_name = cluster_iter->first.c_str();
        LOG(DEBUG)("timer-cluster name:%s", cluster_name);
        cluster->GetSelector()->ShowInfo();
        ++cluster_iter;
        Cluster::GroupMap &groups = cluster->Groups();
        Cluster::GroupIter group_iter = groups.begin();
        Cluster::GroupIter group_end_iter = groups.end();
        for (; group_iter != group_end_iter; )
        {
            SessionGroup *session_group = group_iter->second;
            LOG(DEBUG)("[time] session group id:%u, session size:%zu"
                , session_group->GetId(), session_group->Sessions().size());
//            session_group->GetSelector()->ShowInfo();
            ++group_iter;
            SessionGroup::SessionMap &sessions = session_group->Sessions();
            SessionGroup::SessionIter session_iter = sessions.begin();
            SessionGroup::SessionIter end_session_iter = sessions.end();
            for (; session_iter != end_session_iter; )
            {
                LibcoNetSession *session = session_iter->second;
                // TODO: crash
                LOG(INFO)("[time] %s, group id:%u, handle:%u, sid:%u, score:%u, connect:%u, addr:%s"
                    , cluster_name, session_group->GetId(), session->handle(), session->GetId(), session->GetScore()
                    , session->IsConnected(), FromAddrTostring(session->remote_addr()).c_str());
                ++session_iter;
                // 连接已断，就需要清空
                if (!session->IsConnected())
                {
                    LOG(WARN)("[time] session is disconnected, remove, handle:%u, session id:%u, score:%u"
                        , session->handle(), session->GetId(), session->GetScore());
                    session_group->RemoveSession(session->GetId());
                    // client session 不会重用，需要干掉
                    if (strcmp(cluster_name, CLIENT_CLUSTER_NAME) == 0)
                    {
                        SAFE_DELETE(session);
                    }
                }
            }
            if (sessions.empty())
            {
                LOG(WARN)("[time] group have no session, remove group, id:%u", session_group->GetId());
                cluster->RemoveGroup(session_group->GetId());
                SAFE_DELETE(session_group);
            }
        }
    }
    TimeoutManager::Instance()->RegisterTimer(this, 2000, NULL);
}


void SessionManager::BindId(uint32_t myself_id)
{
    // 设置本服务的id,用于向服务端报告
    ServerSession::SetMyselfServiceId(myself_id);
}

bool SessionManager::IsSelectorTypeEqual(const Selector<SessionGroup *> &s1, const Selector<SessionGroup *> &s2)
{
    return (s1.GetTypeName() == s2.GetTypeName());
}

