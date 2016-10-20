#include "server_config.h"
#include "server_app.h"
#include <sstream>
using namespace common;

namespace utils{

// init max buffer
map<string, uint32_t> ServerConfig::max_buffer_size;


ServerConfig::ServerConfig(void)
    : sys_id_(0)
    , db_host_("127.0.0.1")
    , db_port_(3306)
    , db_user_("root")
    , db_passwd_("")
    , db_charset_("")
    , db_conn_num_(1)
    , db_timeout_(2)
{
    allow_list_.push_back("127.0.0.1");
    check_timeout_interval_ = 10;
    client_ttl_ = 10;
    daemon_ = true;

    tGeneralInfo_.conn_time_out = 5;
    tGeneralInfo_.heart_beat_interval = 2;
    tGeneralInfo_.reconnect_interval = 2;
    tGeneralInfo_.time_out = 20;

    max_buffer_size.insert(make_pair("client", 1024*_1KB));
    max_buffer_size.insert(make_pair("imd", 20*_1KB));
    max_buffer_size.insert(make_pair("msg_center", 20*_1KB));
    max_buffer_size.insert(make_pair("dbproxyd", 1024*_1KB));
    max_buffer_size.insert(make_pair("qgroupd", 20*_1KB));
    max_buffer_size.insert(make_pair("ngroupd", 20*_1KB));
}

ServerConfig::~ServerConfig(void)
{
    delete appConfig;
    appConfig = NULL;
}

bool ServerConfig::load_file(string file)
{
    bool ret = true;
    ONCE_LOOP_ENTER
        appConfig = new TiXmlDocument;
        ret = appConfig->LoadFile(file);
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
    TiXmlElement *linuxElement = RootElement->FirstChildElement("linux");
    CHECK_ERROR_RETURN((linuxElement == NULL), false, "load_daemon_info get linux element falied");
    TiXmlElement *childElement = linuxElement->FirstChildElement("uid");
    CHECK_ERROR_RETURN((childElement == NULL), false, "load_daemon_info get uid element falied");
    uid_ = atoi(childElement->GetText());

/*
    linuxElement = RootElement->FirstChildElement("partner");
    CHECK_ERROR_RETURN((linuxElement == NULL), false, "load_daemon_info get partner element falied");
    childElement = linuxElement->FirstChildElement("file");
    CHECK_ERROR_RETURN((childElement == NULL), false, "load_daemon_info get file element falied");
    partfile_ = childElement->GetText();
*/

    LOG(INFO)("load_daemon_info succeed. uid:%u.", uid_);
    return true;
}

bool ServerConfig::load_sys_info()
{
    bool ret = true;
    uint32_t buf_size = 10*_1KB;
    TiXmlElement *sys_element = RootElement->FirstChildElement("system");
    CHECK_ERROR_RETURN((sys_element == NULL), false, "load_sys_info get system element falied");

    TiXmlElement *id_element=sys_element->FirstChildElement("id");
    if (id_element == NULL)
    {
        LOG(ERROR)("load_sys_info get id element falied");
    }
    else
    {
        sys_id_ = atoi(id_element->GetText());
    }
    
    TiXmlElement *ip_element=sys_element->FirstChildElement("ip");
    if (ip_element == NULL)
    {
        LOG(ERROR)("load_sys_info get ip element falied");
    }
    else
    {
        sys_ip_ = ip_element->GetText();
    }

    TiXmlElement *port_element=sys_element->FirstChildElement("port");
    if (port_element == NULL)
    {
        LOG(ERROR)("load_sys_info get port element falied");
    }
    else
    {
        sys_port_ = port_element->GetText();
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

    TiXmlElement *session_timeout_element = sys_element->FirstChildElement("timeout");
    if (session_timeout_element == NULL)
    {
        LOG(ERROR)("load_sys_info get timeout element falied");
    }
    else
    {
        check_timeout_interval_ = atoi(session_timeout_element->GetText());
    }

    LOG(INFO)("load_sys_info success, ip:%s, port:%s"\
        ", ttl:%u, buf_size:%u, timeout:%u."
        , sys_ip_.c_str(), sys_port_.c_str()
        , client_ttl_, buf_size, check_timeout_interval_);

    return ret;
}


bool ServerConfig::reset_config()
{
    allow_list_.clear();
	return true;
}


bool ServerConfig::load_config()
{
    LOG(INFO)("ServerConfig::load config.");

    // reset config 
    reset_config();
    load_servers_info();

    return true;
}

bool ServerConfig::load_servers_info()
{
    LOG(INFO)("ServerConfig::load servers info.");

    load_sys_info();
    load_mysql_info();
    load_addr_info("udpcmd", udp_cmd_addr_);

    return true;
}


bool ServerConfig::load_server_info(const char *section
    , vector<Server_Info> &server_info
    , Server_Info default_info)
{
    uint32_t buf_size = 20*_1KB;
    TiXmlElement *section_element = RootElement->FirstChildElement(section);
    CHECK_ERROR_RETURN((section_element == NULL), false, "load_server_info get element falied");

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
    CHECK_ERROR_RETURN((section_element == NULL), false, "load_addr_info get element falied");

    TiXmlElement *ip_element=section_element->FirstChildElement("ip");
    CHECK_ERROR_RETURN((ip_element == NULL), false, "load_addr_info get ip element falied");
    const char *ip = ip_element->GetText();
    
    TiXmlElement *port_element=section_element->FirstChildElement("port");
    CHECK_ERROR_RETURN((port_element == NULL), false, "load_addr_info get port element falied");
    const char *port = port_element->GetText();
    addr = string(ip) + ":" + port;

    // todo: not use id yet, ignore it
    TiXmlElement *id_element=section_element->FirstChildElement("id");
    CHECK_ERROR_RETURN((id_element == NULL), false, "load_addr_info get id element falied");
    const char *id = id_element->GetText();

    LOG(INFO)("load %s address info success, ip:%s, port:%s, id:%s", section, ip, port, id);
    return true;
}

bool ServerConfig::load_mysql_info()
{
    bool ret = true;
    ONCE_LOOP_ENTER
        TiXmlElement *mysql_element = RootElement->FirstChildElement("mysql");
        CHECK_ERROR_BREAK((NULL == mysql_element), false, "error: mysql element is null.")
        TiXmlElement *host_element = mysql_element->FirstChildElement("host");
        CHECK_ERROR_BREAK((NULL == host_element), false, "error: errormsg no host element.")
        db_host_ = host_element->GetText();
        TiXmlElement *port_element = mysql_element->FirstChildElement("port");
        CHECK_ERROR_BREAK((NULL == port_element), false, "error: errormsg no port element.")
        db_port_ = atoi(port_element->GetText());
        TiXmlElement *user_element = mysql_element->FirstChildElement("user");
        CHECK_ERROR_BREAK((NULL == user_element), false, "error: errormsg no user element.")
        db_user_ = user_element->GetText();
        TiXmlElement *passwd_element = mysql_element->FirstChildElement("passwd");
        CHECK_ERROR_BREAK((NULL == passwd_element), false, "error: errormsg no passwd element.")
        db_passwd_ = passwd_element->GetText();
        TiXmlElement *charset_element = mysql_element->FirstChildElement("charset");
        CHECK_ERROR_BREAK((NULL == charset_element), false, "error: errormsg no charset element.")
        db_charset_ = charset_element->GetText();
        TiXmlElement *con_num_element = mysql_element->FirstChildElement("con_num");
        CHECK_ERROR_BREAK((NULL == con_num_element), false, "error: errormsg no con_num element.")
        db_conn_num_ = atoi(con_num_element->GetText());
        TiXmlElement *connect_element = mysql_element->FirstChildElement("connect");
        CHECK_ERROR_BREAK((NULL == connect_element), false, "error: errormsg no connect element.")
        db_timeout_ = _1KB*atoi(connect_element->GetText());
        LOG(INFO)("load mysql cfg:host:%s, port:%u, user:%s, passwd:%s, "\
            "char set:%s, connection num:%u, connect timeout:%u"
            , db_host_.c_str(), db_port_, db_user_.c_str(), db_passwd_.c_str(), db_charset_.c_str(), 
            db_conn_num_, db_timeout_);
    ONCE_LOOP_LEAVE
    return ret;

}


}

