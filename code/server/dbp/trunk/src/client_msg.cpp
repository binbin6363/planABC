#include "client_msg.h"

#include <google/protobuf/text_format.h> // header for TextFormat
#include <sstream>

#include "comm.h"
#include "log.h"
#include "constants.h"
#include "protocols.h"
#include "string_util.h"

using namespace utils;
using namespace common;
using namespace com::adv::msg;

void PrintPbData(const Message &message, const char *desc)
{
    // log protobuf info
    const char *tag = "print protobuf info:";
    if (NULL != desc)
    {
        tag = desc;
    }
    std::string printer;
    TextFormat::PrintToString(message, &printer);
    LOG(DEBUG)("%s\n%s", tag, printer.c_str());
}


/*
CliBeatMsg::CliBeatMsg()
    :CoMsg()
{
}


CliBeatMsg::~CliBeatMsg()
{
}

int CliBeatMsg::Decode(const char *data, uint32_t length)
{
    return CoMsg::Decode(data, length);
}

int CliBeatMsg::Encode(char *data, uint32_t &length) const 
{
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    outpkg.offset_head(sizeof(COHEADER));
    outpkg.set_head(msg_header());
    if (!outpkg.good())
    {
        LOG(ERROR)("CliBeatMsg encode msg failed.");
        return -1;
    }
    length = outpkg.length();
    LOG(DEBUG)("CliBeatMsg encode msg succeed. %s", ToString().c_str());
    return 0;
}   
*/

FrontLoginMsg::FrontLoginMsg()
    : CoMsg()
{
}

FrontLoginMsg::~FrontLoginMsg()
{
}

int FrontLoginMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontLoginMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontLoginMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontLoginMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    
}


BackLoginMsg::BackLoginMsg()
    : CoMsg()
    , uid_(0)
    , transid_(0)
    , loginseq_(0)
    , key_("")
{
}

BackLoginMsg::~BackLoginMsg()
{
}

int BackLoginMsg::Encode(char * data, uint32_t & length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackLoginMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, coheader.print());
    front_result_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}


void BackLoginMsg::set_uid(uint32_t uid)
{
    front_result_.set_uid(uid);
}


void BackLoginMsg::set_transid(uint32_t transid)
{
    front_result_.set_transid(transid);
}


void BackLoginMsg::set_key(const string &key)
{
    front_result_.set_key(key);
}


void BackLoginMsg::set_loginseq(uint32_t loginseq)
{
    front_result_.set_loginseq(loginseq);
}


FrontWithdrawMsg::FrontWithdrawMsg()
{
}

FrontWithdrawMsg::~FrontWithdrawMsg()
{
}

int FrontWithdrawMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontWithdrawMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontWithdrawMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontWithdrawMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    

}




BackWithdrawMsg::BackWithdrawMsg()
{
}

BackWithdrawMsg::~BackWithdrawMsg()
{
}

int BackWithdrawMsg::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackWithdrawMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, coheader.print());
    front_result_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;

}


void BackWithdrawMsg::SetTransId(uint32_t value)
{
    front_result_.set_transid(value);
}


BackQueryRequest::BackQueryRequest ()
    : hash_id_(0)
{
}


BackQueryRequest::~BackQueryRequest()
{

}

int BackQueryRequest::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = back_request_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    coheader.cmd = CMD_DB_OPERATE;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackQueryRequest encode msg failed.");
        return -1;
    }

    PrintPbData(back_request_, coheader.print());
    back_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;

}


BackQueryResult::BackQueryResult()
    : null_str_("")
{
}


BackQueryResult::~BackQueryResult()
{

}

int BackQueryResult::Decode(const char * data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("BackQueryResult get header failed.");
        return ret;
    }
    LOG(INFO)("BackQueryResult, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!back_result_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackQueryResult ParsePartialFromArray failed.");
    }

    int row_num = back_result_.rows_size();
    for (int i = 0; i < row_num; ++i) {
        Row row_item;
        const PRow &p_row = back_result_.rows(i);
        int filed_num = p_row.field_size();
        for (int j = 0; j < filed_num; ++j) {
            const PField &p_field = p_row.field(j);
            row_item[p_field.name()] = p_field.value();
        }
        rows_.push_back(row_item);
    }
    PrintPbData(back_result_, msg_header().print());
    return ret;    
}


