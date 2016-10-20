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

#include "request.h"
#include "server_app.h"

using namespace utils;

//void PrintPbData(const Message &message, const char *desc)
//{
//    // log protobuf info
//    const char *tag = "print protobuf info:";
//    if (NULL != desc)
//    {
//        tag = desc;
//    }
//    std::string printer;
//    TextFormat::PrintToString(message, &printer);
//    LOG(DEBUG)("%s\n%s", tag, printer.c_str());
//}


Request::Request(BinInputPacket<> &inpkg)
    : inpkg_(inpkg)
{
}
Request::~Request()
{
}

int Request::decode()
{
	inpkg_.offset_head(sizeof(HEADER));
	inpkg_.get_head(header_);
	LOG(INFO)("client request head [%s]", header_.print());
	return 0;
}

HEADER Request::get_head()
{
	return header_;
}

Result::Result()
	: data_(NULL)
	, data_len_(0)
	, ret_code_(0)
	, ret_msg_("")
{
	data_ = ServerApp::Instance()->get_buffer();
}

Result::~Result()
{
}

char *Result::data()
{
	return data_;
}

uint32_t Result::byte_size()
{
	return data_len_;
}


int Result::encode()
{
	LOG(DEBUG)("base result encode head.");
	return 0;
}

void Result::set_head(const HEADER &head)
{
	header_ = head;
}

const HEADER &Result::get_head()
{
	return header_;
}

void Result::set_len(uint32_t pkglen)
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

int Result::encode_body(const char *data, uint32_t data_len)
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

int Result::encode_head()
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
	: Request(inpkg)
	, devid_("")
	, devtype_(0)
//	, account_type_(0)
//	, token_("")
//	, account_("")
	, passwd_("")
	, version_(0)
	, time_(0)
{
}

FrontLoginRequest::~FrontLoginRequest()
{
}

int FrontLoginRequest::decode()
{
	int ret = 0;
	Request::decode();

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
//	if (root[ACCOUNTTYPE].isUInt()) {
//		account_type_= root[ACCOUNTTYPE].asUInt();
//	} else {
//		LOG(ERROR)("param:%s type is error", ACCOUNTTYPE);
//		ret = -1;
//	}
//	if (root[TOKEN].isString()) {
//		token_= root[TOKEN].asString();
//	} else {
//		LOG(ERROR)("param:%s type is error", TOKEN);
//		ret = -1;
//	}
//	if (root[ACCOUNT].isString()) {
//		account_ = root[ACCOUNT].asString();
//	} else {
//		LOG(ERROR)("param:%s type is error", ACCOUNT);
//		ret = -1;
//	}
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
	if (root[TIME_NOW].isUInt()) {
		time_= root[TIME_NOW].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TIME_NOW);
		ret = -1;
	}
	return ret;
}
//    bool ret_value = login_request_.ParsePartialFromArray();
//	if (!ret_value) {
//    	LOG(ERROR)("decode login protobuf pkg failed.");
//	} else {
//    	LOG(DEBUG)("decode login protobuf ok.");
//		PrintPbData(login_request_);
//	}
//	return ret_value ? 0 : 1;

FrontLoginResult::FrontLoginResult()
	: key_("")
	, uid_(0)
	, time_(0)
{
//	data_ = ServerApp::Instance()->get_buffer();
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
	root[LOGIN_NODE] = login_item;

	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode login result, ret:%d", ret);
	return ret;
}


FrontBeatRequest::FrontBeatRequest(BinInputPacket<> &inpkg)
	: Request(inpkg)
{
}

FrontBeatRequest::~FrontBeatRequest()
{
}

int FrontBeatRequest::decode()
{
	int ret = 0;
	Request::decode();
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
	root[TIME_NOW] = (uint32_t)time(NULL);

	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode beat result, ret:%d", ret);
	return ret;

}

FrontSyncRequest::FrontSyncRequest(BinInputPacket<> &inpkg)
	: Request(inpkg)
{
}

