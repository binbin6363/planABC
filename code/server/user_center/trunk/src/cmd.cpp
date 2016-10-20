
#include "cmd.h"
#include "client_processor.h"
#include "dbp_processor.h"
#include "login_server_app.h"
#include "session_manager.h"
#include "client_session.h"
#include "string_util.h"
#include "redis_client.h"
#include "constants.h"
#include "protocols.h"
#include "errcodes.h"
#include "my_crc32.h"

using namespace common;
using namespace utils;


Command::Command( const Processor *processor, const Param &param, Msg *msg )
    : processor_(processor)
    , param_(param)
    , first_request_msg_(msg)
    , err_code_(0)
{
    memset(err_msg_, 0, MAX_STACK_STR_LENGTH);
    strcpy(err_msg_, "logind ok.");
}

Command::~Command()
{
}   



void Command::ErrMsg(const string &err_msg)
{
    int len = err_msg.length() + 1;
    len = len > ((int)MAX_STACK_STR_LENGTH) ? ((int)MAX_STACK_STR_LENGTH) : len;    
    strncpy(err_msg_, err_msg.c_str(), len);
}


const char *Command::ErrMsg()
{
    return err_msg_;
}


void Command::ErrCode(int errcode)
{
    err_code_ = errcode;
}

int Command::ErrCode()
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

// 心跳
bool BeatCmd::Execute()
{
    // make reply msg
    BeatMsg *beat_msg = dynamic_cast<BeatMsg *>(first_request_msg_);
	if (NULL == beat_msg) {
        LOG(ERROR)("beat failed, cast beat msg failed.");
		return false;
	}
    BeatMsg reply_msg = *beat_msg;
    processor_->Reply(param_, *beat_msg, reply_msg);
    LOG(INFO)("client is alive. reply it. net id:%d, server id:%u, remote:%s"
        , param_.net_id, beat_msg->server_id, FromAddrTostring(param_.remote_addr).c_str());
    return true;
}


// ============================================================
// 1000, login
// ============================================================
LoginCmd::LoginCmd( const Processor *processor, const Param &param, Msg *msg )
: Command(processor, param, msg)
, uid_(0)
{

}

LoginCmd::~LoginCmd()
{
}


int LoginCmd::RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result)
{
    // make SrvLoginResult
    BackLoginMsg back_login_msg;
    
    SrvLoginResult &srv_login_result = back_login_msg.mutable_pb_msg();
    if (NULL != p_result)
    {
        srv_login_result = p_result->mutable_pb_msg();
        LOG(INFO)("user login ok, uid:%u", uid_);
    } else {
        com::adv::msg::RetBase* ret_base = srv_login_result.mutable_retbase();
        ret_base->set_retcode(100);
        ret_base->set_retmsg("login failed, unknown reason.");
        LOG(ERROR)("login failed, unknown reason.");
    }
    COHEADER coheader = p_request->msg_header();
    coheader.uid = srv_login_result.uid();
    coheader.cmd = CMD_LOGIN;
    back_login_msg.set_msg_header(coheader);
    processor_->Reply(param_, *p_request, back_login_msg);
    return 0;
}