int BackQueryResult::get_err_no() const
{
    const com::adv::msg::RetBase &ret_base = back_result_.retbase();
    return ret_base.retcode();
}

string BackQueryResult::get_err_msg() const
{
    const com::adv::msg::RetBase &ret_base = back_result_.retbase();
    return ret_base.retmsg();
}

uint32_t BackQueryResult::affectedrows()
{
    return back_result_.affectedrows();
}

const string &BackQueryResult::get_value(uint32_t row, const string &field_name)
{
    if (rows_.empty() || rows_.size() < row)
    {
        LOG(WARN)("result from mysql is empty or size no match. result size:%zu, row:%u"
            , rows_.size(), row);
        return null_str_;
    }
    const Row &row_item = rows_.at(row);
    Row::const_iterator iter = row_item.find(field_name);
    if (iter != row_item.end()) {
        return iter->second;
    }
    LOG(DEBUG)("not found value in row:%u, field name:%s", row, field_name.c_str());
    return null_str_;
}

// TODO: 
BackRedisPxyRequest::BackRedisPxyRequest ()
{
}

BackRedisPxyRequest::~BackRedisPxyRequest()
{
}

int BackRedisPxyRequest::Encode(char *data, uint32_t &length) const
{
    LOCAL_UNREFERENCED_PARAMETER(data);
    LOCAL_UNREFERENCED_PARAMETER(length);
//    CoMsg::Encode(data, length);
//    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
//    int head_len = sizeof(COHEADER);
//    outpkg.offset_head(head_len);

//    int body_len = back_request_.ByteSize();
//    length = head_len + body_len;
//    COHEADER coheader = msg_header();
//    coheader.len = length;
//    coheader.cmd = CMD_DB_OPERATE;
//    outpkg.set_head(coheader);
//    if (!outpkg.good())
//    {
//        LOG(ERROR)("BackQueryRequest encode msg failed.");
//        return -1;
//    }

//    PrintPbData(back_request_, coheader.print());
//    back_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}


void BackRedisPxyRequest::set_key(const string &key)
{
    redis_result_str_ = key;
}   


// TODO: 
BackRedisPxyResult::BackRedisPxyResult()
{
}

BackRedisPxyResult::~BackRedisPxyResult()
{
}

int BackRedisPxyResult::Decode(const char * data, uint32_t length)
{
    LOCAL_UNREFERENCED_PARAMETER(data);
    LOCAL_UNREFERENCED_PARAMETER(length);
//    int ret = CoMsg::Decode(data, length);
//    if (0 != ret)
//    {
//        LOG(ERROR)("BackQueryResult get header failed.");
//        return ret;
//    }
//    LOG(INFO)("BackQueryResult, Decode. [%s]", msg_header().print());

//    const char *pbody = data + msg_header().head_len;
//    int len = length - msg_header().head_len;
//    if (!back_result_.ParsePartialFromArray(pbody, len))
//    {
//        ret = 2;
//        LOG(ERROR)("BackQueryResult ParsePartialFromArray failed.");
//    }

//    int row_num = back_result_.rows_size();
//    for (int i = 0; i < row_num; ++i) {
//        Row row_item;
//        const PRow &p_row = back_result_.rows(i);
//        int filed_num = p_row.field_size();
//        for (int j = 0; j < filed_num; ++j) {
//            const PField &p_field = p_row.field(j);
//            row_item[p_field.name()] = p_field.value();
//        }
//        rows_.push_back(row_item);
//    }
//    PrintPbData(back_result_, msg_header().print());
    return 0;    
}

    


uint32_t BackRedisPxyResult::get_id()
{
    return utils::StringUtil::strtou32(redis_request_str_);
}


FrontSyncTaskMsg::FrontSyncTaskMsg ()
{
}

FrontSyncTaskMsg::~FrontSyncTaskMsg()
{
}


int FrontSyncTaskMsg::Decode(const char * data, uint32_t length) 
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontSyncTaskMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontSyncTaskMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontSyncTaskMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    
}

const SrvSyncTaskRequest &FrontSyncTaskMsg::pb_msg()
{
    return front_request_;
}

