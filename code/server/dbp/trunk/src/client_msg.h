#ifndef CLI_MSG_H_
#define CLI_MSG_H_
#include "msg.h"
#include "adv_protocol.pb.h"
#include "inner_common_msg.h"

using namespace google::protobuf;

typedef com::adv::msg::LoginRequest      SrvLoginRequest;
typedef com::adv::msg::LoginResult       SrvLoginResult;
typedef com::adv::msg::LogoutRequest     SrvLogoutRequest;
typedef com::adv::msg::LogoutResult      SrvLogoutResult;
typedef com::adv::msg::RetBase           BaseResult;
typedef com::adv::msg::MysqlRequest      QueryRequest;
typedef com::adv::msg::MysqlResult       QueryResult;
typedef com::adv::msg::SyncTaskRequest   SrvSyncTaskRequest;
typedef com::adv::msg::SyncTaskResult    SrvSyncTaskResult;
typedef com::adv::msg::ReportTaskRequest SrvReportTaskRequest;
typedef com::adv::msg::ReportTaskResult  SrvReportTaskResult;
typedef com::adv::msg::GetIncomeRequest  SrvGetIncomeRequest;
typedef com::adv::msg::GetIncomeResult   SrvGetIncomeResult;
typedef com::adv::msg::SaveTaskRequest   SrvSaveTaskRequest;
typedef com::adv::msg::SaveTaskResult    SrvSaveTaskResult;
typedef com::adv::msg::WithdrawRequest   SrvWithdrawRequest;
typedef com::adv::msg::WithdrawResult    SrvWithdrawResult;


typedef std::map<std::string, std::string> Row;
typedef Row::const_iterator RowCIter;
typedef Row::iterator RowIter;
typedef std::vector<Row> Rows;
typedef Rows::const_iterator RowsCIter;
typedef Rows::iterator RowsIter;

#define TAG_RET_CODE "ret"
#define TAG_RET_MSG "err_msg"

#define TAG_TASK_ID "id"
#define TAG_TYPE "type"
#define TAG_TRANS_ID "transid"
#define TAG_TASK_STATUS "status"
#define TAG_TASK_ALGORITHM "algorithm"
#define TAG_TASK_PRE_CASH "pre_cash"
#define TAG_TASK_AWARD_CASH "award_cash"
#define TAG_TASK_LIMIT_NUM "limit_num"
#define TAG_TASK_START_NUM "start_num"
#define TAG_TASK_START_TIME "task_stime"
#define TAG_TASK_END_TIME "task_etime"
#define TAG_TASK_INFO "task_info"


namespace DB{
	static const string  DB_TASK_ID_ITEM      = "Ftaskid";
	static const string  DB_TASK_STATUS       = "Ftask_status";
	static const string  DB_TASK_FLAG         = "Fflag";
	static const string  DB_TASK_PROGRESS     = "Ftask_progress";
	static const string  DB_TASK_ALGORITHM    = "Ftask_algorithm";
	static const string  DB_TASK_PRE_CASH     = "Fpre_cash";
	static const string  DB_TASK_AWARD_CASH   = "Faward_cash";
	static const string  DB_TASK_LIMIT_NUM    = "Flimit_num";
	static const string  DB_TASK_START_NUM    = "Fstart_num";
	static const string  DB_TASK_START_TIME   = "Fstart_time";
	static const string  DB_TASK_END_TIME     = "Fend_time";
	static const string  DB_PRE_CASH_ITEM     = "Fpre_cash";
	static const string  DB_USEABLE_CASH_ITEM = "Fuseable_cash";
	static const string  DB_FETCHED_CASH_ITEM = "Ffetched_cash";
};


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

// define marco
#ifndef CHECK_POINTER
#define CHECK_POINTER(pointer, msg) if(NULL == pointer) \
	{LOG(ERROR)("%s", msg);break;}
#endif
#ifndef CHECK_RET
#define CHECK_RET(ret, msg) if(0 != ret) \
	{LOG(ERROR)("%s, ret:%d", msg, ret);break;}
#endif



void PrintPbData(const Message &message, const char *desc = NULL);


// ======================================================
// 1000, 登录协议，前端过来的
// ======================================================
class FrontLoginMsg : public CoMsg
{
public:
    FrontLoginMsg ();
    virtual ~FrontLoginMsg();
	virtual int Decode(const char *data, uint32_t length);
    const SrvLoginRequest &pb_msg() const {return front_request_;}
    

private:
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
	virtual int Encode(char *data, uint32_t &length) const;
    SrvLoginResult &mutable_pb_msg() {return front_result_;}

	void set_uid(uint32_t uid);
	void set_transid(uint32_t transid);
	void set_key(const string &key);
	void set_loginseq(uint32_t loginseq);

private:
	uint32_t  uid_;
	uint32_t  transid_;
	uint32_t  loginseq_;
	string    key_;
    SrvLoginResult  front_result_;
};