// 1. 去db校验用户存在与否
// 2. 用户存在则直接下发回复，完成
// 3. 用户不存在，则去db写入记录，并返回uid
// 4. 回复
bool LoginCmd::Execute()
{
    bool ret = true;
    LOG(INFO)("LoginCmd::Execute");
    FrontLoginMsg *front_login_msg = dynamic_cast<FrontLoginMsg *>(first_request_msg_);
    COHEADER coheader = front_login_msg->msg_header();
    BackLoginMsg *back_login_result = NULL;
	const SrvLoginRequest &pb_login_req = front_login_msg->pb_msg();
    const string &device_id = pb_login_req.deviceid();
    uint32_t trans_id = pb_login_req.transid();
    uint32_t condid = pb_login_req.condid();
    uint32_t version = pb_login_req.version();
    
    int ret_value = 0;
	Msg *result  = NULL;
    ONCE_LOOP_ENTER

	// 1. db校验用户存在与否
	FrontLoginMsg check_user_request;
    check_user_request.mutable_pb_msg().CopyFrom(pb_login_req);
    coheader.cmd = CMD_INNER_CHECK_USER;
    // 保证连续的请求落在同一台dbp上
    // 不能保证dbp挂掉一台依然落在同一台
    // 此处要解决的是数据一致性问题，保证一个device id不会同时注册两个用户账户
    uint32_t crc_num = crc32(const_cast<char *>(device_id.c_str()), device_id.size());
    LOG(INFO)("check user exist, cmd:%u, device id:%s, crc:%u, trans id:%u", coheader.cmd, device_id.c_str(), crc_num, trans_id);
    check_user_request.set_hashid(crc_num);
    check_user_request.set_msg_header(coheader);
    ret_value = processor_->GetResult(DBP_SERVICE, check_user_request, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "check user from dbp failed.");

    back_login_result = dynamic_cast<BackLoginMsg *>(result);
    CHECK_POINTER(back_login_result, "cast check user result from db failed.");
    if (0 == back_login_result->mutable_pb_msg().retbase().retcode())
    {
        uid_ = back_login_result->mutable_pb_msg().uid();
        LOG(INFO)("check user ok, user exist, login ok, uid:%u, trans id:%u.", uid_, trans_id);
        break;
    }

	// 2. 请求uid并将用户信息插入db
	FrontLoginMsg login_request;
    login_request.mutable_pb_msg().CopyFrom(pb_login_req);
    //login_request.mutable_pb_msg().set_uid(uid_);
    LOG(DEBUG)("[%s] user login, insert user info.", pb_login_req.deviceid().c_str());
    coheader.cmd = CMD_INNER_INSERT_USER;
    login_request.set_msg_header(coheader);
    ret_value = processor_->GetResult(DBP_SERVICE, login_request, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "insert user from dbp failed.");

    back_login_result = dynamic_cast<BackLoginMsg *>(result);
    CHECK_POINTER(back_login_result, "cast check user result from db failed.");
    if (0 == back_login_result->mutable_pb_msg().retbase().retcode())
    {
        LOG(INFO)("insert user ok, login ok.");
        break;
    }
	
    ONCE_LOOP_LEAVE

    // 3. 将用户插入到队列
    if (0 == ret_value)
    {
        User *user = UserMgr::Instance().get_user_by_uid(uid_);
        if (NULL == user)
        {
            LOG(INFO)("user queue not exist, new user, uid:%u.", uid_);
            user = UserMgr::Instance().new_user();
            user->uid = uid_;
            user->device_type = DEV_TYPE_ANDROID; // 手机
            user->device_id = device_id;
            user->cond_id = condid;
            user->client_ver = version;
            UserMgr::Instance().add_user(user);
        }
        else
        {
            LOG(INFO)("user queue exist, update status, uid:%u.", uid_);
            user->uid = uid_;
            user->device_type = DEV_TYPE_ANDROID; // 手机
            user->device_id = device_id;
            user->cond_id = condid;
            user->client_ver = version;
            UserMgr::Instance().update_time(user);
        }
    }
    
    // 4. 给客户端回复
    if (0 != RepylClient(front_login_msg, back_login_result))
    {
        LOG(ERROR)("reply client login result failed.");
        return false;
    }

    return ret;
}


int LoginCmd::GenerateUserId()
{
    FrontLoginMsg *front_login_msg = dynamic_cast<FrontLoginMsg *>(first_request_msg_);
    const string &device_id = front_login_msg->pb_msg().deviceid();
    uint32_t hashid = 0;
    int ret_value = 0;

    ONCE_LOOP_ENTER
    // 管理id生成的redis客户端需要特殊处理，取第一个
    RedisClient *redis_client = LoginApp::Instance()->GetRedisClient(hashid);
    CHECK_POINTER(redis_client, "get redis client failed. redis client is null.");
    ret_value = redis_client->GenerateUserId(uid_);
    CHECK_RET(ret_value, "generate user id failed");
    LOG(INFO)("generate user id ok, deviceid:%s, uid:%u", device_id.c_str(), uid_);
    ONCE_LOOP_LEAVE

    return ret_value;
}


// cmd= 100, user keep alive
UserKeepAliveCmd::UserKeepAliveCmd( const Processor *processor, const Param &param, Msg *msg )
: Command(processor, param, msg)
{
}


UserKeepAliveCmd::~UserKeepAliveCmd()
{
}
    
bool UserKeepAliveCmd::Execute()
{
    bool ret = true;
    LOG(DEBUG)("user keep alive request.");
    UserKeepAliveMsg *keep_alive_request = dynamic_cast<UserKeepAliveMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == keep_alive_request), false, "cast keep alive msg failed.");

    uint32_t uid = keep_alive_request->uid();
    User *user = UserMgr::Instance().get_user_by_uid(uid);
    if (user) {
        UserMgr::Instance().update_time(user);
        LOG(INFO)("user exist, keep alive succeed. [%s]", user->print());
    } else {
        user = UserMgr::Instance().new_user();
        user->uid = uid;
        user->cond_id = keep_alive_request->cond_id();
        user->device_id = keep_alive_request->device_id();
        user->device_type = keep_alive_request->device_type();
        user->client_ver = keep_alive_request->client_ver();
        UserMgr::Instance().add_user(user);
        LOG(INFO)("user not exist, first keep alive request. [%s]", user->print());
    }
    return ret;
}


