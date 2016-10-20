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
#include "request.h"
#include <time.h>
#include "server_app.h"

using namespace utils;

FrontRequest::FrontRequest(BinInputPacket<> &inpkg)
    : inpkg_(inpkg)
    , trans_id_(0)
    , time_(0)
{
}
FrontRequest::~FrontRequest()
{
}

int FrontRequest::decode()
{
	inpkg_.offset_head(sizeof(HEADER));
	inpkg_.get_head(header_);
	LOG(INFO)("client request head [%s]", header_.print());
	return 0;
}

HEADER FrontRequest::get_head()
{
	return header_;
}

FrontResult::FrontResult()
	: data_(NULL)
	, data_len_(0)
	, ret_code_(0)
    , trans_id_(0)
    , time_(time(NULL))
	, ret_msg_("")
{
	data_ = ServerApp::Instance()->get_buffer();
}

FrontResult::~FrontResult()
{
}

char *FrontResult::data()
{
	return data_;
}

uint32_t FrontResult::byte_size()
{
	return data_len_;
}


int FrontResult::encode()
{
	LOG(DEBUG)("base result encode head.");
	return 0;
}

void FrontResult::set_head(const HEADER &head)
{
	header_ = head;
}

const HEADER &FrontResult::get_head()
{
	return header_;
}

void FrontResult::set_len(uint32_t pkglen)
{
	header_.len = pkglen;
	LOG(DEBUG)("set head [%s]", header_.print());
	HEADER head = header_;
	head.len = htonl(header_.len);
	head.cmd = htonl(header_.cmd);
	head.seq = htonl(header_.seq);
	head.head_len = htonl(header_.head_len);
	head.uid = htonl(header_.uid);
	if (NULL == data_) {
		LOG(ERROR)("result set head len failed, data is null.");
		return ;
	}
	memcpy(data_, (void *)&head, sizeof(HEADER));
}

int FrontResult::encode_body(const char *data, uint32_t data_len)
{
	int ret = 0;
	if (NULL == data_) {
		LOG(ERROR)("encode result body failed, result data is null.");
		ret = ERR_SERVER_ENCODE_ERROR;
	} else if (data_len > MOBILED_CLIENT_PKG_MAX_LEN){
		LOG(ERROR)("encode result body failed, result data is bigger than %u.", MOBILED_CLIENT_PKG_MAX_LEN);
		ret = ERR_SERVER_ENCODE_ERROR;
	} else if (0 == header_.head_len){
		LOG(ERROR)("encode result body failed, head len is 0.");
		ret = ERR_SERVER_ENCODE_ERROR;
	} else {
		memcpy(data_ + header_.head_len, data, data_len);
		data_len_ = data_len + header_.head_len;
		LOG(DEBUG)("encode result body ok, encode data len:%u", data_len);
	}
	return ret;
}

int FrontResult::encode_head()
{
	int ret = 0;
	if (NULL == data_) {
		LOG(ERROR)("encode result head failed, data is null.");
		ret = ERR_SERVER_ENCODE_ERROR;
		return ret;
	}
	header_.len = data_len_;
	HEADER head = header_;
	head.len = htonl(header_.len);
	head.cmd = htonl(header_.cmd);
	head.seq = htonl(header_.seq);
	head.head_len = htonl(header_.head_len);
	head.uid = htonl(header_.uid);
	memcpy(data_, (void *)&head, sizeof(HEADER));
	LOG(DEBUG)("encode result head ok, head:%s, encode data len:%u", 
	header_.print(), (uint32_t)sizeof(HEADER));

	return ret;
}

FrontLoginRequest::FrontLoginRequest(BinInputPacket<> &inpkg)
	: FrontRequest(inpkg)
	, devid_("")
	, devtype_(0)
	, passwd_("")
	, version_(0)
{
}

FrontLoginRequest::~FrontLoginRequest()
{
}

