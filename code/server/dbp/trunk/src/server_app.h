/**
* file: server_app.h
* desc: server app.
* auth: bbwang
* date: 2015/3/3
*/

#ifndef LOGIN_SERVER_APP_H_
#define LOGIN_SERVER_APP_H_
#include "comm.h"
#include "timeoutmanager.h"

#include "binpacket_wrap.h"
#include "libco_data_type.h"

using namespace utils;
//using namespace common;

// idÉú³É
uint32_t getAutoId();


// ============ config define ============
struct Config
{
    // log
    std::string log_path;
    std::string log_rank;
    uint32_t    log_size;
    std::string log_isbuf;
    std::string log_lip;
    std::string log_lport;
    std::string log_ip;
    std::string log_port;

    // daemon
    uint32_t    dae_uid;
	// error redict file
	string err_file;
	
    // udp cmd line
    std::string udp_cmd_ip;
    std::string udp_cmd_port;
    uint32_t    udp_cmd_timeout;

    // system info
    uint32_t    sys_id;
    std::string sys_name;
    std::string sys_ip;
    std::string sys_port;
    uint32_t    sys_ttl;
    uint32_t    sys_ftl;
    uint32_t    sys_timeout;
    uint32_t    sys_buf_size;
    uint32_t    sys_libco_timeout;
    uint32_t    sys_max_co_num;
    uint32_t    session_strategy;
    uint32_t    group_strategy;

	// dbagent config
    std::string dbagent_service_name;
    std::vector<Server_Info> dbagent_srv_infos;
	
	// redis proxy config
//    std::string redis_proxy_service_name;
//    std::vector<Server_Info> redis_proxy_infos;

} ;


int InitConfig(const std::string &cfg_path, Config& config);

extern Config config;
// define session name

extern std::string CLIENT_SESSION;
extern std::string DBAGENT_SERVICE;
//extern std::string REDIS_PROXY;

class Net_Manager;
class Option;
class MsgFactory;
class ServerApp : public TimeoutEvent
{
public:
    
    ~ServerApp();
    static ServerApp* Instance();
    int open(const Config &config);
    int open_acceptor(const Config &config);
	int open_dbagent_service(const Config &config);
//	int open_redis_proxy_service(const Config &config);
    virtual void handle_timeout(int id,void *userData);
    int run_service();

    void RegistFactory(const Config &config);
    void UnregistFactory(const Config &config);

//    ClientSession *get_session(const string &session_id);
//    int add_session( const string &session_id, ClientSession *session);
//    void remove_session(const string &session_id);


// make singleton, noncopyable
private:
    ServerApp();
    ServerApp(const ServerApp&);
    ServerApp &operator=(const ServerApp &);

private:
    Net_Manager    *net_manager_;
    Option         *cli_option_;
    MsgFactory     *cli_factory_;
    Option         *dbagent_option_;
    MsgFactory     *dbagent_factory_;
//    Option         *redisproxy_option_;
//    MsgFactory     *redisproxy_factory_;
    Option         *udp_cmd_option_;
//	MysqlDatabaseImpl *mysql_impl_;
//	MysqlConnPool     *con_pool_;

//    std::map<string, ClientSession*> map_session_;
};




#endif //BCON_SERVER_APP_H_

