/**
 * @filedesc: 
 * request.cpp, all request implements here
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/14 20:02:59
 * @modify:
 *
**/
#include <google/protobuf/text_format.h> // header for TextFormat

#include "backend_request.h"
#include "server_app.h"

using namespace utils;
using namespace google::protobuf;
using namespace com::adv::msg;

void PrintPbData(const Message &message, const char *desc = "show proto msg info:")
{
    // log protobuf info
    const char *tag = "print protobuf info:";
    if (NULL != desc)
    {
        tag = desc;
    }
    std::string printer;
    TextFormat::PrintToString(message, &printer);
    LOG(DEBUG)("show %s\n%s", tag, printer.c_str());
}



BackendResult::BackendResult(BinInputPacket<> &inpkg)
    : inpkg_(inpkg)
    , trans_id_(0)
{
}
BackendResult::~BackendResult()
{
}

int BackendResult::decode()
{
    LOG(INFO)("call base BackendResult::decode()");
    return 0;    
}

COHEADER BackendResult::get_head()
{
	return coheader_;
}

BackendRequest::BackendRequest()
	: data_(NULL)
	, data_len_(0)
	, ret_code_(0)
	, ret_msg_("")
{
	data_ = ServerApp::Instance()->get_buffer();
}

BackendRequest::~BackendRequest()
{
}

char *BackendRequest::data()
{
	return data_;
}

uint32_t BackendRequest::byte_size()
{
	return data_len_;
}


int BackendRequest::encode()
{
	LOG(DEBUG)("base backend request encode head.");
	return 0;
}

void BackendRequest::set_head(const COHEADER &head)
{
	coheader_ = head;
}

const COHEADER &BackendRequest::get_head()
{
	return coheader_;
}

void BackendRequest::set_len(uint32_t pkglen)
{
	coheader_.len = pkglen;
	LOG(DEBUG)("set head [%s]", coheader_.print());
	COHEADER cohead = coheader_;
	cohead.len = htonl(coheader_.len);
	cohead.cmd = htonl(coheader_.cmd);
	cohead.seq = htonl(coheader_.seq);
	cohead.head_len = htonl(coheader_.head_len);
	cohead.uid = htonl(coheader_.uid);
	if (NULL == data_) {
		LOG(ERROR)("backend request set head len failed, data is null.");
		return ;
	}
	memcpy(data_, (void *)&cohead, sizeof(COHEADER));
}


BackendLoginRequest::BackendLoginRequest()
{
}

BackendLoginRequest::~BackendLoginRequest()
{
}


int BackendLoginRequest::encode()
{
    BinOutputPacket<> outpkg(data_, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    // set transid
    backend_login_req_.set_transid(trans_id_);
    int body_len = backend_login_req_.ByteSize();
    data_len_ = head_len + body_len;
    COHEADER coheader = coheader_;
    coheader.len = data_len_;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackendLoginRequest encode msg failed.");
        return -1;
    }

    PrintPbData(backend_login_req_, coheader.print());
    backend_login_req_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;

}


void BackendLoginRequest::set_token(const string &token)
{
    backend_login_req_.set_token(token);
}   

void BackendLoginRequest::set_devid(const string &devid)
{
    backend_login_req_.set_deviceid(devid);
}

void BackendLoginRequest::set_device_type(uint32_t dev_type)
{
    backend_login_req_.set_devicetype(dev_type);
}   

void BackendLoginRequest::set_passwd(const string &passwd)
{
    backend_login_req_.set_passwd(passwd);
}   

void BackendLoginRequest::set_condid(uint32_t condid)
{
    backend_login_req_.set_condid(condid);
}   

void BackendLoginRequest::set_loginseq(uint32_t seqid)
{
    backend_login_req_.set_loginseq(seqid);
}   



BackendLoginResult::BackendLoginResult(BinInputPacket<> &inpkg)
    : BackendResult(inpkg)
    , ret_(0)
    , err_msg_("")
    , key_("")
    , uid_(0)
    , loginseq_(0)
{
}


