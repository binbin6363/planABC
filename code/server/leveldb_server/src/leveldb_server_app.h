/**
 * @filedesc: 
 * leveldb_server_app.h, app file
 * @author: 
 *  bbwang
 * @date: 
 *  2014/8/3 12:02:59
 * @modify:
 *
**/

#ifndef __LEVELDB_SERVER_APP_H__
#define __LEVELDB_SERVER_APP_H__

#include "net_connector.h"
#include "net_acceptor.h"
#include "client_accept.h"
#include "udp_cmd_session.h"
#include "master_session.h"
#include "slave_session.h"
#include "list_hash.h"
#include "MemoryPool.h"


using namespace object_pool;

class BackendSync;
typedef Net_Acceptor<SlaveSession, TCP_SESSION>  SLAVE_ACCEPTOR;
typedef Net_Connector<MasterSession>             MASTER_CONNECTOR;

class ClientSplitter_ : public Packet_Splitter
{
    virtual int split(const char *buf, int len, int &packet_begin, int &packet_len);
};

class SlaveSplitter_ : public Packet_Splitter
{
    virtual int split(const char *buf, int len, int &packet_begin, int &packet_len);
};

class MasterSplitter_ : public Packet_Splitter
{
    virtual int split(const char *buf, int len, int &packet_begin, int &packet_len);
};


// 专用于数据同步线程的发送数据包缓存
struct DataSendPacket
{

    uint32_t handle;
    Net_Packet *packet;

    DataSendPacket(uint32_t id, Net_Packet *pkg)
        : handle(id)
        , packet(pkg)
    {}

    ~DataSendPacket()
    {
    }
};

typedef Cycle_Buffer_T<DataSendPacket*, MAX_NET_SEND_TASK_COUNT> Data_Send_Queue;


class LevelDbApp :public TimeoutEvent
{
public:
    int open();
    int open_acceptor();

    int run_service();

    uint32_t get_new_loginseq();
    
    static LevelDbApp* Instance()
    {
        static  LevelDbApp instance;
        return &instance;
    }

    char *get_buffer();
    void reset_buffer();

	Net_Session *get_session(const string &session_id);
	int add_session( const string &session_id, Net_Session *session);
	void remove_session(const string &session_id);

    void DumpDatabaseInfo();
    virtual void handle_timeout(int id,void *userData);
    list_lru_hash<ClientSession> &client_session_list()
    {
        return client_session_list_;
    }

    void set_master_server_name(const string &master_name){master_server_name_ = master_name;}
    std::string get_master_server_name(){return master_server_name_;}

    BackendSync *backend_sync(){return backend_sync_;}
    MASTER_CONNECTOR &master_connector(){return master_connector_;}
	SLAVE_ACCEPTOR &slave_acceptor(){return slave_acceptor_;}
    void set_master_session(MasterSession *session){master_session_ = session;}
    MasterSession *master_session(){return master_session_;}
    MasterSplitter_ &master_splitter() {return master_splitter_;}
    SlaveSplitter_ &slave_splitter() {return slave_splitter_;}

    // 同步线程调用的接口
    int push_back_packet(uint32_t handle, const char *data, uint32_t len);
    DataSendPacket *pop_packet();
    int send_packet();

    void add_perfman(double time_used);
    double get_perfman();
    void set_perfman_interal(uint32_t s) {show_perfman_interal_ = s;}

    
private:
    int open_dbpservice();

    // make sure singleton
    LevelDbApp(void);
    ~LevelDbApp(void);
    LevelDbApp &operator=(const LevelDbApp &other);



public:
    ClientAccept                     client_accept_;
    Net_Acceptor<UdpCmdSession, UDP_SESSION>    udp_cmd_accept_;
    Net_Manager                     *net_manager_;
    ClientSplitter_                  splitter_;
    list_lru_hash<ClientSession>     client_session_list_;
    char                            *send_buffer_;

    std::map<string, Net_Session*>   map_session_;
    std::string                      master_server_name_; // 字符串
    
    MASTER_CONNECTOR                 master_connector_;
    SLAVE_ACCEPTOR                   slave_acceptor_;
    MasterSplitter_                  master_splitter_;
    SlaveSplitter_                   slave_splitter_;
    MasterSession                    *master_session_;
    
    BackendSync                      *backend_sync_;
    // 同步线程待发送的数据包
    Data_Send_Queue                  *sync_data_send_queue_;
//	Thread_Mutex                     mutex_; // 上述队列的互斥锁

    std::map<std::string, double>    perfman_cal_; // usedtime, calcnt
    uint32_t                         show_perfman_interal_; // 间隔多少秒打印一次

};

#endif // __LEVELDB_SERVER_APP_H__

