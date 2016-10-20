
#include "cmd.h"
#include "client_processor.h"
#include "dbagent_processor.h"
#include "redis_proxy_processor.h"
#include "mysql_msg.h"
#include "server_app.h"
#include "session_manager.h"
#include "client_session.h"
#include "string_util.h"
#include "constants.h"
#include "errcodes.h"
#include "my_crc32.h"
#include "json/json.h"
#include "json/config.h"
#include "json/value.h"


using namespace common;
using namespace utils;


Command::Command( const Processor *processor, const Param &param, Msg *msg )
    : processor_(processor)
    , param_(param)
    , first_request_msg_(msg)
    , err_msg_("")
    , err_code_(0)
{
}

Command::~Command()
{
}   



void Command::ErrMsg(const string &err_msg)
{
    err_msg_ = err_msg;
}



const string &Command::ErrMsg() const
{
    return err_msg_;
}


void Command::ErrCode(int errcode)
{
    err_code_ = errcode;
}

int Command::ErrCode() const
{
    return err_code_;
}

// ============================================================
// 0, beat
// ============================================================
BeatCmd::BeatCmd(const Processor *processor, const Param &param, Msg *msg)
: Command(processor, param, msg)
{
}

BeatCmd::~BeatCmd()
{
}

// 心跳中检测session
bool BeatCmd::Execute()
{
    // make reply msg
    BeatMsg *beat_msg = dynamic_cast<BeatMsg *>(first_request_msg_);
    BeatMsg reply_msg = *beat_msg;
    
    processor_->Reply(param_, *beat_msg, reply_msg);
    LOG(INFO)("client is alive. reply it. net id:%d, server id:%u, remote:%s"
        , param_.net_id, reply_msg.server_id, FromAddrTostring(param_.remote_addr).c_str());
    return true;
}

#if 0
LoginCmd::LoginCmd(const Processor *processor, const Param &param, Msg *msg)
: Command(processor, param, msg)
{
}

LoginCmd::~LoginCmd()
{
}

int LoginCmd::RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result)
{
    if (NULL == p_result) {
        LOG(ERROR)("result is null.");
        return -1;
    }
    p_result->set_msg_header(p_request->msg_header());
    
    SrvLoginResult &srv_login_result = p_result->mutable_pb_msg();
    BaseResult *base_ret = srv_login_result.mutable_retbase();
    base_ret->set_retmsg(ErrMsg());
    base_ret->set_retcode(ErrCode());
    srv_login_result.set_loginseq(p_request->pb_msg().loginseq());
    processor_->Reply(param_, *p_request, *p_result);
    return 0;
}

// 1. 去db校验用户存在与否
// 2. 用户存在则直接下发回复，完成
// 3. 用户不存在，则去db写入记录，并返回uid
// 4. 回复
bool LoginCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
	Msg *result  = NULL;
    LOG(INFO)("LoginCmd::Execute");
    FrontLoginMsg *front_login_msg = dynamic_cast<FrontLoginMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_login_msg), false, "cast login request failed.");
    BackLoginMsg back_login_result;
    COHEADER coheader = front_login_msg->msg_header();
    ONCE_LOOP_ENTER
	// 1. db校验
    const SrvLoginRequest &pb_login_req = front_login_msg->pb_msg();
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    LOG(DEBUG)("==show device id:%s", pb_login_req.deviceid().c_str());
    pb_request.set_sql(makeGetUserIdKey(pb_login_req.deviceid()));
    pb_request.set_sqlid(pb_login_req.transid());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(1);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "check user from db failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from dbgent failed.");
    if (0 == query_result->pb_msg().retbase().retcode()
         && 0 != query_result->pb_msg().affectedrows())
    {
        const string &uid_str = query_result->get_value(0, "Fuid");
        back_login_result.set_uid(StringUtil::strtou32(uid_str));
        back_login_result.set_transid(front_login_msg->pb_msg().transid());
        back_login_result.set_loginseq(front_login_msg->pb_msg().loginseq());
        ErrCode(0);
        ErrMsg("login succeed.");
        LOG(INFO)("check user ok, user exist. uid:%s", uid_str.c_str());
        break;
    }

    // TODO: 
      // 2. 获取uid，此步骤需要到id生成中心申请id，暂时在自己服务生成
//    result  = NULL;
//    BackQueryRequest generate_uid_request;
//    QueryRequest &pb_generate_uid_request = generate_uid_request.mutable_pb_msg();
//    pb_generate_uid_request.set_sql(makeGenerateUidKey());
//    pb_generate_uid_request.set_sqlid(pb_login_req.transid());
//    pb_generate_uid_request.set_isrequestfield(0);
//    pb_generate_uid_request.set_timeout(2);
//    ret_value = processor_->GetResult(DBAGENT_SERVICE, generate_uid_request, result);
//    CHECK_RET(ret_value, "generate uid from db failed.");

//    BackQueryResult *generate_uid_result = dynamic_cast<BackQueryResult *>(result);
//    CHECK_POINTER(generate_uid_result, "cast generate uid result from dbgent failed.");
//    if (0 == generate_uid_result->pb_msg().retbase().retcode())
//    {
//        LOG(INFO)("generate uid ok.");
//    } else {
//        LOG(INFO)("generate uid failed.");
//        ErrCode(98);
//        ErrMsg("mysql error, generate uid failed.");
//        break;
//    }

    // 2. 获取uid，此步骤需要到redis申请id
    BackRedisPxyRequest redis_get_id_request;
    redis_get_id_request.set_msg_header(coheader);
    redis_get_id_request.set_key("incr userid");
    ret_value = processor_->GetResult(REDIS_PROXY, redis_get_id_request, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "get id from redis proxy failed.");
    BackRedisPxyResult *redis_get_id_result = dynamic_cast<BackRedisPxyResult *>(result);
    CHECK_POINTER(redis_get_id_result, "cast get id result from redis proxy failed.");

    
    // 3. 去db写入记录,并返回uid信息
    LOG(INFO)("[happy] new user login, devid:%s, dev type:%d, condid:%d", 
    front_login_msg->pb_msg().deviceid().c_str(), 
    front_login_msg->pb_msg().devicetype(), front_login_msg->pb_msg().condid());
    DbUser user;
    // assign use generate_uid_result
    user.deviceid = pb_login_req.deviceid();
    user.account = DEFAULT_USER_NAME;
    user.gender = GIRL;
    user.avatar = DEFAULT_AVATAR_LINK;
    user.income = DEFAULT_INCOME;
    user.status = ONLINE;
    user.uid = redis_get_id_result->get_id();

	result  = NULL;
    BackQueryRequest write;
    QueryRequest &pb_write = write.mutable_pb_msg();
    pb_write.set_sql(makeSetUserKey(pb_login_req.deviceid(), user));
    pb_write.set_sqlid(pb_login_req.transid());
    pb_write.set_isrequestfield(0);
    pb_write.set_timeout(2);
    write.set_msg_header(coheader);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, write, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "write user info to dbgent failed.");
    BackQueryResult *generate_user_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(generate_user_result, "cast write user info result from dbgent failed.");
    back_login_result.set_uid(user.uid);
    back_login_result.set_transid(front_login_msg->pb_msg().transid());
    back_login_result.set_loginseq(front_login_msg->pb_msg().loginseq());
    ErrCode(0);
    ErrMsg("login succeed.");

    ONCE_LOOP_LEAVE
    // 3. 给客户端回复
    LOG(INFO)("reply client. [%s]", coheader.print());
    if (0 != RepylClient(front_login_msg, &back_login_result))
    {
        LOG(ERROR)("reply client login result failed.");
        return false;
    }
    return ret;
}
#endif

CheckUserCmd::CheckUserCmd(const Processor *processor, const Param &param, Msg *msg)
: Command(processor, param, msg)
{

}

CheckUserCmd::~CheckUserCmd()
{

}