// ======================================================
// 2000, 同步协议，前端过来的
// ======================================================
class FrontSyncTaskMsg : public CoMsg
{
public:
    FrontSyncTaskMsg ();
    virtual ~FrontSyncTaskMsg();
	virtual int Decode(const char * data, uint32_t length) ;

	const SrvSyncTaskRequest &pb_msg();

private:
	SrvSyncTaskRequest  front_request_;
};

// ======================================================
// 2000, 同步协议，回复
// ======================================================
class BackSyncTaskMsg : public CoMsg
{
public:
    BackSyncTaskMsg ();
    virtual ~BackSyncTaskMsg();
	virtual int Encode(char *data, uint32_t &length) const;

	SrvSyncTaskResult &mutable_pb_msg();

	void add_taskid(const uint64_t &id);
	void add_task(const Row &row_item);

private:
	SrvSyncTaskResult  front_result_;

};

// ======================================================
// 11002, 获取收入情况，前端过来的
// ======================================================
class FrontGetIncomeMsg : public CoMsg
{
public:
    FrontGetIncomeMsg ();
    virtual ~FrontGetIncomeMsg();
	virtual int Decode(const char * data, uint32_t length) ;

	const SrvGetIncomeRequest &pb_msg();
	uint32_t get_transid();

private:
	SrvGetIncomeRequest  front_request_;
};

// ======================================================
// 11002, 获取收入情况，回复
// ======================================================
class BackGetIncomeMsg : public CoMsg
{
public:
    BackGetIncomeMsg ();
    virtual ~BackGetIncomeMsg();
	virtual int Encode(char *data, uint32_t &length) const;

	SrvGetIncomeResult &mutable_pb_msg();

	void set_pre_cash(const string &cash);
	void set_useable_cash(const string &cash);
	void set_fetched_cash(const string &cash);
	void set_transid(uint32_t transid);

private:
	SrvGetIncomeResult  front_result_;

};


// ======================================================
// 11010, 保存任务到自己的列表
// ======================================================
class FrontSaveTaskMsg: public CoMsg
{
public:
    FrontSaveTaskMsg();
    virtual ~FrontSaveTaskMsg();
	virtual int Decode(const char * data, uint32_t length) ;

	const SrvSaveTaskRequest &pb_msg();
	uint32_t transid();

private:
	SrvSaveTaskRequest  front_request_;
};


class BackSaveTaskMsg: public CoMsg
{
public:
    BackSaveTaskMsg();
    virtual ~BackSaveTaskMsg();
	virtual int Encode(char *data, uint32_t &length) const;

	SrvSaveTaskResult &mutable_pb_msg();
	void set_transid(uint32_t transid);

private:
	SrvSaveTaskResult  front_result_;
};


// ======================================================
// 11020, 上报数据协议
// ======================================================
class FrontReportMsg : public CoMsg
{
public:
    FrontReportMsg ();
    virtual ~FrontReportMsg();
	virtual int Decode(const char *data, uint32_t length);
	// 到后端的请求，也调用这个
	virtual int Encode(char *data, uint32_t &length) const;
    const SrvReportTaskRequest &pb_msg() const {return front_request_;}

	uint32_t transid() const;
    uint32_t task_type() const;
	uint64_t taskid() const;
	string reward() const;
    uint32_t task_step() const;


private:
    SrvReportTaskRequest front_request_;
};

class BackReportMsg : public CoMsg
{
public:
    BackReportMsg ();
    virtual ~BackReportMsg();
	virtual int Encode(char *data, uint32_t &length) const;
    SrvReportTaskResult &mutable_pb_msg() {return front_result_;}

	void set_taskid(uint64_t taskid);
	void set_transid(uint32_t transid);
    void set_task_type(uint32_t task_type);
	void set_done_step(uint32_t done_step);
	void set_time(uint32_t time);

	void set_err_no(uint32_t err_no);
	virtual int get_err_no()  const;
	virtual void set_err_msg(const string &err_msg);
	virtual string get_err_msg() const;

private:
    SrvReportTaskResult front_result_;
	int ret_value_;
	string ret_msg_;
};


class FrontThirdPartyLoginMsg : public CoMsg
{
public:
	FrontThirdPartyLoginMsg ();
	virtual ~FrontThirdPartyLoginMsg();
	virtual int Decode(const char *data, uint32_t length);
	// 到后端的请求，也调用这个
	virtual int Encode(char *data, uint32_t &length) const;
	const SrvLoginRequest &pb_msg() const {return front_request_;}
	SrvLoginResult &mutable_pb_msg() {return front_result_;}

private:
	SrvLoginRequest front_request_;
	SrvLoginResult  front_result_;
};


class CheckTaskRequest : public CoMsg
{
public:
	CheckTaskRequest();
	virtual ~CheckTaskRequest();
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;

