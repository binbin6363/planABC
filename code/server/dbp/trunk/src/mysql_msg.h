/**
 * @filedesc: 
 * mysql_msg.h, handle mysql msg
 * @author: 
 *  bbwang
 * @date: 
 *  2015/11/4 21:52:59
 * @modify:
 *
**/


#ifndef _MYSQL_MSG_H_
#define _MYSQL_MSG_H_

#include <string>
#include "msg.h"

using namespace std;

namespace com{
	namespace adv{
		namespace msg{
class SaveTaskRequest;
		}
	}
}

string EscapeString(const string & s);

struct DbUser
{
    uint32_t version;
    uint32_t uid;
    string deviceid;
    string account;
    uint32_t accounttype;
    string passwd;
    string nick_name;
    string mobile;
    string mail;
    uint32_t gender;
    string avatar;
    string info;
    string sign;
    string location;
    string city;
    uint32_t status;
    string income;

    DbUser()
    : version(0)
    , uid(0)
    , deviceid("")
    , account("")
    , accounttype(0)
    , passwd("")
    , nick_name("test_account")
    , mobile("")
    , mail("")
    , gender(0)
    , avatar("")
    , info("")
    , sign("")
    , location("")
    , city("")
    , status(0)
    , income("0.00")
    {}

    
};


string getDbTableName(const char *sDbName, const char *sTableName, uint32_t unId);
string getDbTableName(const char *sDbName, const char *sTableName, const string & sId);

string makePasswdAddSalt(const string &passwd);

// mysql key
string makeGetUserIdKey(const string &dev_id);
string makeGenerateUserIdKey(const string &dev_id);
string makeSetUserKey(const string &dev_id, const DbUser &user);
string makeGenerateUidKey();
string makeGetTaskListKey(uint32_t uid, uint32_t flag, const uint64_t &taskid, uint32_t limit = 20);
string makeGetIncomeKey(uint32_t uid);
string makeSaveTaskKey(uint32_t uid, const com::adv::msg::SaveTaskRequest &pb_sync_req);
string makeReportTaskKey(uint32_t uid, uint64_t taskid, uint32_t cur_step);
string makeGetTaskAlgorithmKey(uint64_t taskid);
string makeRewardUserKey(uint32_t uid, uint64_t taskid, uint32_t reward);
string makeCheckLoginKey(uint32_t uid, uint32_t account_type);
string makeWithdrawKey(uint32_t uid, uint32_t withdraw, const string &password);
string makeThirdLoginKey(uint32_t uid, const DbUser &user);
string makeUserInfoKey(uint32_t uid, const DbUser &user);
string makeCheckTaskKey(uint32_t uid, uint64_t taskid);
string makeWriteTaskKey(uint32_t uid, uint64_t taskid);
string makeCloseTaskKey(uint64_t taskid);
string makeQueryTaskInfoKey(uint64_t taskid);


#if 0
class DbRequest : public CoMsg
{
public:
	DbRequest(BinInputPacket<> &inpkg);
	virtual ~DbRequest();


protected:
    BinInputPacket<> &inpkg_;

};

class DbResult : public CoMsg
{
public:
	DbResult(BinOutputPacket<> &outpkg);
	virtual ~DbResult();
	
    virtual char *getData();
    virtual uint32_t length();

protected:
    BinOutputPacket<> &outpkg_;
};

// ===========================================
// query request
// ===========================================
class QueryRequest : public DbRequest
{
public:
	QueryRequest(BinInputPacket<> &inpkg);
	virtual ~QueryRequest();

    virtual int decode();

    int iSqlID;
    string sql;
    int IsRequestField;
    //ac::Array vParameters;
    int timeout;

};


// ===========================================
// query result
// ===========================================
typedef vector<string> fields;
typedef vector<fields> rows;

class QueryResult : public DbResult
{
public:
	QueryResult(BinOutputPacket<> &outpkg);
	virtual ~QueryResult();
	
    virtual int encode();

	int ret_code;
	string ret_msg;
	rows db_result_rows;
	int insertid;
	int affectedrows;
};


// ===========================================
// procedure request
// ===========================================
class ProcedureRequest : public DbRequest
{
public:
	ProcedureRequest(BinInputPacket<> &inpkg);
	virtual ~ProcedureRequest();

};


// ===========================================
// procedure result
// ===========================================
class ProcedureResult : public DbResult
{
public:
	ProcedureResult(BinOutputPacket<> &outpkg);
	virtual ~ProcedureResult();

};


// ===========================================
// query mutil request
// ===========================================
class QueryMutilRequest : public DbRequest
{
public:
	QueryMutilRequest(BinInputPacket<> &inpkg);
	virtual ~QueryMutilRequest();

};


// ===========================================
// query mutil result
// ===========================================
class QueryMutilResult : public DbResult
{
public:
	QueryMutilResult(BinOutputPacket<> &outpkg);
	virtual ~QueryMutilResult();

};

#endif

#endif //_MYSQL_MSG_H_




