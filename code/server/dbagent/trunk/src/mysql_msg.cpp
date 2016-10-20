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
#include "mysql_msg.h"

using namespace com::adv::msg;


DbRequest::DbRequest(BinInputPacket<> &inpkg)
    : Msg(inpkg)
    , inpkg_(inpkg)
{
}

DbRequest::~DbRequest()
{
}


DbResult::DbResult(BinOutputPacket<> &outpkg)
    : outpkg_(outpkg)
{
}


DbResult::~DbResult()
{
}

char *DbResult::getData()
{
    if (!outpkg_.good())
    {
        LOG(ERROR)("get data, output pkg is not good!");
    }
    return outpkg_.getData();
}

uint32_t DbResult::length()
{
    if (!outpkg_.good())
    {
        LOG(ERROR)("get length, output pkg is not good!");
    }
    return outpkg_.length();
}


QueryRequest::QueryRequest(BinInputPacket<> &inpkg)
    : DbRequest(inpkg)
    , sqlId(0)
    , sql("")
    , isRequestField(0)
    , timeout(0)
{
}

QueryRequest::~QueryRequest()
{
}

int QueryRequest::decode()
{
    int ret = 0;
    MysqlRequest mysql_request;
    if (!mysql_request.ParsePartialFromArray(inpkg_.getCur(), inpkg_.remainLength()))
    {
        ret = 2;
        LOG(ERROR)("FrontLoginMsg ParsePartialFromArray failed.");
    }
    PrintPbData(mysql_request, "query request decode");

    sqlId = mysql_request.sqlid();
    sql = mysql_request.sql();
    isRequestField = mysql_request.isrequestfield();
    timeout = mysql_request.timeout();
    return ret;
}

QueryResult::QueryResult(BinOutputPacket<> &outpkg)
    : DbResult(outpkg)
    , ret_code(0)
    , ret_msg("")
    , insertid(0)
    , affectedrows(0)
{
}

QueryResult::~QueryResult()
{
}

void QueryResult::set_affected_rows(uint32_t row)
{
    front_result_.set_affectedrows(row);
}

void QueryResult::set_insertid(uint32_t id)
{
    front_result_.set_insertid(id);
}

void QueryResult::set_ret(int ret)
{
    RetBase *ret_base = front_result_.mutable_retbase();
    ret_base->set_retcode(ret);
}

int QueryResult::get_ret()
{
    const RetBase &ret_base = front_result_.retbase();
    return ret_base.retcode();
}

void QueryResult::set_msg(const char *msg)
{
    RetBase *ret_base = front_result_.mutable_retbase();
    ret_base->set_retmsg(msg);
}

void QueryResult::add_row(PRow &prow)
{
    PRow *proto_row = front_result_.add_rows();
    proto_row->CopyFrom(prow);
}



int QueryResult::encode()
{
    int head_len = sizeof(COHEADER);
    outpkg_.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    int length = head_len + body_len;
    header_.len = length;
    header_.ReverseCoid();
    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("BackLoginMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, header_.print());
    front_result_.SerializePartialToArray(outpkg_.getCur(), outpkg_.remainLength());
    outpkg_.offset(body_len);
    return 0;
}