int FrontLoginRequest::decode()
{
	int ret = 0;
	FrontRequest::decode();

	Json::Value root;
	string input_str(inpkg_.getCur(), inpkg_.remainLength());
	LOG(DEBUG)("login request:%s", input_str.c_str());
	reader_.parse(input_str, root);

	if (root[REQUEST_NODE].isObject()) {
		root = root[REQUEST_NODE];
	} else {
		LOG(ERROR)("param:%s type is error", REQUEST_NODE);
		ret = -1;
	}
	if (root[TRANS_ID].isUInt()) {
		trans_id_ = root[TRANS_ID].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TRANS_ID);
	}
	if (root[TIME_NOW].isUInt()) {
		time_= root[TIME_NOW].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TIME_NOW);
		ret = -1;
	}
	if (root[DEVID].isString()) {
		devid_ = root[DEVID].asString();
	} else {
		LOG(ERROR)("param:%s type is error", DEVID);
		ret = -1;
	}
	if (root[DEVTYPE].isUInt()) {
		devtype_= root[DEVTYPE].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", DEVTYPE);
		ret = -1;
	}
	if (root[PASSWD].isString()) {
		passwd_ = root[PASSWD].asString();
	} else {
		LOG(ERROR)("param:%s type is error", PASSWD);
		ret = -1;
	}
	if (root[VERSION].isUInt()) {
		version_= root[VERSION].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", VERSION);
		ret = -1;
	}
	return ret;
}

FrontLoginResult::FrontLoginResult()
	: key_("")
	, uid_(0)
{
}

FrontLoginResult::~FrontLoginResult()
{
}

int FrontLoginResult::encode()
{
	int ret = 0;
	
	string outpkg("");
	Json::Value root;
	Json::Value ret_item;
	Json::Value login_item;

	// make json data
	ret_item[RET_CODE] = ret_code_;
	ret_item[RET_MSG] = ret_msg_;
	root[BASE_RET] = ret_item;
	login_item[KEY] = key_;
	login_item[UID] = uid_;
	login_item[TIME_NOW] = time_;
	login_item[TRANS_ID] = trans_id_;
	root[LOGIN_NODE] = login_item;

	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode login result, ret:%d", ret);
	return ret;
}


FrontBeatRequest::FrontBeatRequest(BinInputPacket<> &inpkg)
	: FrontRequest(inpkg)
{
}

FrontBeatRequest::~FrontBeatRequest()
{
}

int FrontBeatRequest::decode()
{
	int ret = 0;
	FrontRequest::decode();
	Json::Value root;
	string input_str(inpkg_.getCur(), inpkg_.remainLength());
	LOG(DEBUG)("user beat request:%s", input_str.c_str());
	reader_.parse(input_str, root);

	if (root[REQUEST_NODE].isObject()) {
		root = root[REQUEST_NODE];
	} else {
		LOG(ERROR)("param:%s type is error", REQUEST_NODE);
		ret = -1;
	}
	if (root[TIME_NOW].isUInt()) {
		time_ = root[TIME_NOW].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TIME_NOW);
		ret = -1;
	}
	if (root[TRANS_ID].isUInt()) {
		trans_id_ = root[TRANS_ID].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TRANS_ID);
		ret = -1;
	}

	LOG(DEBUG)("done user beat request decode, ret:%d", ret);
	return ret;
}
	

FrontBeatResult::FrontBeatResult()
{
}

FrontBeatResult::~FrontBeatResult()
{
}

int FrontBeatResult::encode()
{
	int ret = 0;
	
	string outpkg("");
	Json::Value root;
	Json::Value item;

	// make json data
	item[RET_CODE] = ret_code_;
	item[RET_MSG] = ret_msg_;
	root[BASE_RET] = item;
	root[TIME_NOW] = time_;
    root[TRANS_ID] = trans_id_;

	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode beat result, ret:%d", ret);
	return ret;

}



FrontSyncRequest::FrontSyncRequest(BinInputPacket<> &inpkg)
	: FrontRequest(inpkg)
{
}

FrontSyncRequest::~FrontSyncRequest()
{
}

