/**
* file: client_msg.h
* desc: client_msg file. msg
* auth: bbwang
* date: 2015/11/4
*/

#ifndef CLI_MSG_H_
#define CLI_MSG_H_
#include "msg.h"
#include "log.h"
#include "task_cache.h"
#include "adv_protocol.pb.h"
#include "inner_common_msg.h"
#include "constants.h"

using namespace google::protobuf;
using namespace common;

#define CURRENT_VERSION "1"

#define TAG_VERSION "version"
#define TAG_RET_CODE "ret"
#define TAG_RET_MSG "err_msg"
#define TAG_TASK_ID "id"
#define TAG_TYPE "type"
#define TAG_NAME "name"
#define TAG_TRANS_ID "transid"
#define TAG_SIGNATURE "signature"
#define TAG_TASK_PRE_CASH "pre_cash"
#define TAG_TASK_AWARD_CASH "award_cash"
#define TAG_TASK_STATUS "status"
#define TAG_PLATFORM "platform"
#define TAG_TOTAL_NUM "total_num"
#define TAG_REWARD_NUM "reward_num"
#define TAG_USED_NUM "used_num"
#define TAG_LINK "link"
#define TAG_DESC "desc"
#define TAG_SIZE "size"
#define TAG_PAY "pay"
#define TAG_FILE "file"
#define TAG_ICON_LINK "icon"
#define TAG_TOTAL_STEP "tstep"
#define TAG_CURR_STEP "cstep"
#define TAG_STATUS "status"
#define TAG_PUBLR_NAME "publisher_name"
#define TAG_PUBLR_EMAIL "publisher_email"
#define TAG_PUBLR_PHONE "publisher_phone"
#define TAG_USER_LIST "uid_list"
#define TAG_STIME "stime"
#define TAG_ETIME "etime"
#define TAG_TASK_INFO "task_info"

#define TASK_SN "task_sn"



class TaskInfoItem;

typedef struct _StrKV
{
	string key;
	string value;

	_StrKV()
	{
		clear();
	}

	_StrKV(const string &key1, const string &value1)
		: key(key1)
		, value(value1)
	{

	}
	void clear()
	{
		key.clear();
		value.clear();
	}
}StrKV;
typedef std::vector< StrKV > StrKVVec;
typedef StrKVVec::const_iterator StrKVVCIter;
typedef StrKVVec::iterator StrKVVIter;
/*
typedef std::vector<std::string> StrVec;
typedef StrVec::const_iterator StrVCIter;
typedef StrVec::iterator StrVIter;
*/

typedef std::vector< uint32_t > UintVec;
typedef UintVec::const_iterator UintVCIter;
typedef UintVec::iterator UintVIter;


typedef com::adv::msg::SyncTaskRequest   SrvSyncTaskRequest;
typedef com::adv::msg::SyncTaskResult    SrvSyncTaskResult;
typedef com::adv::msg::ReportTaskRequest SrvReportTaskRequest;
typedef com::adv::msg::ReportTaskResult  SrvReportTaskResult;
typedef com::adv::msg::SaveTaskRequest   SrvSaveTaskRequest;
typedef com::adv::msg::SaveTaskResult    SrvSaveTaskResult;
typedef com::adv::msg::AckRequest        SrvAckRequest;
typedef com::adv::msg::RetBase           BaseResult;


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

// taskinfo_taskid
#define TASK_INFO_PERFIX       "taskinfo"
// taskprogress_uid_taskid
#define TASK_PROGRESS_PERFIX   "taskprogress"

string makeTaskInfoKey(uint64_t point);
string makeTaskProgressKey(uint32_t uid, uint64_t point);
string makeTaskInfoValue(const StrMap &task_params);
int parseTaskInfoValue(const char *data, uint32_t len, TaskInfoItem &task);


