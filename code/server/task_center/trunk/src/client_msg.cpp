/**
* file: client_msg.cpp
* desc: client_msg file. msg
* auth: bbwang
* date: 2015/11/4
*/

#include "client_msg.h"
#include <google/protobuf/text_format.h> // header for TextFormat
#include <sstream>
#include "comm.h"
#include "log.h"
#include "constants.h"
#include "string_util.h"
#include "protocols.h"
#include "task_cache.h"
#include "json/json.h"
#include "json/config.h"
#include "json/value.h"
#include "json/writer.h"

using namespace utils;
using namespace common;


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
    LOG(DEBUG)("show %s\n%s", tag, printer.c_str());
}


static char make_key[_1KB] = {0};
string makeTaskInfoKey(uint64_t point)
{
    int n = snprintf(make_key, _1KB, "%s_%020lu", TASK_INFO_PERFIX, point);
    make_key[n] = 0;
    LOG(DEBUG)("makeTaskInfoKey:%s", make_key);
    return string(make_key);
}

string makeTaskInfoValue(const StrMap &task_params)
{
    Json::StyledWriter writer;
	Json::Value root;
    root[TAG_VERSION] = "1";

    StrMapCIter cst_iter = task_params.begin();
    StrMapCIter cst_iter_end = task_params.end();
    for (; cst_iter != cst_iter_end; ++cst_iter)
    {
        if (cst_iter->first != "") {
            root[cst_iter->first] = cst_iter->second;
            LOG(DEBUG)("make task info, write k:'%s', v:'%s'"
                , cst_iter->first.c_str(), cst_iter->second.c_str());
        }
    }
    return writer.write(root);
}

int parseTaskInfoValue(const char *data, uint32_t len, TaskInfoItem &task)
{
    int ret = 0;
	Json::Reader reader;
	Json::Value root;
	string input_str(data, len);
	LOG(DEBUG)("parse task info:%s", input_str.c_str());
	reader.parse(input_str, root);

    string task_version("");
	if (root[TAG_VERSION].isString()) {
		task_version = root[TAG_VERSION].asString();
        if (StringUtil::is_equal(CURRENT_VERSION, task_version.c_str()) != 0)
        {
            LOG(ERROR)("task version not match, skip this task.");
            ret = -1;
            return ret;
        }
	} else {
		LOG(ERROR)("param:%s type is error", TAG_VERSION);
		return -1;
	}
	if (root[TAG_TASK_ID].isString()) {
        task.taskid = StringUtil::strtou64(root[TAG_TASK_ID].asString());
	} else {
		LOG(ERROR)("parse task, %s is error, skip this task.", TAG_TASK_ID);
		ret = -1;
        return ret;
	}
	if (root[TAG_TYPE].isString()) {
        task.tasktype= StringUtil::strtou32(root[TAG_TYPE].asString());
	} else {
		LOG(ERROR)("param:%s type is error", TAG_TYPE);
		ret = -1;
	}
	if (root[TAG_NAME].isString()) {
        task.taskname= root[TAG_NAME].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_NAME);
		ret = -1;
	}
	if (root[TAG_LINK].isString()) {
        task.tasklink = root[TAG_LINK].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_LINK);
		ret = -1;
	}
	if (root[TAG_DESC].isString()) {
        task.taskdesc = root[TAG_DESC].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_DESC);
		ret = -1;
	}
	if (root[TAG_SIZE].isString()) {
        task.tasksize = StringUtil::strtou32(root[TAG_SIZE].asString());
	} else {
		LOG(ERROR)("param:%s type is error", TAG_SIZE);
		ret = -1;
	}
	if (root[TAG_PAY].isString()) {
        task.taskprice = root[TAG_PAY].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_PAY);
		ret = -1;
	}
	if (root[TAG_TOTAL_STEP].isString()) {
        task.taskptstep = StringUtil::strtou32(root[TAG_TOTAL_STEP].asString());
	} else {
		LOG(ERROR)("param:%s type is error", TAG_TOTAL_STEP);
		ret = -1;
	}
	if (root[TAG_CURR_STEP].isString()) {
        task.taskpcstep = StringUtil::strtou32(root[TAG_CURR_STEP].asString());
	} else {
		LOG(ERROR)("param:%s type is error", TAG_CURR_STEP);
		ret = -1;
	}
	if (root[TAG_STATUS].isString()) {
        task.taskstatus = StringUtil::strtou32(root[TAG_STATUS].asString());
	} else {
		LOG(ERROR)("param:%s type is error", TAG_STATUS);
		ret = -1;
	}
	if (root[TAG_PUBLR_NAME].isString()) {
        task.taskpublisher = root[TAG_PUBLR_NAME].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_PUBLR_NAME);
		ret = -1;
	}
	if (root[TAG_ICON_LINK].isString()) {
        task.taskicon = root[TAG_ICON_LINK].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_ICON_LINK);
		ret = -1;
	}
	if (root[TAG_STIME].isString()) {
        task.taskstime = StringUtil::strtou32(root[TAG_STIME].asString());
	} else {
		LOG(ERROR)("param:%s type is error", TAG_STIME);
		ret = -1;
	}
	if (root[TAG_ETIME].isString()) {
        task.tasketime = StringUtil::strtou32(root[TAG_ETIME].asString());
	} else {
		LOG(ERROR)("param:%s type is error", TAG_ETIME);
		ret = -1;
	}
	if (root[TAG_TOTAL_NUM].isString()) {
        task.tasknum = StringUtil::strtou32(root[TAG_TOTAL_NUM].asString());
	} else {
		LOG(ERROR)("param:%s type is error", TAG_TOTAL_NUM);
		ret = -1;
	}
	if (root[TAG_USED_NUM].isString()) {
        task.taskusednum = StringUtil::strtou32(root[TAG_USED_NUM].asString());
	} else {
		LOG(ERROR)("param:%s type is error", TAG_USED_NUM);
		ret = -1;
	}
	if (root[TAG_SIGNATURE].isString()) {
        task.taskpkgname = root[TAG_SIGNATURE].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_SIGNATURE);
		ret = -1;
	}

    return ret;
}