BackendLoginResult::~BackendLoginResult()
{
}
	
int BackendLoginResult::decode()
{
    int ret = 0;
	inpkg_.offset_head(sizeof(COHEADER));
	inpkg_.get_head(coheader_);

    LOG(INFO)("BackendLoginResult, decode.");

    char *pbody = inpkg_.getCur();
    int len = inpkg_.remainLength();
    LoginResult back_result;
    if (!back_result.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackendLoginResult ParsePartialFromArray failed.");
    }

    uid_ = back_result.uid();
    loginseq_ = back_result.loginseq();
    trans_id_ = back_result.transid();
    const RetBase &ret_base = back_result.retbase();
    ret_ = ret_base.retcode();
    err_msg_ = ret_base.retmsg();

    PrintPbData(back_result);
    return ret;    
}


UserKeepAliveMsg::UserKeepAliveMsg ()
{
}

UserKeepAliveMsg::~UserKeepAliveMsg()
{
}


int UserKeepAliveMsg::encode()
{
    BinOutputPacket<> outpkg(data_, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = backend_request_.ByteSize();
    data_len_ = head_len + body_len;
    COHEADER coheader = coheader_;
    coheader.len = data_len_;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("UserKeepAliveMsg encode msg failed.");
        return -1;
    }

    PrintPbData(backend_request_, coheader.print());
    backend_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}



void UserKeepAliveMsg::set_uid(uint32_t uid)
{
    backend_request_.set_uid(uid);
}


void UserKeepAliveMsg::set_cond_id(uint32_t cond_id)
{
    backend_request_.set_condid(cond_id);
}


void UserKeepAliveMsg::set_device_type(uint32_t device_type)
{
    backend_request_.set_devicetype(device_type);
}


void UserKeepAliveMsg::set_client_ver(uint32_t client_ver)
{
    backend_request_.set_version(client_ver);
}


void UserKeepAliveMsg::set_device_id(const string &device_id)
{
    backend_request_.set_deviceid(device_id);
}

BackendSyncRequest::BackendSyncRequest()
{
}

BackendSyncRequest::~BackendSyncRequest()
{
}


void BackendSyncRequest::set_syncpoint(const uint64_t &point)
{
    backend_request_.set_syncpoint(point);
}

void BackendSyncRequest::set_synctype(uint32_t type)
{
    backend_request_.set_synctype(type);
}

void BackendSyncRequest::set_synclimit(uint32_t limit)
{
    backend_request_.set_synclimit(limit);
}


int BackendSyncRequest::encode()
{
    BinOutputPacket<> outpkg(data_, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    // set transid
    backend_request_.set_transid(trans_id_);
    int body_len = backend_request_.ByteSize();
    data_len_ = head_len + body_len;
    COHEADER coheader = coheader_;
    coheader.len = data_len_;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackendSyncRequest encode msg failed.");
        return -1;
    }

    PrintPbData(backend_request_, coheader.print());
    backend_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}   


BackendSyncResult::BackendSyncResult(BinInputPacket<> &inpkg)
    : BackendResult(inpkg)
	, synctype_(0)
	, continueflag_(0)
	, ret_value_(0)
	, maxtaskid_(0)
	, err_msg_("")
{
}

BackendSyncResult::~BackendSyncResult()
{
    // delete
    uint32_t size = task_vec_.size();
    for (uint32_t i = 0; i < size; ++i) 
    {
        LOG(DEBUG)("[memory] delete task info, total:%u, index:%u", size, i);
        CustomTaskInfo *task = task_vec_[i];
        SAFE_DELETE(task);
    }
}

