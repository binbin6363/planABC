#ifndef serverconfig_h__
#define serverconfig_h__
#include "comm.h"
#include "tinyxml.h"
#include <vector>
#include <string>
#include <list>
#include <map>
#include "binpacket_wrap.h"
#include "constants.h"
#include "errcodes.h"
#include "protocols.h"

using namespace common;
using namespace utils;
using namespace std;


namespace utils{


class ServerConfig
{

// ensure singleton
private:
    ServerConfig(void);
    ServerConfig &operator=(const ServerConfig &other);
    ServerConfig(const ServerConfig &other);

public:
    ~ServerConfig(void);
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
    // load server info, copy the 3rd parameters, can not modify generate info 
    bool load_server_info(const char *section, vector<Server_Info> &server_info, Server_Info default_info);
    // load ip and port info
    bool load_addr_info(const char *section, string &addr);
    bool load_mysql_info();
    bool load_allow_list();
    
    string udp_cmd_addr() {return udp_cmd_addr_;}

    // get server info
    const vector<Server_Info> &server_info(const char *server_name) const;// {return vtImdInfo_;}
    
    bool IsAllow(const string &remote_ip);
    string file_path();
    bool  load_file(string file);
    bool load_config();
    
    uint32_t check_timeout_interval() const {return check_timeout_interval_;};
    
    const string &sys_ip() const {return sys_ip_;}
    const string &sys_port() const {return sys_port_;}
    bool isDaemon() const {return daemon_;}
    uint32_t client_ttl() const {return client_ttl_;}

	const string &db_host(){return db_host_;}
	uint32_t db_port(){return db_port_;}
	const string &db_user(){return db_user_;}
	const string &db_passwd(){return db_passwd_;}
	const string &db_charset(){return db_charset_;}
	uint32_t db_conn_num(){return db_conn_num_;}
	uint32_t db_timeout(){return db_timeout_;}
    uint32_t uid(){return uid_;}
    
private:
    string                    cfg_file_path_;
    TiXmlElement              *RootElement;
    TiXmlDocument             *appConfig;
    list<string>              allow_list_;
    Server_Info               tGeneralInfo_;

    vector<Server_Info>       vtLevelDbInfo_;
    vector<Server_Info>       vtMasterInfo_;
    uint32_t                  check_timeout_interval_;      //会话超时时间,单位s
    string                    sys_ip_;
    string                    sys_port_;
    uint32_t                  sys_id_;
    uint32_t                  uid_;

    uint32_t                  client_ttl_;
    
    bool                      daemon_; // true daemon, false not daemon

//    string                    udp_addr_;
//    string                    udp_log_addr_;
    string                    udp_cmd_addr_;

    string db_host_;
    uint32_t db_port_;
    string db_user_;
    string db_passwd_;
    string db_charset_;
    uint32_t db_conn_num_;
    uint32_t db_timeout_;
public:

	static map<string, uint32_t> max_buffer_size;
};



}
#endif // serverconfig_h__

