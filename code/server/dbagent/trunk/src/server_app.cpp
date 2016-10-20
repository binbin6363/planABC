/**
 * @filedesc: 
 * leveldb_server_app.cpp, app file
 * @author: 
 *  bbwang
 * @date: 
 *  2014/8/3 12:02:59
 * @modify:
 *
**/

#include "server_app.h"
#include "data_type.h"
#include "comm.h"
#include "dbpool.h"
#include "mysqloperator.h"

using namespace utils;

ServerApp::ServerApp(void)
    : net_manager_(NULL)
    , send_buffer_(NULL)
    , mysql_con_pool_(NULL)
    , mysql_impl_(NULL)
{
    send_buffer_ = new char[MAX_SEND_BUFF_LEN];
}

ServerApp::~ServerApp(void)
{
//    SAFE_DELETE(g_userstatus)
}

int ClientSplitter_::split( const char *buf, int len, int &packet_begin, int &packet_len )
{
    if ((size_t) len < sizeof(COHEADER) )
    {
        return 0;
    }

    COHEADER *hdr = (COHEADER *) buf;
    int32_t pkglen = ntohl(hdr->len);
    int32_t cmd = ntohl(hdr->cmd);
    int32_t seq = ntohl(hdr->seq);
    int32_t head_len = ntohl(hdr->head_len);
    int32_t uid = ntohl(hdr->uid);


    if( pkglen > (int32_t)utils::ServerConfig::max_buffer_size["client"] || pkglen < (int32_t)sizeof(COHEADER))
    {
        LOG(ERROR) ("Splitter_ split error, pkg too long or short. hdr.len:%d, len:%d, limited len:%u, head_len:%d, cmd:%d, seq:%d, uid:%d!",
            pkglen, len, utils::ServerConfig::max_buffer_size["client"], head_len, cmd, seq, uid);
        return -1;
    }

    if(pkglen > len)
    {
        return 0;
    }
    if (len >= pkglen)
    {
        packet_begin = 0;
        packet_len = pkglen;

        return 1;
    } 

    return 1; 
}

uint32_t ServerApp::get_new_loginseq()
{
    static uint32_t loginseq = 0;
    loginseq++;
    if(0 == loginseq)
        loginseq++;
    return loginseq;
}


char *ServerApp::get_buffer()
{
    return send_buffer_;
}

void ServerApp::reset_buffer()
{
    if (NULL != send_buffer_)
        memset(send_buffer_, '\0', MAX_SEND_BUFF_LEN);
}

int ServerApp::open()
{
    net_manager_ = new Net_Manager;
    // todo: start net thread, handle event
    if (0 != net_manager_->start())
    {
        LOG(ERROR)("[start] net_manager_ start failed.");
        return -1;
    }

    // todo: start leveldb and udp_cmd service
    LOG(INFO)("open acceptor");
    if (0 != open_acceptor())
    {
        LOG(ERROR)("[start] open_acceptor failed.");
        return -1;
    }

    // 初始化数据库
    if (0 != init_mysql())
    {
        LOG(ERROR)("[start] init mysql failed.");
        return -1;
    }
    //此处先写死了
    LOG(INFO)("[start] all start succeed. ^_^");
    TimeoutManager::Instance()->RegisterTimer(this, 1000, NULL);
    return 0;
}




int ServerApp::open_acceptor()
{
    client_accept_.net_manager(net_manager_);
    udp_cmd_accept_.net_manager(net_manager_);
   
    INET_Addr inetAddr;
    string strAddr = ServerConfig::Instance()->sys_ip()+":"+ServerConfig::Instance()->sys_port();
    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1){
        LOG(ERROR)("[start] parse addr error,service address=%s", strAddr.c_str());
        return -1;
    }

    LOG(INFO)("[start] client buf size:%u", utils::ServerConfig::max_buffer_size["client"]);
    // todo: open leveldb server. transfer callback function
    LOG(INFO)("[start] init dbagent service, addr=%s.", strAddr.c_str());
    if (client_accept_.open(inetAddr, &splitter_
        , utils::ServerConfig::max_buffer_size["client"]) != 0)
    {
        LOG(ERROR)("[start] dbagent service open failed.");
        return -1;
    }

    // udp client session
    strAddr = utils::ServerConfig::Instance()->udp_cmd_addr();
    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1){
        LOG(ERROR)("[start] parse addr error,udp cmd service address=%s", strAddr.c_str());
        return -1;
    }

    // todo: open udp cmd server. transfer callback function
    LOG(INFO)("[start] init udp cmd service, addr=%s.", strAddr.c_str());
    if (udp_cmd_accept_.open(inetAddr, NULL) != 0)
    {
        LOG(ERROR)("[start] udp cmd service open failed.");
        return -1;
    }

    return 0;
}

