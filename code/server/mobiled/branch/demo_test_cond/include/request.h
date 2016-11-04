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
#ifndef _REQUEST_H_
#define _REQUEST_H_


#include "headers.h"
#include "binpacket_wrap.h"
#include "json/json.h"
#include "json/config.h"
#include "json/value.h"
#include "json/writer.h"
#include "adv_base.pb.h"
#include "adv_protocol.pb.h"
#include "custom_type.h"


using namespace utils;
using namespace com::adv::msg;

#define RET_CODE "ret"
#define RET_MSG "err_msg"
#define REQUEST_NODE "request"
#define DEVID "devid"
#define DEVTYPE "dev_type"
#define PASSWD "passwd"
#define VERSION "version"
#define KEY "key"
#define UID "uid"
#define TIME_NOW "time"
#define BASE_RET "res"
#define LOGIN_NODE "login"
#define SYNC_KEY "synckey"
#define ITEM_SIZE "count"
#define ITEMS "items"
#define ITEM_KEY "k"
#define ITEM_VALUE "v"

#define TASK_ID "id"
#define TASK_TYPE "type"
#define TASK_NAME "name"
#define TASK_LINK "link"
#define TASK_DESC "desc"
#define TASK_SIZE "size"
#define TASK_PKG_NAME "pkgname"
#define TASK_PAY "pay"
#define TASK_TOTAL_STEP "tstep"
#define TASK_CURR_STEP "cstep"
#define TASK_DONE_STEP "done_step"
#define TASK_STATUS "status"
#define TASK_PUBLR "publisher"
#define TASK_ICON_LINK "icon"
#define TASK_TOTAL_NUM "task_num"
#define TASK_USED_NUM "used_num"
#define TASK_STIME "stime"
#define TASK_ETIME "etime"
#define TASK_URL "url"

#define INCOME "income"
#define BUSSINESS_TYPE "type"
#define CONTINUE_FLAG "continue_flag"
#define SYNC_VALUE "sync_value"
#define TRANS_ID "transid"
#define PRE_CASH "pre_cash"
#define USEABLE_CASH "useable_cash"
#define FETCHED_CASH "fetched_cash"
#define TAG_TYPE "type"
#define TAG_LOGIN_FLAG "login_flag"
#define TAG_OPENID "openid"
#define TAG_TOKEN "token"
#define TAG_NICK_NAME "nickname"
#define TAG_PASSWD "passwd"
#define TAG_SEX "sex"
#define TAG_PROVINCE "province"
#define TAG_CITY "city"
#define TAG_COUNTRY "country"
#define TAG_AVATAR "headimgurl"
#define TAG_DESC "desc"
#define TAG_EMAIL "email"
#define TAG_PHONE "phone"
#define TAG_WITHDRAW_CASH "withdraw_cash"
#define TAG_USER_NAME "user_name"
#define TAG_TRADE_NO "trade_no"

// ========================================
// frontend request and result
// ========================================
class FrontRequest
{
public:
	FrontRequest(BinInputPacket<> &inpkg);
	~FrontRequest();

	virtual int decode();
	HEADER get_head();
	
	inline uint32_t trans_id(){return trans_id_;}
	inline uint32_t time(){return time_;}

protected:
	BinInputPacket<> &inpkg_;
	Json::Reader reader_;
	HEADER      header_;
	uint32_t trans_id_;
	uint32_t time_;
};

class FrontResult
{
public:
	FrontResult();
	virtual ~FrontResult();
	
	char *data();
	uint32_t byte_size();
	virtual int encode();
	void set_head(const HEADER &head);
	const HEADER &get_head();
	void set_len(uint32_t len);

	int encode_body(const char *data, uint32_t data_len);
	int encode_head();