BackSyncTaskMsg::BackSyncTaskMsg ()
{
}


BackSyncTaskMsg::~BackSyncTaskMsg()
{
}


int BackSyncTaskMsg::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackSyncTaskMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, coheader.print());
    front_result_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}

SrvSyncTaskResult &BackSyncTaskMsg::mutable_pb_msg()
{
    return front_result_;
}

void BackSyncTaskMsg::add_taskid(const uint64_t &id)
{
    com::adv::msg::TaskInfo *task = front_result_.add_taskinfos();
    task->set_taskid(id);
}


void BackSyncTaskMsg::add_task(const Row &row_item)
{
    com::adv::msg::TaskInfo *task = front_result_.add_taskinfos();
    RowCIter row_iter = row_item.find(DB::DB_TASK_ID_ITEM);
    uint64_t taskid = StringUtil::strtou64(row_iter->second);
    task->set_taskid(taskid);
    row_iter = row_item.find(DB::DB_TASK_FLAG);
    uint32_t task_status = StringUtil::strtou32(row_iter->second);
    task->set_taskstatus(task_status);
    row_iter = row_item.find(DB::DB_TASK_PROGRESS);
    uint32_t task_progress = StringUtil::strtou32(row_iter->second);
    task->set_taskpcstep(task_progress);
    row_iter = row_item.find(DB::DB_TASK_AWARD_CASH);
    task->set_taskprice(row_iter->second);
}

FrontGetIncomeMsg::FrontGetIncomeMsg ()
{
}


FrontGetIncomeMsg::~FrontGetIncomeMsg()
{
}


int FrontGetIncomeMsg::Decode(const char * data, uint32_t length) 
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontGetIncomeMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontGetIncomeMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontGetIncomeMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    
}

const SrvGetIncomeRequest &FrontGetIncomeMsg::pb_msg()
{
    return front_request_;
}   

uint32_t FrontGetIncomeMsg::get_transid()
{
    return front_request_.transid();
}

BackGetIncomeMsg::BackGetIncomeMsg ()
{
}

BackGetIncomeMsg::~BackGetIncomeMsg()
{
}

int BackGetIncomeMsg::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackGetIncomeMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, coheader.print());
    front_result_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}


SrvGetIncomeResult &BackGetIncomeMsg::mutable_pb_msg()
{
    return front_result_;
}   

void BackGetIncomeMsg::set_pre_cash(const string &cash)
{
    front_result_.set_pre_cash(cash);
}

void BackGetIncomeMsg::set_useable_cash(const string &cash)
{
    front_result_.set_useable_cash(cash);
}

void BackGetIncomeMsg::set_fetched_cash(const string &cash)
{
    front_result_.set_fetched_cash(cash);
}

void BackGetIncomeMsg::set_transid(uint32_t transid)
{
    front_result_.set_transid(transid);
}

FrontSaveTaskMsg::FrontSaveTaskMsg()
{

}

FrontSaveTaskMsg::~FrontSaveTaskMsg()
{

}

int FrontSaveTaskMsg::Decode(const char * data, uint32_t length) 
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontSyncTaskMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontSaveTaskMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontSaveTaskMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    
}


const SrvSaveTaskRequest &FrontSaveTaskMsg::pb_msg()
{
    return front_request_;
}


uint32_t FrontSaveTaskMsg::transid()
{
    return front_request_.transid();
}

BackSaveTaskMsg::BackSaveTaskMsg()
{

}

BackSaveTaskMsg::~BackSaveTaskMsg()
{

}

int BackSaveTaskMsg::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackSaveTaskMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, coheader.print());
    front_result_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}

SrvSaveTaskResult &BackSaveTaskMsg::mutable_pb_msg()
{
    return front_result_;
}

void BackSaveTaskMsg::set_transid(uint32_t transid)
{
    front_result_.set_transid(transid);
}


// ======================================================
// 11020, 上报数据协议，前端过来的
// ======================================================
FrontReportMsg::FrontReportMsg ()
{
}

FrontReportMsg::~FrontReportMsg()
{
}

int FrontReportMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontReportMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontReportMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontReportMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    
}

// 到后端的请求，也调用这个
int FrontReportMsg::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_request_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;

    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("FrontReportMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_request_, coheader.print());
    front_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}


