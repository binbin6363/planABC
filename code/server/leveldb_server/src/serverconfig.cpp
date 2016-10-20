#include "serverconfig.h"
#include "leveldb_server_app.h"
#include <sstream>
#include "codeconverter.h"
#include "comm_operate.h"
#include "string_util.h"


extern int LOG_QUEUE_SIZE;

using namespace common;

namespace utils{

// init max buffer
map<string, uint32_t> ServerConfig::max_buffer_size;


ServerConfig::ServerConfig(void)
    : cfg_file_path_("")
    , RootElement(NULL)
    , appConfig(NULL)
    , ldb_ip_("")
    , ldb_port_("")
    , for_slave_ip_("")
    , for_slave_port_("")
    , server_id_("")
    , udp_cmd_addr_("")
    , ldb_dbpath_("")
    , ldb_create_if_missing_("false")
    , ldb_error_if_exists_("false")
    , ldb_paranoid_checks_("false")
    , ldb_write_buffer_size_(0)
    , ldb_max_open_files_(0)
    , ldb_block_size_(0)
    , ldb_block_restart_interval_(0)
    , ldb_compression_type_(0)
    , ldb_bits_per_key_(0)
    , ldb_cache_size_(0)
{
    check_timeout_interval_ = 10;
    client_ttl_ = 10;
    slave_ttl_ = 10;
    daemon_ = 0;

    tGeneralInfo_.conn_time_out = 5;
    tGeneralInfo_.heart_beat_interval = 2;
    tGeneralInfo_.reconnect_interval = 2;
    tGeneralInfo_.time_out = 20;
    sync_speed_limit_ = -1;
/*
    max_buffer_size.insert(make_pair("client", 10*_1KB));
    max_buffer_size.insert(make_pair("imd", 20*_1KB));
    max_buffer_size.insert(make_pair("msg_center", 20*_1KB));
    max_buffer_size.insert(make_pair("dbproxyd", 1024*_1KB));
    max_buffer_size.insert(make_pair("qgroupd", 20*_1KB));
    max_buffer_size.insert(make_pair("ngroupd", 20*_1KB));
*/
}

ServerConfig::~ServerConfig(void)
{
    SAFE_DELETE(appConfig);
}


string ServerConfig::file_path()
{
    return cfg_file_path_;
}


bool ServerConfig::load_file(string file)
{
    bool ret = true;
    if (file.empty()) {
        return false;
    }
    
    cfg_file_path_ = file;
    ONCE_LOOP_ENTER
        SAFE_DELETE(appConfig);
        appConfig = new TiXmlDocument;
        ret = appConfig->LoadFile(cfg_file_path_);
        if (!ret)
        {
            ret = false;
            break;
        }
        RootElement = appConfig->RootElement();
        if (!RootElement)
        {
            ret = false;
            break;
        }
    ONCE_LOOP_LEAVE
    return ret;
}

bool ServerConfig::init_log()
{
    const TiXmlElement *cfgLog = RootElement->FirstChildElement("log");
    if (!cfgLog && !cfgLog->FirstChildElement("file") && !cfgLog->FirstChildElement("rank"))
        return false;

    string logFile = cfgLog->FirstChildElement("file")->GetText();
    string logRank = cfgLog->FirstChildElement("rank")->GetText();
    cout<<"open log file:"<<(const char*)logFile.c_str()<<endl;
    LOG_INIT((const char*)logFile.c_str(),500000000,utils::L_TRACE);
    LOG_OPEN();
    LOG_SET_USEC(true);

    if (logRank == "DEBUG")
    {
        LOG_SET_LEVEL(utils::L_DEBUG);
    }
    else if (logRank == "INFO")
    {
        LOG_SET_LEVEL(utils::L_INFO);
    }
    else if (logRank == "ERROR")
    {
        LOG_SET_LEVEL(utils::L_ERROR);
    }
    return true;

}

bool ServerConfig::IsAllow(const string &remote_ip)
{
    LOG(DEBUG)("remote ip:%s", remote_ip.c_str());
    for (std::list<std::string>::const_iterator it = allow_list_.begin();
            it != allow_list_.end();
            it ++)
    {
        if (memcmp(it->c_str(), remote_ip.c_str(), std::min(it->length(), remote_ip.length())) == 0)
        {
            return true;
        }
    }

    return false;
}


bool ServerConfig::load_daemon_info()
{
    TiXmlElement *runElement = RootElement->FirstChildElement("run");
    if (runElement == NULL)
    {
        LOG(ERROR)("load_daemon_info get linux element falied");
    	 return false;
    }
    TiXmlElement *childElement = runElement->FirstChildElement("daemon");
    if (childElement == NULL)
    {
        LOG(ERROR)("load_daemon_info get uid element falied");
    	 return false;
    }
    daemon_ = atoi(childElement->GetText());

    LOG(INFO)("load_daemon_info succeed. daemon:%u ",  daemon_);
    return true;
}

bool ServerConfig::load_sys_info()
{
    bool ret = true;
    uint32_t buf_size = 10*_1KB;
    TiXmlElement *sys_element = RootElement->FirstChildElement("system");
    if (sys_element == NULL)
    {
        LOG(ERROR)("load_sys_info get system element falied");
        return false;
    }
    
    TiXmlElement *ip_element=sys_element->FirstChildElement("ip");
    if (ip_element == NULL)
    {
        LOG(ERROR)("load_sys_info get ip element falied");
    }
    else
    {
        ldb_ip_ = ip_element->GetText();
    }

    TiXmlElement *port_element=sys_element->FirstChildElement("port");
    if (port_element == NULL)
    {
        LOG(ERROR)("load_sys_info get port element falied");
    }
    else
    {
        ldb_port_ = port_element->GetText();
    }

    TiXmlElement *ttl_element = sys_element->FirstChildElement("ttl");
    if (ttl_element == NULL)
    {
        LOG(ERROR)("load_sys_info get ttl element falied");
    }
    else
    {
        client_ttl_ = atoi(ttl_element->GetText());
    }

    TiXmlElement *id_element = sys_element->FirstChildElement("id");
    if (id_element == NULL)
    {
        LOG(ERROR)("load_sys_info get id element falied");
        ret = false;
    }
    else
    {
        server_id_ = ttl_element->GetText();
    }
    
    TiXmlElement *buffer_element = sys_element->FirstChildElement("buf_size");
    if (buffer_element == NULL)
    {
        LOG(ERROR)("load_sys_info get buf_size element falied");
    }
    else
    {
        buf_size = _1KB*atoi(buffer_element->GetText());
    }
    //max_buffer_size.insert(make_pair("client", buf_size)); // insert 插入已有的键，会失败
    max_buffer_size["client"] = buf_size;

    TiXmlElement *check_timeout_interval_element = sys_element->FirstChildElement("check_timeout_interval");
    if (check_timeout_interval_element == NULL)
    {
        LOG(ERROR)("load_sys_info get check_timeout_interval element falied");
    }
    else
    {
        check_timeout_interval_ = atoi(check_timeout_interval_element->GetText());
    }

    const char *write_log_flag = "true";
    TiXmlElement *binlog_element = sys_element->FirstChildElement("binlog");
    if (binlog_element == NULL)
    {
        LOG(ERROR)("load_sys_info get binlog element falied");
    }
    else
    {
        write_log_flag = binlog_element->GetText();
        write_log_= !strcasecmp(write_log_flag, "true");
    }

    uint32_t perfman_interal = 3600;
    TiXmlElement *show_perfman_interval_element = sys_element->FirstChildElement("show_perfman_interval");
    if (show_perfman_interval_element == NULL)
    {
        LOG(ERROR)("load_sys_info get show_perfman_interval element falied");
    }
    else
    {
        perfman_interal = atoi(show_perfman_interval_element->GetText());
        LevelDbApp::Instance()->set_perfman_interal(perfman_interal);
    }

    LOG(INFO)("load_sys_info success, ip:%s, port:%s, ttl:%u, buf_size:%u, "\
        "check_timeout_interval_:%u, write binlog:%s, show perfman interal:%us."
        , ldb_ip_.c_str(), ldb_port_.c_str(), client_ttl_, buf_size
        , check_timeout_interval_, write_log_flag, perfman_interal);

    return ret;
}


bool ServerConfig::reset_config()
{
    allow_list_.clear();
    allow_list_.push_back("127.0.0.1");
	return true;
}


bool ServerConfig::load_config()
{
    LOG(INFO)("ServerConfig::load config.");

    // reset config 
    reset_config();
    load_servers_info();

    load_allow_list();

    return true;
}

bool ServerConfig::load_servers_info()
{
    LOG(INFO)("ServerConfig::load servers info.");

    load_sys_info();
    load_leveldb_info();
    load_addr_info(UDP_CMD_NAME, udp_cmd_addr_);

    load_slave_info();
    load_server_info(MASTER_SERVER_NAME, vtMasterInfo_, tGeneralInfo_);

    return true;
}


bool ServerConfig::load_server_info(const char *section
    , vector<Server_Info> &server_info
    , Server_Info default_info)
{
    uint32_t buf_size = 20*_1KB;
    TiXmlElement *section_element = RootElement->FirstChildElement(section);
    if (section_element == NULL)
    {
        LOG(ERROR)("load_server_info get %s element falied", section);
        return false;
    }

    TiXmlElement *connect_element = section_element->FirstChildElement("connect");
    if (connect_element == NULL)
    {
        LOG(ERROR)("load_server_info get connect element falied");
    }
    else
    {
        default_info.conn_time_out = atoi(connect_element->GetText());
    }

    TiXmlElement *reconnect_element = section_element->FirstChildElement("reconnect");
    if (reconnect_element == NULL)
    {
        LOG(ERROR)("load_server_info get reconnect element falied");
    }
    else
    {
        default_info.reconnect_interval = atoi(reconnect_element->GetText());
    }

    TiXmlElement *ttl_element = section_element->FirstChildElement("ttl");
    if (ttl_element == NULL)
    {
        LOG(ERROR)("load_server_info get ttl element falied");
    }
    else
    {
        default_info.heart_beat_interval = atoi(ttl_element->GetText());
    }

    TiXmlElement *buf_element = section_element->FirstChildElement("buf_size");
    if (buf_element == NULL)
    {
        LOG(ERROR)("load_server_info get buf_size element falied");
    }
    else
    {
        buf_size = _1KB*atoi(buf_element->GetText());
    }
    //max_buffer_size.insert(make_pair(section, buf_size));// insert 插入已有的键，会失败
    max_buffer_size[section] = buf_size;

    TiXmlElement *timeout_element = section_element->FirstChildElement("timeout");
    if (timeout_element == NULL)
    {
        LOG(ERROR)("load_server_info get timeout element falied");
    }
    else
    {
        default_info.time_out = atoi(timeout_element->GetText());
    }

    LOG(INFO)("load %s info, conn_time_out:%d, reconnect_interval:%d, heart_beat_interval:%d, recv_buf_size:%u, time_out:%d",
              section, default_info.conn_time_out, default_info.reconnect_interval,
              default_info.heart_beat_interval, buf_size, default_info.time_out);

    TiXmlElement *serverdetail_element = section_element->FirstChildElement("serverdetail");
    uint32_t unNum = 0;
    Server_Info tServerDetail;
    server_info.clear();
    while(serverdetail_element)
    {
        unNum++;
        tServerDetail = default_info;
        TiXmlElement *childElement = serverdetail_element->FirstChildElement("ip");
        tServerDetail.server_ip =  childElement->GetText();
        childElement = serverdetail_element->FirstChildElement("port");
        tServerDetail.server_port =  childElement->GetText();
        childElement = serverdetail_element->FirstChildElement("id");
        if (NULL == serverdetail_element)
        {
            LOG(WARN)("server:%s have no id.", section);
            continue;
        }
        tServerDetail.server_id = childElement->GetText();
        server_info.push_back(tServerDetail);
        serverdetail_element = serverdetail_element->NextSiblingElement("serverdetail");

        LOG(INFO)("load %s info, server_id:%s, server_ip:%s, server_port:%s."
                 ,section , tServerDetail.server_id.c_str(), tServerDetail.server_ip.c_str(), tServerDetail.server_port.c_str());
    }

    LOG(INFO)("load %s server info success, num:%u", section, (uint32_t)server_info.size());
    return true;

}


bool ServerConfig::load_addr_info(const char *section, string &addr)
{
    addr = "";
    TiXmlElement *section_element = RootElement->FirstChildElement(section);
    if (section_element == NULL)
    {
        LOG(ERROR)("load_addr_info get %s element falied", section);
        return false;
    }

    TiXmlElement *ip_element=section_element->FirstChildElement("ip");
    if (ip_element == NULL)
    {
        LOG(ERROR)("load_addr_info get ip element falied");
        return false;
    }
    const char *ip = ip_element->GetText();
    
    TiXmlElement *port_element=section_element->FirstChildElement("port");
    if (port_element == NULL)
    {
        LOG(ERROR)("load_addr_info get port element falied");
        return false;
    }
    const char *port = port_element->GetText();
    addr = string(ip) + ":" + port;

    // todo: not use id yet, ignore it
    TiXmlElement *id_element=section_element->FirstChildElement("id");
    if (id_element == NULL)
    {
        LOG(WARN)("load_addr_info get id element falied");
        return false;
    }
    const char *id = id_element->GetText();

    LOG(INFO)("load %s address info success, ip:%s, port:%s, id:%s", section, ip, port, id);
    return true;
}

bool ServerConfig::load_leveldb_info()
{
    bool ret = true;
    ONCE_LOOP_ENTER
        TiXmlElement *leveldb_element = RootElement->FirstChildElement("leveldb");
        CHECK_ERROR_BREAK((NULL == leveldb_element), false, "error: leveldb element is null.")
        TiXmlElement *dbpath_element = leveldb_element->FirstChildElement("dbpath");
        CHECK_ERROR_BREAK((NULL == dbpath_element), false, "error: leveldb no dbpath element.")
        ldb_dbpath_ = dbpath_element->GetText();
        TiXmlElement *create_element = leveldb_element->FirstChildElement("create_if_missing");
        CHECK_ERROR_BREAK((NULL == create_element), false, "error: leveldb no create_if_missing element.")
        ldb_create_if_missing_ = create_element->GetText();
        TiXmlElement *exists_element = leveldb_element->FirstChildElement("error_if_exists");
        CHECK_ERROR_BREAK((NULL == exists_element), false, "error: leveldb no error_if_exists element.")
        ldb_error_if_exists_ = exists_element->GetText();
        TiXmlElement *checks_element = leveldb_element->FirstChildElement("paranoid_checks");
        CHECK_ERROR_BREAK((NULL == checks_element), false, "error: leveldb no paranoid_checks element.")
        ldb_paranoid_checks_ = checks_element->GetText();
        TiXmlElement *write_element = leveldb_element->FirstChildElement("write_buffer_size");
        CHECK_ERROR_BREAK((NULL == write_element), false, "error: leveldb no write_buffer_size element.")
        ldb_write_buffer_size_ = _1KB*atoi(write_element->GetText());
        TiXmlElement *open_element = leveldb_element->FirstChildElement("max_open_files");
        CHECK_ERROR_BREAK((NULL == open_element), false, "error: leveldb no max_open_files element.")
        ldb_max_open_files_ = atoi(open_element->GetText());
        TiXmlElement *block_size_element = leveldb_element->FirstChildElement("block_size");
        CHECK_ERROR_BREAK((NULL == block_size_element), false, "error: leveldb no block_size element.")
        ldb_block_size_ = _1KB*atoi(block_size_element->GetText());
        TiXmlElement *restart_interval_element = leveldb_element->FirstChildElement("block_restart_interval");
        CHECK_ERROR_BREAK((NULL == restart_interval_element), false, "error: leveldb no block_restart_interval element.")
        ldb_block_restart_interval_ = atoi(restart_interval_element->GetText());
        TiXmlElement *bits_per_key_element = leveldb_element->FirstChildElement("bits_per_key");
        CHECK_ERROR_BREAK((NULL == bits_per_key_element), false, "error: leveldb no bits_per_key element.")
        ldb_bits_per_key_ = atoi(bits_per_key_element->GetText());
        TiXmlElement *cache_size_element = leveldb_element->FirstChildElement("cache_size");
        CHECK_ERROR_BREAK((NULL == cache_size_element), false, "error: leveldb no cache_size element.")
        ldb_cache_size_ = _1MB*atoi(cache_size_element->GetText());
        TiXmlElement *compression_type_element = leveldb_element->FirstChildElement("compression_type");
        CHECK_ERROR_BREAK((NULL == compression_type_element), false, "error: leveldb no compression_type element.")
        ldb_compression_type_ = atoi(compression_type_element->GetText());
        TiXmlElement *binlog_size_element = leveldb_element->FirstChildElement("binlog_size");
        if (NULL != binlog_size_element)
        {
            int binlog_size = atoi(binlog_size_element->GetText());
            if ( binlog_size >= LOG_QUEUE_SIZE )
            {
                LOG_QUEUE_SIZE = binlog_size;
            }
            else 
            {
                LOG(WARN)("binlog_size(%d) < default_size(%d), use default.", binlog_size, LOG_QUEUE_SIZE);
            }
        }
        else
        {
            LOG(WARN)("error: leveldb no binlog_size element. use default_size(%d)", LOG_QUEUE_SIZE);
        }

        LOG(INFO)("load leveldb cfg:dbpath:%s, create_if_missing:%s, error_if_exists:%s, paranoid_checks:%s, "\
            "write_buffer_size:%uKB, max_open_files:%u, block_size:%uKB, block_restart_interval:%u, " \
            "bits_per_key:%u, cache_size:%uMB, compression_type:%u, binlog size:%d."
            , ldb_dbpath_.c_str(), ldb_create_if_missing_.c_str(), ldb_error_if_exists_.c_str()
            , ldb_paranoid_checks_.c_str(), ldb_write_buffer_size_/_1KB, ldb_max_open_files_
            , ldb_block_size_/_1KB, ldb_block_restart_interval_, ldb_bits_per_key_
            , ((0 == ldb_cache_size_) ? 8 : ldb_cache_size_/_1MB), ldb_compression_type_, LOG_QUEUE_SIZE);
    ONCE_LOOP_LEAVE
    return ret;

}

bool ServerConfig::load_slave_info()
{
    bool ret = true;
    uint32_t buf_size = 10*_1KB;
    TiXmlElement *sys_element = RootElement->FirstChildElement(SLAVE_SERVER_NAME);
    if (sys_element == NULL)
    {
        LOG(ERROR)("load_slave_info get slave element falied");
        return false;
    }
    
    TiXmlElement *ip_element=sys_element->FirstChildElement("ip");
    if (ip_element == NULL)
    {
        LOG(ERROR)("load_slave_info get ip element falied");
    }
    else
    {
        for_slave_ip_ = ip_element->GetText();
    }

    TiXmlElement *port_element=sys_element->FirstChildElement("port");
    if (port_element == NULL)
    {
        LOG(ERROR)("load_slave_info get port element falied");
    }
    else
    {
        for_slave_port_ = port_element->GetText();
    }

    TiXmlElement *ttl_element = sys_element->FirstChildElement("ttl");
    if (ttl_element == NULL)
    {
        LOG(ERROR)("load_slave_info get ttl element falied");
    }
    else
    {
        slave_ttl_ = atoi(ttl_element->GetText());
    }

    TiXmlElement *buffer_element = sys_element->FirstChildElement("buf_size");
    if (buffer_element == NULL)
    {
        LOG(ERROR)("load_slave_info get buf_size element falied");
    }
    else
    {
        buf_size = _1KB*atoi(buffer_element->GetText());
    }
    max_buffer_size[SLAVE_SERVER_NAME] = buf_size;

    TiXmlElement *speed_element = sys_element->FirstChildElement("sync_speed_limit");
    if (speed_element == NULL)
    {
        LOG(ERROR)("load_slave_info get sync_speed_limit element falied");
    }
    else
    {
        string speed_str = StringUtil::trim(speed_element->GetText());
        if (speed_str.compare("-1") == 0) {
            sync_speed_limit_ = -1;
        } else {
            sync_speed_limit_ = atoi(speed_str.c_str()); // 这个值是以MB/s存在的，不转化
        }
    }
    
    LOG(INFO)("load_slave_info success, ip:%s, port:%s"\
        ", ttl:%u, buf_size:%u, sync speed limit:%dMB/s."
        , for_slave_ip_.c_str(), for_slave_port_.c_str()
        , slave_ttl_, buf_size, sync_speed_limit_);

    return ret;

}


bool ServerConfig::load_allow_list()
{
    bool ret = true;
    allow_list_.push_back("127.0.0.1");
    ONCE_LOOP_ENTER
        TiXmlElement *allow_element = RootElement->FirstChildElement("allow_list");
        CHECK_ERROR_BREAK((NULL == allow_element), false, "error: allow_list is null.")

        TiXmlElement *ip_element = allow_element->FirstChildElement("ip");
        while(ip_element)
        {
            const char *ip = ip_element->GetText();
            allow_list_.push_back(ip);
            ip_element = ip_element->NextSiblingElement("ip");
        }
        
        LOG(INFO)("load allow_list ok.");
        list<string>::const_iterator ip_iter = allow_list_.begin();
        list<string>::const_iterator ip_end_iter = allow_list_.end();
        for (; ip_iter != ip_end_iter; ++ip_iter) {
            LOG(DEBUG)("allow ip:%s", ip_iter->c_str());
        }
    ONCE_LOOP_LEAVE
    return ret;
}

    
const vector<Server_Info> &ServerConfig::server_info(const char *server_name) const
{
    (void)server_name;
    LOG(INFO)("get server info, just can get master info!");
    return vtMasterInfo_;
}

}