int BackendSyncResult::decode()
{
    int ret = 0;
	inpkg_.offset_head(sizeof(COHEADER));
	inpkg_.get_head(coheader_);

    LOG(INFO)("BackendSyncResult, decode.");
    SrvSyncTaskResult back_result;

    char *pbody = inpkg_.getCur();
    int len = inpkg_.remainLength();
    if (!back_result.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackendSyncResult ParsePartialFromArray failed.");
    }

    trans_id_ = back_result.transid();
    synctype_ = back_result.synctype();
    continueflag_ = back_result.continueflag();
    maxtaskid_ = back_result.maxtaskid();
    const RetBase &ret_base = back_result.retbase();
    ret_value_ = ret_base.retcode();
    err_msg_ = ret_base.retmsg();

    int size = back_result.taskinfos_size();
    for (int i = 0; i < size; ++i)
    {
        const com::adv::msg::TaskInfo& taskinfo = back_result.taskinfos(i);
        CustomTaskInfo *custom_taskinfo = ConvertTask(taskinfo);
        task_vec_.push_back(custom_taskinfo);
    }

    PrintPbData(back_result);
    return ret;    
}


CustomTaskInfo *BackendSyncResult::ConvertTask(const com::adv::msg::TaskInfo& taskinfo)
{
    CustomTaskInfo *custom_taskinfo = new CustomTaskInfo;
    LOG(DEBUG)("[memory] new taskinfo.");
    custom_taskinfo->taskid = taskinfo.taskid();
    custom_taskinfo->tasktype = taskinfo.tasktype();
    custom_taskinfo->taskname = taskinfo.taskname();
    custom_taskinfo->tasklink = taskinfo.tasklink();
    custom_taskinfo->taskdesc = taskinfo.taskdesc();
    custom_taskinfo->taskpkgname = taskinfo.taskpkgname();
    custom_taskinfo->tasksize = taskinfo.tasksize();
    custom_taskinfo->taskprice = taskinfo.taskprice();
    custom_taskinfo->taskptstep = taskinfo.taskptstep();
    custom_taskinfo->taskpcstep = taskinfo.taskpcstep();
    custom_taskinfo->taskstatus = taskinfo.taskstatus();
    custom_taskinfo->taskpublisher = taskinfo.taskpublisher();
    custom_taskinfo->taskicon = taskinfo.taskicon();
    custom_taskinfo->tasktotalnum = taskinfo.tasktotalnum();
    custom_taskinfo->taskusednum = taskinfo.taskusednum();
    custom_taskinfo->taskstime = taskinfo.taskstime();
    custom_taskinfo->tasketime = taskinfo.tasketime();
    return custom_taskinfo;
}

BackAckRequest::BackAckRequest()
{

}

BackAckRequest::~BackAckRequest()
{

}

int BackAckRequest::encode()
{
    BinOutputPacket<> outpkg(data_, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    // set transid
    backend_request_.set_transid(trans_id_);
    int body_len = backend_request_.ByteSize();
    data_len_ = head_len + body_len;
    COHEADER coheader = coheader_;
    coheader.len = data_len_;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackAckRequest encode msg failed.");
        return -1;
    }

    PrintPbData(backend_request_, coheader.print());
    backend_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}

void BackAckRequest::set_type(uint32_t type)
{
    backend_request_.set_type(type);
}



void BackAckRequest::add_id(uint64_t id)
{
    backend_request_.add_id(id);
}

BackReportRequest::BackReportRequest()
{
}

BackReportRequest::~BackReportRequest()
{
}


int BackReportRequest::encode()
{
    BinOutputPacket<> outpkg(data_, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    // set transid
    backend_request_.set_transid(trans_id_);
    int body_len = backend_request_.ByteSize();
    data_len_ = head_len + body_len;
    COHEADER coheader = coheader_;
    coheader.len = data_len_;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackReportRequest encode msg failed.");
        return -1;
    }

    PrintPbData(backend_request_, coheader.print());
    backend_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}   


void BackReportRequest::set_task_id(const uint64_t &point)
{
    backend_request_.mutable_taskinfo()->set_taskid(point);
}

void BackReportRequest::set_task_type(uint32_t type)
{
    backend_request_.mutable_taskinfo()->set_tasktype(type);
}

void BackReportRequest::set_task_curr_step(uint32_t limit)
{
    backend_request_.mutable_taskinfo()->set_taskpcstep(limit);
}

