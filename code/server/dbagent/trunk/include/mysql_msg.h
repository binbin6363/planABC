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
#include "adv_protocol.pb.h"

using namespace com::adv::msg;


using namespace std;

class DbRequest : public Msg
{
public:
	DbRequest(BinInputPacket<> &inpkg);
	virtual ~DbRequest();


protected:
    BinInputPacket<> &inpkg_;

};

class DbResult : public Msg
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

    int sqlId;
    string sql;
    int isRequestField;
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

	void set_affected_rows(uint32_t row);
	void set_insertid(uint32_t id);
	void set_ret(int ret);
	void set_msg(const char *msg);
	void add_row(PRow &prow);

	int get_ret();

	
private:
	int ret_code;
	string ret_msg;
	rows db_result_rows;
	int insertid;
	int affectedrows;
    MysqlResult front_result_;
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



#endif //_MYSQL_MSG_H_




