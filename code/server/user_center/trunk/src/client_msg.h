
#ifndef CLI_MSG_H_
#define CLI_MSG_H_
#include "msg.h"
#include "user.h"
#include "log.h"
#include "adv_protocol.pb.h"

using namespace google::protobuf;

typedef com::adv::msg::LoginRequest SrvLoginRequest;
typedef com::adv::msg::LoginResult  SrvLoginResult;
typedef com::adv::msg::LogoutRequest SrvLogoutRequest;
typedef com::adv::msg::LogoutResult  SrvLogoutResult;
typedef com::adv::msg::RetBase       BaseResult;
typedef com::adv::msg::UserKeepAliveRequest SrvKeepAliveRequest;
typedef com::adv::msg::UserKeepAliveResult  SrvKeepAliveResult;
typedef com::adv::msg::WithdrawRequest      SrvWithdrawRequest;
typedef com::adv::msg::WithdrawResult       SrvWithdrawResult;


// define marco
#ifndef CHECK_POINTER
#define CHECK_POINTER(pointer, msg) if(NULL == pointer) \
	{LOG(ERROR)("%s", msg);break;}
#endif
#ifndef CHECK_RET
#define CHECK_RET(ret, msg) if(0 != ret) \
	{LOG(ERROR)("%s, ret:%d", msg, ret);break;}
#endif



#if 0
typedef com::adv::msg::StatusRet  SrvStatusRet;
typedef com::adv::msg::DevStatus  SrvDevStatus;
typedef com::adv::msg::StatusChangeRequest SrvStatusChangeRequest;
typedef com::adv::msg::StatusChangeResult SrvStatusChangeResult;
typedef com::adv::msg::GetStatusRequest SrvGetStatusRequest;
typedef com::adv::msg::GetStatusResult  SrvGetStatusResult;
typedef com::adv::msg::StatusChangeNotice  SrvStatusChangeNotice;
typedef com::adv::msg::BatchStatusChangeNotice  SrvBatchStatusChangeNotice;
#endif
// constant var

void PrintPbData(const Message &message, const char *desc = NULL);

/*
class CliBeatMsg : public CoMsg
{
public:
    CliBeatMsg();
    virtual ~CliBeatMsg();
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;
//	virtual int32_t GetCmd() const;
//private:
//    virtual std::string Serialize() const;

};
*/

// ======================================================
// 1000, 登录协议，前端过来的
// ======================================================
class FrontLoginMsg : public CoMsg
{
public:
    FrontLoginMsg ();
    virtual ~FrontLoginMsg();
	virtual int Decode(const char *data, uint32_t length);
	// 到后端的请求，也调用这个
	virtual int Encode(char *data, uint32_t &length) const;
    const SrvLoginRequest &pb_msg() const {return front_request_;}
	SrvLoginRequest &mutable_pb_msg() {return front_request_;}
    virtual uint32_t hashid() const;
    void set_hashid(uint32_t id);


private:
	uint32_t hash_id_;
    SrvLoginRequest front_request_;
};

// ======================================================
// 1000, 登录协议，回复
// ======================================================
class BackLoginMsg : public CoMsg
{
public:
    BackLoginMsg ();
    virtual ~BackLoginMsg();
	// 后端回来的，也从这里解包
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;
    SrvLoginResult &mutable_pb_msg() {return front_result_;}

private:
    SrvLoginResult  front_result_;
};

// ======================================================
// 100, 用户心跳协议，前端过来的
// ======================================================
class UserKeepAliveMsg : public CoMsg
{
public:
    
    UserKeepAliveMsg ();
    virtual ~UserKeepAliveMsg();
	virtual int Decode(const char *data, uint32_t length);

	uint32_t uid();
	uint32_t cond_id();
	uint32_t device_type();
	uint32_t client_ver();
	const string &device_id();
private:
	SrvKeepAliveRequest front_request_;
};


// ======================================================
// 100, 回复前端
// ======================================================
class UserKeepAliveMsgReply : public CoMsg
{
public:
    
    UserKeepAliveMsgReply ();
    virtual ~UserKeepAliveMsgReply();
	virtual int Encode(char *data, uint32_t &length) const;