int FrontSyncRequest::decode()
{
	int ret = 0;
	FrontRequest::decode();
	Json::Value root;
	Json::Value sync_key_item;
	string input_str(inpkg_.getCur(), inpkg_.remainLength());
	LOG(DEBUG)("sync request:%s", input_str.c_str());
	reader_.parse(input_str, root);

	if (root[REQUEST_NODE].isObject()) {
		sync_key_item = root[REQUEST_NODE];
	} else {
		LOG(ERROR)("param:%s type is error", REQUEST_NODE);
		ret = -1;
	}
	if (sync_key_item[TIME_NOW].isUInt()) {
		time_ = sync_key_item[TIME_NOW].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TIME_NOW);
		ret = -1;
	}
	if (sync_key_item[TRANS_ID].isUInt()) {
		trans_id_ = sync_key_item[TRANS_ID].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TRANS_ID);
		ret = -1;
	}

	uint32_t item_num = 0;
	if (sync_key_item[ITEM_SIZE].isUInt()) {
		item_num= sync_key_item[ITEM_SIZE].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", ITEM_SIZE);
		ret = -1;
	}
	if (sync_key_item[ITEMS].isArray()) {
		sync_key_item = sync_key_item[ITEMS];
	} else {
		LOG(ERROR)("param:%s type is error", ITEMS);
		ret = -1;
	}

	if (item_num != sync_key_item.size()) {
		LOG(ERROR)("sync item size:%u is not match with count:%u", 
		sync_key_item.size(), item_num);
		ret = -1;
		return ret;
	}

	uint32_t key = 0;
	uint64_t value = 0;
	for (uint32_t index = 0; index < item_num; ++index) {
		Json::Value &item = sync_key_item[index];
		if (item[ITEM_KEY].isUInt()) {
			key= item[ITEM_KEY].asUInt();
		} else {
			LOG(ERROR)("param:%s type is error, skip", ITEM_KEY);
			ret = -1;
			continue;
		}
		if (item[ITEM_VALUE].isUInt64()) {
			value = item[ITEM_VALUE].asUInt64();
		} else {
			LOG(ERROR)("param:%s type is error, skip", ITEM_VALUE);
			ret = -1;
			continue;
		}

		sync_items_[key] = value;
        LOG(DEBUG)("parse sync, k:%u, v:%lu", key, value);
	}

	LOG(INFO)("done decode sync request. ret:%d", ret);
	return ret;
}


FrontSyncResult::FrontSyncResult()
    : ret_value_(0)
    , err_msg_("")
    , bus_type_(0)
    , continue_flag_(0)
    , max_taskid_(0)
{
}

FrontSyncResult::~FrontSyncResult()
{
}

int FrontSyncResult::encode()
{
	int ret = 0;
	
	string outpkg("");
	Json::Value root;
	Json::Value item;

    uint32_t task_size = task_infos_.size();
    
	// make json data
	item[RET_CODE] = ret_code_;
	item[RET_MSG] = err_msg_;
	root[BASE_RET] = item;
	root[BUSSINESS_TYPE] = bus_type_;
	root[ITEM_SIZE] = task_size;
	root[CONTINUE_FLAG] = continue_flag_;
	root[TIME_NOW] = time_;
	root[TRANS_ID] = trans_id_;

    for (uint32_t i = 0; i < task_size; ++i)
    {
        item.clear();
        CustomTaskInfo *task_info = task_infos_[i];
        if (max_taskid_ < task_info->taskid) {
            max_taskid_ = task_info->taskid;
        }
    	ret = encodeTask(item, task_info);
        if (0 != ret)
        {
            continue;
        }
    	root[ITEMS].append(item);
    }

    if (task_size == 0) {
        item.clear();
    	root[ITEMS].append(item);
    }
	
	root[SYNC_VALUE] = (Json::Value::UInt64)max_taskid_;
	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode sync result, ret:%d, json result:%s", ret, outpkg.c_str());
	return ret;

}

void FrontSyncResult::set_ret_value(int ret)
{
    ret_value_ = ret;
}

void FrontSyncResult::set_err_msg(const string &err_msg)
{
    err_msg_ = err_msg;
}


void FrontSyncResult::set_bus_type(uint32_t type)
{
    bus_type_ = type;
}

void FrontSyncResult::set_continue_flag(uint32_t flag)
{
    continue_flag_ = flag;
}