string makeTaskProgressKey(uint32_t uid, uint64_t point)
{
    int n = snprintf(make_key, _1KB, "%s_%010u_%020lu", TASK_PROGRESS_PERFIX, uid, point);
    make_key[n] = 0;
    LOG(DEBUG)("makeTaskProgressKey:%s", make_key);
    return string(make_key);
}




FrontSyncTaskMsg::FrontSyncTaskMsg()
    : CoMsg()
{
}

FrontSyncTaskMsg::~FrontSyncTaskMsg()
{
}

int FrontSyncTaskMsg::Decode(const char *data, uint32_t length)
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

int FrontSyncTaskMsg::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_request_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    //coheader.cmd = CMD_INNER_GET_TASK;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("FrontSyncTaskMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_request_, coheader.print());
    front_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;

}

uint32_t FrontSyncTaskMsg::transid() const
{
    return front_request_.transid();
}

uint32_t FrontSyncTaskMsg::synctype() const
{
    return front_request_.synctype();
}

const uint64_t &FrontSyncTaskMsg::cli_sync_point() const
{
    return (uint64_t)(front_request_.syncpoint());
}

BackSyncTaskMsg::BackSyncTaskMsg()
    : CoMsg()
{
}

BackSyncTaskMsg::~BackSyncTaskMsg()
{
}

int BackSyncTaskMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("BackSyncTaskMsg get header failed.");
        return ret;
    }
    LOG(INFO)("BackSyncTaskMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_result_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackSyncTaskMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_result_, msg_header().print());
    return ret;    

}


int BackSyncTaskMsg::Encode(char * data, uint32_t & length) const
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

int BackSyncTaskMsg::task_size()  const
{
    return front_result_.taskinfos_size();
}

void BackSyncTaskMsg::set_transid(uint32_t transid)
{
    front_result_.set_transid(transid);
}

void BackSyncTaskMsg::set_synctype(uint32_t synctype)
{
    front_result_.set_synctype(synctype);
}

void BackSyncTaskMsg::set_continue_flag(uint32_t flag)
{
    front_result_.set_continueflag(flag);
}

void BackSyncTaskMsg::add_task(const TaskInfoItem *task)
{
    if (NULL == task) {
        LOG(WARN)("add task ,task is null");
        return ;
    }
    SrvTaskInfo *task_info = front_result_.add_taskinfos();
    task_info->set_taskid(task->taskid);
    task_info->set_tasktype(task->tasktype);
    task_info->set_taskdesc(task->taskdesc);
    task_info->set_taskname(task->taskname);
    task_info->set_tasklink(task->tasklink);
    task_info->set_taskprice(task->taskprice);
    task_info->set_taskptstep(task->taskptstep);
    task_info->set_taskpcstep(task->taskpcstep);
    task_info->set_taskpublisher(task->taskpublisher);
    task_info->set_tasksize(task->tasksize);
    task_info->set_taskstime(task->taskstime);
    task_info->set_tasketime(task->tasketime);
    task_info->set_taskstatus(task->taskstatus);
    task_info->set_taskicon(task->taskicon);
    task_info->set_taskpkgname(task->taskpkgname);
    task_info->set_tasktotalnum(task->tasknum);
    task_info->set_taskusednum(task->taskusednum);
}

void BackSyncTaskMsg::set_task(const TaskInfoItem *task)
{
    if (NULL == task) {
        LOG(WARN)("add task ,task is null");
        return ;
    }
    int task_size = front_result_.taskinfos_size();
    for (int i = 0; i < task_size; ++i)
    {
        com::adv::msg::TaskInfo *task_info = front_result_.mutable_taskinfos(i);
        if (task_info->taskid() == task->taskid)
        {
            task_info->set_taskid(task->taskid);
            task_info->set_tasktype(task->tasktype);
            task_info->set_taskdesc(task->taskdesc);
            task_info->set_taskname(task->taskname);
            task_info->set_tasklink(task->tasklink);
            task_info->set_taskprice(task->taskprice);
            task_info->set_taskptstep(task->taskptstep);
            task_info->set_taskpcstep(task->taskpcstep);
            task_info->set_taskpublisher(task->taskpublisher);
            task_info->set_tasksize(task->tasksize);
            task_info->set_taskstime(task->taskstime);
            task_info->set_tasketime(task->tasketime);
            task_info->set_taskstatus(task->taskstatus);
            task_info->set_taskicon(task->taskicon);
            task_info->set_taskpkgname(task->taskpkgname);
            task_info->set_tasktotalnum(task->tasknum);
            task_info->set_taskusednum(task->taskusednum);
            LOG(INFO)("set one task, task id:%lu", task->taskid);
            break;
        }
    }
}
const SrvTaskInfo &BackSyncTaskMsg::get(int index) const
{
    return front_result_.taskinfos(index);
}

