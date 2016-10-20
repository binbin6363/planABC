/**
 * @filedesc: 
 * master_session.h, connect master session
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/6 14:55:59
 * @modify:
 */

#ifndef MASTER_SESSION_H_
#define MASTER_SESSION_H_

#include "comm.h"
#include "net_session.h"
#include "binpacket_wrap.h"
#include "leveldb/db.h"
#include "leveldb/write_batch.h"
#include "leveldb/env.h"
#include "leveldb/iterator.h"

using namespace utils;

//namespace leveldb {
//    class DB;
//    class Options;
//}

class MasterSession : public Net_Session
{
public:
    MasterSession();
    virtual ~MasterSession();
    
    virtual int open(void *arg, const INET_Addr &remote_addr);
    virtual int on_receive_message(char *ptr, int len);
    virtual int handle_close(uint32_t handle);
    virtual void handle_timeout(int id,void *userData);
    
    void set_sync_flag(uint32_t flag);
    uint32_t sync_flag();

    int load_status_from_meta_db();
    int save_status_to_meta_db();

    void set_serv_info(Server_Info server_info);
private:
    void keepalive();
    const std::string &status_key();
    int open_meta_db();
    int close_meta_db();
    int detect_master_for_sync_data();

private:
    time_t      last_time_;
    bool        b_connected_;
    Server_Info serv_info_;
    uint8_t     sync_flag_;

    string              meta_path_; // meta 库路径
    leveldb::DB         *meta_db_;
    leveldb::Options    meta_options_;
    bool                meta_open_flag_;

public:
    uint64_t    last_seq_;  // 用于同步的控制，同步中断后重连传过来的seq
    string      last_key_;  // 用于复制的控制，复制中断后重连传过来的key
};

#endif // MASTER_SESSION_H_
