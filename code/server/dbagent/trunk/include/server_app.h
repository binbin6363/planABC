/**
 * @filedesc: 
 * server_app.h, app file
 * @author: 
 *  bbwang
 * @date: 
 *  2014/8/3 12:02:59
 * @modify:
 *
**/

#ifndef _SERVER_APP_H_
#define _SERVER_APP_H_

#include "client_accept.h"
#include "udp_cmd_session.h"
#include "list_hash.h"

class MysqlDatabaseImpl;
class MysqlConnPool;

class ClientSplitter_ : public Packet_Splitter
{
    virtual int split(const char *buf, int len, int &packet_begin, int &packet_len);
};


class ServerApp :public TimeoutEvent
{
public:
    int open();
    int open_acceptor();

    int run_service();

    uint32_t get_new_loginseq();
    
    static ServerApp* Instance()
    {
        static  ServerApp instance;
        return &instance;
    }

    char *get_buffer();
    void reset_buffer();

	Net_Session *get_session(const string &session_id);
	int add_session( const string &session_id, Net_Session *session);
	void remove_session(const string &session_id);

    virtual void handle_timeout(int id,void *userData);
    list_lru_hash<ClientSession> &client_session_list()
    {
        return client_session_list_;
    }

	int init_mysql();
	MysqlDatabaseImpl *db_impl(){return mysql_impl_;}
		

private:
    int open_dbpservice();

    // make sure singleton
    ServerApp(void);
    ~ServerApp(void);
    ServerApp &operator=(const ServerApp &other);



public:
    ClientAccept                     client_accept_;
    Net_Acceptor<UdpCmdSession, UDP_SESSION>    udp_cmd_accept_;
    Net_Manager                     *net_manager_;
    ClientSplitter_                  splitter_;
    list_lru_hash<ClientSession>     client_session_list_;
    char                            *send_buffer_;

    std::map<string, Net_Session*>   map_session_;

	// mysql
	MysqlConnPool     *mysql_con_pool_;
	MysqlDatabaseImpl *mysql_impl_;

};

#endif // _SERVER_APP_H_

