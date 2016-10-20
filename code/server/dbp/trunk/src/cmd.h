
#ifndef DBP_CMD_H_
#define DBP_CMD_H_
#include "comm.h"

#include "adv_protocol.pb.h"

#include "client_msg.h"
#include "constants.h"

using namespace common;

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
    const string &ErrMsg() const ;
    void ErrCode(int errcode);
    int  ErrCode() const;

protected:
    const Processor *processor_;
    const Param &param_;
    Msg *first_request_msg_;

private:
	string err_msg_;
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

#if 0
class LoginCmd : public Command
{
public:
    LoginCmd(const Processor *processor, const Param &param, Msg *msg);
    virtual ~LoginCmd();
	virtual bool Execute();

private:
	int RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result);
};
#endif

class CheckUserCmd : public Command
{
public:
    CheckUserCmd(const Processor *processor, const Param &param, Msg *msg);
    virtual ~CheckUserCmd();
	virtual bool Execute();

private:
	int RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result);
};

class InsertUserCmd : public Command
{
public:
    InsertUserCmd(const Processor *processor, const Param &param, Msg *msg);
    virtual ~InsertUserCmd();
	virtual bool Execute();

private:
	int AccessUid(FrontLoginMsg *p_request, BackLoginMsg *p_result);
	int WriteUserInfo(FrontLoginMsg *p_request, BackLoginMsg *p_result);
	int RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result);

private:
	uint32_t crc_num_;
	uint32_t uid_;
};


class SyncTaskCmd : public Command
{
public:
    SyncTaskCmd(const Processor *processor, const Param &param, Msg *msg);
    virtual ~SyncTaskCmd();
	virtual bool Execute();

private:
	int RepylClient(FrontSyncTaskMsg *p_request, BackSyncTaskMsg *p_result);

	uint32_t  limit_;
	uint64_t  max_task_id_;
	uint32_t  continue_flag_;
};


class GetIncomeCmd : public Command
{
public:
    GetIncomeCmd(const Processor *processor, const Param &param, Msg *msg);
    virtual ~GetIncomeCmd();
	virtual bool Execute();

private:
	int RepylClient(FrontGetIncomeMsg *p_request, BackGetIncomeMsg *p_result);

	string pre_cash_;
	string useable_cash_;
	string fetched_cash_;
};

class SaveTaskCmd : public Command
{
public:
    SaveTaskCmd(const Processor *processor, const Param &param, Msg *msg);
    virtual ~SaveTaskCmd();
	virtual bool Execute();

private:
	int RepylClient(FrontSaveTaskMsg *p_request, BackSaveTaskMsg *p_result);

};

// CMD_INNER_REPORT_TASK_INFO = 11020
class ReportDataCmd : public Command
{
public:
    ReportDataCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~ReportDataCmd();
    
	virtual bool Execute();

private:
	
	int GetTaskAlgorithm(const FrontReportMsg &front_request);
	int RewardUser(const FrontReportMsg &front_request, BackReportMsg &report_result);
	int ReplyClient(const FrontReportMsg &front_request, BackReportMsg &report_result);
private:
	uint32_t reward_;

};

// CMD_WITHDRAW = 6666
class WithdrawCmd : public Command
{
public:
    WithdrawCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~WithdrawCmd();    
	virtual bool Execute();

private:
	int ReplyClient(const FrontWithdrawMsg &front_request, BackWithdrawMsg &front_result);

private:
	uint32_t reward_;
};

class QueryTaskInfoCmd : public Command
{
public:
    QueryTaskInfoCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~QueryTaskInfoCmd();
    
	virtual bool Execute();

private:

	int ReplyClient(const QueryTaskInfoRequest &front_request, QueryTaskInfoResult &front_result);


};


/*
class RewardUserCmd : public Command
{
public:
    RewardUserCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~RewardUserCmd();
    
	virtual bool Execute();

private:
	
	int ReplyClient(const FrontReportMsg &front_request, BackReportMsg &report_result);
};
*/

class FrontThirdPartyLoginMsg;
class ThirdPartyLoginCmd : public Command
{
public:
    ThirdPartyLoginCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~ThirdPartyLoginCmd();
    
	virtual bool Execute();

private:
	
	int CheckAccountTheSame(const FrontThirdPartyLoginMsg &front_request, const Row &row);
	int ReplyClient(const FrontThirdPartyLoginMsg &front_request, FrontThirdPartyLoginMsg &report_result);
};


class FrontCheckTaskMsg;
class BackCheckTaskMsg;
class CheckTaskValid : public Command
{
public:
    CheckTaskValid( const Processor *processor, const Param &param, Msg *msg );
    virtual ~CheckTaskValid();
    
	virtual bool Execute();

private:
	
	int WriteTaskIntoToUser(const CheckTaskRequest &front_request, CheckTaskResult &front_result);
	int ReplyClient(const CheckTaskRequest &front_request, CheckTaskResult &front_result);
};


class FrontWriteTaskMsg;
class BackWriteTaskMsg;
class WriteTaskCmd : public Command
{
public:
    WriteTaskCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~WriteTaskCmd();
    
	virtual bool Execute();

private:
	
	int ReplyClient(const FrontWriteTaskMsg &front_request, BackWriteTaskMsg &front_result);
};

class CloseTaskRequest;
class CloseTaskResult;
class CloseTaskCmd : public Command
{
public:
    CloseTaskCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~CloseTaskCmd();
    
	virtual bool Execute();

private:
	
	int ReplyClient(const CloseTaskRequest &front_request, CloseTaskResult &front_result);
};


#endif //DBP_CMD_H_