// ======================================================
// 2001, 数据同步的ack，前端过来的
// ======================================================
FrontAckMsg::FrontAckMsg ()
    : CoMsg()
{

}

FrontAckMsg::~FrontAckMsg()
{

}

int FrontAckMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontAckMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontAckMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_result_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontAckMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_result_, msg_header().print());
    return ret;    
}


const SrvAckRequest &FrontAckMsg::pb_msg()
{
    return front_result_;
}


// ======================================================
// 3001, 上报数据协议，前端过来的
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

uint32_t BackReportMsg::get_reward() const
{
    return StringUtil::strtou32(front_result_.reward());
}


WebCommonRequest::WebCommonRequest()
{
    task_params_.clear();
    uid_list_.clear();
    empty_str_ = "";
}

WebCommonRequest::~WebCommonRequest()
{

}


int WebCommonRequest::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("WebCommonRequest get header failed.");
        return ret;
    }
    LOG(INFO)("WebCommonRequest, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    BinInputPacket<> inpkg(const_cast<char *>(pbody), len);

    uint32_t kv_num = 0;
    inpkg >> trans_id >> kv_num;
    LOG(DEBUG)("WebCommonRequest, transid:%u, param num:%u", trans_id, kv_num);

    if (kv_num > MAX_LIMIT_NUM) {
        LOG(ERROR)("WebCommonRequest get param num failed, num maybe error, num:%u", kv_num);
        return ret;
    }

    string key = "";
    string value = "";
    for (uint32_t i = 0; i < kv_num; ++i) {
        inpkg >> key >> value;
        if (!inpkg.good()) {
            LOG(ERROR)("WebCommonRequest get param failed, pkg is not good.");
            return ret;
        }
        LOG(DEBUG)("parse param, key:'%s', value:'%s'", key.c_str(), value.c_str());
        if (StringUtil::strcasecmp(key.c_str(), "uid_list") == 0)
        {
            StringUtil::split(value, uid_list_, ";");
            LOG(INFO)("split user uid list. uid num:%zu, uids:%s", uid_list_.size(), value.c_str());
            continue;
        }
        task_params_[key] = value; // 会覆盖
    }
    
    LOG(DEBUG)("WebCommonRequest, done decode.");
    return ret;    

}

/*
uint64_t WebCommonRequest::get_taskid() const
{
    uint64_t task_id = 0;
    map<string, string>::const_iterator taskid_str_iter = task_params_.find(TAG_TASK_ID);
    if (taskid_str_iter == task_params_.end()) {
        LOG(ERROR)("not find taskid field.");
        return task_id;
    }
    task_id = StringUtil::strtou64(taskid_str_iter->second);
    LOG(DEBUG)("publish request, get task id:%lu", task_id);
    return task_id;
}
*/

void WebCommonRequest::set_task_id(const uint64_t &task_id)
{
    if (task_params_.find(TAG_TASK_ID) == task_params_.end()){
        LOG(ERROR)("task param exist id field. not set task id.");
    }
    task_params_[TAG_TASK_ID] = StringUtil::u64tostr(task_id);
}

string WebCommonRequest::get_task_limit() const
{
    string task_reward_num_str("");
    StrMapCIter task_limit_str_iter = task_params_.find(TAG_REWARD_NUM);
    if (task_limit_str_iter == task_params_.end()) {
        LOG(ERROR)("not find task_reward_num field.");
        return task_reward_num_str;
    }
    task_reward_num_str = task_limit_str_iter->second;
    LOG(DEBUG)("publish request, get task_reward_num:%s", task_reward_num_str.c_str());
    return task_reward_num_str;
}

int WebCommonRequest::get_task_expire() const
{
    int expire_time = 0;
    StrMapCIter task_stime_str_iter = task_params_.find(TAG_STIME);
    if (task_stime_str_iter == task_params_.end()) {
        LOG(ERROR)("not find task_stime field.");
        return 0;
    }
    StrMapCIter task_etime_str_iter = task_params_.find(TAG_ETIME);
    if (task_etime_str_iter == task_params_.end()) {
        LOG(ERROR)("not find task_etime field.");
        return 0;
    }
    expire_time = StringUtil::strtoi32(task_etime_str_iter->second) - StringUtil::strtoi32(task_stime_str_iter->second);
    LOG(DEBUG)("publish request, get task expire:%d", expire_time);
    return expire_time;
}

const StrVec &WebCommonRequest::get_uid_list() const
{
    return uid_list_;
}

const string &WebCommonRequest::get_task_sn() const
{
    StrMapCIter task_sn_str_iter = task_params_.find(TASK_SN);
    if (task_sn_str_iter == task_params_.end()) {
        LOG(ERROR)("not find task_sn field.");
        return empty_str_;
    }
    return task_sn_str_iter->second;
}

const StrMap &WebCommonRequest::get_params() const 
{
    return task_params_;
}

uint32_t WebCommonRequest::transid() const
{
    return trans_id;
}