void FrontSyncResult::set_sync_value(uint32_t maxtaskid)
{
    max_taskid_ = maxtaskid;
}

void FrontSyncResult::set_task_infos(const TaskInfoVec &task_infos)
{
    int task_size = task_infos.size();
    task_infos_.reserve(task_size);
    task_infos_ = task_infos;
}

// flag.1表示新任务，2表示历史任务
int FrontSyncResult::encodeTask(Json::Value &item, CustomTaskInfo *task)
{
    if (NULL == task)
    {
        LOG(ERROR)("encode task failed, task is null.");
        return -1;
    }
	item[TASK_ID] = (Json::Value::UInt64)task->taskid;
	item[TASK_TYPE] = task->tasktype;
	item[TASK_NAME] = task->taskname;
	item[TASK_LINK] = task->tasklink;
	item[TASK_DESC] = task->taskdesc;
	item[TASK_PKG_NAME] = task->taskpkgname;
	item[TASK_SIZE] = task->tasksize;
	item[TASK_PAY] = task->taskprice;
	item[TASK_TOTAL_STEP] = task->taskptstep;
	item[TASK_CURR_STEP] = task->taskpcstep;
	item[TASK_STATUS] = task->taskstatus;
	item[TASK_PUBLR] = task->taskpublisher;
	item[TASK_ICON_LINK] = task->taskicon;
	item[TASK_TOTAL_NUM] = task->tasktotalnum;
	item[TASK_USED_NUM] = task->taskusednum;
	item[TASK_STIME] = (Json::Value::UInt64)task->taskstime;
	item[TASK_ETIME] = (Json::Value::UInt64)task->tasketime;
	return 0;
}

//int FrontSyncResult::encodeIncome(Json::Value &item)
//{
//	int ret = 0;
//	item[INCOME] = "120";
//	return ret;
//}

FrontSyncAckRequest::FrontSyncAckRequest(BinInputPacket<> &inpkg)
	: FrontRequest(inpkg)
{

}


FrontSyncAckRequest::~FrontSyncAckRequest()
{

}


int FrontSyncAckRequest::decode()
{
	int ret = 0;
	FrontRequest::decode();
	Json::Value root;
	Json::Value ack_item;
	string input_str(inpkg_.getCur(), inpkg_.remainLength());
	LOG(DEBUG)("ack request:%s", input_str.c_str());
	reader_.parse(input_str, root);

	if (root[REQUEST_NODE].isObject()) {
		ack_item = root[REQUEST_NODE];
	} else {
		LOG(ERROR)("param:%s type is error", REQUEST_NODE);
		ret = -1;
	}
	if (ack_item[TIME_NOW].isUInt()) {
		time_ = ack_item[TIME_NOW].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TIME_NOW);
		ret = -1;
	}
	if (ack_item[TRANS_ID].isUInt()) {
		trans_id_ = ack_item[TRANS_ID].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TRANS_ID);
		ret = -1;
	}

	uint32_t item_num = 0;
	if (ack_item[ITEM_SIZE].isUInt()) {
		item_num = ack_item[ITEM_SIZE].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", ITEM_SIZE);
		ret = -1;
	}
	if (ack_item[ITEMS].isArray()) {
		ack_item = ack_item[ITEMS];
	} else {
		LOG(ERROR)("param:%s type is error", ITEMS);
		ret = -1;
	}

	if (item_num != ack_item.size()) {
		LOG(ERROR)("ack item size:%u is not match with count:%u", 
		ack_item.size(), item_num);
		ret = -1;
		return ret;
	}

	uint32_t key = 0;
	uint64_t value = 0;
	for (uint32_t index = 0; index < item_num; ++index) {
		Json::Value &item = ack_item[index];
		if (item[ITEM_KEY].isUInt()) {
			key= item[ITEM_KEY].asUInt();
		} else {
			LOG(ERROR)("param:%s type is error, skip", ITEM_KEY);
			ret = -1;
			continue;
		}
		if (item[ITEM_VALUE].isUInt64()) {
			value = item[ITEM_VALUE].asUInt64();
		} else {
			LOG(ERROR)("param:%s type is error, skip", ITEM_VALUE);
			ret = -1;
			continue;
		}

		ack_items_.push_back(value);
        LOG(DEBUG)("parse ack, k:%u, v:%lu", key, value);
	}

	LOG(INFO)("done decode ack request. ret:%d", ret);
	return ret;
}


