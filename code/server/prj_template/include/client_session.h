/**
 * @filedesc: 
 * client_session.h, handle client action
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/13 12:02:59
 * @modify:
 *
**/
#ifndef CLIENT_SESSION_H_
#define CLIENT_SESSION_H_

#include "headers.h"

using namespace utils;

//using namespace common;
//using namespace WCCD;


// ��¼״̬:��¼��,��¼��ʱ,��¼ʧ��,��¼�ɹ�,�ѵǳ�
enum LoginStatus
{
    LOGINING = 1,
    LOGIN_TIMEOUT,
    LOGIN_FAILED,
    LOGIN_OK,
    LOGOUT,
};

class ClientSession : public Net_Session, public hash_base
{

public:
    ClientSession(void);
    virtual ~ClientSession(void);

    virtual int open(void *arg, const INET_Addr &remote_addr);
    virtual int on_receive_message(char *ptr, int len);
    virtual int handle_close(uint32_t handle);
    virtual void handle_timeout(int id,void *userData);

    inline time_t last_time(){return last_time_;}
    int Decrypt(BinInputPacket<> &inpkg);

    void login_ok();
    void destory_connect();

    const inline uint32_t uid() {return client_uid_;}
    inline void set_uid(uint32_t uid) {client_uid_ = uid;}
    char *session_key() {return session_key_; }
    void MakeKeyByUid();
    void MakeKeyRand();
    void set_status(uint8_t status) {status_ = status;}
    int status() {return status_;}

    uint32_t login_seq() {return signing_seq_;}
    
private:
    int check_deciph_pkg(BinInputPacket<> &inpkg);
    
//public:
//    list_head signing_item_; // ���ڵ�¼�ı����

private:
    ClientSession(const ClientSession&);
//    time_t    last_cli_alive_time_;
//    bool      recv_first_pkg_;
    time_t    last_time_;
    char      session_key_[CLIENT_PASSWD_LENGTH];
    uint32_t   signing_seq_;        // auto increse
    uint32_t  login_status_;     // ��¼״̬:��¼��,��¼��ʱ,��¼ʧ��,��¼�ɹ�,�ѵǳ�
    uint32_t  connect_status_;   // ����״̬:������,������,������
    uint32_t   client_uid_;
    uint16_t  m_unBuild;
    uint32_t  m_unVersion;        //client version
    uint8_t   status_;

};

#endif // CLIENT_SESSION_H_
