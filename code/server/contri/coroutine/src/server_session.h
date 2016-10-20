/**
 * @filedesc: 
 * server_session.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/2/26 11:18:41
 * @modify:
 *
**/
#ifndef SERVER_SESSION_H_
#define SERVER_SESSION_H_
#include "libco_net_session.h"
#include "libco_data_type.h"
#include "binpacket_wrap.h"
#include "session_manager.h"

using namespace utils;
using namespace libco_src;


// 所有连接服务端的session都是这个
class ServerSession : public LibcoNetSession
{

public:
    ServerSession();
    ~ServerSession();
    virtual int  open(void *arg, const INET_Addr &remote_addr);
    virtual int  handle_close(uint32_t handle);
    virtual void handle_timeout(int id,void *userData);
    virtual void BeatOK(const Msg *msg);
    void         keepalive();
    void set_serv_info(const Server_Info &info);
    const Server_Info &serv_info();

    virtual void SetId(uint32_t id);
    virtual uint32_t GetId();
    virtual uint32_t GetId() const;

    virtual void SetScore(uint32_t score);
    virtual uint32_t GetScore();
    virtual uint32_t GetScore() const;

    virtual uint32_t NodeId(); // 虚拟节点id
    virtual void SetNodeId(uint32_t node_id);

    static void SetMyselfServiceId(uint32_t myself_id);

private:
    static uint32_t myself_service_id_;    // 标识本服务的id，全局唯一，用于通过心跳告知服务端 
    uint32_t      node_id_; // 虚拟节点，暂未使用
    uint32_t      id_;    // 用于selector的调用
    uint32_t      score_;    // 用于selector的选择,此值会动态变化，每次心跳回来都会重新赋值
    Server_Info   serv_info_;
};

#endif //SERVER_SESSION_H_