FrontReportRequest::FrontReportRequest(BinInputPacket<> &inpkg)
	: FrontRequest(inpkg)
	, task_id_(0)
	, task_type_(0)
	, task_curr_step_(0)
	, task_desc_("")
{
}

FrontReportRequest::~FrontReportRequest()
{
}

int FrontReportRequest::decode()
{
	int ret = 0;
	FrontRequest::decode();
	Json::Value root;
	Json::Value report_key_item;
	string input_str(inpkg_.getCur(), inpkg_.remainLength());
	LOG(DEBUG)("report data request:%s", input_str.c_str());
	reader_.parse(input_str, root);

	if (root[REQUEST_NODE].isObject()) {
		report_key_item = root[REQUEST_NODE];
	} else {
		LOG(ERROR)("param:%s type is error", REQUEST_NODE);
		ret = -1;
	}
	if (report_key_item[TRANS_ID].isUInt()) {
		trans_id_ = report_key_item[TRANS_ID].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TRANS_ID);
		ret = -1;
	}
	if (report_key_item[TIME_NOW].isUInt()) {
		time_ = report_key_item[TIME_NOW].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TIME_NOW);
		ret = -1;
	}
	if (report_key_item[TASK_ID].isUInt64()) {
		task_id_ = report_key_item[TASK_ID].asUInt64();
	} else {
		LOG(ERROR)("param:%s type is error", TASK_ID);
		ret = -1;
	}
	if (report_key_item[TASK_TYPE].isUInt()) {
		task_type_ = report_key_item[TASK_TYPE].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TASK_TYPE);
		ret = -1;
	}
	if (report_key_item[TASK_DONE_STEP].isUInt()) {
		task_curr_step_ = report_key_item[TASK_DONE_STEP].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TASK_DONE_STEP);
		ret = -1;
	}
	if (report_key_item[TASK_DESC].isString()) {
		task_desc_ = report_key_item[TASK_DESC].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TASK_DESC);
		ret = -1;
	}

	LOG(INFO)("done decode report request. ret:%d", ret);
	return ret;
}


FrontReportResult::FrontReportResult()
{
}

FrontReportResult::~FrontReportResult()
{
}

int FrontReportResult::encode()
{
	int ret = 0;
	
	string outpkg("");
	Json::Value root;
	Json::Value item;

	// make json data
	item[RET_CODE] = ret_code_;
	item[RET_MSG] = ret_msg_;
	root[BASE_RET] = item;
	root[TASK_TYPE] = (uint32_t)0;
	root[TASK_ID] = (Json::Value::UInt64)577023977136848896;
	root[TASK_CURR_STEP] = (uint32_t)1;
	root[TASK_TOTAL_STEP] = (uint32_t)3;
	root[TIME_NOW] = time_;
	root[TRANS_ID] = trans_id_;

	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode report data result, ret:%d", ret);
	return ret;

}


FrontIncomeResult::FrontIncomeResult()
    : ret_value_(0)
    , err_msg_("")
    , pre_cash_("")
    , useable_cash_("")
    , fetched_cash_("")
{
}


FrontIncomeResult::~FrontIncomeResult()
{
}


int FrontIncomeResult::encode()
{
	int ret = 0;
	
	string outpkg("");
	Json::Value root;
	Json::Value item;

	// make json data
	item[RET_CODE] = ret_code_;
	item[RET_MSG] = err_msg_;
	root[BASE_RET] = item;
	root[BUSSINESS_TYPE] = (uint32_t)3;
	root[ITEM_SIZE] = (uint32_t)1;
	root[CONTINUE_FLAG] = (uint32_t)0;
	root[TIME_NOW] = time_;
	root[TRANS_ID] = trans_id_;
	root[SYNC_VALUE] = time_; // TODO: 

    item.clear();
	encodeIncome(item);
	root[ITEMS].append(item);
	
	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode sync income result, ret:%d", ret);
	return ret;
}