// ======================================================
// 2000, 同步数据协议，前端过来的
// ======================================================
class FrontSyncTaskMsg : public CoMsg
{
public:
    FrontSyncTaskMsg ();
    virtual ~FrontSyncTaskMsg();
	virtual int Decode(const char *data, uint32_t length);
	// 到后端的请求，也调用这个
	virtual int Encode(char *data, uint32_t &length) const;
    const SrvSyncTaskRequest &pb_msg() const {return front_request_;}
    SrvSyncTaskRequest &mutable_pb_msg() {return front_request_;}

	uint32_t transid() const;
    uint32_t synctype() const;

	const uint64_t &cli_sync_point()  const;


private:
    SrvSyncTaskRequest front_request_;
};

// ======================================================
// 2000, 同步数据协议，回复
// ======================================================
typedef com::adv::msg::TaskInfo SrvTaskInfo;
class BackSyncTaskMsg : public CoMsg
{
public:
    BackSyncTaskMsg ();
    virtual ~BackSyncTaskMsg();
	// 后端回来的，也从这里解包
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;
    const SrvSyncTaskResult &pb_msg() const {return front_result_;}
    SrvSyncTaskResult &mutable_pb_msg() {return front_result_;}

	int task_size() const;
	void set_transid(uint32_t transid);
	void set_synctype(uint32_t synctype);
	void set_continue_flag(uint32_t flag);
	void add_task(const TaskInfoItem *task);
	void set_task(const TaskInfoItem *task);
	const SrvTaskInfo &get(int index) const;

private:
    SrvSyncTaskResult  front_result_;
};


class FrontAckMsg : public CoMsg
{
public:
    FrontAckMsg ();
    virtual ~FrontAckMsg();
	virtual int Decode(const char *data, uint32_t length);

	const SrvAckRequest &pb_msg();
private:
    SrvAckRequest  front_result_;
};


// ======================================================
// 3001, 上报数据协议，前端过来的
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
    SrvReportTaskRequest &mutable_pb_msg() {return front_request_;}

	uint32_t transid() const ;
    uint32_t task_type() const ;
	uint64_t taskid() const ;


private:
    SrvReportTaskRequest front_request_;
};

// ======================================================
// 3001, 上报数据协议的返回，返回前端的
// ======================================================
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
	uint32_t get_reward() const ;

private:
    SrvReportTaskResult front_result_;
	int ret_value_;
	string ret_msg_;
};



// ==================================================
// 5000,5001,5002 web request
// web的taskmgr 过来的请求
// ==================================================
class WebCommonRequest : public CoMsg
{
public:
    WebCommonRequest ();
    virtual ~WebCommonRequest();
	virtual int Decode(const char *data, uint32_t length);

    void set_task_id(const uint64_t &task_id);
    string get_task_limit() const;
    int get_task_expire() const;
    const StrVec &get_uid_list() const;
	const string &get_task_sn() const;

	const StrMap &get_params() const;

	uint32_t transid() const;

private:
    StrMap  task_params_;
	StrVec  uid_list_;
	string  empty_str_;
};


class FrontPublishTaskResult : public CoMsg
{
public:
    FrontPublishTaskResult ();
    virtual ~FrontPublishTaskResult();
	virtual int Encode(char *data, uint32_t &length) const;

	uint32_t transid() const;
    void set_transid(uint32_t trans_id);
    //uint32_t synctype();

    void set_err_no(uint32_t err_no);
    virtual int get_err_no()  const;
    virtual void set_err_msg(const string &err_msg);
    virtual string get_err_msg() const;
    void set_task_id(uint64_t task_id);
	uint64_t get_task_id()	const;

private:
    uint32_t ret_value_;
    string ret_msg_;
	uint64_t task_id_;
};


class FrontQueryTaskInfoResult : public CoMsg
{
public:
	FrontQueryTaskInfoResult();
	virtual ~FrontQueryTaskInfoResult();
	virtual int Encode(char *data, uint32_t &length) const;

    void set_err_no(uint32_t err_no);
	virtual int get_err_no() const;
	virtual string get_err_msg() const;
	virtual void set_err_msg(const string &msg_str);