FrontPublishTaskResult::FrontPublishTaskResult ()
    : ret_value_(0)
    , ret_msg_("")
    , task_id_(0)
{

}

FrontPublishTaskResult::~FrontPublishTaskResult()
{

}

void FrontPublishTaskResult::set_err_no(uint32_t err_no)
{
    ret_value_ = err_no;
}

int FrontPublishTaskResult::get_err_no()  const
{
    return ret_value_;
}

void FrontPublishTaskResult::set_err_msg(const string &err_msg)
{
    ret_msg_ = err_msg;
}

void FrontPublishTaskResult::set_task_id(uint64_t task_id)
{
    task_id_ = task_id;
}

uint64_t FrontPublishTaskResult::get_task_id()  const
{
    return task_id_;
}

string FrontPublishTaskResult::get_err_msg()  const
{
    return ret_msg_;
}


int FrontPublishTaskResult::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    // make body
    outpkg << transid() << (uint32_t)get_err_no() << get_err_msg() << StringUtil::u64tostr(get_task_id());

    COHEADER coheader = msg_header();
    length = outpkg.length();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("FrontPublishTaskResult encode msg failed.");
        return -1;
    }

    LOG(INFO)("FrontPublishTaskResult encode, transid:%u, ret:%u, %s"
        , transid(), get_err_no(), coheader.print());
    return 0;
}

uint32_t FrontPublishTaskResult::transid() const
{
    return trans_id;
}

void FrontPublishTaskResult::set_transid(uint32_t trans_id)
{
    this->trans_id = trans_id;
}



FrontQueryTaskInfoResult::FrontQueryTaskInfoResult()
    : ret_value_(0)
    , ret_msg_("")
    , task_info_("")
    , task_status_(0)
    , pre_pay_("")
    , award_pay_("")
{
}

FrontQueryTaskInfoResult::~FrontQueryTaskInfoResult()
{
}
int FrontQueryTaskInfoResult::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    // make body
    outpkg << trans_id << (uint32_t)get_err_no() << get_err_msg();

    // encode task
    //if (0 == get_err_no())
    //{
        EncodeTaskInfo(outpkg);
    //}

    COHEADER coheader = msg_header();
    length = outpkg.length();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("FrontQueryTaskInfoResult encode msg failed.");
        return -1;
    }

    LOG(INFO)("FrontQueryTaskInfoResult encode, transid:%u, ret:%u, %s"
        , trans_id, get_err_no(), coheader.print());
    return 0;
}

// json format output
int FrontQueryTaskInfoResult::EncodeTaskInfo(BinOutputPacket<> &outpkg) const
{
    Json::Value task_root;
    Json::Reader task_reader;
    task_reader.parse(task_info_, task_root);

    task_root[TAG_TASK_PRE_CASH] = pre_pay_;
    task_root[TAG_TASK_AWARD_CASH] = award_pay_;
    task_root[TAG_TASK_STATUS] = task_status_;
    
    Json::StyledWriter writer;
    string task_info_str = writer.write(task_root);
    outpkg << task_info_str;

    LOG(DEBUG)("encode task info:%s", task_info_str.c_str());

    return 0;
}

void FrontQueryTaskInfoResult::set_err_no(uint32_t err_no)
{
    ret_value_ = err_no;
}

int FrontQueryTaskInfoResult::get_err_no() const
{
    return ret_value_;
}

string FrontQueryTaskInfoResult::get_err_msg() const
{
    return ret_msg_;
}

void FrontQueryTaskInfoResult::set_err_msg(const string &msg_str)
{
    ret_msg_ = msg_str;
}

void FrontQueryTaskInfoResult::SetTaskInfo(const string &task_info)
{
    task_info_ = task_info;
}


void FrontQueryTaskInfoResult::SetTaskStatus(uint32_t value)
{
    task_status_ = value;
}

void FrontQueryTaskInfoResult::SetPrePay(const string &pay)
{
    pre_pay_ = pay;
}

void FrontQueryTaskInfoResult::SetAwardPay(const string &pay)
{
    award_pay_ = pay;
}

QueryTaskInfoRequest::QueryTaskInfoRequest()
{

}

QueryTaskInfoRequest::~QueryTaskInfoRequest()
{
}

int QueryTaskInfoRequest::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    const com::adv::msg::InnerCommonMsg &front_result = backend_request_.pb_msg();
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

int QueryTaskInfoRequest::Decode(const char * data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("QueryTaskInfoRequest get header failed.");
        return ret;
    }
    LOG(INFO)("QueryTaskInfoRequest, Decode. [%s]", msg_header().print());

    com::adv::msg::InnerCommonMsg &request = backend_request_.mutable_pb_msg();

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

string QueryTaskInfoRequest::GetPrePay() const 
{
    const com::adv::msg::InnerCommonMsg &result = backend_request_.pb_msg();
    for (int i = 0; i < result.items_size(); ++i)
    {
        const com::adv::msg::PDataCell &cell = result.items(i);
        if (0 == StringUtil::is_equal(cell.tag(), TAG_TASK_PRE_CASH))
        {
            return cell.str_value();
        }
    }
    return "";
}

string QueryTaskInfoRequest::GetAwardPay() const 
{
    const com::adv::msg::InnerCommonMsg &result = backend_request_.pb_msg();
    for (int i = 0; i < result.items_size(); ++i)
    {
        const com::adv::msg::PDataCell &cell = result.items(i);
        if (0 == StringUtil::is_equal(cell.tag(), TAG_TASK_AWARD_CASH))
        {
            return cell.str_value();
        }
    }
    return "";
}

