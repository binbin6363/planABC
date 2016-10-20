/**
 * @filedesc: 
 * server_config.h, handle xml config file
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/8 21:52:59
 * @modify:
 *
**/

#include "server_config.h"
#include <sstream>
#include "server_app.h"
#include "codeconverter.h"
#include "tinyxml.h"

namespace utils{


name_id server_maps[] = {
      name_id(LOGIN_SERVER_NAME, LOGIN_SERVER)
    , name_id(MSG_SERVER_NAME, MSG_SERVER)
    , name_id(DATA_SERVER_NAME, DATA_SERVER)
    , name_id(USER_CENTER_SERVER_NAME, USER_CENTER_SERVER)
    , name_id(STATUS_SERVER_NAME, STATUS_SERVER)
};

name_id client_maps[] = {
    name_id("cmd", CMD_CLIENT)
};

// init max buffer
map<string, uint32_t> ServerConfig::max_buffer_size;


ServerConfig::ServerConfig(void)
    : base_cfg_file_("")
    , partfile_(NULL)
    , proce_msgcenter_cmd_(true)
	, offlinemsg_timer_interval_( 1 )
	, msg_read_req_overtime_( 5 )
	, groupmsg_timer_interval_( 1 )
	, group_read_req_overtime_( 3 )
{
    allow_list_.push_back("127.0.0.1");
    session_timeout_ = 10;
    mobiled_ttl_ = 10;
    mobiled_ftl_ = 60;
    server_daemon_ = true;
    max_user_num_ = 10000;
    max_conn_num_ = 15000;

    tGeneralInfo_.conn_time_out = 5;
    tGeneralInfo_.heart_beat_interval = 2;
    tGeneralInfo_.reconnect_interval = 2;
    tGeneralInfo_.time_out = 20;

    max_buffer_size.insert(make_pair(CLIENT_NODE_NAME, 10*1024));
    max_buffer_size.insert(make_pair(LOGIN_SERVER_NAME, 20*1024));
    max_buffer_size.insert(make_pair(MSG_SERVER_NAME, 20*1024));
    max_buffer_size.insert(make_pair(DATA_SERVER_NAME, 1024*1024));
    max_buffer_size.insert(make_pair(USER_CENTER_SERVER_NAME, 1024*1024));
    max_buffer_size.insert(make_pair(STATUS_SERVER_NAME, 20*1024));

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
	    base_cfg_file_ = file;
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

bool ServerConfig::reload_file()
{
    if( base_cfg_file_.empty() )
    {
        LOG(ERROR)("reload_file base_cfg_file_ is empty.");
        return false;
    }
    
    // clear
    if( appConfig != NULL )
    {
        appConfig->Clear();
    }
    SAFE_DELETE( appConfig );
    RootElement = NULL;
    
    // reopen
    appConfig = new TiXmlDocument();
    if( NULL == appConfig )
    {
        LOG(ERROR)("reload_file new appConfig failed.");
        return false;
    }
    
    if(!appConfig->LoadFile(base_cfg_file_.c_str()))
    {
        LOG(ERROR)("reload_file open file failed. base_cfg_file_:%s", base_cfg_file_.c_str());
        return false;
    }
   
    RootElement = appConfig->RootElement();
    if( NULL == RootElement )
    {
        LOG(ERROR)("reload_file get root element failed. base_cfg_file_:%s", base_cfg_file_.c_str());
        return false;
    }

    return true;
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
    if (linuxElement == NULL)
    {
        LOG(ERROR)("load_daemon_info get linux element falied");
    	 return false;
    }
    TiXmlElement *childElement = linuxElement->FirstChildElement("uid");
    if (childElement == NULL)
    {
        LOG(ERROR)("load_daemon_info get uid element falied");
    	 return false;
    }
    uid_ = atoi(childElement->GetText());

    linuxElement = RootElement->FirstChildElement("partner");
    if (linuxElement == NULL)
    {
        LOG(ERROR)("load_daemon_info get partner element falied");
    	return false;
    }
    childElement = linuxElement->FirstChildElement("file");
    if (childElement == NULL)
    {
        LOG(ERROR)("load_daemon_info get file element falied");
    	 return false;
    }
    partfile_ = childElement->GetText();
    
    LOG(INFO)("load_daemon_info succeed. uid:%u, partfile:%s.", uid_, partfile_);
    return true;
}

bool ServerConfig::load_sys_info()
{
    bool ret = true;
    uint32_t buf_size = 10*1024;
    TiXmlElement *sys_element = RootElement->FirstChildElement("system");
    if (sys_element == NULL)
    {
        LOG(ERROR)("load_sys_info get system element falied");
        return false;
    }

    LOG(DEBUG)("load_sys_info get maxuser element");
    TiXmlElement *maxuser_element=sys_element->FirstChildElement("maxuser");
    if (maxuser_element == NULL)
    {
        LOG(ERROR)("load_sys_info get maxuser element falied");
    }
    else
    {
        max_user_num_ = atoi(maxuser_element->GetText());
    }

    LOG(DEBUG)("load_sys_info get maxcon element");
    TiXmlElement *maxcon_element=sys_element->FirstChildElement("maxcon");
    if (maxcon_element == NULL)
    {
        LOG(ERROR)("load_sys_info get maxcon element falied");
    }
    else
    {
        max_conn_num_ = atoi(maxcon_element->GetText());
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
        mobiled_ttl_ = atoi(ttl_element->GetText());
    }

    TiXmlElement *buffer_element = sys_element->FirstChildElement("buf_size");
    if (buffer_element == NULL)
    {
        LOG(ERROR)("load_sys_info get buf_size element falied");
    }
    else
    {
        buf_size = 1024*atoi(buffer_element->GetText());
    }
    //max_buffer_size.insert(make_pair("client", buf_size)); // insert 插入已有的键，会失败
    max_buffer_size[CLIENT_NODE_NAME] = buf_size;

    TiXmlElement *ftl_element = sys_element->FirstChildElement("ftl");
    if (ftl_element == NULL)
    {
        LOG(ERROR)("load_sys_info get ftl element falied");
    }
    else
    {
        mobiled_ftl_ = atoi(ftl_element->GetText());
    }

    TiXmlElement *session_timeout_element = sys_element->FirstChildElement("timeout");
    if (session_timeout_element == NULL)
    {
        LOG(ERROR)("load_sys_info get timeout element falied");
    }
    else
    {
        session_timeout_ = atoi(session_timeout_element->GetText());
    }

    LOG(INFO)("load_sys_info success, maxuser:%u, maxcon:%u, ip:%s, port:%s"\
		", ttl:%u, buf_size:%u, ftl:%u, timeout:%u."
        , max_user_num_, max_conn_num_, sys_ip_.c_str(), sys_port_.c_str()
        , mobiled_ttl_, buf_size, mobiled_ftl_, session_timeout_);

    return ret;
}


bool ServerConfig::reset_config()
{
    allow_list_.clear();
    vtLoginInfo_.clear();
    vtMsgcenterInfo_.clear();
    vtDataInfo_.clear();
    vtStatusInfo_.clear();
    udp_addr_.clear();
    udp_cmd_addr_.clear();
    udp_log_addr_.clear();
    
	return true;
}


bool ServerConfig::load_config()
{
    LOG(INFO)("ServerConfig::load config.");

    // reset config 
    reset_config();
    load_servers_info();
    load_msg_file();
    load_err_msg();
    load_maintenance();

    return true;
}

bool ServerConfig::reload_config()
{
    if(!reload_file())
        return false;

    return load_config();
}

bool ServerConfig::load_servers_info()
{
    LOG(INFO)("ServerConfig::load servers info.");

    load_sys_info();
    // logind server
    load_server_info(LOGIN_SERVER_NAME, vtLoginInfo_, tGeneralInfo_);
    // msg_center server
    load_server_info(MSG_SERVER_NAME, vtMsgcenterInfo_, tGeneralInfo_);
    // datad server
    load_server_info(DATA_SERVER_NAME, vtDataInfo_, tGeneralInfo_);
    // statusd service
    load_server_info(STATUS_SERVER_NAME, vtStatusInfo_, tGeneralInfo_);
    // user center service
    load_server_info(USER_CENTER_SERVER_NAME, vtUsereCenterInfo_, tGeneralInfo_);

    load_cond_info();
//    load_upload_server();
//    load_udp_transfer_server();
    load_addr_info(UDP_CMD_NAME, udp_cmd_addr_);

    return true;
}

bool ServerConfig::load_cond_info()
{
    bool ret = true;
	TiXmlElement *condElement = RootElement->FirstChildElement(COND_NODE_NAME);
	if (condElement == NULL)
	{
        LOG(ERROR)("load_cond_dev get cond element falied");
		return false;
	}
	TiXmlElement *childElement = condElement->FirstChildElement("id");
	if (childElement == NULL)
	{
        LOG(ERROR)("load_cond_dev get cond_dev element falied");
        ret = false;
	}
    {
        cond_id_ = atoi(childElement->GetText());
    }   
    
    childElement = condElement->FirstChildElement("cond_dev");
	if (childElement == NULL)
	{
        LOG(ERROR)("load_cond_dev get cond_dev element falied");
        ret = false;
	}
    {
	    allow_dev_ = atoi(childElement->GetText());
	}
    LOG(INFO)("cond dev type:%u, cond_id:%u", allow_dev_, cond_id_);
	
    return ret;

}

bool ServerConfig::forceUpdateServersInfo()
{
    LOG(INFO)("ServerConfig::forceUpdateServersInfo is triggered");
    load_servers_info();
//    ServerApp::Instance()->updateImdServerInfo();
    return true;
}

//bool ServerConfig::load_upload_server()
//{
//	TiXmlElement* pServerList = RootElement->FirstChildElement("UploadList");
//	if (NULL == pServerList)
//	{
//        LOG(ERROR)("load_upload_server get uploadlist element falied");
//		return false;
//	}

//	upload_server_list_.clear();
//	TiXmlElement* pServer = pServerList->FirstChildElement("UploadServer");
//	THostAddr stHostAddr;
//	while (pServer)
//	{
//		TiXmlElement* pIp = pServer->FirstChildElement("ip");
//		TiXmlElement* pPort = pServer->FirstChildElement("port");
//		TiXmlElement* pType = pServer->FirstChildElement("type");

//		if (pIp && pPort && pType)
//		{
//			inet_pton(AF_INET, pIp->GetText(), &stHostAddr.uHostIp);
//			stHostAddr.uHostPort = htons(static_cast<short>(atoi(pPort->GetText())));
//			stHostAddr.uNetType = (uint32_t)atoi(pType->GetText());
//			upload_server_list_.push_back(stHostAddr);
//		}
//		pServer = pServer->NextSiblingElement();
//	}

//	return true;
//}

//bool ServerConfig::load_udp_transfer_server()
//{
//	TiXmlElement* pServerList = RootElement->FirstChildElement("UdpTransferList");
//	if (!pServerList)
//	{
//        LOG(ERROR)("load_udp_transfer_server get udptransferlist element falied");
//		return false;
//	}

//	udp_transfer_list_.clear();
//	TiXmlElement* pServer = pServerList->FirstChildElement("UdpTransferServer");
//	THostAddr stHostAddr;
//	while (pServer)
//	{
//		TiXmlElement* pIp = pServer->FirstChildElement("ip");
//		TiXmlElement* pPort = pServer->FirstChildElement("port");

//		if (pIp && pPort)
//		{
//			inet_pton(AF_INET, pIp->GetText(), &stHostAddr.uHostIp);
//			stHostAddr.uHostPort = htons(static_cast<short>(atoi(pPort->GetText())));
// 			LOG(INFO)("Load Udptransfer config, ip:%s, port:%s"
//				, pIp->GetText(), pPort->GetText());
//			udp_transfer_list_.push_back(stHostAddr);
//		}
//		pServer = pServer->NextSiblingElement();
//	}
//	return true;
//}


bool ServerConfig::load_server_info(const char *section
                                    , vector<Server_Info> &server_info
                                    , Server_Info default_info)
{
    uint32_t buf_size = 20*1024;
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
        buf_size = 1024*atoi(buf_element->GetText());
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

// return 0 is error
uint32_t ServerConfig::id_from_sname(const char *server_name) const 
{
    int size = sizeof(server_maps)/sizeof(name_id);
    for (int i = 0; i < size; ++i)
        {
        if (0 == strcmp(server_name, server_maps[i].name()))
            return server_maps[i].id();
        }
    return 0;
}

const vector<Server_Info> &ServerConfig::server_info(const char *server_name) const
{
    int id = id_from_sname(server_name);
    switch (id)
        {
        case LOGIN_SERVER:
            return vtLoginInfo_;
        case MSG_SERVER:
            return vtMsgcenterInfo_;
        case DATA_SERVER:
            return vtDataInfo_;
        case STATUS_SERVER:
            return vtStatusInfo_;
        case USER_CENTER_SERVER:
            return vtUsereCenterInfo_;
        default:
            return vtNullInfo_;    
        }
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

bool ServerConfig::load_udp_server()
{
   LOG(INFO)("load_udp_server | do nothing.");
   return true;
}


bool ServerConfig::check_update_element()
{
    bool ret = true;
    TiXmlElement *update_element = RootElement->FirstChildElement("update");
    if (update_element == NULL)
    {
        LOG(DEBUG)("no update element");
        ret = false;
        return ret;
    }
    TiXmlElement *file_element = update_element->FirstChildElement("file");
    if (file_element == NULL)
    {
        LOG(DEBUG)("no update file element");
        ret = false;
        return ret;
    }
    update_file_ = file_element->GetText();
    return ret;
}

bool ServerConfig::check_strategy_ok(int strategy)
{
    bool ret = true;
    if (strategy >= NO_UPDATE && strategy < MAX_UPDATE)
    {
        ret = true;
        LOG(INFO)("force update strategy valild, strategy:%u", strategy);
    }
    else
    {
        ret = false;
        LOG(ERROR)("force update strategy invalild, strategy:%u", strategy);
    }
    return ret;
}

bool ServerConfig::load_update_info()
{
    bool ret = true;
    ONCE_LOOP_ENTER
    if (0 != access(update_file_.c_str(), F_OK))
    {
        LOG(WARN)("load_update_info config file:%s not exist!", update_file_.c_str());
        break;
    }

    TiXmlDocument *document = new TiXmlDocument(update_file_.c_str());
    CHECK_ERROR_BREAK((NULL == document), false, "load_update_info, new TiXmlDocument failed")
    document->LoadFile();
    TiXmlElement *RootElement = document->RootElement();
    CHECK_ERROR_BREAK((NULL == RootElement), true, "update file not exist root element. xml file format error. not load update info")

    // 重置强制升级策略
    force_strategy = 0;
    // 重置升级信息
    cond_update_msg.clear();
    // 重置强制升级的版本
    version_build_force_update.clear();
    // 重置建议升级的版本
    version_build_tips_update.clear();

    // 加载升级策略
    TiXmlElement * strategy_element = RootElement->FirstChildElement("force_strategy");
    CHECK_ERROR_BREAK((NULL == strategy_element), false, "update file, not exist force_strategy element.")
    string str_tmp = strategy_element->GetText();
    force_strategy = atoi(str_tmp.c_str());
    if (!check_strategy_ok(force_strategy))
    {
        LOG(ERROR)("force_strategy element error, it must valued between [%u, %u), force update invalid.", NO_UPDATE, MAX_UPDATE);
        break;
    }
    
    TiXmlElement * url_element = NULL;
    TiXmlElement * ver_element = NULL;
    int ele_strategy = 0;
    string str_ver = "";
    int iBuild = 0;
    int iDev = 0;
    CCodeConverter converter;
    version_build_info build_info;
    TiXmlElement * force_element = RootElement->FirstChildElement("forceUpdate");
    CHECK_ERROR_BREAK((NULL == force_element), false, "update file, not exist forceUpdate element.")
    for (; force_element != NULL; )
    {
        force_element->Attribute("strategy", &ele_strategy);
        if (force_strategy == (uint32_t)ele_strategy)
        {
            // fetch iosUpdateURL
            url_element  = force_element->FirstChildElement("iosUpdateURL");
            CHECK_ERROR_BREAK((NULL == url_element), false, "update file, forceUpdate not exist iosUpdateURL element.")
            str_tmp = url_element->GetText();
            LOG(INFO)("read force ios update data Url:%s", str_tmp.c_str());
            converter.Convert("utf-8", "unicodelittle", str_tmp, cond_update_msg[IOS_FORCE_UPDATE]);

            // fetch andUpdateURL
            url_element = force_element->FirstChildElement("andUpdateURL");
            CHECK_ERROR_BREAK((NULL == url_element), false, "update file, forceUpdate not exist andUpdateURL element.")
            str_tmp = url_element->GetText();
            LOG(INFO)("read force android update data Url:%s", str_tmp.c_str());
            converter.Convert("utf-8", "unicodelittle", str_tmp, cond_update_msg[ANDROID_FORCE_UPDATE]);

            // fetch version
            ver_element  = force_element->FirstChildElement("version");
            CHECK_ERROR_BREAK((NULL == ver_element), false, "update file, forceUpdate not exist version element.")
            for (; NULL != ver_element; )
            {
                build_info.reset();
                str_ver = ver_element->GetText();
                iBuild = 0;
                iDev = 0;
                CHECK_ERROR_BREAK((NULL == ver_element->Attribute("build", &iBuild)), false, "error: update file build element is null.")
                CHECK_ERROR_BREAK((NULL == ver_element->Attribute("dev", &iDev)), false, "error: update file dev element is null.")
                build_info.un8Dev    = (uint8_t)iDev;
                build_info.un16Build = (uint16_t)iBuild;
                build_info.un8Version= (uint8_t)atoi(str_ver.c_str());
                LOG(DEBUG)("this version will be forced update:dev:%u, build:%u, version:%u"
                    , build_info.un8Dev, build_info.un16Build, build_info.un8Version);
                version_build_force_update.push_back(build_info);
                ver_element = ver_element->NextSiblingElement("version");
            }
            break;
        }
        force_element = force_element->NextSiblingElement("forceUpdate");
    }
    
    TiXmlElement * tips_element = RootElement->FirstChildElement("tipsUpdate");
    CHECK_ERROR_BREAK((NULL == tips_element), false, "update file, not exist tipsUpdate element.")
    url_element = tips_element->FirstChildElement("iosUpdateURL");
    CHECK_ERROR_BREAK((NULL == url_element), false, "update file, tipsUpdate not exist iosUpdateURL element.")
    str_tmp = url_element->GetText();
    LOG(DEBUG)("read tips ios update data Url:%s", str_tmp.c_str());
    converter.Convert("utf-8", "unicodelittle", str_tmp, cond_update_msg[IOS_TIPS_UPDATE]);
    url_element = tips_element->FirstChildElement("andUpdateURL");
    CHECK_ERROR_BREAK((NULL == url_element), false, "update file, tipsUpdate not exist andUpdateURL element.")
    str_tmp = url_element->GetText();
    LOG(DEBUG)("read tips and update data Url:%s", str_tmp.c_str());
    
    converter.Convert("utf-8", "unicodelittle", str_tmp, cond_update_msg[ANDROID_TIPS_UPDATE]);

    TiXmlElement * andUpInfoElement = tips_element->FirstChildElement("updateInfo");
    CHECK_ERROR_BREAK((NULL == andUpInfoElement), false, "update file, tipsUpdate not exist updateInfo element.")
    string strAndUpInfo = andUpInfoElement->GetText();
    converter.Convert("utf-8", "unicodelittle", strAndUpInfo, cond_update_msg[TIPS_UPDATE_INFO]);

    iBuild = 0;
    ver_element  = tips_element->FirstChildElement("version");
    CHECK_ERROR_BREAK((NULL == ver_element), false, "update file, tipsUpdate not exist version element.")
    for (; NULL != ver_element; )
    {
        str_ver = ver_element->GetText();
        iBuild = 0;
        iDev = 0;
        build_info.reset();
        CHECK_ERROR_BREAK((NULL == ver_element->Attribute("build", &iBuild)), false, "error: update file build element is null.")
        CHECK_ERROR_BREAK((NULL == ver_element->Attribute("dev", &iDev)), false, "error: update file dev element is null.")
        build_info.un8Dev    = (uint8_t)iDev;
        build_info.un16Build = (uint16_t)iBuild;
        build_info.un8Version= (uint8_t)atoi(str_ver.c_str());
        LOG(DEBUG)("this version will be tip update:dev:%u, build:%u, version:%u"
            , build_info.un8Dev, build_info.un16Build, build_info.un8Version);
        version_build_tips_update.push_back(build_info);
        ver_element = ver_element->NextSiblingElement("version");
    }

    ONCE_LOOP_LEAVE
    return ret;
}

bool ServerConfig::load_msg_file()
{
    bool ret = true;
    ONCE_LOOP_ENTER
        TiXmlElement *errormsg_element = RootElement->FirstChildElement("errormsg");
        CHECK_ERROR_BREAK((NULL == errormsg_element), false, "error: errormsg element is null.")
        TiXmlElement *file_element = errormsg_element->FirstChildElement("file");
        CHECK_ERROR_BREAK((NULL == file_element), false, "error: errormsg no file element.")
        err_msg_file_ = file_element->GetText();
    ONCE_LOOP_LEAVE
    return ret;
}

bool ServerConfig::load_err_msg()
{
    bool ret = true;
    LOG(INFO)("load err msg.");
    ONCE_LOOP_ENTER
        if (0 != access(err_msg_file_.c_str(), F_OK))
        {
            LOG(WARN)("load_err_msg file:%s not exist!", err_msg_file_.c_str());
            break;
        }

        g_errormsg.clear();
        TiXmlDocument *document = new TiXmlDocument(err_msg_file_.c_str());
        CHECK_ERROR_BREAK((NULL == document), false, "load_err_msg, new TiXmlDocument failed")
        document->LoadFile();
        TiXmlElement *RootElement = document->RootElement();
        CHECK_ERROR_BREAK((NULL == RootElement), true, "err msg file not exist root element. xml file format error. not load update info")
        TiXmlElement *errnum_element = RootElement->FirstChildElement("errornum");
        CHECK_ERROR_BREAK((NULL == errnum_element), true, "err msg file not exist errnum element. xml file format error. not load update info")
        int num = 0;
        string text = "";
        for (; NULL != errnum_element; )
        {
            errnum_element->Attribute("num", &num);
            text = errnum_element->GetText();
            g_errormsg[num] = text;
            LOG(INFO)("load err msg, err code:%d, msg:%s", num, g_errormsg[num].c_str());
            errnum_element = errnum_element->NextSiblingElement("errormsg");
        }
    ONCE_LOOP_LEAVE
    return ret;
}


bool ServerConfig::load_maintenance()
{
    bool ret = true;
    LOG(INFO)("load maintenance");
    // reset 
    g_maintenance_open = false;
    g_maintenance_hour = 0;
    g_maintenance_type = 0;
    ONCE_LOOP_ENTER
        TiXmlElement *cond_element = RootElement->FirstChildElement(COND_NODE_NAME);
        CHECK_ERROR_BREAK((NULL == cond_element), false, "error: cond element is null.")
        TiXmlElement *maintenance_element = cond_element->FirstChildElement("maintenance");
        CHECK_ERROR_BREAK((NULL == maintenance_element), false, "error: cond no maintenance element.")
        TiXmlElement *sub_element = maintenance_element->FirstChildElement("open");
        CHECK_ERROR_BREAK((NULL == sub_element), false, "error: cond no open element.")
        g_maintenance_open = (strcasecmp("true", sub_element->GetText()) == 0) ? true:false;
        sub_element = maintenance_element->FirstChildElement("duration_hour");
        CHECK_ERROR_BREAK((NULL == sub_element), false, "error: cond no duration_hour element.")
        g_maintenance_hour = atoi(sub_element->GetText());
        sub_element = maintenance_element->FirstChildElement("type");
        CHECK_ERROR_BREAK((NULL == sub_element), false, "error: cond no type element.")
        g_maintenance_type = atoi(sub_element->GetText());
        LOG(INFO)("load maintenance ok. maintenance_open:%u, maintenance_hour:%u, maintenance_type:%u"
            , g_maintenance_open, g_maintenance_hour, g_maintenance_type);
    ONCE_LOOP_LEAVE
    return ret;
}

}

