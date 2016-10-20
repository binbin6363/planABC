/**
* file: login_server_app.h
* desc: ban liao, server app.
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

	// user timeout
	uint32_t   user_timeout;       // 超时，状态转为后台在线，单位是秒
	uint32_t   user_check_interval;// 检测间隔，单位是秒
	uint32_t   user_clean;         // 清除用户信息的时间间隔，单位是天

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
    
    // dbp srv info
    std::string dbp_service_name;
    std::vector<Server_Info> dbp_srv_infos;

    // rds srv info
    std::string rds_service_name;
    std::vector<Server_Info> rds_srv_infos;

} ;

extern Config config;
// define session name
extern std::string DBP_SERVICE;

extern std::string CLIENT_SESSION;


class Net_Manager;
class Option;
class MsgFactory;
class RedisClient;
class LoginApp : public TimeoutEvent
{
public:
    
    ~LoginApp();
    static LoginApp* Instance();
    int open(const Config &config);
    int open_acceptor(const Config &config);
    int open_dbpservice(const Config &config);
	int open_rdsservice(const Config &config);
    virtual void handle_timeout(int id,void *userData);
    int run_service();

    void RegistFactory(const Config &config);
    void UnregistFactory(const Config &config);
	
	RedisClient *GetRedisClient(uint32_t hash_id);

//    ClientSession *get_session(const string &session_id);
//    int add_session( const string &session_id, ClientSession *session);
//    void remove_session(const string &session_id);


// make singleton, noncopyable
private:
    LoginApp();
    LoginApp(const LoginApp&);
    LoginApp &operator=(const LoginApp &);

private:
    Net_Manager    *net_manager_;
    Option         *dbp_option_;
    Option         *cli_option_;
    MsgFactory     *dbp_factory_;
    MsgFactory     *cli_factory_;
    MsgFactory     *status_factory_;
	
	vector<RedisClient*> redis_client_list_;
//    std::map<string, ClientSession*> map_session_;
};




#endif //BCON_SERVER_APP_H_