uint32_t QueryTaskInfoRequest::GetTaskStatus() const 
{
    const com::adv::msg::InnerCommonMsg &result = backend_request_.pb_msg();
    for (int i = 0; i < result.items_size(); ++i)
    {
        const com::adv::msg::PDataCell &cell = result.items(i);
        if (0 == StringUtil::is_equal(cell.tag(), TAG_TASK_STATUS))
        {
            return cell.i32_value();
        }
    }
    return 0;
}


FrontQueryTaskListResult::FrontQueryTaskListResult()
    : ret_code_(0)
    , ret_msg_("")
{
}

FrontQueryTaskListResult::~FrontQueryTaskListResult()
{
}

int FrontQueryTaskListResult::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    uint32_t task_num = task_list_.size();
    outpkg << trans_id << ret_code_ << ret_msg_ << task_num;

    for (int i = 0; i < task_num; ++i)
    {
        outpkg << task_list_[i];
    }

    if (!outpkg.good())
    {
        LOG(ERROR)("FrontQueryTaskListResult encode msg failed.");
        length = 0;
        return -1;
    }

    length = outpkg.length();
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("FrontQueryTaskListResult encode msg failed.");
        return -1;
    }

    LOG(INFO)("FrontQueryTaskListResult done encode, [%s], trans_id:%u, num:%u"
        , coheader.print(), trans_id, task_num);
    LOG(DEBUG)("FrontQueryTaskListResult encode request, [%s]", coheader.print());
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);

    return 0;

}

int FrontQueryTaskListResult::AddTask(const string &task_str)
{
    task_list_.push_back(task_str);
    return 0;
}

// ==================================================
// leveldb request
// ==================================================
// 30000
LdbBatchGetRequest::LdbBatchGetRequest()
    : biz_cmd_(0)
    , transfer_str_("")
    , trans_id_(0)
{
}

LdbBatchGetRequest::~LdbBatchGetRequest()
{
}


// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)num + [(string)key + ...]
int LdbBatchGetRequest::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    uint32_t num = key_vec_.size();
    outpkg << biz_cmd_ << transfer_str_ << trans_id_ << num;
    if (num > (1024 * 1024))
    {
        LOG(ERROR)("batch get request, num is too big, num:%u", num);
        return -1;
    }
    StrVCIter key_iter = key_vec_.begin();
    StrVCIter end_iter = key_vec_.end();
    for (; key_iter != end_iter; ++key_iter)
    {
        outpkg << *key_iter;
    }

    length = outpkg.length();
    COHEADER coheader = msg_header();
    coheader.len = length;
    coheader.cmd = LEVELDB_GET;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("LdbBatchGetRequest encode msg failed.");
        return -1;
    }

    LOG(INFO)("LdbBatchGetRequest done encode, [%s], biz_cmd:%u, transfer_str:%s, trans_id:%u, num:%u"
        , coheader.print(), biz_cmd_, transfer_str_.c_str(), trans_id_, num);
    LOG(DEBUG)("LdbBatchGetRequest encode request, [%s]", coheader.print());
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);
    return 0;
}

void LdbBatchGetRequest::add_key(const string &key)
{
    key_vec_.push_back(key);
}

LdbBatchGetResult::LdbBatchGetResult()
{
}

LdbBatchGetResult::~LdbBatchGetResult()
{
}

	
int LdbBatchGetResult::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("LdbBatchGetResult get header failed.");
        return ret;
    }
    LOG(INFO)("LdbBatchGetResult, Decode. [%s]", msg_header().print());

    BinInputPacket<> inpkg(const_cast<char *>(data), length);
    int head_len = sizeof(COHEADER);
    inpkg.offset_head(head_len);
    uint32_t num = 0;
    inpkg >> biz_cmd_ >> transfer_str_ >> trans_id_ >> ret_value_ >> num;
    ONCE_LOOP_ENTER
    if (num > (1024*1024))
    {
        LOG(ERROR)("inpkg from leveldb maybe error. [%s], num>%u", msg_header().print(), num);
        ret = -1;
        break;
    }
    if (!inpkg.good())
    {
        LOG(ERROR)("LdbBatchGetResult, inpkg is error. [%s]", msg_header().print());
        ret = -1;
        return ret;
    }
    kv_data_vec_.reserve(num);
    StrKV kv_data;
    for (uint32_t i = 0; i < num; ++i)
    {
        kv_data.clear();
        kv_data_vec_.push_back(kv_data);
        inpkg >> kv_data_vec_[i].key >> kv_data_vec_[i].value; 
        LOG(DEBUG)("decode key and value, index:%u, key:%s", i, kv_data_vec_[i].key.c_str());
    }
    LOG(INFO)("LdbBatchGetResult done decode, biz_cmd:%u, transfer_str:%s, trans_id:%u, ret:%u, num:%u"
        , biz_cmd_, transfer_str_.c_str(), trans_id_, ret_value_, num);
    if (!inpkg.good())
    {
        LOG(ERROR)("LdbBatchGetResult, after decode, inpkg is error. [%s]", msg_header().print());
        ret = -1;
        return ret;
    }
    ONCE_LOOP_LEAVE
    return ret;
}