bool CheckUserCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
	Msg *result  = NULL;
    LOG(INFO)("CheckUserCmd::Execute");
    FrontLoginMsg *front_login_msg = dynamic_cast<FrontLoginMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_login_msg), false, "cast login request failed.");
    BackLoginMsg back_login_result;
    COHEADER coheader = front_login_msg->msg_header();
    ONCE_LOOP_ENTER
	// 1. db校验
    const SrvLoginRequest &pb_login_req = front_login_msg->pb_msg();
    const string &device_id = pb_login_req.deviceid();
    uint32_t trans_id = pb_login_req.transid();
    uint32_t crc_num = crc32(const_cast<char *>(device_id.c_str()), device_id.size());
    LOG(INFO)("check user exist, device id:%s, crc:%u, trans id:%u", device_id.c_str(), crc_num, trans_id);
    back_login_result.set_transid(trans_id);
    back_login_result.set_loginseq(front_login_msg->pb_msg().loginseq());
    
    BackQueryRequest query;
    query.set_msg_header(coheader);
    query.set_hashid(crc_num);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeGetUserIdKey(device_id));
    pb_request.set_sqlid(trans_id);
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(1);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "check user from db failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "check user failed, cast query result from dbgent failed.");
    // 用户存在，直接返回登录结果
    if (0 == query_result->get_err_no()
         && 0 != query_result->pb_msg().affectedrows())
    {
        const string &uid_str = query_result->get_value(0, "Fuid");
        back_login_result.set_uid(StringUtil::strtou32(uid_str));
        ErrCode(0);
        ErrMsg("login succeed.");
        LOG(INFO)("check user ok, user exist. uid:%s, device id:%s", uid_str.c_str(), device_id.c_str());
        break;
    } else {
    // 用户不存在，前端需要生成用户信息并插入到db
        back_login_result.set_uid(0);
        ErrCode(ERR_USER_NOT_EXIST);
        ErrMsg("user not exist, need register user.");
        LOG(INFO)("check user ok, user not exist. device id:%s, transid:%u"
            , device_id.c_str(), trans_id);
        break;

    }

    ONCE_LOOP_LEAVE
    // 3. 给客户端回复
    LOG(INFO)("check user, reply client. [%s]", coheader.print());
    if (0 != RepylClient(front_login_msg, &back_login_result))
    {
        LOG(ERROR)("reply check user result failed.");
        return false;
    }
    return ret;

}

int CheckUserCmd::RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result)
{
    if (NULL == p_result) {
        LOG(ERROR)("result is null.");
        return -1;
    }
    p_result->set_msg_header(p_request->msg_header());
    
    SrvLoginResult &srv_login_result = p_result->mutable_pb_msg();
    BaseResult *base_ret = srv_login_result.mutable_retbase();
    base_ret->set_retmsg(ErrMsg());
    base_ret->set_retcode(ErrCode());
    srv_login_result.set_loginseq(p_request->pb_msg().loginseq());
    processor_->Reply(param_, *p_request, *p_result);
    return 0;
}

InsertUserCmd::InsertUserCmd(const Processor *processor, const Param &param, Msg *msg)
: Command(processor, param, msg)
, crc_num_(0)
, uid_(0)
{

}

InsertUserCmd::~InsertUserCmd()
{

}

bool InsertUserCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
	Msg *result  = NULL;
    LOG(INFO)("InsertUserCmd::Execute");
    FrontLoginMsg *front_login_msg = dynamic_cast<FrontLoginMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_login_msg), false, "cast login request failed.");

    BackLoginMsg back_login_result;
    COHEADER coheader = front_login_msg->msg_header();
    ONCE_LOOP_ENTER
    const SrvLoginRequest &pb_login_req = front_login_msg->pb_msg();
    const string &device_id = pb_login_req.deviceid();
    uint32_t trans_id = pb_login_req.transid();
    crc_num_ = crc32(const_cast<char *>(device_id.c_str()), device_id.size());
    back_login_result.set_transid(trans_id);
    back_login_result.set_loginseq(pb_login_req.loginseq());

    // 1. 先获取uid
    if (0 != AccessUid(front_login_msg, &back_login_result))
    {
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("access mysql for uid failed.");
        LOG(ERROR)("insert user info, access uid failed, device id:%s, trans id:%u, crc:%u"
            , device_id.c_str(), trans_id, crc_num_);
        break;
    }

    // 2. 写入用户信息
    WriteUserInfo(front_login_msg, &back_login_result);

    ONCE_LOOP_LEAVE
        
    // 3. 给客户端回复
    LOG(INFO)("reply client. [%s]", coheader.print());
    if (0 != RepylClient(front_login_msg, &back_login_result))
    {
        LOG(ERROR)("reply insert user result failed.");
        return false;
    }
    return ret;
}



int InsertUserCmd::AccessUid(FrontLoginMsg *p_request, BackLoginMsg *p_result)
{
    uint32_t ret_value = 0;
    if (p_request == NULL || p_result == NULL)
    {
        LOG(ERROR)("request is NULL.");
        return ERR_PARAM_INVALID;
    }
    
    ONCE_LOOP_ENTER

    // 1. 写入deviceid，生成uid
    COHEADER coheader = p_request->msg_header();
    const SrvLoginRequest &pb_login_req = p_request->pb_msg();
    uint32_t trans_id = pb_login_req.transid();
    const string &device_id = pb_login_req.deviceid();

    BackQueryRequest write;
    QueryRequest &pb_write = write.mutable_pb_msg();
    string user_key = makeGenerateUserIdKey(device_id);
    pb_write.set_sql(user_key);
    pb_write.set_sqlid(trans_id);
    pb_write.set_isrequestfield(0);
    pb_write.set_timeout(2);
    write.set_hashid(crc_num_);
    write.set_msg_header(coheader);

    LOG(INFO)("start register one user, user key:%s, device id:%s, crc:%u, trans id:%u"
        , user_key.c_str(), device_id.c_str(), crc_num_, trans_id);

    Msg *result = NULL;
    ret_value = processor_->GetResult(DBAGENT_SERVICE, write, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "write device id to dbgent failed.");
    BackQueryResult *write_deviceid_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(write_deviceid_result, "cast write device id result from dbgent failed.");
    int oper_ret = write_deviceid_result->get_err_no();
    if (oper_ret != 0)
    {
        LOG(ERROR)("write deviceid failed, ret:%d, msg:%s.", oper_ret, write_deviceid_result->get_err_msg().c_str());
        break;
    }
    
    // 2. 获取刚才写入的uid
    pb_write.Clear();    
    user_key = makeGetUserIdKey(device_id);
    pb_write.set_sql(user_key);
    pb_write.set_sqlid(trans_id);
    pb_write.set_isrequestfield(0);
    pb_write.set_timeout(2);
    write.set_hashid(crc_num_);
    write.set_msg_header(coheader);
    
    result = NULL;
    ret_value = processor_->GetResult(DBAGENT_SERVICE, write, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "get uid from dbgent failed.");
    BackQueryResult *get_deviceid_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(get_deviceid_result, "cast get uid from dbgent failed.");
    oper_ret = get_deviceid_result->get_err_no();
    if (oper_ret != 0)
    {
        LOG(ERROR)("get uid from dbgent failed, ret:%d, msg:%s.", oper_ret, get_deviceid_result->get_err_msg().c_str());
        break;
    } else {
        const Rows &result_rows = get_deviceid_result->rows();
        uint32_t num = result_rows.size();
        if (0 == num || num > 1) {
            LOG(ERROR)("get uid from dbgent occur error. row num(%u) invaild.", num);
            break;
        }
        const Row &row_item = result_rows[0];
        Row::const_iterator item = row_item.find("Fuid");
        if (row_item.end() == item)
        {
            LOG(ERROR)("get uid from dbgent failed, row have not 'Fuid' field.");
            break;
        }
        uid_ = StringUtil::strtou32(item->second);
    }
    ErrCode(0);
    ErrMsg("register user succeed.");
    LOG(INFO)("access uid successed, uid:%u, user key:%s, device id:%s, trans id:%u"
        , uid_, user_key.c_str(), device_id.c_str(), trans_id);
    
    ONCE_LOOP_LEAVE

    if (uid_ < UID_MIN_VALUE)
    {
        LOG(ERROR)("user id is illegal, insert user failed. uid(%u) < min_uid(%u)", uid_, UID_MIN_VALUE);
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("user id is illegal, insert user info failed");
        ret_value = ERR_MYSQL_ERROR;
    }

    return ret_value;

}

