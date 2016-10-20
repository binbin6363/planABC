#include "xml_parser.h"

#include <sstream>
#include "utils.h"
#include "login_server_app.h"

using namespace libco_src;

namespace utils{

XmlParser::XmlParser(void)
    : document_(NULL)
    , root_element_(NULL)
{

}

XmlParser::~XmlParser(void)
{
}

bool XmlParser::LoadXmlFile(const std::string &file_path)
{
    bool ret = true;
    ONCE_LOOP_ENTER
        SAFE_DELETE(document_);
        document_ = new TiXmlDocument;
        CHECK_ERROR_BREAK(!document_, false, "LoadXmlFile, create document failed.");
        ret = document_->LoadFile(file_path.c_str());
        CHECK_ERROR_BREAK(!ret, false, "LoadXmlFile, load file failed.");
        root_element_ = document_->RootElement();
        CHECK_ERROR_BREAK(!root_element_, false, "LoadXmlFile, get root element failed.");
    ONCE_LOOP_LEAVE
    return ret;
}

bool XmlParser::LoadServerInfo(const char *section
    , std::vector<Server_Info> &server_info
    , std::string &service_name
    , Server_Info default_info)
{
    uint32_t buf_size = 20;
    TiXmlElement *section_element = root_element_->FirstChildElement(section);
    if (section_element == NULL)
    {
        LOG(ERROR)("load_server_info get %s element falied", section);
        return false;
    }

    GetStringValue (section_element, "servicename", service_name, "UNKNOWN_SERVICE");
    default_info.service_name = service_name;
    GetIntegerValue(section_element, "connect", default_info.conn_time_out, 10);
    GetIntegerValue(section_element, "reconnect", default_info.reconnect_interval, 2);
    GetIntegerValue(section_element, "ttl", default_info.heart_beat_interval, 5);
    GetIntegerValue(section_element, "buf_size", buf_size, buf_size);
    default_info.max_buf_size = (buf_size * _1K);
    GetIntegerValue(section_element, "timeout", default_info.time_out, 5);
    GetIntegerValue(section_element, "session_strategy", default_info.session_strategy, MOD_HASH_STRATEGY);
    GetIntegerValue(section_element, "group_strategy", default_info.group_strategy, MOD_HASH_STRATEGY);

    LOG(INFO)("load %s info, service name:%s, conn_time_out:%d, reconnect_interval:%d"\
        ", heart_beat_interval:%d, recv_buf_size:%u, time_out:%d",
        section, default_info.service_name.c_str(), default_info.conn_time_out, default_info.reconnect_interval,
        default_info.heart_beat_interval, default_info.max_buf_size, default_info.time_out);

    TiXmlElement *serverdetail_element = section_element->FirstChildElement("serverdetail");
    uint32_t unNum = 0;
    Server_Info server_detail;
    server_info.clear();
    while(serverdetail_element)
    {
        unNum++;
        server_detail = default_info;
        GetStringValue(serverdetail_element, "ip", server_detail.server_ip, "127.0.0.1");
        GetStringValue(serverdetail_element, "port", server_detail.server_port);
        GetStringValue(serverdetail_element, "id", server_detail.server_id);
        GetIntegerValue(serverdetail_element, "group_id", server_detail.group_id, 0);
        if (0 == server_detail.group_id)
        {
            server_detail.group_id = (uint32_t)strtol(server_detail.server_id.c_str(), 0, 0);
            LOG(WARN)("load server:%s info, but have not group_id node in serverdetail, set default use id:%s"
                , section, server_detail.server_id.c_str());
        }
        if (server_detail.server_id.empty())
        {
            LOG(WARN)("server:%s have no id.", section);
            continue;
        }
        server_info.push_back(server_detail);
        serverdetail_element = serverdetail_element->NextSiblingElement("serverdetail");

        LOG(INFO)("load %s info, server_id:%s, server_ip:%s, server_port:%s."
                 ,section , server_detail.server_id.c_str(), server_detail.server_ip.c_str(), server_detail.server_port.c_str());
    }

    LOG(INFO)("load %s server info success, num:%u", section, (uint32_t)server_info.size());
    return true;
}

bool XmlParser::LoadDaemonInfo(Config &config)
{
    bool ret = true;
    ONCE_LOOP_ENTER
    TiXmlElement *linux_element = root_element_->FirstChildElement("linux");
    CHECK_WARN_BREAK((linux_element == NULL), false, "load_daemon_info get linux element falied");
    GetIntegerValue(linux_element, "uid", config.dae_uid);

//    linux_element = root_element_->FirstChildElement("partner");
//    CHECK_WARN_BREAK((linux_element == NULL), false, "load_daemon_info get partner element falied");
//    GetStringValue(linux_element, "file", config.dae_partfile);
    
    LOG(INFO)("load_daemon_info succeed. uid:%u", config.dae_uid);
    ONCE_LOOP_LEAVE
    return ret;
}

bool XmlParser::LoadSystemInfo(Config &config)
{
    bool ret = true;
    ONCE_LOOP_ENTER
    uint32_t buf_size = 10;
    TiXmlElement *sys_element = root_element_->FirstChildElement("system");
    CHECK_ERROR_BREAK((sys_element == NULL), false, "load_sys_info get system element falied");

    GetIntegerValue(sys_element, "id", config.sys_id);
    GetStringValue(sys_element, "servicename", config.sys_name);
    GetStringValue(sys_element, "ip", config.sys_ip);
    GetStringValue(sys_element, "port", config.sys_port);
    GetIntegerValue(sys_element, "ttl", config.sys_ttl);
    GetIntegerValue(sys_element, "ftl", config.sys_ftl);
    GetIntegerValue(sys_element, "buf_size", config.sys_buf_size, buf_size);
    config.sys_buf_size *= _1K;
    GetIntegerValue(sys_element, "timeout", config.sys_timeout, 5);
    GetIntegerValue(sys_element, "libco_timeout", config.sys_libco_timeout, 5);
    GetIntegerValue(sys_element, "max_coro_num", config.sys_max_co_num, 0);
    GetIntegerValue(sys_element, "session_strategy", config.session_strategy, SPECIFIED_STRATEGY);
    GetIntegerValue(sys_element, "group_strategy", config.group_strategy, MOD_HASH_STRATEGY);
    // 客户端的session，必须指定为按id直接选取
    
    LOG(INFO)("LoadSystemInfo succeed. id:%u, servicename:%s, ip:%s, port:%s, ttl:%u, ftl:%u"\
        ", bufsize:%u, timeout:%u, libco_timeout:%u, co max:%u.\n"
        , config.sys_id, config.sys_name.c_str(), config.sys_ip.c_str(), config.sys_port.c_str()
        , config.sys_ttl, config.sys_ftl, config.sys_buf_size
        , config.sys_timeout, config.sys_libco_timeout, config.sys_max_co_num);
    ONCE_LOOP_LEAVE
    return ret;
}

bool XmlParser::LoadUserInfo(Config &config)
{
    bool ret = true;
    ONCE_LOOP_ENTER
    TiXmlElement *user_element = root_element_->FirstChildElement("user");
    CHECK_ERROR_BREAK((user_element == NULL), false, "LoadUserInfo get user element falied");

    GetIntegerValue(user_element, "user_check_interval", config.user_check_interval, 30);
    GetIntegerValue(user_element, "user_timeout", config.user_timeout, 60);
    GetIntegerValue(user_element, "user_clean_time", config.user_clean, 90);

    ONCE_LOOP_LEAVE
    return ret;
}



void XmlParser::GetStringValue(const TiXmlElement *ele, const std::string &field
    , std::string &value, const std::string &default_str)
{
    if (NULL == ele)
    {
        LOG(ERROR)("GetStringValue, element is null.");
        return;
    }
    const TiXmlElement *target_ele = ele->FirstChildElement(field.c_str());
    if (target_ele == NULL )
    {
        LOG(WARN)("%s not exist, assign it %s", field.c_str(), default_str.c_str());
        value = default_str;
    }
    else
    {
        value = target_ele->GetText();
    }
}


void XmlParser::GetIntegerValue(const TiXmlElement *ele, const std::string &field
    , uint32_t &value, const uint32_t default_uint)
{
    std::stringstream sstream_value;
    sstream_value << default_uint;
    std::string str_value = sstream_value.str();
    GetStringValue(ele, field, str_value);
    if (str_value.empty())
    {
        value = default_uint;
    }
    else
    {
        value = atol(str_value.c_str());
    }
}

bool XmlParser::LoadLogInfo(Config &config)
{
    bool ret = true;
    ONCE_LOOP_ENTER
    const TiXmlElement *cfg_log = root_element_->FirstChildElement("log");
    if (!cfg_log && !cfg_log->FirstChildElement("file") && !cfg_log->FirstChildElement("rank"))
    {
        ret = false;
        break;
    }

    GetStringValue(cfg_log, "file", config.log_path);
    GetStringValue(cfg_log, "rank", config.log_rank, "DEBUG");
    GetIntegerValue(cfg_log, "size", config.log_size, (500 * 1024 * 1024));
    GetStringValue(cfg_log, "isbuf", config.log_isbuf, "FALSE");
    GetStringValue(cfg_log, "lip", config.log_lip);
    GetStringValue(cfg_log, "lport", config.log_lport);
    GetStringValue(cfg_log, "ip", config.log_ip);
    GetStringValue(cfg_log, "port", config.log_port);
    ONCE_LOOP_LEAVE
    return true;
}



}

