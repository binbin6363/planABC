/**
* file: login_server_app.cpp
* desc: server app.
* auth: bbwang
* date: 2015/11/3
*/

#include "server_app.h"
#include "session_manager.h"
#include "libco_wrap.h"
#include "option.h"
#include "translater.h"
#include "dbagent_processor.h"
#include "redis_proxy_processor.h"
#include "client_processor.h"
#include "udp_cmdline_processor.h"
#include "msg.h"
#include "factories.h"

Config config;
std::string CLIENT_SESSION("");
std::string DBAGENT_SERVICE("");
//std::string REDIS_PROXY("");


// 递增id，从50000000开始
uint32_t getAutoId()
{
    static uint32_t id = 50000000;
    return ++id;
//    uint32_t server_id = config.sys_id;
//    ++id;
//    if (0xFFFFF == id)
//    {
//        LOG(ERROR)("id used out!!!");
//        id = 0;
//    }
//    return (server_id << 20 | id);
}

ServerApp::ServerApp()
    : net_manager_(NULL)
    , cli_option_(NULL)
    , cli_factory_(NULL)
    , dbagent_option_(NULL)
    , dbagent_factory_(NULL)
    , udp_cmd_option_(NULL)
//    , redisproxy_option_(NULL)
//    , redisproxy_factory_(NULL)
{
    cli_option_ = new Option;
    cli_option_->service_name = config.sys_name.c_str();//CLIENTSESSION;
    cli_option_->translater   = ProtocTranslater::Instance();
    cli_option_->processor    = ClientProcessor::Instance();

    dbagent_option_ = new Option;
    dbagent_option_->service_name = config.dbagent_service_name.c_str();//dbagent;
    dbagent_option_->translater   = ProtocTranslater::Instance();
    dbagent_option_->processor    = DbagentProcessor::Instance();

//    redisproxy_option_ = new Option;
//    redisproxy_option_->service_name = config.redis_proxy_service_name.c_str();//redis_proxy;
//    redisproxy_option_->translater   = ProtocTranslater::Instance();
//    redisproxy_option_->processor    = RedisProxyProcessor::Instance();

    udp_cmd_option_ = new Option;
    udp_cmd_option_->translater   = CmdLineTranslater::Instance();
    udp_cmd_option_->processor    = UdpCmdLineProcessor::Instance();

}


ServerApp::~ServerApp()
{
    SAFE_DELETE(cli_option_)
    SAFE_DELETE(dbagent_option_)
    SAFE_DELETE(udp_cmd_option_);
    SAFE_DELETE(net_manager_)
}


ServerApp* ServerApp::Instance()
{
    static  ServerApp instance;
    return &instance;
}

int ServerApp::open(const Config &config)
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

    LOG(INFO)("connect dbagent service");
    if (0 != open_dbagent_service(config))
    {
        LOG(ERROR)("[start] open_dbagent_service failed.");
        return -1;
    }

//    LOG(INFO)("connect redis proxy service");
//    if (0 != open_redis_proxy_service(config))
//    {
//        LOG(ERROR)("[start] open_redis_proxy_service failed.");
//        return -1;
//    }
    return 0;
}

int ServerApp::open_acceptor(const Config &config)
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


    // udp cmd line service
    info.server_ip = config.udp_cmd_ip;
    info.server_port = config.udp_cmd_port;
    info.conn_time_out = config.udp_cmd_timeout;
    info.heart_beat_interval = config.sys_ttl;
    info.max_buf_size = config.sys_buf_size;
    info.reconnect_interval = 2;
    info.time_out = config.udp_cmd_timeout;
    info.service_name = "udpcmdline";
    LOG(INFO)("start to open udp cmdline service, info:%s", info.print());
    SessionManager::Instance().OpenUdpAcceptor(info, udp_cmd_option_);
    return 0;
}

int ServerApp::open_dbagent_service(const Config &config)
{
    LOG(INFO)("[start] open_dbagent_service");
    const vector<Server_Info> &list_serv = config.dbagent_srv_infos;
    for (vector<Server_Info>::const_iterator it=list_serv.begin(); it!=list_serv.end(); it++)
    {
        const Server_Info &info = *it;
        SessionManager::Instance().ConnectServer(info, dbagent_option_);
	    LOG(INFO)("[start] connect db:%s succeed. max buf size:%u."
            , info.print(), info.max_buf_size);
    }
    return 0;
}

//int ServerApp::open_redis_proxy_service(const Config &config)
//{
//    LOG(INFO)("[start] open_redis_proxy_service");
//    const vector<Server_Info> &list_serv = config.redis_proxy_infos;
//    for (vector<Server_Info>::const_iterator it=list_serv.begin(); it!=list_serv.end(); it++)
//    {
//        const Server_Info &info = *it;
//        SessionManager::Instance().ConnectServer(info, redisproxy_option_);
//	    LOG(INFO)("[start] connect redis proxy:%s succeed. max buf size:%u."
//            , info.print(), info.max_buf_size);
//    }
//    return 0;
//}

void ServerApp::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;
    LOG(DEBUG)("handle_timeout, do nothing");
}

int ServerApp::run_service()
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

void ServerApp::RegistFactory(const Config &config)
{
    CLIENT_SESSION = config.sys_name;
    DBAGENT_SERVICE = config.dbagent_service_name;
//    REDIS_PROXY = config.redis_proxy_service_name;
    cli_factory_ = new CliMsgFactory;
    dbagent_factory_ = new DbagentMsgFactory;
//    redisproxy_factory_ = new DbagentMsgFactory;
    GlobalFactoryRegister(CLIENT_SESSION.c_str(), cli_factory_);
    GlobalFactoryRegister(DBAGENT_SERVICE.c_str(), dbagent_factory_);
//    GlobalFactoryRegister(REDIS_PROXY.c_str(), redisproxy_factory_);
}



void ServerApp::UnregistFactory(const Config &)
{
    GlobalFactoryUnregister(CLIENT_SESSION.c_str(), cli_factory_);
    GlobalFactoryUnregister(DBAGENT_SERVICE.c_str(), dbagent_factory_);
//    GlobalFactoryUnregister(REDIS_PROXY.c_str(), redisproxy_factory_);
}