	const SrvInnerCommonMsg &common_msg() const {return front_request_;}
	SrvInnerCommonMsg &mutable_common_msg(){return front_request_;}

	int RetCode() const;

	uint32_t GetTaskType() const;
	uint64_t GetTaskId() const;
	uint32_t GetTransId() const;
	void SetTaskType(uint32_t value);
	void SetTaskId(const uint64_t &value);
	void SetTransId(uint32_t value);

private:	
	SrvInnerCommonMsg front_request_;
};

#define CheckTaskResult CheckTaskRequest
#define CloseTaskRequest CheckTaskRequest
#define CloseTaskResult CheckTaskRequest


class FrontWithdrawMsg : public CoMsg
{
public:
	FrontWithdrawMsg();
	virtual ~FrontWithdrawMsg();
	virtual int Decode(const char *data, uint32_t length);
	//virtual int Encode(char *data, uint32_t &length) const;

	const SrvWithdrawRequest &pb_msg() const {return front_request_;}

private:	
	SrvWithdrawRequest front_request_;
};


class BackWithdrawMsg : public CoMsg
{
public:
	BackWithdrawMsg();
	virtual ~BackWithdrawMsg();
	//virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;

	SrvWithdrawResult &mutable_pb_msg() {return front_result_;}
	void SetTransId(uint32_t value);

private:	
	SrvWithdrawResult front_result_;
};




// ======================================================
// 查询用户的mysql请求，主动发起，向后端的请求
// ======================================================
class BackQueryRequest : public CoMsg
{
public:
    BackQueryRequest ();
    virtual ~BackQueryRequest();
	virtual int Encode(char *data, uint32_t &length) const;
    QueryRequest &mutable_pb_msg()  {return back_request_;}
    
	inline void set_hashid(uint32_t id){hash_id_ = id;}
	virtual uint32_t hashid() const {return hash_id_;}

private:
	uint32_t hash_id_;
    QueryRequest back_request_;
};


// ======================================================
// 查询用户的mysql请求，主动发起，向后端的请求
// ======================================================
class BackQueryResult : public CoMsg
{
public:
    BackQueryResult ();
    virtual ~BackQueryResult();
	virtual int Decode(const char * data, uint32_t length) ;
    const QueryResult &pb_msg() const {return back_result_;}

	const Rows &rows(){return rows_;}
	const string &get_value(uint32_t row, const string &field_name);
    
	virtual int get_err_no() const;
	virtual string get_err_msg() const;

	uint32_t affectedrows();

private:
    QueryResult back_result_;
	Rows        rows_;
	string      null_str_;
};


class QueryTaskInfoRequest : public CoMsg
{
public:
	QueryTaskInfoRequest ();
	virtual ~QueryTaskInfoRequest();
	virtual int Decode(const char * data, uint32_t length) ;
	virtual int Encode(char *data, uint32_t &length) const;

	const SrvInnerCommonMsg &common_msg() const {return front_request_;}
	SrvInnerCommonMsg &mutable_common_msg(){return front_request_;}

	void SetTaskId(const uint64_t &value);
	void SetTaskStatus(uint32_t value);
	void SetTaskAlgorithm(const string &value);
	void SetTaskPreCash(const string &value);
	void SetTaskAwardCash(const string &value);
	void SetTaskLimitNum(uint32_t value);
	void SetTaskStartNum(uint32_t value);
	void SetTaskStartTime(uint32_t value);
	void SetTaskEndTime(uint32_t value);

	void set_err_no(int ret_code) {ret_value_ = ret_code;}
	virtual int get_err_no() const {return ret_value_;}
	virtual string get_err_msg() const {return ret_msg_;}
	virtual void set_err_msg(const string &msg_str) {ret_msg_ = msg_str;}

	uint32_t GetTransId() const;
	void SetTransId(uint32_t value);

	void SetTaskInfo(const string &task_info);

	uint64_t GetTaskId() const;

private:	
	SrvInnerCommonMsg front_request_;
	int ret_value_;
	string ret_msg_;

};
#define QueryTaskInfoResult QueryTaskInfoRequest




// ======================================================
// redis proxy请求，主动发起，向后端的请求
// ======================================================
class BackRedisPxyRequest : public CoMsg
{

public:
    BackRedisPxyRequest ();
    virtual ~BackRedisPxyRequest();
	virtual int Encode(char *data, uint32_t &length) const;
	void set_key(const string &key);
    
private:
    string redis_result_str_;
};

class BackRedisPxyResult : public CoMsg
{

public:
    BackRedisPxyResult ();
    virtual ~BackRedisPxyResult();
	virtual int Decode(const char * data, uint32_t length) ;

	uint32_t get_id();

private:
    string redis_request_str_;
};



#endif // CLI_MSG_H_

