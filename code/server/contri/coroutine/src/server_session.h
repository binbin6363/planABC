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


// �������ӷ���˵�session�������
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

    virtual uint32_t NodeId(); // ����ڵ�id
    virtual void SetNodeId(uint32_t node_id);

    static void SetMyselfServiceId(uint32_t myself_id);

private:
    static uint32_t myself_service_id_;    // ��ʶ�������id��ȫ��Ψһ������ͨ��������֪����� 
    uint32_t      node_id_; // ����ڵ㣬��δʹ��
    uint32_t      id_;    // ����selector�ĵ���
    uint32_t      score_;    // ����selector��ѡ��,��ֵ�ᶯ̬�仯��ÿ�����������������¸�ֵ
    Server_Info   serv_info_;
};

#endif //SERVER_SESSION_H_