void BackReportRequest::set_task_total_step(uint32_t limit)
{
    backend_request_.mutable_taskinfo()->set_taskptstep(limit);
}




BackendIncomeRequest::BackendIncomeRequest ()
{
}

BackendIncomeRequest::~BackendIncomeRequest()
{
}


int BackendIncomeRequest::encode()
{
    BinOutputPacket<> outpkg(data_, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    // set transid
    backend_request_.set_transid(trans_id_);
    int body_len = backend_request_.ByteSize();
    data_len_ = head_len + body_len;
    COHEADER coheader = coheader_;
    coheader.len = data_len_;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackendIncomeRequest encode msg failed.");
        return -1;
    }

    PrintPbData(backend_request_, coheader.print());
    backend_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}   

// result
BackendIncomeResult::BackendIncomeResult(BinInputPacket<> &inpkg)
    : BackendResult(inpkg)
    , ret_value_(0)
	, err_msg_("")
	, pre_cash_("")
	, useable_cash_("")
	, fetched_cash_("")
{
}

BackendIncomeResult::~BackendIncomeResult()
{
}


int BackendIncomeResult::decode()
{
    int ret = 0;
	inpkg_.offset_head(sizeof(COHEADER));
	inpkg_.get_head(coheader_);

    LOG(INFO)("BackendIncomeResult, decode.");

    char *pbody = inpkg_.getCur();
    int len = inpkg_.remainLength();
    SrvGetIncomeResult back_result;
    if (!back_result.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackendIncomeResult ParsePartialFromArray failed.");
    }

    pre_cash_ = back_result.pre_cash();
    useable_cash_ = back_result.useable_cash();
    fetched_cash_ = back_result.fetched_cash();
    trans_id_ = back_result.transid();
    const RetBase &ret_base = back_result.retbase();
    ret_value_ = ret_base.retcode();
    err_msg_ = ret_base.retmsg();

    PrintPbData(back_result);
    return ret;    
}


BackendStartTaskRequest::BackendStartTaskRequest()
{
}

BackendStartTaskRequest::~BackendStartTaskRequest()
{
}

int BackendStartTaskRequest::encode()
{
    BinOutputPacket<> outpkg(data_, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = backend_request_.pb_msg().ByteSize();
    data_len_ = head_len + body_len;
    COHEADER coheader = coheader_;
    coheader.len = data_len_;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackendStartTaskRequest encode msg failed.");
        return -1;
    }

    PrintPbData(backend_request_.pb_msg(), coheader.print());
    backend_request_.pb_msg().SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}


BackendStartTaskResult::BackendStartTaskResult (BinInputPacket<> &inpkg)
    :BackendResult(inpkg)
{
}

BackendStartTaskResult::~BackendStartTaskResult()
{
}

int BackendStartTaskResult::decode()
{
    int ret = 0;
	inpkg_.offset_head(sizeof(COHEADER));
	inpkg_.get_head(coheader_);

    LOG(INFO)("BackendStartTaskResult, decode.");

    char *pbody = inpkg_.getCur();
    int len = inpkg_.remainLength();
    SrvInnerCommonMsg back_result;
    com::adv::msg::InnerCommonMsg &pb_msg = back_result.mutable_pb_msg();
    if (!pb_msg.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackendStartTaskResult ParsePartialFromArray failed.");
    }

    for (int i = 0; i < pb_msg.items_size(); ++i)
    {
        const com::adv::msg::PDataCell &cell = pb_msg.items(i);
        if (StringUtil::is_equal(cell.tag(), "type") == 0)
        {
            task_type_ = cell.i32_value();
        }
        if (StringUtil::is_equal(cell.tag(), "ret") == 0)
        {
            ret_value_ = cell.i32_value();
        }
        if (StringUtil::is_equal(cell.tag(), "err_msg") == 0)
        {
            ret_msg_ = cell.str_value();
        }
        if (StringUtil::is_equal(cell.tag(), "id") == 0)
        {
            task_id_ = cell.i64_value();
            LOG(DEBUG)("== TASK ID:%lu, id:%lu", cell.i64_value(), task_id_);
        }
    }
    trans_id_ = pb_msg.transid();
    time_ = pb_msg.time();

    PrintPbData(pb_msg);
    return ret;    

}

