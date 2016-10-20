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

// declare
class UdpCmdSession;
class ClientSession;
class DataSession;
class Binary_Splitter;
class Net_Manager;

typedef Net_Acceptor<ClientSession, TCP_SESSION>    TCP_CLIENT_ACCEPTOR;
typedef Net_Acceptor<UdpCmdSession, UDP_SESSION>    UDP_CLIENT_ACCEPTOR;
typedef Net_Connector<DataSession>                  TCP_DATA_CONNECTOR;


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

    ClientSession *get_uid_session(int32_t uid);
    void add_uid_session(ClientSession * session);
    void remove_uid_session(ClientSession * session);
    void move_session_to_tail(ClientSession *session);

    inline Binary_Splitter *splitter(){return &splitter_;}

    int run_service();

    char *get_buffer();
    void reset_buffer();

    virtual void handle_timeout(int id,void *userData);
    
    TCP_DATA_CONNECTOR &data_connector(){return data_connector_;}
    const vector<DataSession*> &get_data_session() const {return list_data_sesison_;}

private:
    int open_dataservice();
    void remove_data_session();

    // make sure singleton
    ServerApp(void);
    ~ServerApp(void);
    ServerApp &operator=(const ServerApp &other);

public:
    TCP_CLIENT_ACCEPTOR              client_accept_;
    UDP_CLIENT_ACCEPTOR              udp_client_accept_;
    TCP_DATA_CONNECTOR               data_connector_;
    Binary_Splitter                  splitter_;
    Net_Manager                      *net_manager_;

    // client sessions
    list_lru_hash<ClientSession>     client_session_list_;// 登录成功的用户列表

    // server sessions
    vector<DataSession*>             list_data_sesison_;
    std::map<string, Net_Session*>   map_session_;

    // send buffer, all session use this buffer
    char                            *send_buffer_;

};


#endif // SERVER_APP_H__