uint32_t FrontReportMsg::transid() const
{
    return front_request_.transid();
}

uint32_t FrontReportMsg::task_type() const
{
    return front_request_.taskinfo().tasktype();
}

uint64_t FrontReportMsg::taskid() const
{
    return front_request_.taskinfo().taskid();
}

string FrontReportMsg::reward() const
{
    return front_request_.reward();
}

uint32_t FrontReportMsg::task_step() const
{
    return front_request_.taskinfo().taskpcstep();
}

BackReportMsg::BackReportMsg()
    : ret_value_(0)
    , ret_msg_("")
{
}

BackReportMsg::~BackReportMsg()
{
}

int BackReportMsg::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackReportMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, coheader.print());
    front_result_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}

void BackReportMsg::set_taskid(uint64_t taskid)
{
    front_result_.set_taskid(taskid);
}

void BackReportMsg::set_transid(uint32_t transid)
{
    front_result_.set_transid(transid);
}

void BackReportMsg::set_task_type(uint32_t task_type)
{
    front_result_.set_tasktype(task_type);
}

void BackReportMsg::set_done_step(uint32_t done_step)
{
    front_result_.set_done_step(done_step);
}

void BackReportMsg::set_time(uint32_t time)
{
    front_result_.set_report_time(time);
}

void BackReportMsg::set_err_no(uint32_t err_no)
{
    ret_value_ = err_no;
}

int BackReportMsg::get_err_no()  const
{
    return ret_value_;
}

void BackReportMsg::set_err_msg(const string &err_msg)
{
    ret_msg_ = err_msg;
}

string BackReportMsg::get_err_msg() const
{
    return ret_msg_;
}


FrontThirdPartyLoginMsg::FrontThirdPartyLoginMsg()
{
}

FrontThirdPartyLoginMsg::~FrontThirdPartyLoginMsg()
{
}

int FrontThirdPartyLoginMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontThirdPartyLoginMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontThirdPartyLoginMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontThirdPartyLoginMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    
}

// 到后端的请求，也调用这个
int FrontThirdPartyLoginMsg::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;

    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("FrontThirdPartyLoginMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, coheader.print());
    front_result_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}


CheckTaskRequest::CheckTaskRequest()
{
}

CheckTaskRequest::~CheckTaskRequest()
{
}

int CheckTaskRequest::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("CheckTaskRequest get header failed.");
        return ret;
    }
    LOG(INFO)("CheckTaskRequest, Decode. [%s]", msg_header().print());

    com::adv::msg::InnerCommonMsg &request = front_request_.mutable_pb_msg();

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!request.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("CheckTaskRequest ParsePartialFromArray failed.");
    }
    PrintPbData(request, msg_header().print());
    return ret;    
}

int CheckTaskRequest::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    const com::adv::msg::InnerCommonMsg &front_result = front_request_.pb_msg();
    int body_len = front_result.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("CheckTaskRequest encode msg failed.");
        return -1;
    }

    PrintPbData(front_result, coheader.print());
    front_result.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;

}

int CheckTaskRequest::RetCode() const
{
    const com::adv::msg::InnerCommonMsg &result = front_request_.pb_msg();
    for (int i = 0; i < result.items_size(); ++i)
    {
        const com::adv::msg::PDataCell &cell = result.items(i);
        if (0 == StringUtil::is_equal(cell.tag(), TAG_RET_CODE))
        {
            return cell.i32_value();
        }
    }
    return -1;
}

uint32_t CheckTaskRequest::GetTaskType() const
{
    const com::adv::msg::InnerCommonMsg &result = front_request_.pb_msg();
    for (int i = 0; i < result.items_size(); ++i)
    {
        const com::adv::msg::PDataCell &cell = result.items(i);
        if (0 == StringUtil::is_equal(cell.tag(), TAG_TYPE))
        {
            return cell.i32_value();
        }
    }
    return 0;
}

uint64_t CheckTaskRequest::GetTaskId() const
{
    const com::adv::msg::InnerCommonMsg &result = front_request_.pb_msg();
    for (int i = 0; i < result.items_size(); ++i)
    {
        const com::adv::msg::PDataCell &cell = result.items(i);
        if (0 == StringUtil::is_equal(cell.tag(), TAG_TASK_ID))
        {
            return cell.i64_value();
        }
    }
    return 0;
}