void FrontIncomeResult::set_ret_value(int ret)
{
    ret_value_ = ret;
}

void FrontIncomeResult::set_err_msg(const string &err_msg)
{
    err_msg_ = err_msg;
}

void FrontIncomeResult::set_pre_cash(const string &cash)
{
    pre_cash_ = cash;
}

void FrontIncomeResult::set_useable_cash(const string &cash)
{
    useable_cash_ = cash;
}

void FrontIncomeResult::set_fetched_cash(const string &cash)
{
    fetched_cash_ = cash;
}


int FrontIncomeResult::encodeIncome(Json::Value &item)
{
    item[PRE_CASH] = pre_cash_;
    item[USEABLE_CASH] = useable_cash_;
    item[FETCHED_CASH] = fetched_cash_;
    return 0;
}


FrontStartTaskRequest::FrontStartTaskRequest(BinInputPacket<> &inpkg)
	: FrontRequest(inpkg)
    , task_id_(0)
    , task_type_(0)
    , task_url_("")
{
}

FrontStartTaskRequest::~FrontStartTaskRequest()
{
}


int FrontStartTaskRequest::decode()
{
	int ret = 0;
	FrontRequest::decode();
	Json::Value root;
	Json::Value report_key_item;
	string input_str(inpkg_.getCur(), inpkg_.remainLength());
	LOG(DEBUG)("start task request:%s", input_str.c_str());
	reader_.parse(input_str, root);

	if (root[REQUEST_NODE].isObject()) {
		report_key_item = root[REQUEST_NODE];
	} else {
		LOG(ERROR)("param:%s type is error", REQUEST_NODE);
		ret = -1;
	}
	if (report_key_item[TRANS_ID].isUInt()) {
		trans_id_ = report_key_item[TRANS_ID].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TRANS_ID);
		ret = -1;
	}
	if (report_key_item[TIME_NOW].isUInt()) {
		time_ = report_key_item[TIME_NOW].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TIME_NOW);
		ret = -1;
	}
	if (report_key_item[TASK_ID].isUInt64()) {
		task_id_ = report_key_item[TASK_ID].asUInt64();
	} else {
		LOG(ERROR)("param:%s type is error", TASK_ID);
		ret = -1;
	}
	if (report_key_item[TASK_TYPE].isUInt()) {
		task_type_ = report_key_item[TASK_TYPE].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TASK_TYPE);
		ret = -1;
	}
	if (report_key_item[TASK_URL].isString()) {
		task_url_ = report_key_item[TASK_URL].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TASK_URL);
		ret = -1;
	}

	LOG(INFO)("done decode start task request. ret:%d", ret);
	return ret;

}


FrontStartTaskResult::FrontStartTaskResult()
    : FrontResult()
    , task_id_(0)
    , task_type_(0)
{

}

FrontStartTaskResult::~FrontStartTaskResult()
{
}

int FrontStartTaskResult::encode()
{
	int ret = 0;
	
	string outpkg("");
	Json::Value root;
	Json::Value item;

	// make json data
	item[RET_CODE] = ret_code_;
	item[RET_MSG] = ret_msg_;
	root[BASE_RET] = item;
	root[TASK_TYPE] = task_type_;
	root[TASK_ID] = (Json::Value::UInt64)task_id_;
	root[TIME_NOW] = time_;
	root[TRANS_ID] = trans_id_;

	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode start task result, ret:%d, result:%s", ret, outpkg.c_str());
	return ret;
}



FrontThirdPartyLoginRequest::FrontThirdPartyLoginRequest(BinInputPacket<> &inpkg)
	: FrontRequest(inpkg)
	, login_type_(0)
	, login_flag_(0)
	, login_openid_("")
	, login_nick_name_("")
	, login_passwd_("")
	, login_token_("")
	, login_sex_(0)
	, login_province_("")
	, login_city_("")
	, login_country_("")
	, login_avatar_("")
	, login_desc_("")
	, login_email_("")
	, login_phone_("")
{
}

