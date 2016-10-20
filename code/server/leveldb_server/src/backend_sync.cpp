/**
 * @filedesc: 
 * backend_sync.cpp, backend sync data, in another netmanager instance
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/18 10:02:59
 * @modify:
 *
**/
#include "backend_sync.h"
#include "serverconfig.h"
#include "data_type.h"
#include "leveldb_server_app.h"

using namespace common;

BackendSync::BackendSync()
    : net_manager_(NULL)
    , master_session_(NULL)
{
}

BackendSync::~BackendSync()
{
}


// 打开监听slave的端口或者连接到master
// master和slave至少启动一个，否则报错，服务启动失败
int BackendSync::Start(Net_Manager *net_manager)
{
    int ret = 0;
    net_manager_ = net_manager;

    LOG(INFO)("[backend] open salve acceptor");
    if (0 != open_slave_acceptor())
    {
        LOG(ERROR)("[backend] open_acceptor failed.");
        ret += 1;
    }

    // 实例化一个连接master的session，尝试连接master
    MASTER_CONNECTOR &master_connector = LevelDbApp::Instance()->master_connector();
    master_connector.open(net_manager_);
    LOG(INFO)("[backend] open master session");
    // open and collect master server session
    if (0 != open_master_service())
    {
        LOG(ERROR)("[backend] open_masterservice failed.");
        ret += 1;
    }

    ret = (ret <= 1) ? 0 : 1;
    if (0 != ret) {
        LOG(ERROR)("must start other a master or a slave. all failed here.");
    }
	return ret;
}


// open port for slave connect
int BackendSync::open_slave_acceptor()
{
    int ret = 0;
    SLAVE_ACCEPTOR &slave_acceptor = LevelDbApp::Instance()->slave_acceptor();
    SlaveSplitter_ &slave_splitter = LevelDbApp::Instance()->slave_splitter();
    slave_acceptor.net_manager(net_manager_);
    INET_Addr inetAddr;
    string strAddr = utils::ServerConfig::Instance()->for_slave_ip()+":"+utils::ServerConfig::Instance()->for_slave_port();
    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1){
        LOG(ERROR)("[backend] parse addr error,service address=%s", strAddr.c_str());
        return -1;
    }

    LOG(INFO)("[backend] backend sync buf size:%u, addr=%s."
        , utils::ServerConfig::max_buffer_size[SLAVE_SERVER_NAME], strAddr.c_str());
    if (slave_acceptor.open(inetAddr, &slave_splitter
        , utils::ServerConfig::max_buffer_size[SLAVE_SERVER_NAME]) != 0)
    {
        LOG(ERROR)("[backend] backend sync open failed.");
        return -1;
    }
    return ret;
}


// connect master
int BackendSync::open_master_service()
{
    LOG(INFO)("[backend] open_master_service");

    const vector<Server_Info> &list_serv = utils::ServerConfig::Instance()->server_info(MASTER_SERVER_NAME);
    if (list_serv.empty())
    {
        LOG(WARN)("[backend] no config one master, connect master failed.");
        return 0;
    }
    
    if (ONLY_ONE_MASTER != (uint32_t)list_serv.size())
    {
        LOG(ERROR)("[backend] just support only one master! open master failed.");
        return -1;
    }

    MASTER_CONNECTOR &master_connector = LevelDbApp::Instance()->master_connector();
    MasterSplitter_ &master_splitter = LevelDbApp::Instance()->master_splitter();
    const Server_Info &server_info = list_serv[0];
    
    master_session_ = new MasterSession;
    master_session_->set_serv_info(server_info);
    INET_Addr addr;

    string strAddr = server_info.server_ip+":"+server_info.server_port;
    if(FromStringToAddr(strAddr.c_str(), addr) == -1){
        LOG(ERROR)("[backend] parse addr error,service address=%s", strAddr.c_str());
        return false;
    }

    if (master_connector.connect(master_session_, addr, &master_splitter
        , server_info.conn_time_out, utils::ServerConfig::max_buffer_size[MASTER_SERVER_NAME]) != 0)
    {
        LOG(WARN)("[backend] connect master:%s failed.", server_info.print());
        // connect failed, reconnect
        master_session_->handle_close(0);
    }

    LevelDbApp::Instance()->set_master_session(master_session_);
    LOG(INFO)("[backend] connect master:%s succeed. max buf size:%u."
        , server_info.print(), utils::ServerConfig::max_buffer_size[MASTER_SERVER_NAME]);

    return 0;
}





//! 停止线程
//int BackendSync::Stop()
//{
//	m_notify_stop = true;
//	wait();

//	m_is_run = false;
//	return 0;
//}


//! 线程函数，这是后端的同步线程，千万不要和主线程抢资源
//int BackendSync::svc()
//{
//	while (m_notify_stop != true)
//	{
//        //TIME_SPEC nowTime = gettimeofday();
//        //TimeoutManager::Instance()->UpdateTimeout(nowTime);// 时间事件统一放在主线程
//        Net_Event*  ev = net_manager_->get_event();
//        if (ev)
//        {
//            ev->handler(*ev);
//            delete ev;
//        }
//        else
//        {
//            usleep(10);
//        }
//        
//	    if (m_notify_stop)
//        {
//            LOG(WARN)("stop backend sync thread!");
//            break;
//        }   
//    }
//    return 0;
//}




