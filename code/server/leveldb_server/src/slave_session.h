/**
 * @filedesc: 
 * slave_session.h, accept slave connect
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/6 14:55:59
 * @modify:
 */

#ifndef SLAVE_SESSION_H_
#define SLAVE_SESSION_H_

#include "comm.h"
#include "inet_addr.h"
#include "net_session.h"
#include "constants.h"
#include "nobinlog_iterator.h"
#include "binpacket_wrap.h"
#include "config.h"
#include "cycle_buffer.h"

using namespace common;
using namespace utils;

class Msg;
class ResultSyncMsg;
class SyncDataCmdThread;


class SlaveSession : public Net_Session
{
public:
    SlaveSession();
    virtual ~SlaveSession();
    
    virtual int open(void *arg, const INET_Addr &remote_addr);
    virtual int on_receive_message(char *ptr, int len);
    virtual int handle_close(uint32_t handle);
    virtual void handle_timeout(int id,void *userData);
//    leveldb::Iterator *reset_dbiterator(const string &key);
    
    int send_msg_to_client(Msg &msg);


private:
    time_t      last_time_;
    uint8_t     connected_flag_;
    Server_Info serv_info_;
    SyncDataCmdThread *sync_thread_;

	

};

#endif // SLAVE_SESSION_H_
