/**
 * @filedesc: 
 * request.h, all request define here
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/14 20:02:59
 * @modify:
 *
**/
#ifndef _BACKEND_REQUEST_H_
#define _BACKEND_REQUEST_H_


#include "headers.h"
#include "binpacket_wrap.h"
#include "adv_base.pb.h"
#include "adv_protocol.pb.h"
#include "custom_type.h"
#include "inner_common_msg.h"


using namespace utils;
using namespace com::adv::msg;

typedef com::adv::msg::UserKeepAliveRequest SrvKeepAliveRequest;
typedef com::adv::msg::SyncTaskRequest      SrvSyncTaskRequest;
typedef com::adv::msg::SyncTaskResult       SrvSyncTaskResult;
typedef com::adv::msg::GetIncomeRequest     SrvGetIncomeRequest;
typedef com::adv::msg::GetIncomeResult      SrvGetIncomeResult;
typedef com::adv::msg::ReportTaskRequest    SrvReportTaskRequest;
typedef com::adv::msg::ReportTaskResult     SrvReportTaskResult;
typedef com::adv::msg::AckRequest           SrvAckRequest;
typedef com::adv::msg::TaskInfo             SrvTaskInfo;
typedef com::adv::msg::LoginRequest         SrvLoginRequest;
typedef com::adv::msg::LoginResult          SrvLoginResult;
typedef com::adv::msg::WithdrawRequest      SrvWithdrawRequest;
typedef com::adv::msg::WithdrawResult       SrvWithdrawResult;



// ========================================
// backend request and result
// ========================================
class BackendRequest
{
public:
	BackendRequest();
	virtual ~BackendRequest();
	
	char *data();
	uint32_t byte_size();
	virtual int encode();
	void set_head(const COHEADER &head);
	const COHEADER &get_head();
	void set_len(uint32_t len);
	
	inline void set_trans_id(uint32_t trans_id){trans_id_ = trans_id;}
	inline void set_time(uint32_t time){time_ = time;}

protected:
	char     *data_;
	uint32_t data_len_;
	COHEADER coheader_;
public:
	uint32_t ret_code_;
	string   ret_msg_;
	uint32_t trans_id_;
	uint32_t time_;

};

class BackendResult
{
public:
	BackendResult(BinInputPacket<> &inpkg);
	~BackendResult();

	virtual int decode();
	COHEADER get_head();

	inline uint32_t trans_id(){return trans_id_;}
	inline uint32_t time(){return time_;}
	
protected:
	BinInputPacket<> &inpkg_;
	COHEADER         coheader_;
	uint32_t         trans_id_;
	uint32_t         time_;
};



class BackendLoginRequest : public BackendRequest
{
public:
	BackendLoginRequest();
	virtual ~BackendLoginRequest();

	virtual int encode();

	void set_token(const string &token);
	void set_devid(const string &devid);
	void set_device_type(uint32_t dev_type);
	void set_passwd(const string &passwd);
	void set_condid(uint32_t condid);
	void set_loginseq(uint32_t seqid);

	LoginRequest backend_login_req_;
};


class BackendLoginResult : public BackendResult
{
public:
	BackendLoginResult(BinInputPacket<> &inpkg);
	virtual ~BackendLoginResult();
	
	virtual int decode();

	int ret_;
	string err_msg_;
	string key_;
	uint32_t uid_;
	uint32_t loginseq_;
};


// ======================================================
// 100, 用户心跳协议，转移到后端
// ======================================================
class UserKeepAliveMsg : public BackendRequest
{
public:
    
    UserKeepAliveMsg ();
    virtual ~UserKeepAliveMsg();
	virtual int encode();

	void set_uid(uint32_t uid);
	void set_cond_id(uint32_t cond_id);
	void set_device_type(uint32_t device_type);
	void set_client_ver(uint32_t client_ver);
	void set_device_id(const string &device_id);
private:
	SrvKeepAliveRequest backend_request_;
};


class BackendSyncRequest : public BackendRequest
{
public:
    
    BackendSyncRequest ();
    virtual ~BackendSyncRequest();
	virtual int encode();

	void set_syncpoint(const uint64_t &point);
	void set_synctype(uint32_t type);
	void set_synclimit(uint32_t limit);

private:
	SrvSyncTaskRequest backend_request_;
};

class BackendSyncResult : public BackendResult
{
public:
    
