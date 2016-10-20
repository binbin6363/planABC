/**
 * @filedesc: 
 * user_center_session.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/11/3 12:02:59
 * @modify:
 *
**/

#ifndef _TASK_CENTER_SESSION_H_
#define _TASK_CENTER_SESSION_H_
#include "headers.h"

using namespace utils;


class TaskCenterSession :
    public Net_Session
{
public:
    TaskCenterSession(void);
    virtual ~TaskCenterSession(void);

    virtual int open(void *arg, const INET_Addr &remote_addr);
    virtual int on_receive_message(char *ptr, int len);
    virtual int handle_close(uint32_t handle);
    virtual void handle_timeout(int id,void *userData);
    inline bool connected(){return b_connected_;}

    void set_serv_info(const Server_Info &info);
    const Server_Info &serv_info(){return serv_info_;}

    void keepalive();
    uint32_t msg_center_id(){return msg_center_id_;}
    void set_msg_center_id(uint32_t msg_center_id){msg_center_id_ = msg_center_id;}

    static int SendToTaskCenter(int hashid, char *data, int len);
private:
    time_t      last_time_;
    Server_Info serv_info_;
    bool        b_connected_;
    uint32_t    msg_center_id_;
};


#endif //_TASK_CENTER_SESSION_H_