	void SetTaskInfo(const string &task_info);
	void SetPrePay(const string &pay);
	void SetAwardPay(const string &pay);
	void SetTaskStatus(uint32_t value);

private:
	
	int EncodeTaskInfo(BinOutputPacket<> &outpkg) const;

private:
    uint32_t ret_value_;
    string   ret_msg_;
	string   task_info_;     // 纯粹的任务信息，json格式
    uint32_t task_status_;   // 任务当前的状态
	string   pre_pay_;       // 该任务预奖励的金额
	string   award_pay_;     // 该任务已奖励的金额
};


class QueryTaskInfoRequest : public CoMsg
{
public:
	QueryTaskInfoRequest();
	virtual ~QueryTaskInfoRequest();
	virtual int Encode(char *data, uint32_t &length) const;
	virtual int Decode(const char * data, uint32_t length);

	string GetPrePay() const ;
	string GetAwardPay() const ;
	uint32_t GetTaskStatus() const ;
	SrvInnerCommonMsg &mutable_common_msg() {return backend_request_;}

	void set_err_no(int err_no) {ret_code_ = err_no;}
	virtual int get_err_no() const {return ret_code_;}
	virtual string get_err_msg() const {return ret_msg_;}
	virtual void set_err_msg(const string &msg_str) {ret_msg_ = msg_str;}


private:
	int ret_code_;
	string ret_msg_;
	SrvInnerCommonMsg backend_request_;
};

#define QueryTaskInfoResult QueryTaskInfoRequest


class FrontQueryTaskListResult : public CoMsg
{
public:
	FrontQueryTaskListResult();
	virtual ~FrontQueryTaskListResult();
	virtual int Encode(char *data, uint32_t &length) const;

	int AddTask(const string &task_str);
	void set_err_no(int err_no){ret_code_ = err_no;}
	virtual int get_err_no() const {return ret_code_;}
	virtual string get_err_msg() const {return ret_msg_;}
	virtual void set_err_msg(const string &msg_str) {ret_msg_ = msg_str;}

private:
	int ret_code_;
	string ret_msg_;
	StrVec task_list_;
};



// ==================================================
// leveldb request
// ==================================================
// 30000
const static uint32_t MSG_LIMIT = 50;   // 协议里面msg limit的最大取值

class LdbBatchGetRequest : public CoMsg
{
public:
	LdbBatchGetRequest();
	~LdbBatchGetRequest();
	
	virtual int Encode(char *data, uint32_t &length) const;
	void add_key(const string &key);

public:
	uint32_t biz_cmd_;
	string   transfer_str_;
	uint32_t trans_id_;
	StrVec   key_vec_;
};

class LdbBatchGetResult: public CoMsg
{
public:
	LdbBatchGetResult();
	~LdbBatchGetResult();
	
	virtual int Decode(const char *data, uint32_t length);
	uint32_t size();
	StrKVVec &kv_data();
	
	virtual int get_err_no() const {return ret_value_;}

public:
    uint32_t          biz_cmd_;
    std::string       transfer_str_;
    uint32_t          trans_id_;
    uint32_t          ret_value_;
	StrKVVec          kv_data_vec_;
};

// 30001
class LdbBatchPutRequest : public CoMsg
{
public:
	LdbBatchPutRequest();
	~LdbBatchPutRequest();
	
	virtual int Encode(char *data, uint32_t &length) const;

	int AddKV(const string &key, const string &value);

public:
	uint32_t biz_cmd_;
	string   transfer_str_;
	uint32_t trans_id_;
	StrKVVec kv_data_vec_;
	
};

class LdbBatchPutResult: public CoMsg
{
public:
	LdbBatchPutResult();
	~LdbBatchPutResult();
	
	virtual int Decode(const char *data, uint32_t length);

public:
    uint32_t          biz_cmd_;
    std::string       transfer_str_;
    uint32_t          trans_id_;
    uint32_t          ret_value_;

};

// 30002
class LdbBatchDelRequest : public CoMsg
{
public:
	LdbBatchDelRequest();
	~LdbBatchDelRequest();
	