	void set_uid(uint32_t uid);
	void set_cond_id(uint32_t cond_id);
	void set_device_type(uint32_t device_type);
	void set_client_ver(uint32_t client_ver);
	void set_device_id(const string &device_str);

	SrvKeepAliveResult &mutable_pb_msg(){return front_result_;}
private:
	SrvKeepAliveResult front_result_;
};


class FrontWithdrawMsg : public CoMsg
{
public:
    
    FrontWithdrawMsg();
    virtual ~FrontWithdrawMsg();
	virtual int Decode(const char *data, uint32_t length);

	inline const SrvWithdrawRequest &pb_msg() const {return front_request_;}
	inline SrvWithdrawRequest &mutable_pb_msg() {return front_request_;}

	void set_hashid(uint32_t id) {hash_id_ = id;}
	virtual uint32_t hashid() const {return hash_id_;}
	
private:
	uint32_t hash_id_;
	SrvWithdrawRequest front_request_;
};


class BackWithdrawMsg : public CoMsg
{
public:
    
    BackWithdrawMsg();
    virtual ~BackWithdrawMsg();
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;

	inline SrvWithdrawResult &mutable_pb_msg(){return front_result_;}
	
private:
	SrvWithdrawResult front_result_;
};


#if 0
// ======================================================
// 4000，批量获取状态信息的msg,发往status server的请求
// ======================================================
class FrontGetStatusMsg : public CoMsg
{
public:
    FrontGetStatusMsg ();
    virtual ~FrontGetStatusMsg();
	virtual int Encode(char *data, uint32_t &length) const;
    SrvGetStatusRequest &mutable_pb_msg() {return getStatusRequest_;}
    virtual uint32_t hashid() const;

private:
    SrvGetStatusRequest getStatusRequest_;
};

// ======================================================
// 4000，批量获取状态信息的msg,status server返回的结果
// ======================================================
class BackGetStatusMsg : public CoMsg
{
public:
    BackGetStatusMsg();
    virtual ~BackGetStatusMsg();
	virtual int Decode(const char *data, uint32_t length);
    inline User *GetUser() {return &user_info_;}
    const SrvGetStatusResult &pb_msg() {return back_status_result_;}
    
private:
    virtual std::string Serialize() const;

private:
    SrvGetStatusResult back_status_result_;
    User user_info_;
};

class StatusChangeRequestMsg : public CoMsg
{
public:
    StatusChangeRequestMsg();
    virtual ~StatusChangeRequestMsg();
	virtual int Encode(char *data, uint32_t &length) const;

    SrvStatusChangeRequest &mutable_pb_msg();


private:
    SrvStatusChangeRequest status_change_request_;
};

class StatusChangeResultMsg : public CoMsg
{
public:
    StatusChangeResultMsg();
    virtual ~StatusChangeResultMsg();
	virtual int Decode(const char *data, uint32_t length);

    const SrvStatusChangeResult &pb_msg();


private:
    SrvStatusChangeResult status_change_result_;
};



// ======================================================
// cmd=200,从dbp拉取联系人列表的请求消息
// output: (int32_t)uid
// ======================================================
class DbpGetContactorMsg : public CoMsg
{
public:
    DbpGetContactorMsg ();
    virtual ~DbpGetContactorMsg();
	virtual int Encode(char *data, uint32_t &length) const;
    void set_uid(int32_t uid) { uid_ = uid;}
private:
    virtual std::string Serialize() const;

private:
    int32_t  uid_;
};


// ======================================================
// cmd=200,从dbp拉取联系人列表的结果消息
// input: (int32_t)uid + (uint32_t)unRet + (uint32_t)unRowNum + [contactoruid + contactoruid + ...]
// ======================================================
class BackDbpGetContactorMsg : public CoMsg
{
public:
    
    
    BackDbpGetContactorMsg ();
    virtual ~BackDbpGetContactorMsg();
	virtual int Decode(const char *data, uint32_t length);

    const UserArray &user_array() const ;
    const UserArray &user_array() ;
//private:
//    virtual std::string Serialize() const;

private:
    UserArray user_array_;
};

#endif

#endif // CLI_MSG_H_