FrontSyncRequest::~FrontSyncRequest()
{
}

int FrontSyncRequest::decode()
{
	int ret = 0;
	Request::decode();
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
			key= item[ITEM_VALUE].asUInt64();
		} else {
			LOG(ERROR)("param:%s type is error, skip", ITEM_VALUE);
			ret = -1;
			continue;
		}

		sync_items_[key] = value;
	}

	LOG(INFO)("done decode sync request. ret:%d", ret);
	return ret;
}


TaskInfo task1 = {144115188143095872
	, 0
	, "download app1"
	, "http://www.apple.com/cn//?afid=p238%7C1wM6yQbQJ-"\
		"dc_mtid_18707vxu38484_pcrid_7528694023_&cid=aos-cn-kwb-brand"
	, "task describe"
	, 1024000
	, "2.5"
	, 3
	, "bbwang"
	, 1444998492
	, 1445458490};

TaskInfo task2 = {144915348143095876
	, 0
	, "download app2"
	, "http://www.apple.com/cn//?afid=p238%7C1wM6yQbQJ-"\
		"dc_mtid_18707vxu38484_pcrid_7528694023_&cid=aos-cn-kwb-brand"
	, "task describe2"
	, 2048000
	, "2.5"
	, 3
	, "bbwang"
	, 1444998492
	, 1445458490};

FrontSyncResult::FrontSyncResult()
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

	// make json data
	item[RET_CODE] = ret_code_;
	item[RET_MSG] = ret_msg_;
	root[BASE_RET] = item;
	root[BUSSINESS_TYPE] = (uint32_t)1;
	root[ITEM_SIZE] = (uint32_t)2;
	root[CONTINUE_FLAG] = (uint32_t)0;
	root[SYNC_VALUE] = (Json::Value::UInt64)144915348143095876;
	root[TIME_NOW] = (uint32_t)time(NULL);

	encodeTask(item, task1, 1);
	root[ITEMS].append(item);
	
	encodeTask(item, task2, 1);
	root[ITEMS].append(item);
	
	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode sync result, ret:%d", ret);
	return ret;

}

// flag.1表示新任务，2表示历史任务
int FrontSyncResult::encodeTask(Json::Value &item, TaskInfo task, int flag)
{
	int ret = 0;
	item[TASK_ID] = (Json::Value::UInt64)task.id;
	item[TASK_TYPE] = task.type;
	item[TASK_NAME] = task.name;
	item[TASK_LINK] = task.link;
	item[TASK_DESC] = task.desc;
	item[TASK_SIZE] = task.size;
	item[TASK_PAY] = task.pay;
	item[TASK_STEP] = task.step;
	item[TASK_PUBLR] = task.publisher;
	item[TASK_STIME] = task.stime;
	item[TASK_ETIME] = task.etime;
	return ret;
}

int FrontSyncResult::encodeIncome(Json::Value &item)
{
	int ret = 0;
	item[INCOME] = "120";
	return ret;
}


FrontReportRequest::FrontReportRequest(BinInputPacket<> &inpkg)
	: Request(inpkg)
{
}

FrontReportRequest::~FrontReportRequest()
{
}

int FrontReportRequest::decode()
{
	int ret = 0;
	Request::decode();
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
		task_done_step_ = report_key_item[TASK_DONE_STEP].asUInt();
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
	if (report_key_item[TIME_NOW].isUInt()) {
		time_ = report_key_item[TIME_NOW].asUInt();
	} else {
		LOG(ERROR)("param:%s type is error", TIME_NOW);
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
	root[TASK_DONE_STEP] = (uint32_t)1;
	root[TIME_NOW] = (uint32_t)time(NULL);

	outpkg = root.toStyledString();

	// encode
	ret = encode_body(outpkg.data(), outpkg.size());
	ret |= encode_head();
	
	LOG(DEBUG)("done encode report data result, ret:%d", ret);
	return ret;

}