int InsertUserCmd::WriteUserInfo(FrontLoginMsg *p_request, BackLoginMsg *p_result)
{
    uint32_t ret_value = 0;
    if (p_request == NULL || p_result == NULL)
    {
        LOG(ERROR)("request is NULL.");
        return ERR_PARAM_INVALID;
    }
    
    COHEADER coheader = p_request->msg_header();
    const SrvLoginRequest &pb_login_req = p_request->pb_msg();
    const string &device_id = pb_login_req.deviceid();
    uint32_t trans_id = pb_login_req.transid();

    DbUser user;
    // assign use generate_uid_result
    user.deviceid = device_id;
    user.account = DEFAULT_USER_NAME;
    user.gender = GIRL;
    user.avatar = DEFAULT_AVATAR_LINK;
    user.income = DEFAULT_INCOME;
    user.status = ONLINE;
    user.uid = uid_;
    LOG(INFO)("check user exist, device id:%s, dev type:%d, condid:%d, crc:%u, trans id:%u"
        , device_id.c_str(), pb_login_req.devicetype(), pb_login_req.condid(), crc_num_, trans_id);

    BackQueryRequest write;
    QueryRequest &pb_write = write.mutable_pb_msg();
    string user_key = makeSetUserKey(device_id, user);
    pb_write.set_sql(user_key);
    pb_write.set_sqlid(pb_login_req.transid());
    pb_write.set_isrequestfield(0);
    pb_write.set_timeout(2);
    write.set_hashid(crc_num_);
    write.set_msg_header(coheader);

    LOG(INFO)("start register one user, uid:%u, user key:%s, device id:%s, crc:%u, trans id:%u"
        , uid_, user_key.c_str(), user.deviceid.c_str(), crc_num_, trans_id);

    ONCE_LOOP_ENTER
    Msg *result = NULL;
    ret_value = processor_->GetResult(DBAGENT_SERVICE, write, result);
    int oper_ret = result->get_err_no();
    ErrCode(oper_ret);
    ErrMsg(result->get_err_msg());
    if (0 != ret_value)
    {
        LOG(ERROR)("register one user failed, server abnomal, ret:%u, uid:%u, user key:%s, device id:%s, trans id:%u"
            , ret_value, uid_, user_key.c_str(), user.deviceid.c_str(), trans_id);
    }
    if (0 != oper_ret)
    {
        // 此处应该报警，数据不一致!!!
        LOG(ERROR)("register one user failed, bisnuess abnomal, ret:%d, uid:%u, user key:%s, device id:%s, trans id:%u"
            , oper_ret, uid_, user_key.c_str(), user.deviceid.c_str(), trans_id);
        ErrCode(oper_ret);
        ErrMsg("register user failed. see ret code corresponding to mysql err code.");
        break;
    }
    CHECK_RET(ret_value, "write user info to dbgent failed.");
    BackQueryResult *generate_user_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(generate_user_result, "cast write user info result from dbgent failed.");
    p_result->set_uid(user.uid);
    ErrCode(0);
    ErrMsg("register user succeed.");

    LOG(INFO)("register one user successed, uid:%u, user key:%s, device id:%s, trans id:%u"
        , uid_, user_key.c_str(), user.deviceid.c_str(), trans_id);
    ONCE_LOOP_LEAVE

    return ret_value;
}

int InsertUserCmd::RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result)
{
    if (NULL == p_result) {
        LOG(ERROR)("result is null.");
        return -1;
    }
    p_result->set_msg_header(p_request->msg_header());
    
    SrvLoginResult &srv_login_result = p_result->mutable_pb_msg();
    BaseResult *base_ret = srv_login_result.mutable_retbase();
    base_ret->set_retmsg(ErrMsg());
    base_ret->set_retcode(ErrCode());
    srv_login_result.set_loginseq(p_request->pb_msg().loginseq());
    processor_->Reply(param_, *p_request, *p_result);
    return 0;
}


SyncTaskCmd::SyncTaskCmd(const Processor *processor, const Param &param, Msg *msg)
: Command(processor, param, msg)
, limit_(10)
, max_task_id_(0)
, continue_flag_(NOT_CONTINUE)
{
}

SyncTaskCmd::~SyncTaskCmd()
{
}


bool SyncTaskCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
	Msg *result  = NULL;
    LOG(INFO)("SyncTaskCmd::Execute");
    FrontSyncTaskMsg *front_sync_msg = dynamic_cast<FrontSyncTaskMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_sync_msg), false, "cast sync request failed.");
    BackSyncTaskMsg back_sync_result;
    COHEADER coheader = front_sync_msg->msg_header();
    
    ONCE_LOOP_ENTER;
    const SrvSyncTaskRequest &pb_sync_req = front_sync_msg->pb_msg();
    max_task_id_ = pb_sync_req.syncpoint();
    if (pb_sync_req.has_synclimit()) {
        limit_ = pb_sync_req.synclimit();
    }
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeGetTaskListKey(coheader.uid, pb_sync_req.syncflag(), pb_sync_req.syncpoint(), limit_));
    pb_request.set_sqlid(pb_sync_req.transid());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(limit_);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "get task list from db failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from dbgent failed.");
    const Rows &result_rows = query_result->rows();
    uint32_t num = result_rows.size();
    uint64_t taskid = 0;
    for (uint32_t i = 0; i < num; ++i) {
        const Row &row_item = result_rows[i];
        back_sync_result.add_task(row_item);
        RowCIter row_iter = row_item.find(DB::DB_TASK_ID_ITEM);
        taskid = StringUtil::strtou64(row_iter->second);
        if (max_task_id_ < taskid) {
            max_task_id_ = taskid;
        }
    }

    if (num == limit_) {
        continue_flag_ = NEED_CONTINUE;
    } else {
        continue_flag_ = NOT_CONTINUE;
    }
    
    back_sync_result.mutable_pb_msg().set_continueflag(continue_flag_);
    ONCE_LOOP_LEAVE;

    LOG(INFO)("reply client. [%s]", coheader.print());
    RepylClient(front_sync_msg, &back_sync_result);
    return true;
        
}   

int SyncTaskCmd::RepylClient(FrontSyncTaskMsg *p_request, BackSyncTaskMsg *p_result)
{
    if (NULL == p_result) {
        LOG(ERROR)("result is null.");
        return -1;
    }
    p_result->set_msg_header(p_request->msg_header());
    
    SrvSyncTaskResult &srv_sync_result = p_result->mutable_pb_msg();
    BaseResult *base_ret = srv_sync_result.mutable_retbase();
    base_ret->set_retmsg(ErrMsg());
    base_ret->set_retcode(ErrCode());
    srv_sync_result.set_synctype(p_request->pb_msg().synctype());
    srv_sync_result.set_transid(p_request->pb_msg().transid());
    srv_sync_result.set_continueflag(continue_flag_);
    srv_sync_result.set_maxtaskid(max_task_id_);
    processor_->Reply(param_, *p_request, *p_result);
    return 0;
}



GetIncomeCmd::GetIncomeCmd(const Processor *processor, const Param &param, Msg *msg)
: Command(processor, param, msg)
, pre_cash_("0.00")
, useable_cash_("0.00")
, fetched_cash_("0.00")
{
}


GetIncomeCmd::~GetIncomeCmd()
{

}