uint32_t CheckTaskRequest::GetTransId() const
{
    return front_request_.GetTransid();
}

void CheckTaskRequest::SetTaskType(uint32_t value)
{
    com::adv::msg::InnerCommonMsg &result = front_request_.mutable_pb_msg();
    front_request_.AddTagValue(TAG_TYPE, value);
}

void CheckTaskRequest::SetTaskId(const uint64_t &value)
{
    com::adv::msg::InnerCommonMsg &result = front_request_.mutable_pb_msg();
    front_request_.AddTagValue(TAG_TASK_ID, value);
}

void CheckTaskRequest::SetTransId(uint32_t value)
{
    com::adv::msg::InnerCommonMsg &result = front_request_.mutable_pb_msg();
    result.set_transid(value);
}


QueryTaskInfoRequest::QueryTaskInfoRequest()
{
}

QueryTaskInfoRequest::~QueryTaskInfoRequest()
{

}

int QueryTaskInfoRequest::Decode(const char * data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("QueryTaskInfoRequest get header failed.");
        return ret;
    }
    LOG(INFO)("QueryTaskInfoRequest, Decode. [%s]", msg_header().print());

    com::adv::msg::InnerCommonMsg &request = front_request_.mutable_pb_msg();

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!request.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("QueryTaskInfoRequest ParsePartialFromArray failed.");
    }
    PrintPbData(request, msg_header().print());
    return ret;    
}


int QueryTaskInfoRequest::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    const com::adv::msg::InnerCommonMsg &front_result = front_request_.pb_msg();
    int body_len = front_result.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("QueryTaskInfoRequest encode msg failed.");
        return -1;
    }

    PrintPbData(front_result, coheader.print());
    front_result.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}


void QueryTaskInfoRequest::SetTaskId(const uint64_t &value)
{
    front_request_.AddTagValue(TAG_TASK_ID, value);
}

void QueryTaskInfoRequest::SetTaskStatus(uint32_t value)
{
    front_request_.AddTagValue(TAG_TASK_STATUS, value);
}

void QueryTaskInfoRequest::SetTaskAlgorithm(const string &value)
{
    front_request_.AddTagValue(TAG_TASK_ALGORITHM, value);
}

void QueryTaskInfoRequest::SetTaskPreCash(const string &value)
{
    front_request_.AddTagValue(TAG_TASK_PRE_CASH, value);
}

void QueryTaskInfoRequest::SetTaskAwardCash(const string &value)
{
    front_request_.AddTagValue(TAG_TASK_AWARD_CASH, value);
}

void QueryTaskInfoRequest::SetTaskLimitNum(uint32_t value)
{
    front_request_.AddTagValue(TAG_TASK_LIMIT_NUM, value);
}

void QueryTaskInfoRequest::SetTaskStartNum(uint32_t value)
{
    front_request_.AddTagValue(TAG_TASK_START_NUM, value);
}

void QueryTaskInfoRequest::SetTaskStartTime(uint32_t value)
{
    front_request_.AddTagValue(TAG_TASK_START_TIME, value);
}

void QueryTaskInfoRequest::SetTaskEndTime(uint32_t value)
{
    front_request_.AddTagValue(TAG_TASK_END_TIME, value);
}

uint32_t QueryTaskInfoRequest::GetTransId() const
{
    return front_request_.GetTransid();
}

void QueryTaskInfoRequest::SetTransId(uint32_t value)
{
    front_request_.SetTransid(value);
}

void QueryTaskInfoRequest::SetTaskInfo(const string &task_info)
{
    front_request_.AddTagValue(TAG_TASK_INFO, task_info);
}

uint64_t QueryTaskInfoRequest::GetTaskId() const
{
    const com::adv::msg::InnerCommonMsg &result = front_request_.pb_msg();
    for (int i = 0; i < result.items_size(); ++i)
    {
        const com::adv::msg::PDataCell &cell = result.items(i);
        if (0 == StringUtil::is_equal(cell.tag(), TAG_TASK_ID))
        {
            return cell.i64_value();
        }
    }
    return 0;
}


