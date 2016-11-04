/**
 * @filedesc: 
 * server_app.h, app file
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/8 21:52:59
 * @modify:
 *
**/

#ifndef SERVER_APP_H__
#define SERVER_APP_H__

#include "headers.h"

using namespace utils;

extern uint32_t beat_seq;
// declare
class UdpCmdSession;
class ClientSession;
class DataSession;
class UserCenterSession;
class TaskCenterSession;
class DbpSession;
class Binary_Splitter;
class Net_Manager;

typedef Net_Acceptor<ClientSession, TCP_SESSION>    TCP_CLIENT_ACCEPTOR;
typedef Net_Acceptor<UdpCmdSession, UDP_SESSION>    UDP_CLIENT_ACCEPTOR;
typedef Net_Connector<DataSession>                  TCP_DATA_CONNECTOR;
typedef Net_Connector<UserCenterSession>            TCP_USER_CENTER_CONNECTOR;
typedef Net_Connector<TaskCenterSession>            TCP_TASK_CENTER_CONNECTOR;
typedef Net_Connector<DbpSession>                   TCP_DBP_CONNECTOR;


uint32_t get_login_seq();
int bin2ascii(const string &bin_str, string &ascii_str);
int ascii2bin(const string &ascii_str, string &bin_str);

void MakeKeyRand(char *key);

class ClientSplitter_ : public Binary_Splitter
{
	int split( const char *buf, int len, int &packet_begin, int &packet_len );
};

class ServerApp :public TimeoutEvent
{
public:
    int open();
    int open_acceptor();

    static ServerApp* Instance()
    {
        static  ServerApp instance;
        return &instance;
    }

    int connect_serv(INET_Addr addr, Net_Session *&session);

    Net_Session *get_session(const string &session_id);

    int add_session( const string &session_id, Net_Session *session);

    void remove_session(const string &session_id);

	// 启动登录流程，向登录列表添加一个用户
	void add_to_sign_list( ClientSession * session );
	// 登录流程结束，将用户从登录列表删除
	void remove_from_sign_list( ClientSession * session );
	// 从登录列表中获取用户
	ClientSession *get_from_sign_list( uint32_t login_seq );

    ClientSession *get_uid_session(uint32_t uid);
    void add_uid_session(ClientSession * session);
    void remove_uid_session(ClientSession * session);
    void move_session_to_tail(ClientSession *session);

    inline Binary_Splitter *splitter(){return &splitter_;}

    int run_service();

    char *get_buffer();
    void reset_buffer();

    virtual void handle_timeout(int id,void *userData);
    
    TCP_DATA_CONNECTOR &data_connector(){return data_connector_;}
    TCP_USER_CENTER_CONNECTOR &user_center_connector(){return user_center_connector_;}
    TCP_TASK_CENTER_CONNECTOR &task_center_connector(){return task_center_connector_;}
    TCP_DBP_CONNECTOR &dbp_center_connector(){return dbp_connector_;}
    const vector<DataSession*> &get_data_session() const {return list_data_sesison_;}
    const vector<UserCenterSession*> &get_user_center_session() const {return list_user_center_sesison_;}
    const vector<TaskCenterSession*> &get_task_center_session() const {return list_task_center_sesison_;}
    const vector<DbpSession*> &get_dbp_session() const {return list_dbp_sesison_;}

private:
    int open_dataservice();
    int open_user_center_service();
    int open_task_center_service();
    int open_dbp_service();
    void remove_user_center_session();
	void remove_task_center_session();
	void remove_dbp_session();

    // make sure singleton
    ServerApp(void);
    ~ServerApp(void);
    ServerApp &operator=(const ServerApp &other);

public:
    TCP_CLIENT_ACCEPTOR              client_accept_;
    UDP_CLIENT_ACCEPTOR              udp_client_accept_;
    TCP_DATA_CONNECTOR               data_connector_;
    TCP_USER_CENTER_CONNECTOR        user_center_connector_;
    TCP_TASK_CENTER_CONNECTOR        task_center_connector_;
    TCP_DBP_CONNECTOR                dbp_connector_;
    ClientSplitter_                  splitter_;
    Net_Manager                      *net_manager_;

    // client sessions
    list_lru_hash<ClientSession>     client_signing_list_; // 正在登陆的用户列表
    list_lru_hash<ClientSession>     client_session_list_;// 登录成功的用户列表

    // server sessions
    vector<DataSession*>             list_data_sesison_;
    vector<UserCenterSession*>       list_user_center_sesison_;
    vector<TaskCenterSession*>       list_task_center_sesison_;
    vector<DbpSession*>              list_dbp_sesison_;
    std::map<string, Net_Session*>   map_session_;

    // send buffer, all session use this buffer
    char                            *send_buffer_;

};


#endif // SERVER_APP_H__