bool GetIncomeCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
    Msg *result  = NULL;
    LOG(INFO)("GetIncomeCmd::Execute");
    FrontGetIncomeMsg *front_get_income_msg = dynamic_cast<FrontGetIncomeMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_get_income_msg), false, "cast get income request failed.");
    BackGetIncomeMsg back_get_income_result;
    back_get_income_result.set_transid(front_get_income_msg->get_transid());
    COHEADER coheader = front_get_income_msg->msg_header();
    
    ONCE_LOOP_ENTER;
    const SrvGetIncomeRequest &pb_sync_req = front_get_income_msg->pb_msg();
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeGetIncomeKey(coheader.uid));
    pb_request.set_sqlid(pb_sync_req.transid());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(10);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "get income from db failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from dbgent failed.");

    const Rows &result_rows = query_result->rows();
    uint32_t num = result_rows.size();
    if (0 == num || num > 1) {
        LOG(ERROR)("get income occur error. row num(%u) invaild.", num);
        break;
    }
    const Row &row_item = result_rows[0];
    Row::const_iterator item = row_item.find(DB::DB_PRE_CASH_ITEM);
    if (row_item.end() == item)
    {
        LOG(ERROR)("get income failed, row have not 'Fpre_cash' field.");
        break;
    }
    pre_cash_ = item->second;
    item = row_item.find(DB::DB_USEABLE_CASH_ITEM);
    if (row_item.end() == item)
    {
        LOG(ERROR)("get income failed, row have not 'Fpre_cash' field.");
        ErrCode(ERR_MYSQL_NO_VALID_VALUE);
        ErrMsg("get income failed, MYSQL no valid value.");
        break;
    }
    useable_cash_ = item->second;
    item = row_item.find(DB::DB_FETCHED_CASH_ITEM);
    if (row_item.end() == item)
    {
        LOG(ERROR)("get income failed, row have not 'Fpre_cash' field.");
        break;
    }
    fetched_cash_ = item->second;
    ONCE_LOOP_LEAVE;

    back_get_income_result.set_pre_cash(pre_cash_);
    back_get_income_result.set_useable_cash(useable_cash_);
    back_get_income_result.set_fetched_cash(fetched_cash_);

    LOG(INFO)("reply client. [%s]", coheader.print());
    RepylClient(front_get_income_msg, &back_get_income_result);
    return true;
}


int GetIncomeCmd::RepylClient(FrontGetIncomeMsg *p_request, BackGetIncomeMsg *p_result)
{
    if (NULL == p_result) {
        LOG(ERROR)("result is null.");
        return -1;
    }
    p_result->set_msg_header(p_request->msg_header());
    
    SrvGetIncomeResult &srv_get_income_result = p_result->mutable_pb_msg();
    BaseResult *base_ret = srv_get_income_result.mutable_retbase();
    base_ret->set_retmsg(ErrMsg());
    base_ret->set_retcode(ErrCode());
    p_result->set_pre_cash(pre_cash_);
    p_result->set_useable_cash(useable_cash_);
    p_result->set_fetched_cash(fetched_cash_);
    processor_->Reply(param_, *p_request, *p_result);
    return 0;
}



SaveTaskCmd::SaveTaskCmd(const Processor *processor, const Param &param, Msg *msg)
    : Command(processor, param, msg)
{
}


SaveTaskCmd::~SaveTaskCmd()
{

}


bool SaveTaskCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
    Msg *result  = NULL;
    LOG(INFO)("SaveTaskCmd::Execute");
    FrontSaveTaskMsg *front_save_task_msg = dynamic_cast<FrontSaveTaskMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_save_task_msg), false, "cast save task request failed.");
    BackSaveTaskMsg back_save_task_result;
    COHEADER coheader = front_save_task_msg->msg_header();
    back_save_task_result.set_msg_header(coheader);
    
    ONCE_LOOP_ENTER;
    const SrvSaveTaskRequest &pb_save_req = front_save_task_msg->pb_msg();
    if (pb_save_req.taskinfo_size() == 0) 
    {
        LOG(WARN)("save task size is 0, no execute mysql, return. [%s]", coheader.print());
        ErrCode(0);
        ErrMsg("save task size is 0, no insert.");
        break;
    }
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeSaveTaskKey(coheader.uid, pb_save_req));
    pb_request.set_sqlid(pb_save_req.transid());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(10);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "save task to mysql failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from dbgent failed.");

    uint32_t affected_rows = query_result->affectedrows();
    if (0 == affected_rows) {
        LOG(ERROR)("save task to mysql failed, affected rows is 0.");
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("save task to mysql failed, affected rows is 0.");
        break;
    }

    ONCE_LOOP_LEAVE

    LOG(INFO)("reply client. [%s]", coheader.print());
    RepylClient(front_save_task_msg, &back_save_task_result);
    return true;

}


int SaveTaskCmd::RepylClient(FrontSaveTaskMsg *p_request, BackSaveTaskMsg *p_result)
{
    if (NULL == p_result) {
        LOG(ERROR)("result is null.");
        return -1;
    }
    p_result->set_msg_header(p_request->msg_header());
    
    SrvSaveTaskResult &srv_save_result = p_result->mutable_pb_msg();
    BaseResult *base_ret = srv_save_result.mutable_retbase();
    base_ret->set_retmsg(ErrMsg());
    base_ret->set_retcode(ErrCode());
    p_result->set_transid(p_request->transid());
    processor_->Reply(param_, *p_request, *p_result);
    return 0;
}

ReportDataCmd::ReportDataCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
    , reward_(0)
{

}

ReportDataCmd::~ReportDataCmd()
{

}

bool ReportDataCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
    LOG(INFO)("ReportDataCmd::Execute");
    FrontReportMsg *front_report_msg = dynamic_cast<FrontReportMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_report_msg), false, "report data, cast request failed.");
    COHEADER coheader = front_report_msg->msg_header();
    uint64_t taskid = front_report_msg->taskid();
    BackReportMsg back_report_msg;
    back_report_msg.set_transid(front_report_msg->transid());
    back_report_msg.set_task_type(front_report_msg->task_type());
    back_report_msg.set_taskid(taskid);
    back_report_msg.set_msg_header(coheader);

    ONCE_LOOP_ENTER
    // 1. 修改任务进度
    Msg *result = NULL;
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeReportTaskKey(coheader.uid, taskid, front_report_msg->task_step()));
    pb_request.set_sqlid(front_report_msg->transid());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(10);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "report task progress to mysql failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from dbgent failed.");
    
    uint32_t affected_rows = query_result->affectedrows();
    if (0 == affected_rows) {
        LOG(ERROR)("report task progress to mysql failed, affected rows is 0.");
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("report task progress to mysql failed, affected rows is 0.");
        break;
    }

    // 2. 获取任务奖励方案
    ret_value = GetTaskAlgorithm(*front_report_msg);
    if (0 != ret_value)
    {
        LOG(ERROR)("get task algorithm failed.");
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("get task algorithm failed.");
        break;
    }
    
    // 3. 为用户分配金额
    ret_value = RewardUser(*front_report_msg, back_report_msg);
    if (0 != ret_value)
    {
        LOG(ERROR)("reward user failed, uid:%u, taskid:%lu.", coheader.uid, taskid);
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("get task algorithm failed.");
        break;
    }

    LOG(INFO)("reward user ok, uid:%u, taskid:%lu, reward:%u.", coheader.uid, taskid, reward_);

    ONCE_LOOP_LEAVE

    if(ret_value == 0)
    {
        LOG(INFO)("report task progress succeed. reply client. [%s]", coheader.print());
    }
    ReplyClient(*front_report_msg, back_report_msg);
    
    return ret;
}

