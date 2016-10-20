/**
 * @filedesc: 
 * client_session.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/2/26 11:18:41
 * @modify:
 *
**/
#ifndef CLIENT_SESSION_H_
#define CLIENT_SESSION_H_
#include "session_manager.h"
#include "libco_net_session.h"
#include "libco_data_type.h"


class ClientSession : public LibcoNetSession
{
public:
    ClientSession();
    ~ClientSession();
    virtual int open(void *arg, const INET_Addr &remote_addr);
    virtual int handle_close(uint32_t handle);
    virtual void handle_timeout(int id,void *userData);

    virtual void SetGroupId(uint32_t group_id);
    virtual void SetId(uint32_t id);
    virtual uint32_t GetId();
    virtual uint32_t GetId() const;

    virtual uint32_t NodeId(); // ����ڵ�id
    virtual void SetNodeId(uint32_t node_id);

    void set_serv_info(const Server_Info &serv_info);
    Server_Info &serv_info();

    uint32_t GetSessionStrategy();
    

private:
    uint32_t      node_id_; // һ����hashʱ����Ϊ����ڵ��id
    uint32_t      id_;      // �ͻ��������ϱ��ķ���id����ʶĳ���ͻ���
    time_t        last_cli_alive_time_;
    Server_Info   serv_info_;

};

#endif  //CLIENT_SESSION_H_