	virtual int Encode(char *data, uint32_t &length) const;


public:
	uint32_t biz_cmd_;
	string   transfer_str_;
	uint32_t trans_id_;
	StrVec   key_vec_;
};

class LdbBatchDelResult : public CoMsg
{
public:
	LdbBatchDelResult();
	~LdbBatchDelResult();
	
	virtual int Decode(const char *data, uint32_t length);
public:
    uint32_t          biz_cmd_;
    std::string       transfer_str_;
    uint32_t          trans_id_;
    uint32_t          ret_value_;

};

// 30003
class LdbRangeGetRequest : public CoMsg
{
public:
	LdbRangeGetRequest();
	~LdbRangeGetRequest();
	
	virtual int Encode(char *data, uint32_t &length) const;

	void set_biz_cmd(uint32_t biz_cmd);
	void set_transfer(const string &transfer);
	void set_trans_id(uint32_t trans_id);
	void set_key_start(const string &key_start);
	void set_key_end(const string &key_end);

public:
	uint32_t biz_cmd_;
	string   transfer_str_;
	uint32_t trans_id_;
	string   key_start_;
	string   key_end_;
};

class LdbRangeGetResult : public CoMsg
{
public:
	LdbRangeGetResult();
	~LdbRangeGetResult();
	
	virtual int Decode(const char *data, uint32_t length);
	virtual int get_err_no() const {return ret_value_;}

public:
	uint32_t biz_cmd_;
	string   transfer_str_;
	uint32_t trans_id_;
	uint32_t ret_value_;
	uint32_t end_flag_;
	StrKVVec kv_data_vec_;
};


// 30004
class LdbRangeDelRequest : public CoMsg
{
public:
	LdbRangeDelRequest();
	~LdbRangeDelRequest();
	
	virtual int Encode(char *data, uint32_t &length) const;

public:
	uint32_t biz_cmd_;
	string   transfer_str_;
	uint32_t trans_id_;
	string   key_start_;
	string   key_end_;
};

class LdbRangeDelResult : public CoMsg
{
public:
	LdbRangeDelResult();
	~LdbRangeDelResult();
	
	virtual int Decode(const char *data, uint32_t length);
public:
    uint32_t          biz_cmd_;
    std::string       transfer_str_;
    uint32_t          trans_id_;
    uint32_t          ret_value_;

};

// 保存任务的请求
class InsertTaskRequest : public CoMsg
{
public:
	InsertTaskRequest();
	~InsertTaskRequest();

	virtual int Encode(char * data, uint32_t & length) const;
	int add(const SrvTaskInfo &task_info);
	void set_transid(uint32_t transid);

public:
    SrvSaveTaskRequest backend_request_;

};

class InsertTaskResult : public CoMsg
{
public:
	InsertTaskResult();
	~InsertTaskResult();

	virtual int Decode(const char * data, uint32_t length);
	
public:
    SrvSaveTaskResult backend_result_;

};


class StartTaskRequest : public CoMsg
{
public:
	StartTaskRequest();
	virtual ~StartTaskRequest();
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char * data, uint32_t & length) const;

	const SrvInnerCommonMsg &common_msg() const {return front_request_;}
	SrvInnerCommonMsg &mutable_common_msg(){return front_request_;}

	int RetCode() const;
	string RetMsg() const;
	uint64_t GetTaskId() const;
	uint32_t GetTaskType() const;
	uint32_t GetTransId() const;
	void SetTaskType(uint32_t value);
	void SetTaskId(const uint64_t &value);
	void SetTransId(uint32_t value);

private:	
	SrvInnerCommonMsg front_request_;
};

typedef StartTaskRequest StartTaskResult;

/*
class StartTaskResult : public CoMsg
{
public:
	StartTaskResult();
	virtual ~StartTaskResult();
	virtual int Encode(char *data, uint32_t &length) const;

	SrvInnerCommonMsg &mutable_common_msg(){return front_result_;}

private:
	SrvInnerCommonMsg front_result_;

};
*/


#endif // CLI_MSG_H_