int ReportDataCmd::ReplyClient(const FrontReportMsg &front_request, BackReportMsg &report_result)
{
    LOG(INFO)("reply client report data, ret:%u, ret msg:%s"
        , report_result.get_err_no(), report_result.get_err_msg().c_str());
    SrvReportTaskResult &request = report_result.mutable_pb_msg();
    com::adv::msg::RetBase *ret_base = request.mutable_retbase();
    ret_base->set_retcode(ErrCode());
    ret_base->set_retmsg(ErrMsg());
    request.set_reward(StringUtil::u32tostr(reward_));
    return processor_->Reply(param_, front_request, report_result);
}




// 要检测奖励的金额是否在合理区间
int ReportDataCmd::GetTaskAlgorithm(const FrontReportMsg &front_request)
{
    COHEADER coheader = front_request.msg_header();
    int ret_value = 0;
    ONCE_LOOP_ENTER
    Msg *result = NULL;
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeGetTaskAlgorithmKey(front_request.taskid()));
    pb_request.set_sqlid(front_request.transid());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(1);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "get algorithm from dbgent failed.");
    
    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from dbgent failed.");
    
    uint32_t affected_rows = query_result->affectedrows();
    if (0 == affected_rows) {
        LOG(ERROR)("get algorithm from dbgent failed, affected rows is 0.");
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("get algorithm from dbgent failed, affected rows is 0.");
        break;
    }

    const Rows & rows = query_result->rows();
    if (rows.size() != 1)
    {
        LOG(ERROR)("task is repeated, or data is dirty. skip this!");
        ret_value = ERR_TASK_REPEATED;
        break;
    }

    string algorithm_str("");
    const Row &one_row = rows[0];
    Row::const_iterator Falgorithm_str = one_row.find("Ftask_algorithm");
    if (Falgorithm_str != one_row.end())
    {
        algorithm_str = Falgorithm_str->second;
    } else {
        LOG(ERROR)("task have no Ftask_algorithm, or data is dirty. skip this!");
        ret_value = ERR_TASK_INVALID;
        break;
    }
	Json::Value root;
	Json::Reader reader;
	reader.parse(algorithm_str, root);
    if (root["alg"].isObject())
    {
        root = root["alg"];
        string step = StringUtil::u32tostr(front_request.task_step());
        if (root[step].isNumeric())
        {
            reward_ = root[step].asUInt();
        } else {
            reward_ = 0;
            LOG(ERROR)("get task algorithm failed, set reward to 0.");
        }
    }
    LOG(INFO)("get algorithm from dbgent succeed, uid:%u, taskid:%lu, step:%u, reward:%u"
        , coheader.uid, front_request.taskid(), front_request.task_step(), reward_);

    ONCE_LOOP_LEAVE

    return ret_value;
}

int ReportDataCmd::RewardUser(const FrontReportMsg &front_request, BackReportMsg &report_result)
{
    COHEADER coheader = front_request.msg_header();
    int ret_value = 0;
    ONCE_LOOP_ENTER
        if (reward_ > MAX_REWARD_NUM)
        {
            LOG(ERROR)("get reward, but reward is abnormal! reward:%u", reward_);
            ret_value = ERR_MYSQL_ERROR;
            ErrCode(ret_value);
            ErrMsg("reward user failed, reward is abnormal");
            break;
        }
        Msg *result = NULL;
        BackQueryRequest query;
        query.set_msg_header(coheader);
        QueryRequest &pb_request = query.mutable_pb_msg();
        pb_request.set_sql(makeRewardUserKey(coheader.uid, front_request.taskid(), reward_));
        pb_request.set_sqlid(front_request.transid());
        pb_request.set_isrequestfield(0);
        pb_request.set_limit(10);
        pb_request.set_timeout(2);
        ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
        ErrCode(result->get_err_no());
        ErrMsg(result->get_err_msg());
        CHECK_RET(ret_value, "reward user to mysql failed.");
    
        BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
        CHECK_POINTER(query_result, "cast query result from dbgent failed.");
        
        uint32_t affected_rows = query_result->affectedrows();
        if (0 == affected_rows) {
            LOG(ERROR)("reward user to mysql failed, affected rows is 0.");
            ErrCode(ERR_MYSQL_ERROR);
            ErrMsg("reward user to mysql failed, affected rows is 0.");
            break;
        }

        LOG(INFO)("reward user succeed, uid:%u, taskid:%lu, reward:%u"
            , coheader.uid, front_request.taskid(), reward_);

    ONCE_LOOP_LEAVE

    return ret_value;
}

/*
RewardUserCmd::RewardUserCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
{
}


RewardUserCmd::~RewardUserCmd()
{
}
 
bool RewardUserCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
    LOG(INFO)("RewardUserCmd::Execute");
    FrontReportMsg *front_report_msg = dynamic_cast<FrontReportMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_report_msg), false, "reward user, cast request failed.");
    COHEADER coheader = front_report_msg->msg_header();
    BackReportMsg back_report_msg;
    back_report_msg.set_transid(front_report_msg->transid());
    back_report_msg.set_task_type(front_report_msg->task_type());
    back_report_msg.set_taskid(front_report_msg->taskid());
    back_report_msg.set_msg_header(coheader);

    ONCE_LOOP_ENTER
    Msg *result = NULL;
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeRewardUserKey(coheader.uid, front_report_msg->taskid(), front_report_msg->reward()));
    pb_request.set_sqlid(front_report_msg->transid());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(10);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "reward user to mysql failed.");
    
    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from dbgent failed.");
    
    uint32_t affected_rows = query_result->affectedrows();
    if (0 == affected_rows) {
        LOG(ERROR)("reward user to mysql failed, affected rows is 0.");
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("reward user to mysql failed, affected rows is 0.");
        break;
    }

    ONCE_LOOP_LEAVE

    if(ret_value == 0)
    {
        LOG(INFO)("reward user succeed. reply client. [%s]", coheader.print());
    }
    ReplyClient(*front_report_msg, back_report_msg);
    
    return ret;

}


int RewardUserCmd::ReplyClient(const FrontReportMsg &front_request, BackReportMsg &report_result)
{
    LOG(INFO)("reply client reward user, ret:%u, ret msg:%s"
        , report_result.get_err_no(), report_result.get_err_msg().c_str());
    SrvReportTaskResult &request = report_result.mutable_pb_msg();
    com::adv::msg::RetBase *ret_base = request.mutable_retbase();
    ret_base->set_retcode(ErrCode());
    ret_base->set_retmsg(ErrMsg());
    return processor_->Reply(param_, front_request, report_result);

}
*/


WithdrawCmd::WithdrawCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
    , reward_(0)
{

}

WithdrawCmd::~WithdrawCmd()
{
}
    
