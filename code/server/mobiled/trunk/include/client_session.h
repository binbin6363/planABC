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


// 登录状态:登录中,登录超时,登录失败,登录成功,已登出
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
	void update_time();
    int Decrypt(BinInputPacket<> &inpkg);

    void login_ok();
	bool is_login_ok(){return login_status_ == LOGIN_OK;}
    void destory_connect();

    inline uint32_t uid() {return client_uid_;}
    inline void set_uid(uint32_t uid) {client_uid_ = uid;}

	inline uint32_t client_version(){return m_unVersion;}
	inline void set_client_version(uint32_t ver){m_unVersion = ver;}

	inline const string &device_id(){return device_id_;}
	inline void set_device_id(const string & device_id){device_id_ = device_id;}

	inline uint32_t device_type(){return device_type_;}
	inline void set_device_type(uint32_t device_type){device_type_ = device_type;}
	
	// 此处不能调用strncpy，遇到0就不会copy了。
    void session_key(const char *key, uint32_t length)
    {
        if (CLIENT_PASSWD_LENGTH == length)
    	{
        	memcpy(session_key_, key, length);
    	}
    }
	void passwd(const string &psd)
	{
		memcpy(passwd_, psd.data(), CLIENT_PASSWD_LENGTH);
	}
	char *passwd() {return passwd_;}
    char *session_key() {return session_key_; }
//    void MakeKeyByUid();
//    void MakeKeyRand();
    void set_status(uint8_t status) {status_ = status;}
    int status() {return status_;}

    uint32_t login_seq() {return signing_seq_;}

    //下发消息,传入的是未加密的数据，在这里面加密并发出
    int send_msg_to_client(char *data, uint32_t length);

	int ascii2bin(const string &ascii_str, string &bin_str);
	int bin2ascii(const string &bin_str, string &ascii_str);
private:
    int check_deciph_pkg(BinInputPacket<> &inpkg);
    
//public:
//    list_head signing_item_; // 正在登录的标记项

private:
    ClientSession(const ClientSession&);
//    time_t    last_cli_alive_time_;
//    bool      recv_first_pkg_;
    time_t    last_time_;
    char      session_key_[CLIENT_PASSWD_LENGTH];
    char      passwd_[CLIENT_PASSWD_LENGTH];
    uint32_t   signing_seq_;        // auto increse
    uint32_t  login_status_;     // 登录状态:登录中,登录超时,登录失败,登录成功,已登出
    uint32_t  connect_status_;   // 连接状态:连接中,已连接,断连接
    uint32_t   client_uid_;
    uint32_t  m_unVersion;        //client version
    uint8_t   status_;
    string    device_id_;
    uint32_t  device_type_;

};

#endif // CLIENT_SESSION_H_