WithdrawCmd::WithdrawCmd(const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
{
}

WithdrawCmd::~WithdrawCmd()
{
}

// 1. 校验用户是否已进行第三方绑定以及验证密码信息
// 未绑定则需要先绑定，无密码的不允许提现，提示错误码
// 2. 校验用户金额
// 3. 在本系统中给用户扣款
// 4. 成功之后提现请求入消息队列
// 5. 返回结果
// 最终的提现结果是异步的，依赖服务器推送系统消息给客户端
bool WithdrawCmd::Execute()
{
    bool ret = true;
    LOG(DEBUG)("user withdraw request.");
    FrontWithdrawMsg *front_request = dynamic_cast<FrontWithdrawMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_request), false, "cast withdraw request failed.");

    BackWithdrawMsg front_result;
    ONCE_LOOP_ENTER
    // 1. 校验用户是否已进行第三方绑定以及验证密码信息
    if (0 != AuthUser(*front_request)) {
        break;
    }
    
    // 2. 校验用户金额
    if (0 != AuthCash(*front_request)) {
        break;
    }
    
    // 3. 在本系统中给用户扣款
    if (0 != DoPreWithdraw(*front_request, front_result)) {
        break;
    }
    
    // 4. 成功之后提现请求入消息队列
    if (0 != NotifyWithdraw(*front_request, front_result)) {
        break;
    }
    

    ONCE_LOOP_LEAVE
    
    // 5. 返回结果
    ReplyClient(*front_request, front_result);
    return ret;

}


int WithdrawCmd::AuthUser(const FrontWithdrawMsg& front_request)
{
    LOG(WARN)("AuthUser, no supported yet.");
    return 0;
}

int WithdrawCmd::AuthCash(const FrontWithdrawMsg& front_request)
{
    LOG(WARN)("AuthCash, no supported yet.");
    return 0;
}

// 从本系统的db中操作金额
int WithdrawCmd::DoPreWithdraw(const FrontWithdrawMsg& front_request, BackWithdrawMsg &front_result)
{
    int ret_value = 0;
    LOG(INFO)("DoPreWithdraw.");
    COHEADER coheader = front_result.msg_header();
    const SrvWithdrawRequest &front_pb_request = front_request.pb_msg();

    ONCE_LOOP_ENTER

    FrontWithdrawMsg backend_request;
    SrvWithdrawRequest &backend_pb_request =  backend_request.mutable_pb_msg();
    backend_pb_request.CopyFrom(front_pb_request);
    backend_request.set_hashid(coheader.uid);
    backend_request.set_msg_header(coheader);
    
    Msg *result = NULL;
    ret_value = processor_->GetResult(DBP_SERVICE, backend_request, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_RET(ret_value, "do pre withdraw cash failed.");

    BackWithdrawMsg *back_result = dynamic_cast<BackWithdrawMsg *>(result);
    CHECK_POINTER(back_result, "cast pre withdraw cash result from db failed.");

    SrvWithdrawResult &back_pb_result = front_result.mutable_pb_msg();
    back_pb_result.CopyFrom(back_result->mutable_pb_msg());
    ONCE_LOOP_LEAVE
    
    LOG(INFO)("done pre withdraw from db, ret:%u.", ret_value);
    return ret_value;
}

// 请求丢到队列中，与第三方支付接口操作实际金额
int WithdrawCmd::NotifyWithdraw(const FrontWithdrawMsg& front_request, BackWithdrawMsg &front_result)
{
    LOG(INFO)("NotifyWithdraw not implements.");
    SrvWithdrawResult &front_pb_result = front_result.mutable_pb_msg();
    const SrvWithdrawRequest &front_pb_request = front_request.pb_msg();
    front_pb_result.set_desc(front_pb_request.desc());
    front_pb_result.set_type(front_pb_request.type());
    front_pb_result.set_transid(front_pb_request.transid());
    uint32_t now_time = time(NULL);
    string trade_no = string("dspwd_") + StringUtil::u32tostr(now_time);
    front_pb_result.set_trade_id(trade_no);
    front_pb_result.set_withdraw_cash(front_pb_request.withdraw_cash());
    front_pb_result.set_time(now_time);

    return 0;
}

int WithdrawCmd::ReplyClient(const FrontWithdrawMsg& front_request, BackWithdrawMsg &front_result)
{    
    SrvWithdrawResult &front_pb_result = front_result.mutable_pb_msg();
    com::adv::msg::RetBase* ret_base = front_pb_result.mutable_retbase();
    ret_base->set_retcode(ErrCode());
    ret_base->set_retmsg(ErrMsg());
    COHEADER coheader = front_result.msg_header();
    front_result.set_msg_header(coheader);
    
    LOG(INFO)("reply client withdraw result. %s, ret%u, msg:%s", coheader.print(), ErrCode(), ErrMsg());
    processor_->Reply(param_, front_request, front_result);
    return 0;
}