bool WithdrawCmd::Execute()
{
    int ret_value = 0;
    LOG(INFO)("WithdrawCmd");
    FrontWithdrawMsg *front_request = dynamic_cast<FrontWithdrawMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_request), false, "withdraw, cast request failed.");
    COHEADER coheader = front_request->msg_header();
    const SrvWithdrawRequest &front_pb_request = front_request->pb_msg();
    BackWithdrawMsg front_result;
    front_result.SetTransId(front_pb_request.transid());
    front_result.set_msg_header(coheader);

    ONCE_LOOP_ENTER

    // 1. 操作db
    Msg *result = NULL;
    BackQueryRequest withdraw_request;
    withdraw_request.set_msg_header(coheader);
    QueryRequest &withdraw_pb_request = withdraw_request.mutable_pb_msg();
    uint32_t withdraw_cash = front_pb_request.withdraw_cash();
    // 单位是分，提现限制的金额是上限5000元，下限5元
    // 1元= 10角= 100分
    if ( withdraw_cash > 5000 * 100 )
    {
        LOG(WARN)("withdraw cash more than 5000 yuan rmb, warn! withdraw failed.");
        ErrCode(ERR_HUGE_MONEY);
        ErrMsg("withdraw cash too huge.");
        break;
    }
    if ( withdraw_cash < 5 * 100 )
    {
        LOG(WARN)("withdraw cash less than 5 yuan rmb, warn! withdraw failed.");
        ErrCode(ERR_TINY_MONEY);
        ErrMsg("withdraw cash too tiny.");
        break;
    }
    withdraw_pb_request.set_sql(makeWithdrawKey(coheader.uid, withdraw_cash, front_pb_request.passwd()));
    withdraw_pb_request.set_sqlid(front_request->trans_id);
    withdraw_pb_request.set_isrequestfield(0);
    withdraw_pb_request.set_limit(1);
    withdraw_pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, withdraw_request, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "withdraw from dbgent failed.");

    // 2. 获取操作之后的金额数目
    result = NULL;
    BackQueryRequest query_request;
    withdraw_request.set_msg_header(coheader);
    QueryRequest &query_pb_request = query_request.mutable_pb_msg();
    query_pb_request.set_sql(makeGetIncomeKey(coheader.uid));
    query_pb_request.set_sqlid(front_request->trans_id);
    query_pb_request.set_isrequestfield(0);
    query_pb_request.set_limit(1);
    query_pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query_request, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "query cash from dbgent failed.");
    
    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query cash result from dbgent failed.");

    const Rows &result_rows = query_result->rows();
    uint32_t num = result_rows.size();
    if (0 == num || num > 1) {
        LOG(ERROR)("get income occur error. row num(%u) invaild.", num);
        break;
    }
    const Row &row_item = result_rows[0];
    Row::const_iterator item = row_item.find(DB::DB_PRE_CASH_ITEM);
    if (row_item.end() == item)
    {
        LOG(ERROR)("get income failed, row have not 'Fpre_cash' field.");
        break;
    }
    const string &pre_cash = item->second;
    item = row_item.find(DB::DB_USEABLE_CASH_ITEM);
    if (row_item.end() == item)
    {
        LOG(ERROR)("get income failed, row have not 'Fpre_cash' field.");
        ErrCode(ERR_MYSQL_NO_VALID_VALUE);
        ErrMsg("get income failed, MYSQL no valid value.");
        break;
    }
    const string &useable_cash = item->second;
    item = row_item.find(DB::DB_FETCHED_CASH_ITEM);
    if (row_item.end() == item)
    {
        LOG(ERROR)("get income failed, row have not 'Fpre_cash' field.");
        break;
    }
    const string &fetched_cash = item->second;

    SrvWithdrawResult &front_pb_result = front_result.mutable_pb_msg();
    front_pb_result.set_pre_cash(pre_cash);
    front_pb_result.set_useable_cash(useable_cash);
    front_pb_result.set_fetched_cash(fetched_cash);
    front_pb_result.set_withdraw_cash(withdraw_cash);
    ONCE_LOOP_LEAVE;

    ReplyClient(*front_request, front_result);
    return true;

}

	
int WithdrawCmd::ReplyClient(const FrontWithdrawMsg &front_request, BackWithdrawMsg &front_result)
{
    LOG(INFO)("reply client withdraw cash, ret:%u, ret msg:%s"
        , front_result.get_err_no(), front_result.get_err_msg().c_str());
    SrvWithdrawResult &request = front_result.mutable_pb_msg();
    com::adv::msg::RetBase *ret_base = request.mutable_retbase();
    ret_base->set_retcode(ErrCode());
    ret_base->set_retmsg(ErrMsg());
    return processor_->Reply(param_, front_request, front_result);
}



ThirdPartyLoginCmd::ThirdPartyLoginCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param_, msg)
{
}

ThirdPartyLoginCmd::~ThirdPartyLoginCmd()
{
}

/**
  1. 按照类型检测用户第三方账户是否存在本账户
  2. 不存在就创建绑定关系
  3. 存在就验证第三方账户的信息
  4. 返回
**/
bool ThirdPartyLoginCmd::Execute()
{
    int ret_value = 0;
    LOG(INFO)("ThirdPartyLoginCmd");
    FrontThirdPartyLoginMsg *front_request = dynamic_cast<FrontThirdPartyLoginMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_request), false, "thirdparty login, cast request failed.");
    COHEADER coheader = front_request->msg_header();
    const SrvLoginRequest &login_request = front_request->pb_msg();
    FrontThirdPartyLoginMsg front_result;
    front_result.set_msg_header(coheader);

    ONCE_LOOP_ENTER
    // 1. 按照类型检测用户第三方账户是否存在本账户
    Msg *result = NULL;
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeCheckLoginKey(coheader.uid, login_request.accounttype()));
    pb_request.set_sqlid(front_request->trans_id);
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(1);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "check thirdparty account login failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from dbgent failed.");
    
    const Rows &user_rows = query_result->rows();
    if (user_rows.size() != 0)
    {
        // 返回0就是一致的
        if (0 == CheckAccountTheSame(*front_request, user_rows[0]))
        {
            LOG(WARN)("user uid:%u have logined, account type:%u, account:%s"
                , coheader.uid, login_request.accounttype(), login_request.account().c_str());
            ErrCode(0);
            ErrMsg("user have bind the same thirdparty account, login ok.");
            break;
        } else {
            LOG(WARN)("user uid:%u have logined, account type:%u, account:%s"
                , coheader.uid, login_request.accounttype(), login_request.account().c_str());
            ErrCode(ERR_ALREADY_BIND);
            ErrMsg("user have bind another thirdparty account, bind failed.");
            break;
        }
    }

    // 未绑定第三方账户，写入两张表，个人信息表和第三方账号绑定信息表
    DbUser user;
    // assign use generate_uid_result
    user.uid = coheader.uid;
    user.account = login_request.account();
    user.deviceid = login_request.token();
    user.accounttype = login_request.accounttype();
    user.nick_name = login_request.nickname();
    user.mobile = login_request.mobile();
    user.gender = login_request.gender();
    user.avatar = login_request.avatar();
    user.info = login_request.desc();
    user.passwd = makePasswdAddSalt(login_request.passwd());
    user.sign = "";
    user.location = login_request.province();
    user.city = login_request.city();
    user.status = ACTIVATED_ACCOUNT;

    // 写第三方信息表，此乃第一步
    BackQueryRequest write_thirdparty;
    write_thirdparty.set_msg_header(coheader);
    QueryRequest &pb_request2 = write_thirdparty.mutable_pb_msg();
    pb_request2.set_sql(makeThirdLoginKey(coheader.uid, user));
    pb_request2.set_sqlid(front_request->trans_id);
    pb_request2.set_isrequestfield(0);
    pb_request2.set_limit(1);
    pb_request2.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, write_thirdparty, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "write thirdparty account failed.");

    BackQueryResult *write_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(write_result, "cast write thirdparty account result from dbgent failed.");
    
    // 写用户信息表，第二步出错就会出现数据不一致的问题
    BackQueryRequest write_user_info;
    write_user_info.set_msg_header(coheader);
    QueryRequest &pb_request3 = write_user_info.mutable_pb_msg();
    pb_request3.set_sql(makeUserInfoKey(coheader.uid, user));
    pb_request3.set_sqlid(front_request->trans_id);
    pb_request3.set_isrequestfield(0);
    pb_request3.set_limit(1);
    pb_request3.set_timeout(2);
    result = NULL;
    ret_value = processor_->GetResult(DBAGENT_SERVICE, write_user_info, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "[DATA INCONSISTENT] write user info failed.");

    write_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(write_result, "[DATA INCONSISTENT] cast write user info result from dbgent failed.");

    ErrCode(OK);
    ErrMsg("thirdparty login ok.");

    ONCE_LOOP_LEAVE

    ReplyClient(*front_request, front_result);
    return true;
}