int ServerApp::init_mysql()
{
    int ret = 0;
    mysql_con_pool_ = new MysqlConnPool;
    utils::ServerConfig *config = utils::ServerConfig::Instance();
    string host = config->db_host();
    uint32_t port = config->db_port();
    string user = config->db_user();
    string paswd = config->db_passwd();
    string charset = config->db_charset();
    uint32_t connect_num = config->db_conn_num();
    uint32_t timeout = config->db_timeout();
    ret = mysql_con_pool_->Init( host, user, paswd, port, charset, connect_num, timeout);
    if (0 != ret)
    {
        LOG(ERROR)("init mysql connection pool failed. ret:%d", ret);
        return ret;
    }

    mysql_impl_ = new MysqlDatabaseImpl;
    mysql_impl_->Init(mysql_con_pool_, 1, 0);
    LOG(INFO)("done init mysql.");
    return ret;
}

int ServerApp::run_service()
{
    if (!net_manager_)
    {
        LOG(WARN)("[start] leveldb app is not initialized.");
        return -1;
    }

    while(1)
    {
        TIME_SPEC nowTime = gettimeofday();
        TimeoutManager::Instance()->UpdateTimeout(nowTime);
        Net_Event*  ev = net_manager_->get_event();
        if (ev)
        {
            ev->handler(*ev);
            delete ev;
        }
        else
        {
            usleep(10);
        }
    }
}


void ServerApp::handle_timeout( int id,void *userData )
{
    (void)id;
    (void)userData;
    LOG(DEBUG)("enter SessionManager onTimer");
    time_t curTime = time(NULL);
    uint32_t iCount = 0;
    list_head* pPos;
    list_head* pHead = client_session_list_.begin();
    for( pPos = pHead->next; pPos != pHead; )
    {
        list_head *pCurPos = pPos;
        pPos = pPos->next;

        ClientSession* pHeadSession = list_entry(pCurPos, ClientSession, item_);
        if(NULL != pHeadSession)
        {
            if( static_cast<uint32_t>(curTime - pHeadSession->last_time()) > ServerConfig::Instance()->client_ttl())
            {
                iCount ++;
                LOG(ERROR)("[clean] user session_id:%s timeout and do auto clean, cur_time:%u, last_time:%u."
                    , pHeadSession->session_id().c_str(), (uint32_t)curTime, (uint32_t)pHeadSession->last_time());
                pHeadSession->close();
            }
            else
            {
                break;
            }
        }
    }
    if( iCount > 0 )
    {
        LOG(INFO)("[sum] SUM CLEAN user session num:%u.", iCount);
    }

    //此处先写死了
    TimeoutManager::Instance()->RegisterTimer(this, 1000, NULL);
}


Net_Session * ServerApp::get_session( const string &session_id )
{
    std::map<string, Net_Session*>::iterator it = map_session_.find(session_id);
    if (it != map_session_.end())
        return it->second;

    return NULL;
}


int ServerApp::add_session( const string &session_id, Net_Session *session )
{
    LOG(DEBUG)("add_session id:%s", session_id.c_str());
    std::map<string, Net_Session*>::iterator it = map_session_.find(session_id);
    if (it != map_session_.end())
    {
        LOG(WARN)("session is exists, session_id:%s", session_id.c_str());
        return -1;
    }

    map_session_.insert(make_pair(session_id, session));
    return 0;
}

void ServerApp::remove_session( const string &session_id )
{
    if (map_session_.find(session_id) != map_session_.end())
    {
        map_session_.erase(session_id);
        LOG(DEBUG)("remove_session id:%s", session_id.c_str());
    }
    else
    {
        LOG(DEBUG)("remove_session failed, session with id:%s not exist.", session_id.c_str());
    }
}


