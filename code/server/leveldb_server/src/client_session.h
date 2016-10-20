/**
 * @filedesc: 
 * client_session.h, handle client action
 * @author: 
 *  bbwang
 * @date: 
 *  2014/8/3 12:02:59
 * @modify:
 *
**/
#ifndef __CLIENT_SESSION_H__
#define __CLIENT_SESSION_H__

#include <vector>
#include "comm.h"
#include "inet_addr.h"
#include "net_session.h"
#include "list_hash.h"


class Msg;


// 获取数据库的所有数据
const uint32_t GET_ALL_LEVELDB_DATA = 201412; 


class ClientSession : public Net_Session, public hash_base
{

public:
    ClientSession(void);
    virtual ~ClientSession(void);

    virtual int open(void *arg, const INET_Addr &remote_addr);
    virtual int on_receive_message(char *ptr, int len);
    virtual int handle_close(uint32_t handle);
    virtual void handle_timeout(int id,void *userData);

    inline uint32_t last_time(){return last_time_;}

//    void set_uid(uint32_t unUid) { m_unUid = unUid;}
//    uint32_t uid() const { return m_unUid; }

//    uint32_t login_seq() const {return login_seq_;}
//    void destory_connect();

//    void set_cid(uint32_t unCid) {m_unCid = unCid;}
//    uint32_t cid() const {return m_unCid;}

    //下发消息
    int send_msg_to_client(char *data, uint32_t length);
    int send_msg_to_client(Msg &msg);
    inline uint32_t alive_time()
    {
        return last_cli_alive_time_;
    }

private:
    void log_msg(char *data, int len);
    

private:
    uint32_t    last_cli_alive_time_;
    uint32_t    last_time_;
    string     transfer_str_;

//    uint32_t  login_seq_;        //auto increse
//    uint32_t  m_unCid;
//    uint32_t  m_unUid;
//    uint16_t  m_unBuild;
//    uint32_t  m_unVersion;        //client version

};

#endif // __CLIENT_SESSION_H__