    BackendSyncResult (BinInputPacket<> &inpkg);
    virtual ~BackendSyncResult();
	virtual int decode();
	CustomTaskInfo *ConvertTask(const TaskInfo& taskinfo);

	
	uint32_t synctype_;
	uint32_t continueflag_;
	uint32_t ret_value_;
	uint64_t maxtaskid_;
	string err_msg_;
	TaskInfoVec task_vec_;
	
};

class BackAckRequest : public BackendRequest
{
public:
	BackAckRequest();
	virtual ~BackAckRequest();

	virtual int encode();

	void set_type(uint32_t type);
	void add_id(uint64_t id);

	
private:
	SrvAckRequest backend_request_;
};


class BackReportRequest : public BackendRequest
{
public:
	BackReportRequest();
	~BackReportRequest();

	virtual int encode();

	void set_task_id(const uint64_t &point);
	void set_task_type(uint32_t type);
	void set_task_curr_step(uint32_t limit);
	void set_task_total_step(uint32_t limit);

	
private:
	SrvReportTaskRequest backend_request_;
};



class BackendIncomeRequest : public BackendRequest
{
public:
    
    BackendIncomeRequest ();
    virtual ~BackendIncomeRequest();
	virtual int encode();


private:
	SrvGetIncomeRequest backend_request_;
};


class BackendIncomeResult : public BackendResult
{
public:
    
    BackendIncomeResult (BinInputPacket<> &inpkg);
    virtual ~BackendIncomeResult();
	virtual int decode();
	/* virtual int encode(); */


	int ret_value_;
	string err_msg_;
	string pre_cash_;
    string useable_cash_;   // 可以提取的财fu
    string fetched_cash_;   // 已经提取的财富值
	
};


class BackendStartTaskRequest : public BackendRequest
{
public:
    
    BackendStartTaskRequest ();
    virtual ~BackendStartTaskRequest();
	virtual int encode();

	inline SrvInnerCommonMsg &mutable_msg(){return backend_request_;}

private:
	SrvInnerCommonMsg backend_request_;
};


class BackendStartTaskResult : public BackendResult
{
public:
	
	BackendStartTaskResult (BinInputPacket<> &inpkg);
	virtual ~BackendStartTaskResult();
	virtual int decode();

	void set_ret(int value);
	void set_task_id(uint32_t value);
	void set_task_type(uint32_t value);
	void set_time(uint32_t value);

	int ret_value(){return ret_value_;}
	const string &ret_msg(){return ret_msg_;}
	uint64_t task_id(){return task_id_;}
	uint32_t task_type(){return task_type_;}

	int ret_value_;
	string ret_msg_;
	uint64_t task_id_;
	uint32_t task_type_;
	
};



class BackendThirdPartyLoginRequest : public BackendRequest
{
public:
    
    BackendThirdPartyLoginRequest ();
    virtual ~BackendThirdPartyLoginRequest();
	virtual int encode();

	inline SrvLoginRequest &mutable_msg(){return backend_request_;}

private:
	SrvLoginRequest backend_request_;
};

class BackendThirdPartyLoginResult : public BackendResult
{
public:
	BackendThirdPartyLoginResult (BinInputPacket<> &inpkg);
	virtual ~BackendThirdPartyLoginResult();
	virtual int decode();

	inline uint32_t ret_value() {return ret_value_;}
	inline string &ret_msg() {return ret_msg_;}
	inline uint32_t type() {return type_;}

private:
	uint32_t ret_value_;
	string   ret_msg_;
	uint32_t type_;
	
};



class BackendWithdrawRequest : public BackendRequest
{
public:
    
    BackendWithdrawRequest ();
    virtual ~BackendWithdrawRequest();
	virtual int encode();
	inline SrvWithdrawRequest &mutable_msg(){return backend_request_;}
	
private:
	SrvWithdrawRequest backend_request_;
};


class BackendWithdrawResult : public BackendResult
{
public:
	BackendWithdrawResult (BinInputPacket<> &inpkg);
	virtual ~BackendWithdrawResult();
	virtual int decode();

	inline uint32_t ret_value(){return ret_value_;}
	inline string &ret_msg(){return ret_msg_;}
	inline string &trade_no(){return trade_no_;}
	inline string &desc(){return desc_;}
	inline uint32_t withdraw_type(){return withdraw_type_;}
	inline uint32_t withdraw_cash(){return withdraw_cash_;}

private:
	uint32_t ret_value_;
	string   ret_msg_;
	string   trade_no_;
	string   desc_;
	uint32_t withdraw_type_;
	uint32_t withdraw_cash_;
};



#endif // _BACKEND_REQUEST_H_

