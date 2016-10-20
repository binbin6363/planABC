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

//using namespace google::protobuf;
//typedef com::adv::msg::LoginRequest SrvLoginRequest;
//typedef com::adv::msg::LoginResult  SrvLoginResult;

/*
*/

using namespace utils;

#define RET_CODE "ret"
#define RET_MSG "err_msg"

class Request
{
public:
	Request(BinInputPacket<> &inpkg);
	~Request();

	virtual int decode();
	HEADER get_head();
	
protected:
	BinInputPacket<> &inpkg_;
	Json::Reader reader_;
	HEADER      header_;
};

class Result
{
public:
	Result();
	virtual ~Result();
	
	char *data();
	uint32_t byte_size();
	virtual int encode();
	void set_head(const HEADER &head);
	const HEADER &get_head();
	void set_len(uint32_t len);

	int encode_body(const char *data, uint32_t data_len);
	int encode_head();

protected:
	char     *data_;
	uint32_t data_len_;
	HEADER   header_;
public:
	uint32_t ret_code_;
	string   ret_msg_;

};

#define REQUEST_NODE "request"
#define DEVID "devid"
#define DEVTYPE "dev_type"
//#define ACCOUNTTYPE "accounttype"
//#define TOKEN "token"
//#define ACCOUNT "account"
#define PASSWD "passwd"
#define VERSION "version"
class FrontLoginRequest : public Request
{
public:
	FrontLoginRequest(BinInputPacket<> &inpkg);
	~FrontLoginRequest();

	int decode();
	
	string      devid_;
	uint32_t    devtype_;
//	uint32_t    account_type_;
//	string      token_;
//	string      account_;
	string      passwd_;
	uint32_t    version_;
	uint32_t    time_;

};

#define KEY "key"
#define UID "uid"
#define TIME_NOW "time"
#define BASE_RET "res"
#define LOGIN_NODE "login"
class FrontLoginResult : public Result
{
public:
	FrontLoginResult();
	virtual ~FrontLoginResult();

	virtual int encode();
	
	string   key_;
	uint32_t uid_;
	uint32_t time_;
};

class FrontBeatRequest : public Request
{
public:
	FrontBeatRequest(BinInputPacket<> &inpkg);
	~FrontBeatRequest();

	int decode();
	
	uint32_t    time_;

};


class FrontBeatResult : public Result
{
public:
	FrontBeatResult();
	~FrontBeatResult();

	int encode();

};


#define SYNC_KEY "synckey"
#define ITEM_SIZE "count"
#define ITEMS "items"
#define ITEM_KEY "k"
#define ITEM_VALUE "v"

class FrontSyncRequest : public Request
{
public:
	FrontSyncRequest(BinInputPacket<> &inpkg);
	~FrontSyncRequest();

	int decode();

	map<uint32_t, uint64_t> sync_items_;
	uint32_t   time_;
};


struct TaskInfo
{
	uint64_t id;
	uint32_t type;
	string name;
	string link;
	string desc;
	uint32_t size;
	string pay;
	uint32_t step;
	string publisher;
	uint32_t stime;
	uint32_t etime;
};

#define TASK_ID "id"
#define TASK_TYPE "type"
#define TASK_NAME "name"
#define TASK_LINK "link"
#define TASK_DESC "desc"
#define TASK_SIZE "size"
#define TASK_PAY "pay"
#define TASK_STEP "step"
#define TASK_DONE_STEP "done_step"
#define TASK_PUBLR "publisher"
#define TASK_STIME "stime"
#define TASK_ETIME "etime"

#define INCOME "income"

#define BUSSINESS_TYPE "type"
#define CONTINUE_FLAG "continue_flag"
#define SYNC_VALUE "sync_value"

class FrontSyncResult : public Result
{
public:
	FrontSyncResult();
	~FrontSyncResult();

	int encode();
	
	// flag.1表示新任务，2表示历史任务
	int encodeTask(Json::Value &item, TaskInfo task, int flag = 1); 
	int encodeIncome(Json::Value &item);

	
};


class FrontReportRequest : public Request
{
public:
	FrontReportRequest(BinInputPacket<> &inpkg);
	~FrontReportRequest();

	int decode();

	uint64_t   task_id_;
	uint32_t   task_type_;
	uint32_t   task_done_step_;
	string     task_desc_;
	uint32_t   time_;
};

class FrontReportResult : public Result
{
public:
	FrontReportResult();
	~FrontReportResult();

	int encode();
	
};

#endif // _REQUEST_H_