uint32_t LdbBatchGetResult::size()
{
    return kv_data_vec_.size();
}

StrKVVec &LdbBatchGetResult::kv_data()
{
    return kv_data_vec_;
}


// 30001

LdbBatchPutRequest::LdbBatchPutRequest()
    : biz_cmd_(0)
    , transfer_str_("")
    , trans_id_(0)
{
}
LdbBatchPutRequest::~LdbBatchPutRequest()
{
}
	
// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)num + [(string)key + (string)value ...]
int LdbBatchPutRequest::Encode(char *data, uint32_t &length) const
{
    int ret = 0;
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    outpkg << biz_cmd_ << transfer_str_ << trans_id_;
    uint32_t num = kv_data_vec_.size();
    outpkg << num;
    StrKVVCIter key_iter = kv_data_vec_.begin();
    StrKVVCIter end_iter = kv_data_vec_.end();
    for (; key_iter != end_iter; ++key_iter)
    {
        LOG(DEBUG)("LdbBatchPutRequest, put msg key:%s, msg len:%zu", key_iter->key.c_str(), key_iter->value.size());
        if (key_iter->value.size() > 10*_1MB)
        {
            LOG(ERROR)("value length is too long. len:%u, skip key:%s", key_iter->value.size(), key_iter->key.c_str());
            continue;
        }
        outpkg << key_iter->key << key_iter->value;
    }
    
    length = outpkg.length();
    COHEADER coheader = msg_header();
    coheader.len = length;
    coheader.cmd = LEVELDB_PUT;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("LdbBatchPutRequest encode msg failed.");
        return -1;
    }

    LOG(INFO)("LdbBatchPutRequest done encode, biz_cmd:%u, transfer_str:%s, trans_id:%u, num:%u"
        , biz_cmd_, transfer_str_.c_str(), trans_id_, num);
    return ret;
}

int LdbBatchPutRequest::AddKV(const string &key, const string &value)
{
    StrKV kv(key, value);
    kv_data_vec_.push_back(kv);
    return 0;
}

LdbBatchPutResult::LdbBatchPutResult()
    : biz_cmd_(0)
    , transfer_str_("")
    , trans_id_(0)
    , ret_value_(0)
{
}

LdbBatchPutResult::~LdbBatchPutResult()
{
}
	
int LdbBatchPutResult::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("LdbBatchPutResult get header failed.");
        return ret;
    }
    LOG(INFO)("LdbBatchPutResult, Decode. [%s]", msg_header().print());

    BinInputPacket<> inpkg(const_cast<char *>(data), length);
    int head_len = sizeof(COHEADER);
    inpkg.offset_head(head_len);

    inpkg >> biz_cmd_ >> transfer_str_ >> trans_id_ >> ret_value_;
    LOG(INFO)("LdbBatchPutResult done decode, biz_cmd:%u, transfer_str:%s, trans_id:%u, ret:%u"
        , biz_cmd_, transfer_str_.c_str(), trans_id_, ret_value_);
    
    if (!inpkg.good())
    {
        LOG(ERROR)("LdbBatchPutResult, inpkg is error. [%s]", msg_header().print());
        ret = -1;
        return ret;
    }
    return ret;
}


// 30002

LdbBatchDelRequest::LdbBatchDelRequest()
    : biz_cmd_(0)
    , transfer_str_("")
    , trans_id_(0)
{
}

LdbBatchDelRequest::~LdbBatchDelRequest()
{
}
	
int LdbBatchDelRequest::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    uint32_t num = key_vec_.size();
    outpkg << biz_cmd_ << transfer_str_ << trans_id_ << num;
    if (num > (1024 * 1024))
    {
        LOG(ERROR)("batch del request, num is too big, num:%u", num);
        return -1;
    }
    StrVCIter key_iter = key_vec_.begin();
    StrVCIter end_iter = key_vec_.end();
    for (; key_iter != end_iter; ++key_iter)
    {
        outpkg << *key_iter;
    }

    length = outpkg.length();
    COHEADER coheader = msg_header();
    coheader.len = length;
    coheader.cmd = LEVELDB_RANGE_DEL;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("LdbBatchDelRequest encode msg failed.");
        return -1;
    }

    LOG(INFO)("LdbBatchDelRequest done encode, [%s], biz_cmd:%u, transfer_str:%s, trans_id:%u, num:%u"
        , coheader.print(), biz_cmd_, transfer_str_.c_str(), trans_id_, num);
    LOG(DEBUG)("LdbBatchDelRequest encode request, [%s]", coheader.print());
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);
    return 0;
}



LdbBatchDelResult::LdbBatchDelResult()
    : biz_cmd_(0)
    , transfer_str_("")
    , trans_id_(0)
    , ret_value_(0)
{
}

LdbBatchDelResult::~LdbBatchDelResult()
{
}
	
int LdbBatchDelResult::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("LdbBatchDelResult get header failed.");
        return ret;
    }
    LOG(INFO)("LdbBatchDelResult, Decode. [%s]", msg_header().print());

    BinInputPacket<> inpkg(const_cast<char *>(data), length);
    int head_len = sizeof(COHEADER);
    inpkg.offset_head(head_len);

    inpkg >> biz_cmd_ >> transfer_str_ >> trans_id_ >> ret_value_;
    LOG(INFO)("LdbBatchDelResult done decode, biz_cmd:%u, transfer_str:%s, trans_id:%u, ret:%u"
        , biz_cmd_, transfer_str_.c_str(), trans_id_, ret_value_);
    
    if (!inpkg.good())
    {
        LOG(ERROR)("LdbBatchDelResult, inpkg is error. [%s]", msg_header().print());
        ret = -1;
        return ret;
    }
    return ret;
}