	inline void set_trans_id(uint32_t trans_id){trans_id_ = trans_id;}
	inline void set_time(uint32_t time){time_ = time;}

protected:
	char     *data_;
	uint32_t data_len_;
	HEADER   header_;
public:
	uint32_t ret_code_;
	uint32_t trans_id_;
	uint32_t time_;
	string   ret_msg_;

};


class FrontLoginRequest : public FrontRequest
{
public:
	FrontLoginRequest(BinInputPacket<> &inpkg);
	virtual ~FrontLoginRequest();

	virtual int decode();
	
	string      devid_;
	uint32_t    devtype_;
	string      passwd_;
	uint32_t    version_;
};


class FrontLoginResult : public FrontResult
{
public:
	FrontLoginResult();
	virtual ~FrontLoginResult();

	virtual int encode();
	
	string   key_;
	uint32_t uid_;
};

class FrontBeatRequest : public FrontRequest
{
public:
	FrontBeatRequest(BinInputPacket<> &inpkg);
	virtual ~FrontBeatRequest();

	virtual int decode();

};


class FrontBeatResult : public FrontResult
{
public:
	FrontBeatResult();
	virtual ~FrontBeatResult();

	virtual int encode();

};

class FrontLcsBeatRequest //: public FrontRequest
{
public:
	FrontLcsBeatRequest(BinInputPacket<> &inpkg);
	virtual ~FrontLcsBeatRequest();

	virtual int decode();
	LCSHEADER get_head();
	
protected:
	BinInputPacket<> &inpkg_;
	Json::Reader reader_;
	LCSHEADER		header_;
	uint32_t time_;
public:
	uint32_t trans_id_;

};


class FrontLcsBeatResult// : public FrontResult
{
public:
	FrontLcsBeatResult();
	virtual ~FrontLcsBeatResult();

	virtual int encode();
	
	int encode_body(const char *data, uint32_t data_len);
	int encode_head();
	void set_head(const LCSHEADER &header);
	const LCSHEADER &get_head();

	char *data();
	int byte_size();
protected:
	char	 *data_;
	uint32_t data_len_;
	LCSHEADER	 header_;
public:
	uint32_t ret_code_;
	uint32_t trans_id_;
	uint32_t time_;
	string	 ret_msg_;

};


class FrontSyncRequest : public FrontRequest
{
public:
	FrontSyncRequest(BinInputPacket<> &inpkg);
	virtual ~FrontSyncRequest();

	virtual int decode();

	map<uint32_t, uint64_t> sync_items_;
};



class FrontSyncResult : public FrontResult
{
public:
	FrontSyncResult();
	virtual ~FrontSyncResult();

	virtual int encode();
	
	void set_ret_value(int ret);
	void set_err_msg(const string &err_msg);
	void set_bus_type(uint32_t type);
	void set_continue_flag(uint32_t flag);
	void set_sync_value(uint32_t maxtaskid);
	void set_task_infos(const TaskInfoVec &task_infos);

private:
	int encodeTask(Json::Value &item, CustomTaskInfo *task);

	int ret_value_;
	string  err_msg_;
    uint32_t bus_type_;
    uint32_t continue_flag_;
	uint64_t max_taskid_;
	TaskInfoVec task_infos_;
	
};


class FrontSyncAckRequest : public FrontRequest
{
public:
	FrontSyncAckRequest(BinInputPacket<> &inpkg);
	virtual ~FrontSyncAckRequest();

	virtual int decode();

	vector<uint64_t> ack_items_;
};


class FrontReportRequest : public FrontRequest
{
public:
	FrontReportRequest(BinInputPacket<> &inpkg);
	virtual ~FrontReportRequest();

	virtual int decode();

	uint64_t   task_id_;
	uint32_t   task_type_;
	uint32_t   task_curr_step_;
	string     task_desc_;
};

class FrontReportResult : public FrontResult
{
public:
	FrontReportResult();
	virtual ~FrontReportResult();

	virtual int encode();

	uint32_t type_;
	uint32_t id_;
};


