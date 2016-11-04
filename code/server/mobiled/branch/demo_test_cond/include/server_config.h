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

#ifndef serverconfig_h__
#define serverconfig_h__
#include "headers.h"

using namespace utils;
using namespace std;


class TiXmlElement;
class TiXmlDocument;

namespace utils{

// 版本信息
typedef struct version_build_info_
{
    uint8_t un8Dev;
    uint8_t un8Version;
    uint16_t un16Build;

    version_build_info_()
        : un8Dev(0)
        , un8Version(0)
        , un16Build(0)
    {
    
    }

    void reset()
    {
        un8Dev = 0;
        un8Version = 0;
        un16Build = 0;
    }

    bool enabled()
    {
        // 只要指定了某个号，就认为已经启用
        return (0 != (un8Dev|un8Version|un16Build));
    }
    
}version_build_info;


enum MOBILE_UPDATE
{
    ANDROID_TIPS_UPDATE = 1,
    IOS_TIPS_UPDATE,
    ANDROID_FORCE_UPDATE, 
    IOS_FORCE_UPDATE,
    ANDROID_FORCE_ALL_UPDATE,
    IOS_FORCE_ALL_UPDATE,
    
    TIPS_UPDATE_INFO,
    FORCE_ALL_UPDATE_INFO,
};


typedef struct tagHostAddr
{
    uint32_t    uHostIp;        
    uint32_t    uHostPort;        //
    uint32_t    uNetType;        //        
} THostAddr, *PHostAddr;

typedef vector<THostAddr>    THostList;

enum SERVER_ID
{
    LOGIN_SERVER   = 1,
    MSG_SERVER,
    DATA_SERVER,
    USER_CENTER_SERVER,
    TASK_CENTER_SERVER,
    DBP_SERVER,
    STATUS_SERVER,
};

enum CLIENT_ID
{
    CMD_CLIENT  = 1

};


struct name_id
{
    name_id()
    {
    }
    
    name_id(const char *name, int id)
        : name_(name)
        , id_(id)
    {
    }
    const char *name()
    {
        return name_.c_str();
    }
    
    int id()
    {
        return id_;
    }
    string name_;
    int id_;

};




class ServerConfig
{

// ensure singleton
private:
    ServerConfig(void);
    ~ServerConfig(void);
    ServerConfig &operator=(const ServerConfig &other);

    uint32_t id_from_sname(const char *server_name) const;
    bool check_strategy_ok(int strategy);
    bool load_msg_file();

public:
    static ServerConfig *Instance()
    {
        static ServerConfig server_config_;
        return &server_config_;
    }

    bool reset_config();
    bool init_log();
    bool load_sys_info();
    bool load_daemon_info();
    bool load_servers_info();
    bool forceUpdateServersInfo();
    bool load_cond_info();
    bool load_upload_server();
    bool check_update_element();
    bool load_update_info();
    bool load_err_msg();
    bool load_udp_transfer_server();
    bool load_udp_server();
    // load server info, copy the 3rd parameters, can not modify generate info 
    bool load_server_info(const char *section, vector<Server_Info> &server_info, Server_Info default_info);
    // load ip and port info
    bool load_addr_info(const char *section, string &addr);

    // get server info
    const vector<Server_Info> &server_info(const char *server_name) const;// {return vtImdInfo_;}
    
    bool IsAllow(const string &remote_ip);
    bool load_config();
    bool reload_config();
    
    bool  load_file(string file);
    bool reload_file(); 
    uint32_t session_timeout() const {return session_timeout_;};
    void  session_timeout(uint32_t timeout_){session_timeout_ = timeout_;};
    bool server_daemon()const {return server_daemon_;}
    void server_daemon(bool bDaemon_){server_daemon_ = bDaemon_;};
    
    //const string &service_id() const {return service_id_;}
    const vector<Server_Info> &login_info() const {return vtLoginInfo_;}
    const vector<Server_Info> &data_info() const {return vtDataInfo_;}
    const vector<Server_Info> &status_info() const {return vtStatusInfo_;}
    const vector<Server_Info> &msg_center_info() const {return vtMsgcenterInfo_;}

    uint32_t get_cond_dev() const {return allow_dev_;};

    const THostList *upload_server() const {return &upload_server_list_;}
    const THostList *udp_transfer_server() const {return &udp_transfer_list_;}
    const char *udp_addr() const {return udp_addr_.c_str();}
    const char *udp_log_addr() const {return udp_log_addr_.c_str();}
    const char *udp_cmd_addr() const {return udp_cmd_addr_.c_str();}
    
    const string &sys_ip() const {return sys_ip_;}
    const string &sys_port() const {return sys_port_;}
    
    uint32_t cond_id() const {return cond_id_;}
    uint32_t client_ttl() const {return mobiled_ttl_;}
    uint32_t client_ftl() const {return mobiled_ftl_;}
    uint32_t uid() const {return uid_;}
    uint32_t max_user_num() const {return max_user_num_;}
    uint32_t max_con_num() const {return max_conn_num_;}
    const char *partfile() const {return partfile_;}
    
    bool load_maintenance();

    
    // buffer size setting, key:client, imd, dbproxyd, qgroupd, ngroupd, qgroupd
    static map<string, uint32_t> max_buffer_size;
    
    
 private:
    string                   base_cfg_file_;
    TiXmlElement             *RootElement;
    TiXmlDocument            *appConfig;
    list<string>             allow_list_;
    Server_Info              tGeneralInfo_;

    vector<Server_Info>      vtLoginInfo_;
    vector<Server_Info>      vtMsgcenterInfo_;
    vector<Server_Info>      vtDataInfo_;
    vector<Server_Info>      vtStatusInfo_;
    vector<Server_Info>      vtUserCenterInfo_;
    vector<Server_Info>      vtTaskCenterInfo_;
    vector<Server_Info>      vtDbpInfo_;
    vector<Server_Info>      vtNullInfo_;
    uint32_t                 session_timeout_;      //会话超时时间
    bool                      server_daemon_;
    uint32_t                  max_user_num_;         // max user number
    uint32_t                  max_conn_num_;         // max connnect number
    uint32_t                  imd_ttl_;
    string                      sys_ip_;
    string                      sys_port_;
    uint32_t                  mobiled_ttl_;
    uint32_t                  mobiled_ftl_;
    uint32_t                 allow_dev_;            // current server allow login equipment
    THostList                upload_server_list_;     // 上传服务器列表
    THostList                udp_transfer_list_;
    uint32_t                 uid_;                      // linux uid
    const char              *partfile_;
    string                    udp_addr_;
    string                    udp_log_addr_;
    string                    udp_cmd_addr_;
    bool                      proce_msgcenter_cmd_;
    uint32_t                  cond_id_;
    string                    update_file_;
    uint32_t                  resend_interval_;
    uint32_t				  offlinemsg_timer_interval_;
    uint32_t				  msg_read_req_overtime_;
    uint32_t				  groupmsg_timer_interval_;
    uint32_t				  group_read_req_overtime_;
    string                    err_msg_file_;
    map<uint32_t, string>     g_errormsg;
    uint32_t                  force_strategy;
    std::map<uint32_t, std::string>  cond_update_msg;
    std::vector<version_build_info>  version_build_force_update;
    std::vector<version_build_info>  version_build_tips_update;
    // maintenance
    bool                       g_maintenance_open;
    uint32_t                   g_maintenance_hour;
    uint32_t                   g_maintenance_type;

};



}
#endif // serverconfig_h__