// 30003

LdbRangeGetRequest::LdbRangeGetRequest()
    : biz_cmd_(0)
    , transfer_str_("")
    , trans_id_(0)
    , key_start_("")
    , key_end_("")
{
}

LdbRangeGetRequest::~LdbRangeGetRequest()
{
}
	
// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (string)key_start_perfix + (string)key_end_perfix + (uint32_t)limit_num
int LdbRangeGetRequest::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    outpkg << biz_cmd_ << transfer_str_ << trans_id_;
    outpkg << key_start_ << key_end_ << MSG_LIMIT;

    length = outpkg.length();
    COHEADER coheader = msg_header();
    coheader.cmd = LEVELDB_RANGE_GET;
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("LdbRangeGetRequest encode msg failed.");
        return -1;
    }

    LOG(INFO)("LdbRangeGetRequest done encode, biz_cmd:%u, transfer_str:%s"\
        ", trans_id:%u, key_start:%s, key_end:%s, limit num:%u"
        , biz_cmd_, transfer_str_.c_str(), trans_id_, key_start_.c_str(), key_end_.c_str(), MSG_LIMIT);
    LOG(DEBUG)("LdbRangeGetRequest encode request, [%s]", coheader.print());
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);
    return 0;
}   

void LdbRangeGetRequest::set_biz_cmd(uint32_t biz_cmd)
{
    biz_cmd_ = biz_cmd;
}

void LdbRangeGetRequest::set_transfer(const string &transfer)
{
    transfer_str_ = transfer;
}

void LdbRangeGetRequest::set_trans_id(uint32_t trans_id)
{
    trans_id_ = trans_id;
}

void LdbRangeGetRequest::set_key_start(const string &key_start)
{
    key_start_ = key_start;
}

void LdbRangeGetRequest::set_key_end(const string &key_end)
{
    key_end_ = key_end;
}



LdbRangeGetResult::LdbRangeGetResult()
    : biz_cmd_(0)
	, transfer_str_("")
	, trans_id_(0)
	, ret_value_(0)
	, end_flag_(0)
{
}

LdbRangeGetResult::~LdbRangeGetResult()
{
}


// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)ret + (uint32_t)num + [(string)key + (string)value + ...] + (uint32_t)end_flag
int LdbRangeGetResult::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("LdbRangeGetResult get header failed.");
        return ret;
    }
    LOG(INFO)("LdbRangeGetResult, Decode. [%s]", msg_header().print());

    BinInputPacket<> inpkg(const_cast<char *>(data), length);
    int head_len = sizeof(COHEADER);
    inpkg.offset_head(head_len);
    uint32_t num = 0;
    inpkg >> biz_cmd_ >> transfer_str_ >> trans_id_ >> ret_value_ >> num;
    ONCE_LOOP_ENTER
    if (num > (1024*1024))
    {
        LOG(ERROR)("inpkg from leveldb maybe error. [%s], num>%u", msg_header().print(), num);
        ret = -1;
        break;
    }
    if (!inpkg.good())
    {
        LOG(ERROR)("LdbRangeGetResult, inpkg is error. [%s]", msg_header().print());
        ret = -1;
        return ret;
    }
    kv_data_vec_.reserve(num);
    StrKV kv_data;
    for (uint32_t i = 0; i < num; ++i)
    {
        kv_data.clear();
        kv_data_vec_.push_back(kv_data);
        inpkg >> kv_data_vec_[i].key >> kv_data_vec_[i].value; 
        LOG(DEBUG)("decode key and value, index:%u, key:%s", i, kv_data_vec_[i].key.c_str());
    }
    inpkg >> end_flag_;
    LOG(INFO)("LdbRangeGetResult done decode, biz_cmd:%u, transfer_str:%s, trans_id:%u, ret:%u, num:%u, end_flag:%u"
        , biz_cmd_, transfer_str_.c_str(), trans_id_, ret_value_, num, end_flag_);
    if (!inpkg.good())
    {
        LOG(ERROR)("LdbRangeGetResult, after decode, inpkg is error. [%s]", msg_header().print());
        ret = -1;
        return ret;
    }
    ONCE_LOOP_LEAVE
    return ret;
}



// 30004
LdbRangeDelRequest::LdbRangeDelRequest()
    : biz_cmd_(0)
    , transfer_str_("")
    , trans_id_(0)
    , key_start_("")
    , key_end_("")
{
}

LdbRangeDelRequest::~LdbRangeDelRequest()
{
}
	
// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (string)key_start_perfix + (string)key_end_perfix
int LdbRangeDelRequest::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    outpkg << biz_cmd_ << transfer_str_ << trans_id_;
    outpkg << key_start_ << key_end_ << MSG_LIMIT;

    length = outpkg.length();
    COHEADER coheader = msg_header();
    coheader.len = length;
    coheader.cmd = LEVELDB_RANGE_DEL;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("LdbRangeDelRequest encode msg failed.");
        return -1;
    }

    LOG(INFO)("LdbRangeDelRequest done encode, biz_cmd:%u, transfer_str:%s"\
        ", trans_id:%u, key_start:%s, key_end:%s, limit num:%u"
        , biz_cmd_, transfer_str_.c_str(), trans_id_, key_start_.c_str(), key_end_.c_str(), MSG_LIMIT);
    LOG(DEBUG)("LdbRangeDelRequest encode request, [%s]", coheader.print());
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);
    return 0;
}




LdbRangeDelResult::LdbRangeDelResult()
    : biz_cmd_(0)
    , transfer_str_("")
    , trans_id_(0)
    , ret_value_(0)
{
}

LdbRangeDelResult::~LdbRangeDelResult()
{
}
	
int LdbRangeDelResult::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("LdbRangeDelResult get header failed.");
        return ret;
    }
    LOG(INFO)("LdbRangeDelResult, Decode. [%s]", msg_header().print());

    BinInputPacket<> inpkg(const_cast<char *>(data), length);
    int head_len = sizeof(COHEADER);
    inpkg.offset_head(head_len);

    inpkg >> biz_cmd_ >> transfer_str_ >> trans_id_ >> ret_value_;
    LOG(INFO)("LdbRangeDelResult done decode, biz_cmd:%u, transfer_str:%s, trans_id:%u, ret:%u"
        , biz_cmd_, transfer_str_.c_str(), trans_id_, ret_value_);
    
    if (!inpkg.good())
    {
        LOG(ERROR)("LdbRangeDelResult, inpkg is error. [%s]", msg_header().print());
        ret = -1;
        return ret;
    }
    return ret;
}



// 
InsertTaskRequest::InsertTaskRequest()
{

}

InsertTaskRequest::~InsertTaskRequest()
{

}

int InsertTaskRequest::Encode(char * data, uint32_t & length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = backend_request_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("InsertTaskRequest encode msg failed.");
        return -1;
    }

    PrintPbData(backend_request_, coheader.print());
    backend_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}

int InsertTaskRequest::add(const SrvTaskInfo &task_info)
{
    SrvTaskInfo* taskinfo = backend_request_.add_taskinfo();
    taskinfo->CopyFrom(task_info);
    return 0;
}

void InsertTaskRequest::set_transid(uint32_t transid)
{
    backend_request_.set_transid(transid);
}

InsertTaskResult::InsertTaskResult()
{

}

InsertTaskResult::~InsertTaskResult()
{

}

int InsertTaskResult::Decode(const char * data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("InsertTaskResult get header failed.");
        return ret;
    }
    LOG(INFO)("InsertTaskResult, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!backend_result_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("InsertTaskResult ParsePartialFromArray failed.");
    }
    PrintPbData(backend_result_, msg_header().print());
    return ret;    
}


StartTaskRequest::StartTaskRequest()
{

}

StartTaskRequest::~StartTaskRequest()
{

}

int StartTaskRequest::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("StartTaskRequest get header failed.");
        return ret;
    }
    LOG(INFO)("StartTaskRequest, Decode. [%s]", msg_header().print());
    com::adv::msg::InnerCommonMsg &request = front_request_.mutable_pb_msg();

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!request.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("StartTaskRequest ParsePartialFromArray failed.");
    }
    PrintPbData(request, msg_header().print());
    return ret;    
}

int StartTaskRequest::Encode(char * data, uint32_t & length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    const com::adv::msg::InnerCommonMsg &request = front_request_.pb_msg();
    int body_len = request.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("StartTaskRequest encode msg failed.");
        return -1;
    }

    PrintPbData(request, coheader.print());
    request.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}

int StartTaskRequest::RetCode() const
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

string StartTaskRequest::RetMsg() const
{
    const com::adv::msg::InnerCommonMsg &result = front_request_.pb_msg();
    for (int i = 0; i < result.items_size(); ++i)
    {
        const com::adv::msg::PDataCell &cell = result.items(i);
        if (0 == StringUtil::is_equal(cell.tag(), TAG_RET_MSG))
        {
            return cell.str_value();
        }
    }
    return "unknown error";
}
uint64_t StartTaskRequest::GetTaskId() const
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

uint32_t StartTaskRequest::GetTaskType() const
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

uint32_t StartTaskRequest::GetTransId() const
{
    return front_request_.GetTransid();
}


void StartTaskRequest::SetTaskType(uint32_t value)
{
    com::adv::msg::InnerCommonMsg &result = front_request_.mutable_pb_msg();
    front_request_.AddTagValue(TAG_TYPE, value);
}

void StartTaskRequest::SetTaskId(const uint64_t &value)
{
    com::adv::msg::InnerCommonMsg &result = front_request_.mutable_pb_msg();
    front_request_.AddTagValue(TAG_TASK_ID, value);
}

void StartTaskRequest::SetTransId(uint32_t value)
{
    com::adv::msg::InnerCommonMsg &result = front_request_.mutable_pb_msg();
    result.set_transid(value);
}



/*
StartTaskResult::StartTaskResult()
    :CoMsg()
{
}

StartTaskResult::~StartTaskResult()
{
}

int StartTaskResult::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.pb_msg().ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("StartTaskResult encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_.pb_msg(), coheader.print());
    front_result_.pb_msg().SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}
*/