class FrontIncomeResult : public FrontResult
{
public:
	FrontIncomeResult();
	virtual ~FrontIncomeResult();

	virtual int encode();

	void set_ret_value(int ret);
	void set_err_msg(const string &err_msg);
	void set_pre_cash(const string &cash);
	void set_useable_cash(const string &cash);
	void set_fetched_cash(const string &cash);

private:
	int encodeIncome(Json::Value &item);


	int ret_value_;
	string  err_msg_;
    string  pre_cash_;   // 预财富值
    string  useable_cash_;   // 可以提取的财富值
    string  fetched_cash_;   // 已经提取的财富值
};


class FrontStartTaskRequest : public FrontRequest
{
public:
	FrontStartTaskRequest(BinInputPacket<> &inpkg);
	virtual ~FrontStartTaskRequest();

	virtual int decode();

	uint64_t   task_id_;
	uint32_t   task_type_;
	string     task_url_;
};

class FrontStartTaskResult : public FrontResult
{
public:
	FrontStartTaskResult();
	virtual ~FrontStartTaskResult();

	virtual int encode();

	uint64_t   task_id_;
	uint32_t   task_type_;
};


class FrontThirdPartyLoginRequest : public FrontRequest
{
public:
	FrontThirdPartyLoginRequest(BinInputPacket<> &inpkg);
	virtual ~FrontThirdPartyLoginRequest();

	virtual int decode();

	uint32_t   login_type_;
	uint32_t   login_flag_;
	string     login_openid_;
	string     login_nick_name_;
	string     login_passwd_;    // 32 char hex string
	string     login_token_;     // 32 char hex string
	uint32_t   login_sex_;
	string     login_province_;
	string     login_city_;
	string     login_country_;
	string     login_avatar_;
	string     login_desc_;
	string     login_email_;
	string     login_phone_;
};

class FrontThirdPartyLoginResult : public FrontResult
{
public:
	FrontThirdPartyLoginResult();
	virtual ~FrontThirdPartyLoginResult();

	virtual int encode();

	inline void set_ret_value(uint32_t ret_value){ret_value_ = ret_value;}
	inline void set_ret_msg(const string &ret_msg){ret_msg_ = ret_msg;}
	inline void set_type(uint32_t type){type_ = type;}

private:
	uint32_t ret_value_;
	string	 ret_msg_;
	uint32_t type_;
};


class FrontWithdrawRequest : public FrontRequest
{
public:
	FrontWithdrawRequest(BinInputPacket<> &inpkg);
	virtual ~FrontWithdrawRequest();

	virtual int decode();

	inline string &trade_id(){return trade_id_;}
	inline string &open_id(){return open_id_;}
	inline string &user_name(){return user_name_;}
	inline string &desc(){return desc_;}
	inline uint32_t withdraw_type(){return withdraw_type_;}
	inline uint32_t withdraw_cash(){return withdraw_cash_;}

private:
	string   trade_id_;
	string   open_id_;
	string   user_name_;
	string   desc_;
	uint32_t withdraw_type_;
	uint32_t withdraw_cash_;
};

class FrontWithdrawResult : public FrontResult
{
public:
	FrontWithdrawResult();
	virtual ~FrontWithdrawResult();

	virtual int encode();

	inline void set_ret_value(uint32_t ret_value){ret_code_ = ret_value;}
	inline void set_ret_msg(const string &ret_msg){ret_msg_ = ret_msg;}
	inline void set_trade_no(const string &trade_no){trade_no_ = trade_no;}
	inline void set_withdraw_type(uint32_t withdraw_type){withdraw_type_ = withdraw_type;}
	inline void set_withdraw_cash(uint32_t withdraw_cash){withdraw_cash_ = withdraw_cash;}

private:
	string   trade_no_;
	uint32_t withdraw_type_;
	string   withdraw_cash_;
};

#endif // _REQUEST_H_

