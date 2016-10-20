
#ifndef LOGIN_CMD_H_
#define LOGIN_CMD_H_
#include "comm.h"

#include "adv_protocol.pb.h"

#include "client_msg.h"

//#include "status_server.pb.h"

class Processor;
class Param;
//class Msg;
//class FrontLoginMsg;
//class UserArray;
//class GroupArray;
//class User;




class Command
{
public:
	Command( const Processor *processor, const Param &param, Msg *msg );
	virtual ~Command();

	virtual bool Execute() = 0;

	void ErrMsg(const string &err_msg);
    const char *ErrMsg();
    void ErrCode(int errcode);
    int  ErrCode();

protected:
    const Processor *processor_;
    const Param &param_;
    Msg *first_request_msg_;

private:
    char  err_msg_[MAX_STACK_STR_LENGTH];
    int   err_code_;
};


// 服务器内部心跳
class BeatCmd : public Command
{
public:
    BeatCmd(const Processor *processor, const Param &param, Msg *msg);
    virtual ~BeatCmd();
	virtual bool Execute();

};


// ============================================================
// 1000, login
// ============================================================
class LoginCmd : public Command
{
public:
    LoginCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~LoginCmd();
    
	virtual bool Execute();

    int RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result);

	int GenerateUserId();



private:
    uint32_t uid_;
    
};

class UserKeepAliveCmd : public Command
{
public:
    UserKeepAliveCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~UserKeepAliveCmd();
    
	virtual bool Execute();

};


class WithdrawCmd : public Command
{
public:
    WithdrawCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~WithdrawCmd();
    
	virtual bool Execute();

private:
	int AuthUser(const FrontWithdrawMsg& front_request);
	int AuthCash(const FrontWithdrawMsg& front_request);
	int DoPreWithdraw(const FrontWithdrawMsg& front_request, BackWithdrawMsg &front_result);
	int NotifyWithdraw(const FrontWithdrawMsg& front_request, BackWithdrawMsg &front_result);
	int ReplyClient(const FrontWithdrawMsg& front_request, BackWithdrawMsg &front_result);

};


#endif //LOGIN_CMD_H_