void BackendStartTaskResult::set_ret(int value)
{
    ret_value_ = value;
}

void BackendStartTaskResult::set_task_id(uint32_t value)
{
    task_id_ = value;
}

void BackendStartTaskResult::set_task_type(uint32_t value)
{
    task_type_ = value;
}



BackendThirdPartyLoginRequest::BackendThirdPartyLoginRequest()
{
}


BackendThirdPartyLoginRequest::~BackendThirdPartyLoginRequest()
{
}

int BackendThirdPartyLoginRequest::encode()
{
    BinOutputPacket<> outpkg(data_, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    // set transid
    backend_request_.set_transid(trans_id_);
    int body_len = backend_request_.ByteSize();
    data_len_ = head_len + body_len;
    COHEADER coheader = coheader_;
    coheader.len = data_len_;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackendLoginRequest encode msg failed.");
        return -1;
    }

    PrintPbData(backend_request_, coheader.print());
    backend_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;

}

BackendThirdPartyLoginResult::BackendThirdPartyLoginResult (BinInputPacket<> &inpkg)
    :BackendResult(inpkg)
{
}


BackendThirdPartyLoginResult::~BackendThirdPartyLoginResult()
{
}


int BackendThirdPartyLoginResult::decode()
{
    int ret = 0;
	inpkg_.offset_head(sizeof(COHEADER));
	inpkg_.get_head(coheader_);

    LOG(INFO)("BackendThirdPartyLoginResult, decode.");

    char *pbody = inpkg_.getCur();
    int len = inpkg_.remainLength();
    SrvLoginResult back_result;
    if (!back_result.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackendThirdPartyLoginResult ParsePartialFromArray failed.");
    }

    const com::adv::msg::RetBase &ret_base = back_result.retbase();
    ret_value_ = ret_base.retcode();
    ret_msg_ = ret_base.retmsg();
    
    type_ = back_result.accounttype();
    time_ = back_result.time();
    trans_id_ = back_result.transid();

    PrintPbData(back_result);
    return ret;    

}


BackendWithdrawRequest::BackendWithdrawRequest()
{
}

BackendWithdrawRequest::~BackendWithdrawRequest()
{
}

int BackendWithdrawRequest::encode()
{
    BinOutputPacket<> outpkg(data_, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    // set transid
    backend_request_.set_transid(trans_id_);
    int body_len = backend_request_.ByteSize();
    data_len_ = head_len + body_len;
    COHEADER coheader = coheader_;
    coheader.len = data_len_;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackendWithdrawRequest encode msg failed.");
        return -1;
    }

    PrintPbData(backend_request_, coheader.print());
    backend_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;

}



BackendWithdrawResult::BackendWithdrawResult(BinInputPacket<> &inpkg)
    :BackendResult(inpkg)
{

}

BackendWithdrawResult::~BackendWithdrawResult()
{
}

int BackendWithdrawResult::decode()
{
    int ret = 0;
	inpkg_.offset_head(sizeof(COHEADER));
	inpkg_.get_head(coheader_);

    LOG(INFO)("BackendWithdrawResult, decode.");

    char *pbody = inpkg_.getCur();
    int len = inpkg_.remainLength();
    SrvWithdrawResult back_result;
    if (!back_result.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackendWithdrawResult ParsePartialFromArray failed.");
    }

    const com::adv::msg::RetBase &ret_base = back_result.retbase();
    ret_value_ = ret_base.retcode();
    ret_msg_ = ret_base.retmsg();
    
    withdraw_type_ = back_result.type();
    withdraw_cash_ = back_result.withdraw_cash();
    trade_no_ = back_result.trade_id();
    desc_ = back_result.desc();
    time_ = back_result.time();
    trans_id_ = back_result.transid();

    PrintPbData(back_result);
    return ret;    

}


