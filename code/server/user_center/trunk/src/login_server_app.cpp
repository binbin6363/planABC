/**
* file: login_server_app.cpp
* desc: ban liao, server app.
* auth: bbwang
* date: 2015/3/3
*/

#include "login_server_app.h"
#include "session_manager.h"
#include "libco_wrap.h"
#include "option.h"
#include "translater.h"
#include "dbp_processor.h"
#include "ldb_processor.h"
#include "status_processor.h"
#include "client_processor.h"
#include "msg.h"
#include "factories.h"
#include "user.h"
#include "redis_client.h"

Config config;
std::string DBP_SERVICE("");
std::string CLIENT_SESSION("");

LoginApp::LoginApp()
    : net_manager_(NULL)
    , dbp_option_(NULL)
    , cli_option_(NULL)
    , dbp_factory_(NULL)
    , cli_factory_(NULL)
    , status_factory_(NULL)
{
    dbp_option_ = new Option;
    dbp_option_->service_name = config.dbp_service_name.c_str();//DBPSESSION;
    dbp_option_->translater   = ProtocTranslater::Instance();
    dbp_option_->processor    = DbpProcessor::Instance();

    cli_option_ = new Option;
    cli_option_->service_name = config.sys_name.c_str();//CLIENTSESSION;
    cli_option_->translater   = ProtocTranslater::Instance();
    cli_option_->processor    = ClientProcessor::Instance();
}


LoginApp::~LoginApp()
{
    SAFE_DELETE(cli_option_)
    SAFE_DELETE(dbp_option_)
}


LoginApp* LoginApp::Instance()
{
    static  LoginApp instance;
    return &instance;
}

int LoginApp::open(const Config &config)
{
    net_manager_ = new Net_Manager;
    // todo: start net thread, handle event
    if (0 != net_manager_->start())
    {
        LOG(ERROR)("[start] net_manager_ start failed.");
        return -1;
    }
    SessionManager::Instance().net_manager(net_manager_);

    // todo: start acceptor service
    LOG(INFO)("open acceptor");
    if (0 != open_acceptor(config))
    {
        LOG(ERROR)("[start] open_acceptor failed.");
        return -1;
    }
    LOG(INFO)("[start] open dbp session");
    // open and collect db server session
    if (0 != open_dbpservice(config))
    {
        LOG(ERROR)("[start] open_dbpservice failed.");
        return -1;
    }

    LOG(INFO)("[start] open rds session");
    // open and collect db server session
    if (0 != open_rdsservice(config))
    {
        LOG(ERROR)("[start] open_rdsservice failed.");
        return -1;
    }
    // init user info
    User::set_check_interval(config.user_check_interval);
    User::set_user_timeout(config.user_timeout);
    User::set_user_clean(config.user_clean);
    UserMgr::Instance();
    
    return 0;
}

int LoginApp::open_acceptor(const Config &config)
{
    Server_Info info;
    info.server_ip = config.sys_ip;
    info.server_port = config.sys_port;
    info.conn_time_out = config.sys_timeout;
    info.heart_beat_interval = config.sys_ttl;
    info.max_buf_size = config.sys_buf_size;
    info.reconnect_interval = 2;
    info.service_name = config.sys_name.c_str();
    info.time_out = config.sys_timeout;
    info.session_strategy = config.session_strategy;
    info.group_strategy = config.group_strategy;
    SessionManager::Instance().OpenAcceptor(info, cli_option_);
    return 0;
}

int LoginApp::open_dbpservice(const Config &config)
{
    LOG(INFO)("[start] open_dbpservice");
    const vector<Server_Info> &list_serv = config.dbp_srv_infos;
    for (vector<Server_Info>::const_iterator it=list_serv.begin(); it!=list_serv.end(); it++)
    {
        const Server_Info &info = *it;
        SessionManager::Instance().ConnectServer(info, dbp_option_);
	    LOG(INFO)("[start] connect db:%s succeed. max buf size:%u."
            , info.print(), info.max_buf_size);
    }
    return 0;
}

int LoginApp::open_rdsservice(const Config &config)
{
    LOG(INFO)("[start] open_rdsservice");
    const vector<Server_Info> &list_serv = config.rds_srv_infos;
    for (vector<Server_Info>::const_iterator it=list_serv.begin(); it!=list_serv.end(); it++)
    {
        const Server_Info &info = *it;
        RedisClient *rds_cli = new RedisClient;
        rds_cli->set_serv_info(info);
        rds_cli->SyncConnectRedis(info.max_buf_size);
        TimeoutManager::Instance()->RegisterTimer(rds_cli, MAKE_SECOND_INTERVAL(1), NULL);
        redis_client_list_.push_back(rds_cli);
    }
    return 0;
}


void LoginApp::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;
    LOG(DEBUG)("handle_timeout, do nothing");
}

int LoginApp::run_service()
{
    if (!net_manager_)
    {
        LOG(WARN)("[start] mobiled app is not initialized.");
        return -1;
    }
    // 设置co的最大值
    set_max_co_num(config.sys_max_co_num);
	set_service_id(config.sys_id);
    return co_run_service(net_manager_);
}


void LoginApp::RegistFactory(const Config &config)
{
    CLIENT_SESSION = config.sys_name;
    DBP_SERVICE = config.dbp_service_name;

    cli_factory_ = new CliMsgFactory;
    dbp_factory_ = new DbpMsgFactory;
    GlobalFactoryRegister(CLIENT_SESSION.c_str(), cli_factory_);
    GlobalFactoryRegister(DBP_SERVICE.c_str(), dbp_factory_);
}



void LoginApp::UnregistFactory(const Config &)
{
    GlobalFactoryUnregister(CLIENT_SESSION.c_str(), cli_factory_);
    GlobalFactoryUnregister(DBP_SERVICE.c_str(), dbp_factory_);
}


RedisClient *LoginApp::GetRedisClient(uint32_t hash_id)
{
    uint32_t redis_size = redis_client_list_.size();
    uint32_t index = hash_id % redis_size;
    return redis_client_list_[index];
}




//ClientSession *LoginApp::get_session( const string &session_id )
//{
//    std::map<string, ClientSession*>::iterator it = map_session_.find(session_id);
//    if (it != map_session_.end())
//        return it->second;

//    return NULL;
//}

//int LoginApp::add_session( const string &session_id, ClientSession *session )
//{
//    LOG(INFO)("add_session id:%s", session_id.c_str());
//    std::map<string, ClientSession*>::iterator it = map_session_.find(session_id);
//    if (it != map_session_.end())
//    {
//        LOG(WARN)("session is exists, session_id:%s", session_id.c_str());
//        return -1;
//    }

//    map_session_.insert(make_pair(session_id, session));
//    return 0;
//}

//void LoginApp::remove_session( const string &session_id )
//{
//    LOG(INFO)("remove_session id:%s", session_id.c_str());
//    map_session_.erase(session_id);
//}