FrontThirdPartyLoginRequest::~FrontThirdPartyLoginRequest()
{
}

int FrontThirdPartyLoginRequest::decode()
{
	int ret = 0;
	FrontRequest::decode();
	Json::Value root;
	Json::Value login_item;
	string input_str(inpkg_.getCur(), inpkg_.remainLength());
	LOG(INFO)("thirdparty login request:%s", input_str.c_str());
	reader_.parse(input_str, root);

	if (root[REQUEST_NODE].isObject()) {
		login_item = root[REQUEST_NODE];
	} else {
		LOG(ERROR)("param:%s type is error", REQUEST_NODE);
		ret = -1;
	}
	if (login_item[TRANS_ID].isUInt()) {
		trans_id_ = login_item[TRANS_ID].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TRANS_ID);
		ret = -1;
	}
    if (login_item[TIME_NOW].isUInt()) {
        time_ = login_item[TIME_NOW].asUInt();
    } else {
        LOG(ERROR)("param:%s type is error", TIME_NOW);
    }
	if (login_item[TAG_TYPE].isUInt()) {
		login_type_ = login_item[TAG_TYPE].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_TYPE);
		ret = -1;
	}
	if (login_item[TAG_LOGIN_FLAG].isUInt()) {
		login_flag_ = login_item[TAG_LOGIN_FLAG].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_LOGIN_FLAG);
		ret = -1;
	}
	if (login_item[TAG_TOKEN].isString()) {
		login_token_ = login_item[TAG_TOKEN].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_TOKEN);
		ret = -1;
	}
	if (login_item[TAG_OPENID].isString()) {
		login_openid_ = login_item[TAG_OPENID].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_OPENID);
		ret = -1;
	}
	if (login_item[TAG_NICK_NAME].isString()) {
		login_nick_name_ = login_item[TAG_NICK_NAME].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_NICK_NAME);
		ret = -1;
	}
	if (login_item[TAG_PASSWD].isString()) {
		login_passwd_ = login_item[TAG_PASSWD].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_PASSWD);
		ret = -1;
	}
	if (login_item[TAG_SEX].isUInt()) {
		login_sex_ = login_item[TAG_SEX].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_SEX);
	}
	if (login_item[TAG_PROVINCE].isString()) {
		login_province_ = login_item[TAG_PROVINCE].asString();
	} else {
		LOG(WARN)("param:%s type is error", TAG_PROVINCE);
	}
	if (login_item[TAG_CITY].isString()) {
		login_city_ = login_item[TAG_CITY].asString();
	} else {
		LOG(WARN)("param:%s type is error", TAG_CITY);
	}
	if (login_item[TAG_COUNTRY].isString()) {
		login_country_ = login_item[TAG_COUNTRY].asString();
	} else {
		LOG(WARN)("param:%s type is error", TAG_COUNTRY);
	}
	if (login_item[TAG_AVATAR].isString()) {
		login_avatar_ = login_item[TAG_AVATAR].asString();
	} else {
		LOG(WARN)("param:%s type is error", TAG_AVATAR);
		ret = -1;
	}
	if (login_item[TAG_DESC].isString()) {
		login_desc_ = login_item[TAG_DESC].asString();
	} else {
		LOG(WARN)("param:%s type is error", TAG_DESC);
	}
	if (login_item[TAG_EMAIL].isString()) {
		login_email_ = login_item[TAG_EMAIL].asString();
	} else {
		LOG(WARN)("param:%s type is error", TAG_EMAIL);
	}
	if (login_item[TAG_PHONE].isString()) {
		login_phone_ = login_item[TAG_PHONE].asString();
	} else {
		LOG(WARN)("param:%s type is error", TAG_PHONE);
	}
	if (login_item[TAG_PHONE].isString()) {
		login_phone_ = login_item[TAG_PHONE].asString();
	} else {
		LOG(WARN)("param:%s type is error", TAG_PHONE);
	}

	LOG(INFO)("done decode start task request. ret:%d", ret);
	return ret;

}

FrontThirdPartyLoginResult::FrontThirdPartyLoginResult()
{
}

FrontThirdPartyLoginResult::~FrontThirdPartyLoginResult()
{
}


