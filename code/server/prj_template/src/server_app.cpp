/**
 * @filedesc: 
 * server_app.cpp, app file
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/8 21:52:59
 * @modify:
 *
**/

#include "server_app.h"
#include "client_session.h"
#include "udp_cmd_session.h"
#include "server_config.h"

using namespace utils;
using namespace common;

ServerApp::ServerApp(void)
    : net_manager_(NULL)
    , send_buffer_(NULL)
{
    send_buffer_ = new char[MAX_SEND_BUFF_LEN];
}

ServerApp::~ServerApp(void)
{
    SAFE_DELETE(net_manager_)
    SAFE_DELETE_ARRAY(send_buffer_)
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

    // todo: start cond and udp_cmd service
    LOG(INFO)("open acceptor");
    if (0 != open_acceptor())
    {
        LOG(ERROR)("[start] open_acceptor failed.");
        return -1;
    }

#if 0
    // todo: transfer net_manager_ to imd_connector_
    login_connector_.open(net_manager_);
    LOG(INFO)("[start] open login service");
    // open and collect imd server session
    if (0 != open_loginservice())
    {
        LOG(ERROR)("[start] open_imdservice failed.");
        return -1;
    }
    // msgcenter
    msgcenter_connector_.open(net_manager_);
    LOG(INFO)("[start] open msg center service");
    // open and collect msg center server session
    if (0 != open_msg_center_service())
    {
        LOG(ERROR)("[start] open_msg_center_service failed.");
        return -1;
    }

    // status
    status_connector_.open(net_manager_);
    LOG(INFO)("[start] open status service");
    // open and collect qgroup server session
    if (0 != open_status_service())
    {
        LOG(ERROR)("[start] open_status_service failed.");
        return -1;
    }
#endif

    // todo: transfer net_manager_ to db_connector_
    data_connector_.open(net_manager_);
    LOG(INFO)("[start] open data service");
    // open and collect data server session
    if (0 != open_dataservice())
    {
        LOG(ERROR)("[start] open data service failed.");
        return -1;
    }

    //此处先写死了
    LOG(INFO)("[start] all start succeed. ^_^");
    TimeoutManager::Instance()->RegisterTimer(this, 1000, NULL);
    return 0;
}

void ServerApp::remove_data_session()
{
    for (vector<DataSession*>::iterator it = list_data_sesison_.begin(); it!= list_data_sesison_.end(); ++it)
    {
        SAFE_DELETE(*it);
    }

    list_data_sesison_.clear();
}

int ServerApp::open_dataservice()
{
    LOG(INFO)("[start] open_dataservice");
	/*
    remove_data_session();
    const vector<Server_Info> &list_serv = utils::ServerConfig::Instance()->server_info(DATA_SERVER_NAME);
    for (vector<Server_Info>::const_iterator it=list_serv.begin(); it!=list_serv.end(); it++)
    {
        const Server_Info &info = *it;
        DataSession *db_session = new DataSession;
        db_session->set_serv_info(info);
        INET_Addr addr;

        string strAddr = info.server_ip + ":" + info.server_port;
        LOG(DEBUG)("[start] data address info:%s", strAddr.c_str());
        if(FromStringToAddr(strAddr.c_str(), addr) == -1){
            LOG(ERROR)("[start] parse addr error,service address=%s", strAddr.c_str());
            return -1;
        }

        if (data_connector_.connect(db_session, addr, &dbp_splitter_
            , info.conn_time_out, utils::ServerConfig::max_buffer_size[DATA_SERVER_NAME]) != 0)
        {
            LOG(WARN)("[start] connect data:%s failed.", info.print());
            //连接失败，则重连
            db_session->handle_close(0);
        }
        list_data_sesison_.push_back(db_session);
	LOG(INFO)("[start] connect data:%s succeed. max buf size:%u.", info.print(), utils::ServerConfig::max_buffer_size["msg_center"]);
    }
    */
    return 0;
}

int ServerApp::open_acceptor()
{
    client_accept_.net_manager(net_manager_);
    udp_client_accept_.net_manager(net_manager_);
   
    INET_Addr inetAddr;
    string strAddr = utils::ServerConfig::Instance()->sys_ip()+":"+utils::ServerConfig::Instance()->sys_port();
    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1){
        LOG(ERROR)("[start] parse addr error,service address=%s", strAddr.c_str());
        return -1;
    }

    LOG(INFO)("[start] cond buf size:%u", utils::ServerConfig::max_buffer_size[CLIENT_NODE_NAME]);
    // todo: open cond server. transfer callback function
    LOG(INFO)("[start] init cond service, addr=%s.", strAddr.c_str());
    if (client_accept_.open(inetAddr, &splitter_
        , utils::ServerConfig::max_buffer_size[CLIENT_NODE_NAME]) != 0)
    {
        LOG(ERROR)("[start] cond service open failed.");
        return -1;
    }

//    // udp client session
//    strAddr = utils::ServerConfig::Instance()->udp_addr();
//    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1){
//        LOG(ERROR)("[start] parse addr error,udp service address=%s", strAddr.c_str());
//        return -1;
//    }

//    // todo: open udp client server. transfer callback function
//    LOG(INFO)("[start] init udp client service, addr=%s.", strAddr.c_str());
//    if (udp_client_accept_.open(inetAddr, NULL) != 0)
//    {
//        LOG(ERROR)("[start] udp service open failed.");
//        return -1;
//    }
  
    // udp client session
    strAddr = utils::ServerConfig::Instance()->udp_cmd_addr();
    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1){
        LOG(ERROR)("[start] parse addr error,udp cmd service address=%s", strAddr.c_str());
        return -1;
    }

    // todo: open udp cmd server. transfer callback function
    LOG(INFO)("[start] init udp cmd service, addr=%s.", strAddr.c_str());
    if (udp_client_accept_.open(inetAddr, NULL) != 0)
    {
        LOG(ERROR)("[start] udp cmd service open failed.");
        return -1;
    }
    
    return 0;
}

int ServerApp::run_service()
{
    if (!net_manager_)
    {
        LOG(WARN)("[start] mobiled app is not initialized.");
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

ClientSession * ServerApp::get_uid_session( int32_t uid )
{
    LOG(DEBUG)("ServerApp::get_uid_session | ClientSession uid:%d", uid);
    return client_session_list_.find(uid);
}

void ServerApp::add_uid_session( ClientSession * session )
{
    LOG(DEBUG)("ServerApp::add_uid_session | ClientSession uid:%d", session->uid());
    client_session_list_.insert(session->uid(), session);
}

void ServerApp::remove_uid_session( ClientSession * session )
{
    LOG(DEBUG)("ServerApp::remove_uid_session | ClientSession uid:%d", session->uid());
    if (client_session_list_.find(session->uid()) == session)
        client_session_list_.remove(session->uid());
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
            if( static_cast<uint32_t>(curTime - pHeadSession->last_time()) > utils::ServerConfig::Instance()->client_ttl())
            {
                iCount ++;
                LOG(INFO)("[clean] user uid:%u  timeout and do auto clean, cur_time:%u, last_time:%u."
                    , pHeadSession->uid(), (uint32_t)curTime, (uint32_t)pHeadSession->last_time());
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