int ThirdPartyLoginCmd::CheckAccountTheSame(const FrontThirdPartyLoginMsg &front_request, const Row &row)
{
    // 比较Fuid,Fthird_id的一致性，相同就是医院的账号
    string existed_tid("");
    string existed_uid("");
    string new_uid = StringUtil::u32tostr(front_request.msg_header().uid);
    string new_tid = front_request.pb_msg().account();
    
    RowCIter tid_iter = row.find("Fthird_id");
    RowCIter uid_iter = row.find("Fuid");
    if (tid_iter != row.end())
    {
        existed_tid = tid_iter->first;
    } else {
        LOG(WARN)("thirdparty account has not Fthird_id");
    }
    if (uid_iter != row.end())
    {
        existed_uid = uid_iter->first;
    } else {
        LOG(WARN)("thirdparty account has not Fuid");
    }
    
    if (new_uid == existed_tid && existed_tid == new_tid)
    {
        return 0;
    } else {
        return -1;
    }
}

int ThirdPartyLoginCmd::ReplyClient(const FrontThirdPartyLoginMsg &front_request, FrontThirdPartyLoginMsg &report_result)
{
    LOG(INFO)("reply client thirdparty login, ret:%u, ret msg:%s"
        , report_result.get_err_no(), report_result.get_err_msg().c_str());
    SrvLoginResult &request = report_result.mutable_pb_msg();
    com::adv::msg::RetBase *ret_base = request.mutable_retbase();
    ret_base->set_retcode(ErrCode());
    ret_base->set_retmsg(ErrMsg());
    request.set_accounttype(front_request.pb_msg().accounttype());
    request.set_time(time(NULL));
    request.set_transid(front_request.pb_msg().transid());
    return processor_->Reply(param_, front_request, report_result);
}


CheckTaskValid::CheckTaskValid( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
{
}

CheckTaskValid::~CheckTaskValid()
{
}


/**
  1. 按照类型检测用户第三方账户是否存在本账户
  2. 不存在就创建绑定关系
  3. 存在就验证第三方账户的信息
  4. 返回
**/
bool CheckTaskValid::Execute()
{
    bool ret = true;
    int ret_value = 0;
    CheckTaskRequest *front_request = dynamic_cast<CheckTaskRequest *>(first_request_msg_);
    CHECK_ERROR_RETURN((front_request == NULL), false, "cast checktask request failed");

    COHEADER coheader = front_request->msg_header();
    CheckTaskResult front_result;
    front_result.set_msg_header(coheader);
    uint64_t taskid = front_request->GetTaskId();
    uint32_t Ftask_progress = 0;
    uint32_t total_step = 0;
    front_result.SetTransId(taskid);
    LOG(INFO)("check task valid, taskid:%lu, [%s]", taskid, coheader.print());
    ONCE_LOOP_ENTER
    Msg *result = NULL;
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeCheckTaskKey(coheader.uid, taskid));
    pb_request.set_sqlid(front_request->GetTransId());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(1);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "check task from mysql failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from dbgent failed.");
    
    if (0 != ErrCode()) {
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("db error.");
        ReplyClient(*front_request, front_result);
        return ret;
    } 
    
    const Rows &result_rows = query_result->rows();
    if (result_rows.size()==0) {
        LOG(INFO)("check task, the user start the task for the first time.");
        WriteTaskIntoToUser(*front_request, front_result);
        return ret;
    }

    if (result_rows.size()!=1)
    {
        LOG(ERROR)("check task valid return row is not 1, row:%zu.", result_rows.size());
        ret = ERR_TASK_REPEATED;
        ErrCode(ret);
        ErrMsg("task is repeated");
        ReplyClient(*front_request, front_result);
        return ret;
    }

    const Row result_row = result_rows[0];
    Row::const_iterator row_iter = result_row.begin();
    Row::const_iterator row_iter_end = result_row.end();

    StrMap result_map;
    for (; row_iter != row_iter_end; ++row_iter)
    {
        const string &key = row_iter->first;
        const string &value = row_iter->second;
        result_map[key] = value;
    }
    result_map["Ftaskid"] = StringUtil::u64tostr(taskid);

    // 检测任务是否关闭
    // Ftask_status为1表示打开的任务，为0表示任务关闭
    uint32_t Ftask_status = StringUtil::strtou32(result_map["Ftask_status"]);
    if (Ftask_status == 0)
    {
        ret = ERR_TASK_INVALID;
        LOG(INFO)("task is closed. taskid:%lu, task status:%u", taskid, Ftask_status);
        ErrCode(ret);
        ErrMsg("task is invalid");
        break;
    }
    // 检测任务是否过期
    uint32_t task_end_time = StringUtil::strtou32(result_map["Fend_time"]);
    if (time(NULL) >= task_end_time)
    {
        ret = ERR_TASK_EXPIRE;
        LOG(INFO)("task is expire. taskid:%lu, task end time:%u", taskid, task_end_time);
        ErrCode(ret);
        ErrMsg("task is expire");
        break;
    }
    // 检测任务是否已超额
    uint32_t Flimit_num = StringUtil::strtou32(result_map["Flimit_num"]);
    uint32_t Fstart_num = StringUtil::strtou32(result_map["Fstart_num"]);
    if (Fstart_num >= Flimit_num)
    {
        ret = ERR_TASK_OVERFLOW;
        LOG(INFO)("task is overflow. taskid:%lu, limit num:%u, start num:%u", taskid, Flimit_num, Fstart_num);
        ErrCode(ret);
        ErrMsg("task is overflow");
        break;
    }
    // 检测用户是否已经完成任务
    uint32_t Fflag = StringUtil::strtou32(result_map["Fflag"]);
    if (Fflag == 2)
    {
        ret = ERR_TASK_DONE;
        LOG(INFO)("task is done. taskid:%lu, task flag:%u", taskid, Fflag);
        ErrCode(ret);
        ErrMsg("task is done");
        break;
    }
    // 任务步骤异常，任务步骤大于5了可能是用户恶意刷单
    Ftask_progress = StringUtil::strtou32(result_map["Ftask_progress"]);
    string algorithm_str = result_map["Ftask_algorithm"];
	LOG(INFO)("task algorithm:%s", algorithm_str.c_str());

	Json::Value root;
	Json::Reader reader;
	reader.parse(algorithm_str, root);
    if (root["step"].isNumeric()) {
        total_step = root["step"].asUInt();
    } else {
        total_step = 0;
        LOG(ERROR)("Ftask_algorithm param step type is error");
    }

    if (Ftask_progress >= total_step)
    {
        ret = ERR_TASK_DONE;
        LOG(INFO)("task is done. uid:%u, task current progress:%u, taskid:%lu, total step:%u"
            , coheader.uid, Ftask_progress, taskid, total_step);
        ErrCode(ret);
        ErrMsg("task is done");
        break;
    }
    
    ErrCode(ERR_TASK_ALREADY_START);
    ErrMsg("user start task already");
    LOG(INFO)("user start task already. uid:%u, task current progress:%u, taskid:%lu, total step:%u"
            , coheader.uid, Ftask_progress, taskid, total_step);
    ONCE_LOOP_LEAVE

    ReplyClient(*front_request, front_result);
    return ret;
}


int CheckTaskValid::WriteTaskIntoToUser(const CheckTaskRequest &front_request, CheckTaskResult &front_result)
{
    bool ret = true;
    int ret_value = 0;

    COHEADER coheader = front_request.msg_header();
    ONCE_LOOP_ENTER
    Msg *result = NULL;
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeWriteTaskKey(coheader.uid, front_request.GetTaskId()));
    pb_request.set_sqlid(front_request.GetTransId());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(1);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "write task to mysql failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast write task result from dbgent failed.");

    if (ERR_MYSQL_DUPLICATE_KEY == (uint32_t)ErrCode())
    {
        LOG(INFO)("check task, task started already, uid:%u, taskid:%lu.", coheader.uid, front_request.GetTaskId());
        ErrCode(ERR_TASK_ALREADY_START);
        ErrMsg("check task, task started already.");
        break;
    }
    
    uint32_t affected_rows = query_result->affectedrows();
    if (0 == affected_rows || ErrCode() != 0) {
        LOG(ERROR)("check task, write task info to the user task list table failed, uid:%u, taskid:%lu.", coheader.uid, front_request.GetTaskId());
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("check task, write task info to the user task list table failed.");
        break;
    }
    ErrCode(ERR_TASK_FIRST_START);
    ErrMsg("user start task the first, write task list ok.");
    LOG(INFO)("write task info to the user task list table succeed, uid:%u, taskid:%lu.", coheader.uid, front_request.GetTaskId());
    ONCE_LOOP_LEAVE

    ReplyClient(front_request, front_result);
    
    return ret;
}