int FrontThirdPartyLoginResult::encode()
{
	int ret = 0;
	
	string outpkg("");
	Json::Value root;
	Json::Value item;

	// make json data
	item[RET_CODE] = ret_value_;
	item[RET_MSG] = ret_msg_;
	root[BASE_RET] = item;
	root[TAG_TYPE] = type_;
	root[TIME_NOW] = time_;
	root[TRANS_ID] = trans_id_;

	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(INFO)("done encode thihrdparty login result, ret:%d, result:%s", ret, outpkg.c_str());
	return ret;
}



FrontWithdrawRequest::FrontWithdrawRequest(BinInputPacket<> &inpkg)
    : FrontRequest(inpkg)    
    , trade_id_("")
    , open_id_("")
    , user_name_("")
    , desc_("")
    , withdraw_type_(0)
    , withdraw_cash_(0)
{
}

FrontWithdrawRequest::~FrontWithdrawRequest()
{
}


int FrontWithdrawRequest::decode()
{
	int ret = 0;
	FrontRequest::decode();
	Json::Value root;
	Json::Value withdraw_item;
	string input_str(inpkg_.getCur(), inpkg_.remainLength());
	LOG(INFO)("FrontWithdraw request:%s", input_str.c_str());
	reader_.parse(input_str, root);

	if (root[REQUEST_NODE].isObject()) {
		withdraw_item = root[REQUEST_NODE];
	} else {
		LOG(ERROR)("param:%s type is error", REQUEST_NODE);
		ret = -1;
	}
	if (withdraw_item[TASK_ID].isString()) {        // just id element
		trade_id_ = withdraw_item[TASK_ID].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TASK_ID);
		ret = -1;
	}
	if (withdraw_item[TRANS_ID].isUInt()) {
		trans_id_ = withdraw_item[TRANS_ID].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TRANS_ID);
		ret = -1;
	}
    if (withdraw_item[TIME_NOW].isUInt()) {
        time_ = withdraw_item[TIME_NOW].asUInt();
    } else {
        LOG(ERROR)("param:%s type is error", TIME_NOW);
    }
	if (withdraw_item[BUSSINESS_TYPE].isUInt()) {
		withdraw_type_ = withdraw_item[BUSSINESS_TYPE].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", BUSSINESS_TYPE);
		ret = -1;
	}
	if (withdraw_item[TAG_WITHDRAW_CASH].isUInt()) {
		withdraw_cash_ = withdraw_item[TAG_WITHDRAW_CASH].isUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_WITHDRAW_CASH);
		ret = -1;
	}
	if (withdraw_item[TAG_OPENID].isString()) {
		open_id_ = withdraw_item[TAG_OPENID].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_OPENID);
		ret = -1;
	}
	if (withdraw_item[TAG_USER_NAME].isString()) {
		user_name_ = withdraw_item[TAG_USER_NAME].asString();
	} else {
		LOG(ERROR)("param:%s type is error", TAG_USER_NAME);
	}
	if (withdraw_item[TAG_DESC].isString()) {
		desc_ = withdraw_item[TAG_DESC].asString();
	} else {
		LOG(WARN)("param:%s type is error", TAG_DESC);
	}

	LOG(INFO)("done decode withdraw request. ret:%d", ret);
	return ret;
}

FrontWithdrawResult::FrontWithdrawResult()
{
}

FrontWithdrawResult::~FrontWithdrawResult()
{
}

int FrontWithdrawResult::encode()
{
	int ret = 0;
	
	string outpkg("");
	Json::Value root;
	Json::Value item;

	// make json data
	item[RET_CODE] = ret_code_;
	item[RET_MSG] = ret_msg_;
	root[BASE_RET] = item;
	root[TAG_TYPE] = withdraw_type_;
	root[TAG_WITHDRAW_CASH] = withdraw_cash_;
	root[TIME_NOW] = time_;
	root[TRANS_ID] = trans_id_;
	root[TAG_TRADE_NO] = trade_no_;

	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(INFO)("done encode thihrdparty login result, ret:%d, result:%s", ret, outpkg.c_str());
	return ret;

}