int CheckTaskValid::ReplyClient(const CheckTaskRequest &front_request, CheckTaskResult &front_result)
{
    LOG(INFO)("reply client check task valid, ret:%u, ret msg:%s"
        , ErrCode(), ErrMsg().c_str());
    SrvInnerCommonMsg &request = front_result.mutable_common_msg();
    request.AddTagValue(TAG_RET_CODE, (uint32_t)ErrCode());
    request.AddTagValue(TAG_RET_MSG, ErrMsg());
    request.AddTagValue(TAG_TYPE, front_request.GetTaskType());
    request.AddTagValue(TAG_TASK_ID, front_request.GetTaskId());
    request.SetTransid(front_request.GetTransId());
    request.SetTime(time(NULL));
    return processor_->Reply(param_, front_request, front_result);
}


WriteTaskCmd::WriteTaskCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
{
}

WriteTaskCmd::~WriteTaskCmd()
{
}

bool WriteTaskCmd::Execute()
{
}

int WriteTaskCmd::ReplyClient(const FrontWriteTaskMsg &front_request, BackWriteTaskMsg &report_result)
{
    LOG(ERROR)("WriteTaskCmd::ReplyClient not implements");
    return 0;
}


CloseTaskCmd::CloseTaskCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
{

}

CloseTaskCmd::~CloseTaskCmd()
{

}

bool CloseTaskCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
    CloseTaskRequest *front_request = dynamic_cast<CloseTaskRequest *>(first_request_msg_);
    CHECK_ERROR_RETURN((front_request == NULL), false, "cast close task request failed");

    COHEADER coheader = front_request->msg_header();
    CloseTaskResult front_result;
    front_result.set_msg_header(coheader);
    uint64_t taskid = front_request->GetTaskId();
    LOG(INFO)("close task, taskid:%lu, [%s]", taskid, coheader.print());
    ONCE_LOOP_ENTER
    if (taskid == 0)
    {
        LOG(ERROR)("taskid is invalid. return");
        ErrCode(ERR_PARAM_INVALID);
        ErrMsg("param taskid is invalid.");
        break;
    }
    Msg *result = NULL;
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeCloseTaskKey(taskid));
    pb_request.set_sqlid(front_request->GetTransId());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(1);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "close task from dbgent failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast close task result from dbgent failed.");
    
    uint32_t affected_rows = query_result->affectedrows();
    if (0 != ErrCode()) {
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("db error.");
        ReplyClient(*front_request, front_result);
        return ret;
    }

    LOG(INFO)("close task ok. taskid:%lu, [%s]", taskid, coheader.print());
    ONCE_LOOP_LEAVE
    ReplyClient(*front_request, front_result);

    return ret;
}

	
int CloseTaskCmd::ReplyClient(const CloseTaskRequest &front_request, CloseTaskResult &front_result)
{
    LOG(INFO)("reply client close task, ret:%u, ret msg:%s"
        , ErrCode(), ErrMsg().c_str());
    SrvInnerCommonMsg &request = front_result.mutable_common_msg();
    request.AddTagValue(TAG_RET_CODE, (uint32_t)ErrCode());
    request.AddTagValue(TAG_RET_MSG, ErrMsg());
    request.AddTagValue(TAG_TYPE, front_request.GetTaskType());
    request.AddTagValue(TAG_TASK_ID, front_request.GetTaskId());
    request.SetTransid(front_request.GetTransId());
    request.SetTime(time(NULL));
    return processor_->Reply(param_, front_request, front_result);
}

QueryTaskInfoCmd::QueryTaskInfoCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
{

}

QueryTaskInfoCmd::~QueryTaskInfoCmd()
{

}

bool QueryTaskInfoCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
    QueryTaskInfoRequest *front_request = dynamic_cast<QueryTaskInfoRequest *>(first_request_msg_);
    CHECK_ERROR_RETURN((front_request == NULL), false, "cast query task request failed.");

    COHEADER coheader = front_request->msg_header();
    QueryTaskInfoResult front_result;
    front_result.set_msg_header(coheader);
    uint64_t taskid = front_request->GetTaskId();
    LOG(INFO)("query task info, taskid:%lu, [%s]", taskid, coheader.print());

    ONCE_LOOP_ENTER

    Msg *result = NULL;
    BackQueryRequest query;
    query.set_msg_header(coheader);
    QueryRequest &pb_request = query.mutable_pb_msg();
    pb_request.set_sql(makeQueryTaskInfoKey(taskid));
    pb_request.set_sqlid(front_request->GetTransId());
    pb_request.set_isrequestfield(0);
    pb_request.set_limit(1);
    pb_request.set_timeout(2);
    ret_value = processor_->GetResult(DBAGENT_SERVICE, query, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "query task info from mysql failed.");

    BackQueryResult *query_result = dynamic_cast<BackQueryResult *>(result);
    CHECK_POINTER(query_result, "cast query result from mysql failed.");
    
    if (0 != ErrCode()) {
        ErrCode(ERR_MYSQL_ERROR);
        ErrMsg("db error.");
        ReplyClient(*front_request, front_result);
        break;
    }
    
    const Rows &result_rows = query_result->rows();
    if (result_rows.size() == 1)
    {
        LOG(INFO)("query task info result ok.");
        Row result_row = result_rows[0];

        Json::Value root;
        Json::StyledWriter writer;
        // add task id
        root[TAG_TASK_ID] = (Json::UInt64)taskid;

        root[TAG_TASK_STATUS] = StringUtil::strtou32(result_row[DB::DB_TASK_STATUS]);
        root[TAG_TASK_ALGORITHM] = result_row[DB::DB_TASK_ALGORITHM];
        root[TAG_TASK_PRE_CASH] = result_row[DB::DB_TASK_PRE_CASH];
        root[TAG_TASK_AWARD_CASH] = result_row[DB::DB_TASK_AWARD_CASH];
        root[TAG_TASK_LIMIT_NUM] = StringUtil::strtou32(result_row[DB::DB_TASK_LIMIT_NUM]);
        root[TAG_TASK_START_NUM] = StringUtil::strtou32(result_row[DB::DB_TASK_START_NUM]);
        root[TAG_TASK_START_TIME] = StringUtil::strtou32(result_row[DB::DB_TASK_START_TIME]);
        root[TAG_TASK_END_TIME] = StringUtil::strtou32(result_row[DB::DB_TASK_END_TIME]);

        const string &task_str = writer.write(root);
        LOG(INFO)("done query task info ok, %s", task_str.c_str());
        front_result.SetTaskInfo(task_str);
        ErrCode(0);
        ErrMsg("query task info ok");
    }

    ONCE_LOOP_LEAVE

    ReplyClient(*front_request, front_result);
    return ret;
}


int QueryTaskInfoCmd::ReplyClient(const QueryTaskInfoRequest &front_request, QueryTaskInfoResult &front_result)
{
    LOG(INFO)("reply client request task info, ret:%u, ret msg:%s"
        , ErrCode(), ErrMsg().c_str());
    SrvInnerCommonMsg &result = front_result.mutable_common_msg();
    result.AddTagValue(TAG_RET_CODE, (uint32_t)ErrCode());
    result.AddTagValue(TAG_RET_MSG, ErrMsg());
    //result.AddTagValue(TAG_TYPE, front_request.GetTaskType());
    result.AddTagValue(TAG_TASK_ID, front_request.GetTaskId());
    result.SetTransid(front_request.GetTransId());
    result.SetTime(time(NULL));
    return processor_->Reply(param_, front_request, front_result);
}


