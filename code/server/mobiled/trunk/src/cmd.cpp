/**
 * @filedesc: 
 * cmd.cpp, handle command
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/13 12:02:59
 * @modify:
 *
**/

#include "cmd.h"
#include "request.h"
#include "backend_request.h"
#include "server_app.h"
#include "client_session.h"
#include "hex_string.h"
#include "server_config.h"
#include "user_center_session.h"
#include "task_center_session.h"
#include "dbp_session.h"

using namespace common;
using namespace utils;

// =========================
// base cmd
// =========================
Cmd::Cmd(Net_Session *session, BinInputPacket<> &inpkg)
	: session_(session)
	, inpkg_(inpkg)
{
}

Cmd::~Cmd()
{
}


// =========================
// base cmd which running in separete thread
// =========================
CmdThread::CmdThread(Net_Session* session, BinInputPacket<>& inpkg)
    : Task_Base()
    , m_is_run(false)
    , m_notify_stop(false)
    , session_(session)
    , inpkg_(inpkg)
{
}

CmdThread::~CmdThread()
{
}

int CmdThread::doPreStart()
{
    LOG(ERROR)("err, call base class doPreStart.");
    return -1;
}

int CmdThread::start()
{
	if (false != m_is_run) {
		return -1;
	}

	m_notify_stop = false;
	int rc = activate();
	if (rc != 0) {
        LOG(ERROR)("start cmd thread failed. ret:%d", rc);
		return -1;
	}

	m_is_run = true;
	return 0;
}

int CmdThread::stop()
{
	m_notify_stop = true;
	wait();

	m_is_run = false;
	return 0;
}

int CmdThread::execute(void *)
{
    LOG(INFO)("enter cmd thread.");
    
    int rc = doPreStart();
    
    rc |= start();
    return rc;
}

int CmdThread::svc()
{
    LOG(ERROR)("err, call base class svc.");
    return -1;
}


// =========================
// other command define as follows
// =========================

// =========================
// å®¢æˆ·ç«¯çš„å¿ƒè·³ï¼Œcmd=0
// =========================
int BeatCmd::execute(void *)
{
	ClientSession *client_session = dynamic_cast<ClientSession *>(session_);
	if (NULL == client_session) {
		LOG(ERROR)("BeatCmd, client session cast failed.");
		return -1;
	}
	
	int ret = 0;
	FrontBeatRequest front_request(inpkg_);
	FrontBeatResult front_result;
	ONCE_LOOP_ENTER
	ret = front_request.decode();
	if (0 != ret) {
		LOG(ERROR)("BeatCmd decode request failed.");
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
		front_result.ret_msg_ = "inpkg data format is not match, BeatCmd decode request failed";
		front_result.set_head(front_request.get_head());
		break;
	}

	front_result.ret_code_ = ERR_SUCCESS;
	front_result.ret_msg_ = "user beat success.";
	front_result.set_head(front_request.get_head());
    front_result.set_trans_id(front_request.trans_id());

	ONCE_LOOP_LEAVE

	//front_result;
	ret = front_result.encode();
	if (0 != ret) {
		LOG(ERROR)("BeatCmd encode result failed. ret:%d", ret);
		return ret;
	}
    LOG(DEBUG)("show session key:");
    LOG_HEX(client_session->session_key(), CLIENT_PASSWD_LENGTH, utils::L_DEBUG);

	LOG(DEBUG)("show client result head [%s]", front_result.get_head().print());
	// åŠ å¯†
	ret = client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
	if (0 != ret) {
		LOG(ERROR)("BeatCmd send response failed. ret:%d", ret);
		return ret;
	}
	LOG(INFO)("done beat request. ret code:%d", ret);

    // °ÑÇëÇó×ªÒÆµ½usercenter
    COHEADER coheader;
    HEADER header = front_request.get_head();
    UserKeepAliveMsg keep_alive_request;
    keep_alive_request.set_uid(client_session->uid());
    keep_alive_request.set_cond_id(utils::ServerConfig::Instance()->cond_id());
    keep_alive_request.set_device_type(client_session->device_type());
    keep_alive_request.set_client_ver(client_session->client_version());
    keep_alive_request.set_device_id(client_session->device_id());
    coheader.seq = header.seq;
    coheader.cmd = header.cmd;
    coheader.head_len = sizeof(COHEADER);
    coheader.uid = header.uid;
    keep_alive_request.set_head(coheader);
    ret = keep_alive_request.encode();
	if (0 != ret) {
		LOG(ERROR)("user keep alive to usercenter encode failed. ret:%d", ret);
		return ret;
	}

	ret = UserCenterSession::SendToUserCenter(coheader.uid, keep_alive_request.data(), keep_alive_request.byte_size());
	if (0 != ret) {
		LOG(ERROR)("BeatCmd, keep alive request to usercenter failed. ret:%d", ret);
		return ret;
	}
    LOG(INFO)("send usercenter keepalive request, [%s]", coheader.print());
	return ret;
}


// =========================
// ç™»å½•åè®®å¤„ç†ï¼Œcmd=1000
// =========================
int LoginCmd::execute(void *)
{
	ClientSession *client_session = dynamic_cast<ClientSession *>(session_);
	if (NULL == client_session) {
		LOG(ERROR)("client session cast failed.");
		return -1;
	}
	
	int ret = 0;
//	char random_key[CLIENT_PASSWD_LENGTH] = {0};
//	MakeKeyRand(random_key);
//	client_session->session_key(random_key, CLIENT_PASSWD_LENGTH);
//    LOG(DEBUG)("show session key:");
//    LOG_HEX(client_session->session_key(), CLIENT_PASSWD_LENGTH, utils::L_DEBUG);

    HEADER cmhdr;
    inpkg_.get_head(cmhdr);
    char client_random_key[CLIENT_PASSWD_LENGTH] = {0};
    inpkg_.read(client_random_key, CLIENT_PASSWD_LENGTH);

    if (!inpkg_.good())
    {
        LOG(WARN)("[login] login pkg is error! %s", cmhdr.print());
        client_session->destory_connect();
		ret = -1;
        return ret;
    }
    if (client_session->is_login_ok())
    {
        LOG(ERROR)("[login] user has logined!, %s", cmhdr.print());
		ret = -1;
        client_session->destory_connect();
        return ret;
    }

    // debugÄ£Ê½ÏÂ£¬±ãÓÚµ÷ÊÔ£¬Êı¾İ°ü²»¼ÓÃÜ
    #ifndef _DEBUG
    LOG(DEBUG)("[login] before decrypt pkg body:");
    LOG_HEX(inpkg_.getCur(), cmhdr.len - sizeof(HEADER) - CLIENT_PASSWD_LENGTH, utils::L_DEBUG);
    int nOutLen = 0;
    XTEA::decipher(client_random_key, inpkg_.getCur(),
        cmhdr.len - sizeof(HEADER) - CLIENT_PASSWD_LENGTH,
        inpkg_.getCur(),
        cmhdr.len - sizeof(HEADER) - CLIENT_PASSWD_LENGTH,
        nOutLen);
    LOG(DEBUG)("[login] after decrypt pkg");
    LOG_HEX(inpkg_.getData(), inpkg_.size(), utils::L_DEBUG);
	if (0 == nOutLen) {
		LOG(ERROR)("[login] decrypt pkg failed. [%s]", cmhdr.print());
		ret = -1;
        client_session->destory_connect();
        return ret;
	}
    nOutLen = nOutLen + sizeof(HEADER) + CLIENT_PASSWD_LENGTH;
    inpkg_.set_pkglen(nOutLen);
    #endif
    // ÒÔÉÏ¶ÔµÇÂ½°ü½âÃÜÍê³É

//	FrontLoginResult front_result;
//	char random_key_str[2 * CLIENT_PASSWD_LENGTH] = {0};
//	int len = 0;
//	HexString::bin2asc(random_key, 
//	CLIENT_PASSWD_LENGTH, random_key_str, 2 * CLIENT_PASSWD_LENGTH, len);
//	if (2 * CLIENT_PASSWD_LENGTH != len) {
//		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
//		LOG(ERROR)("randomkey convert error.");
//		return ret;
//	}
//	front_result.key_ = string(random_key_str, 2*CLIENT_PASSWD_LENGTH);
	
//	ONCE_LOOP_ENTER

    // ½âĞ­Òé°ü
	FrontLoginRequest front_request(inpkg_);
	ret = front_request.decode();
	if (0 != ret) {
		LOG(ERROR)("LoginCmd decode request failed.");
	    FrontLoginResult front_result;
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
        front_result.set_trans_id(front_request.trans_id());
		front_result.ret_msg_ = "inpkg data format is not match, LoginCmd decode request failed";
		front_result.set_head(front_request.get_head());
        front_result.encode();
        client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
        client_session->destory_connect();
        return 0;
	}

	// do something
	string bin_passwd("");
    ascii2bin(front_request.passwd_, bin_passwd);
	client_session->passwd(bin_passwd);
    uint32_t loginseq = client_session->login_seq();
    uint32_t transid = front_request.trans_id(); // ºóĞøÓÉ¿Í»§¶ËÉÏ´«

    // ·¢ÍùÓÃ»§ÖĞĞÄ£¬Ğ£ÑéÓÃ»§
    BackendLoginRequest back_login_request;
    back_login_request.set_token(front_request.devid_);
    back_login_request.set_device_type(0);
    back_login_request.set_passwd(bin_passwd);
    back_login_request.set_condid(utils::ServerConfig::Instance()->cond_id());
    back_login_request.set_loginseq(loginseq);
    back_login_request.set_trans_id(transid);
    back_login_request.set_devid(front_request.devid_);

    COHEADER coheader;
    coheader.seq = cmhdr.seq;
    coheader.cmd = cmhdr.cmd;
    coheader.head_len = sizeof(COHEADER);
    coheader.uid = 0;
    back_login_request.set_head(coheader);
    back_login_request.encode();
    LOG(DEBUG)("send login request to usercenter server, logseq:%u. [%s]"
        , client_session->login_seq(), coheader.print());
    LOG_HEX(back_login_request.data(), back_login_request.byte_size(), utils::L_DEBUG);
    UserCenterSession::SendToUserCenter(loginseq, back_login_request.data(), back_login_request.byte_size());
    client_session->set_client_version(front_request.version_);
    client_session->set_device_type(front_request.devtype_);
    client_session->set_device_id(front_request.devid_);
    ServerApp::Instance()->add_to_sign_list(client_session);
    
//	front_result.ret_code_ = 0;
//	front_result.ret_msg_ = "log ok!";
//	front_result.uid_ = 12345678;
//	front_result.time_ = time(NULL);
//	front_result.set_head(front_request.get_head());
//	ONCE_LOOP_LEAVE

	//front_result;
//	ret = front_result.encode();
//	if (0 != ret) {
//		LOG(ERROR)("LoginCmd encode result failed. ret:%d", ret);
//		return ret;
//	}

//	LOG(DEBUG)("show client result head [%s]", front_result.get_head().print());
//	// åŠ å¯†
//	ret = client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
//	// ä¸åŠ å¯†
//	if (0 != ret) {
//		LOG(ERROR)("LoginCmd send response failed. ret:%d", ret);
//		return ret;
//	}
//	LOG(INFO)("done login request. ret code:%d", ret);
	return ret;
}


int BackLoginCmd::reply_client(ClientSession *session, FrontLoginResult &log_result)
{
    int ret = log_result.encode();

    if (0 != ret)
    {
        LOG(ERROR)("encode result failed. login failed.");
        return ret;
    }

    LOG(INFO)("[%u] login reply client, login seq:%u.", log_result.uid_, session->login_seq());
    return session->send_msg_to_client(log_result.data(), log_result.byte_size());
}

int BackLoginCmd::execute(void *)
{
    COHEADER coheader;
    inpkg_.get_head(coheader);

    // 1. ½â°ü
	BackendLoginResult backend_login_result(inpkg_);
    FrontLoginResult front_login_result;
    int ret = backend_login_result.decode();
    if (0 != ret)
    {
        LOG(ERROR)("login result decode failed. [%s]", coheader.print());
        return 0;
    }

    ret = backend_login_result.ret_;
    const string &err_msg = backend_login_result.err_msg_;
    uint32_t login_seq = backend_login_result.loginseq_;
    uint32_t uid = backend_login_result.uid_;
    uint32_t transid = backend_login_result.trans_id();
    HEADER head;
    head.cmd = coheader.cmd;
    head.seq = coheader.seq;
    head.head_len = sizeof(HEADER);
    head.uid = uid;
    front_login_result.set_head(head);

    // 2. »ñÈ¡session
    ClientSession *client_session = ServerApp::Instance()->get_from_sign_list(login_seq);
	if (NULL == client_session) {
		LOG(ERROR)("client session not exist, maybe login timeout. [%s]", coheader.print());
		return -1;
	}

    // 3. Éú³Érandom key
    LOG(INFO)("check user info from usercenter server succeed, [%s]", coheader.print());
	char random_key[CLIENT_PASSWD_LENGTH] = {0};
	MakeKeyRand(random_key);
	client_session->session_key(random_key, CLIENT_PASSWD_LENGTH);
    LOG(DEBUG)("show session key:");
    LOG_HEX(client_session->session_key(), CLIENT_PASSWD_LENGTH, utils::L_DEBUG);
    string random_key_bin_str(random_key, CLIENT_PASSWD_LENGTH);
    string random_key_ascii_str("");
    bin2ascii(random_key_bin_str, random_key_ascii_str);

    // 4. »Ø¸´¿Í»§¶ËµÇÂ¼×´Ì¬
    front_login_result.ret_code_ = ret;
    front_login_result.ret_msg_ = err_msg;
    front_login_result.uid_ = uid;
    front_login_result.key_ = random_key_ascii_str;
    front_login_result.set_trans_id(transid);
    ret = reply_client(client_session, front_login_result);

    client_session->set_status((uint8_t)FRONT_ONLINE_STATUS);
    client_session->set_uid(uid);

    if (0 == ret) 
    {
        // 5. µÇÂ¼£¬ºó¶Ë¼ì²âÍ¨¹ı£¬¾Í½«session·Åµ½uid sessionÁĞ±íÖĞ£¬Í¬Ê±½«seqÁĞ±íÉ¾³ı¸ÃÓÃ»§
        ServerApp::Instance()->remove_from_sign_list(client_session);
        ServerApp::Instance()->add_uid_session(client_session);
        LOG(INFO)("user login succeed. uid:%d, login seq:%d.", uid, login_seq);
    } else {
        LOG(ERROR)("reply client login failed.");
    }
    return 0;
}




// =========================
// ä¸‹çº¿åè®®å¤„ç†ï¼Œcmd=1001
// =========================
int LogoutCmd::execute(void *)
{
	LOG(WARN)("LogoutCmd not support.");
	return 0;
}


// =========================
// åŒæ­¥æ•°æ®åè®®ï¼Œcmd=2000
// =========================
int SyncDataCmd::execute(void * )
{
	ClientSession *client_session = dynamic_cast<ClientSession *>(session_);
	if (NULL == client_session) {
		LOG(ERROR)("SyncDataCmd, client session cast failed.");
		return -1;
	}
	
	int ret = 0;
	FrontSyncRequest front_request(inpkg_);
	FrontSyncResult front_result;
	ONCE_LOOP_ENTER
	ret = front_request.decode();
    m_cmhdr = front_request.get_head();
	if (0 != ret) {
		LOG(ERROR)("SyncDataCmd decode request failed.");
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
		front_result.ret_msg_ = "inpkg data format is not match, SyncDataCmd decode request failed";
		front_result.set_head(front_request.get_head());
		break;
	}

    // transid¸³Öµ
    transid = front_request.trans_id();
    const map<uint32_t, uint64_t> &sync_item = front_request.sync_items_;
    map<uint32_t, uint64_t>::const_iterator it_ftask = sync_item.find(SYNC_FUTUTE_TASK);
    if (it_ftask != sync_item.end())
    {
        sync_task(it_ftask->second, SYNC_FUTUTE_TASK);
    }
    map<uint32_t, uint64_t>::const_iterator it_htask = sync_item.find(SYNC_HISTORY_TASK);
    if (it_htask != sync_item.end())
    {
        sync_task(it_htask->second, SYNC_HISTORY_TASK);
    }
    map<uint32_t, uint64_t>::const_iterator it_income = sync_item.find(SYNC_INCOME);
    if (it_income != sync_item.end())
    {
        sync_income(it_income->second);
    }
//	front_result.ret_code_ = ERR_SUCCESS;
//	front_result.ret_msg_ = "sync success.";
//	front_result.set_head(front_request.get_head());
	ONCE_LOOP_LEAVE

//	//front_result;
//	ret = front_result.encode();
//	if (0 != ret) {
//		LOG(ERROR)("SyncDataCmd encode result failed. ret:%d", ret);
//		return ret;
//	}

//	LOG(DEBUG)("show client result head [%s]", front_result.get_head().print());
//	// åŠ å¯†
//	ret = client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
//	if (0 != ret) {
//		LOG(ERROR)("SyncDataCmd send response failed. ret:%d", ret);
//		return ret;
//	}
	LOG(INFO)("done sync request. ret code:%d", ret);
	return ret;
}

// => taskcenter
// SyncTaskRequest
// ÈÎÎñ´Ótaskcenter»ñÈ¡
int SyncDataCmd::sync_task(const uint64_t &point, uint32_t sync_type)
{
    int ret = 0;
    COHEADER coheader;
    coheader.cmd = m_cmhdr.cmd;
    coheader.seq = m_cmhdr.seq;
    coheader.head_len = sizeof(COHEADER);
    coheader.uid = m_cmhdr.uid;

    BackendSyncRequest sync_request;
    sync_request.set_syncpoint(point);
    sync_request.set_synctype(sync_type);
    sync_request.set_synclimit(SYNC_NUM_LIMIT);
    sync_request.set_trans_id(transid);
    sync_request.set_head(coheader);

    ret = sync_request.encode();
    if (0 != ret)
    {
        LOG(ERROR)("sync_task encode backend request failed. [%s]", coheader.print());
        return ret;
    }

    ret = TaskCenterSession::SendToTaskCenter(coheader.uid, sync_request.data(), sync_request.byte_size());
    return ret;
}


// ²Æ¸»Ö±½Ó´Ódb»ñÈ¡
int SyncDataCmd::sync_income(const uint64_t &point)
{
    (void)point;
    int ret = 0;
    COHEADER coheader;
    coheader.cmd = CMD_INNER_GET_INCOME;
    coheader.seq = m_cmhdr.seq;
    coheader.head_len = sizeof(COHEADER);
    coheader.uid = m_cmhdr.uid;

    BackendIncomeRequest income_request;
    income_request.set_trans_id(transid);
    income_request.set_head(coheader);

    ret = income_request.encode();
    if (0 != ret)
    {
        LOG(ERROR)("sync_income encode backend request failed. [%s]", coheader.print());
        return ret;
    }

    ret = DbpSession::SendToDbp(income_request.data(), income_request.byte_size());
    return ret;
}



int SyncDataAckCmd::execute(void *)
{
	ClientSession *client_session = dynamic_cast<ClientSession *>(session_);
	if (NULL == client_session) {
		LOG(ERROR)("SyncDataAckCmd, client session cast failed.");
		return -1;
	}
	
	int ret = 0;
	FrontSyncAckRequest front_request(inpkg_);
	ONCE_LOOP_ENTER
	ret = front_request.decode();
    m_cmhdr = front_request.get_head();
	if (0 != ret) {
		LOG(ERROR)("SyncDataAckCmd decode request failed.");
		break;
	}

    NotifyTaskCenter(front_request);

    ONCE_LOOP_LEAVE

    return 0;
}

int SyncDataAckCmd::NotifyTaskCenter(FrontSyncAckRequest &front_request)
{
    BackAckRequest backend_request;
    COHEADER coheader;
    coheader.cmd = m_cmhdr.cmd;
    coheader.seq = m_cmhdr.seq;
    coheader.head_len = sizeof(COHEADER);
    coheader.uid = m_cmhdr.uid;
    backend_request.set_head(coheader);
    backend_request.set_type(SYNC_FUTUTE_TASK);
    backend_request.set_trans_id(front_request.trans_id());
    backend_request.set_time(front_request.time());
    vector<uint64_t> &ids = front_request.ack_items_;
    uint32_t size = (uint32_t)ids.size();
    for (uint32_t i = 0; i < size; ++i)
    {
        if (0 != ids[i]) {
            backend_request.add_id(ids[i]);
        }
    }

    int ret = backend_request.encode();
    if (0 != ret)
    {
        LOG(ERROR)("sync data ack encode backend request failed. [%s]", coheader.print());
        return ret;
    }

    ret = TaskCenterSession::SendToTaskCenter(coheader.uid, backend_request.data(), backend_request.byte_size());
    return 0;
}


int ReportDataCmd::execute(void *)
{
	ClientSession *client_session = dynamic_cast<ClientSession *>(session_);
	if (NULL == client_session) {
		LOG(ERROR)("ReportDataCmd, client session cast failed.");
		return -1;
	}
	
	int ret = 0;
	FrontReportRequest front_request(inpkg_);
    BackReportRequest backend_request;
    COHEADER coheader;
	ONCE_LOOP_ENTER
	ret = front_request.decode();
    m_cmhdr = front_request.get_head();
	if (0 != ret) {
		LOG(ERROR)("ReportDataCmd decode request failed. inpkg is error.");
    	FrontReportResult front_result;
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
		front_result.ret_msg_ = "ReportDataCmd, inpkg data format is not match, decode request failed";
		front_result.set_head(m_cmhdr);
        front_result.encode();
	    client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
		return ret;
	}

    coheader.cmd = m_cmhdr.cmd;
    coheader.seq = m_cmhdr.seq;
    coheader.head_len = sizeof(COHEADER);
    coheader.uid = m_cmhdr.uid;

    backend_request.set_task_id(front_request.task_id_);
    backend_request.set_task_type(front_request.task_type_);
    //backend_request.set_task_total_step(front_request.task_total_step_);
    backend_request.set_task_curr_step(front_request.task_curr_step_);
    backend_request.set_trans_id(front_request.trans_id());
    backend_request.set_head(coheader);

	ONCE_LOOP_LEAVE

	//backend_request;
	ret = backend_request.encode();
	if (0 != ret) {
		LOG(ERROR)("ReportDataCmd encode backend request failed. ret:%d", ret);
		return ret;
	}

    ret = TaskCenterSession::SendToTaskCenter(coheader.uid, backend_request.data(), backend_request.byte_size());
	if (0 != ret) {
		LOG(ERROR)("ReportDataCmd send backend request failed. ret:%d", ret);
		return ret;
	}
	LOG(INFO)("done report task data request. ret code:%d", ret);
	return ret;
}

/**
// Í¸´«µ½taskcenter
// ÕâÌõĞ­ÒéµÄÄ¿µÄÊÇ¼ì²âÈÎÎñ×´Ì¬ÒÔ¼°ĞŞ¸ÄÈÎÎñ¿ÉÓÃÊı£¬
// Èç¹ûÊÇÊ×´ÎÆô¶¯ÈÎÎñ£¬¾ÍĞ£ÑéÈÎÎñ×´Ì¬£¬
// ²¢ÇÒÔÚÈÎÎñ×´Ì¬okµÄÇé¿öÏÂ½«ÈÎÎñ³ØÖĞµÄ±¾ÈÎÎñµÄ¿É
// ÓÃÈÎÎñÊı¼õÒ»£¬·´Ö®Ö®Ç°ÒÑ¾­Æô¶¯¹ı£¬Ôò½ö½öÊÇ¼ìÑé
// ÈÎÎñ×´Ì¬¡£
**/
int StartTaskCmd::execute(void *)
{
	ClientSession *client_session = dynamic_cast<ClientSession *>(session_);
	if (NULL == client_session) {
		LOG(ERROR)("StartTaskCmd, client session cast failed.");
		return -1;
	}
	
	int ret = 0;
	FrontStartTaskRequest front_request(inpkg_);
    BackendStartTaskRequest backend_request;
    COHEADER coheader;
	ONCE_LOOP_ENTER
	ret = front_request.decode();
    m_cmhdr = front_request.get_head();
	if (0 != ret) {
		LOG(ERROR)("StartTaskCmd decode request failed. inpkg is error.");
    	FrontStartTaskResult front_result;
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
        front_result.set_trans_id(front_request.task_id_);
		front_result.ret_msg_ = "StartTaskCmd, inpkg data format is not match, decode request failed";
		front_result.set_head(m_cmhdr);
        front_result.encode();
	    client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
		return ret;
	}

    coheader.cmd = m_cmhdr.cmd;
    coheader.seq = m_cmhdr.seq;
    coheader.head_len = sizeof(COHEADER);
    coheader.uid = m_cmhdr.uid;

    SrvInnerCommonMsg &common_msg = backend_request.mutable_msg();
    common_msg.AddTagValue(TASK_ID, front_request.task_id_);
    common_msg.AddTagValue(TASK_TYPE, front_request.task_type_);
    common_msg.AddTagValue(TASK_URL, front_request.task_url_);
    common_msg.SetTransid(front_request.trans_id());
    common_msg.SetTime(front_request.time());
    backend_request.set_head(coheader);

	ONCE_LOOP_LEAVE

	ret = backend_request.encode();
	if (0 != ret) {
		LOG(ERROR)("StartTaskCmd encode backend request failed. ret:%d", ret);
		return ret;
	}

    ret = TaskCenterSession::SendToTaskCenter(coheader.uid, backend_request.data(), backend_request.byte_size());
	if (0 != ret) {
		LOG(ERROR)("StartTaskCmd send backend request failed. ret:%d", ret);
		return ret;
	}
	LOG(INFO)("done start task request. ret code:%d", ret);
	return ret;
}


int ThirdPartyLoginCmd::execute(void *)
{
	ClientSession *client_session = dynamic_cast<ClientSession *>(session_);
	if (NULL == client_session) {
		LOG(ERROR)("ThirdPartyLoginCmd, client session cast failed.");
		return -1;
	}

	int ret = 0;
	FrontThirdPartyLoginRequest front_request(inpkg_);
    BackendThirdPartyLoginRequest backend_request;
    COHEADER coheader;
	ONCE_LOOP_ENTER
	ret = front_request.decode();
    m_cmhdr = front_request.get_head();
    LOG(INFO)("user request thirdparty login, [%s]", m_cmhdr.print());
	if (0 != ret) {
		LOG(ERROR)("ThirdPartyLoginCmd decode request failed. inpkg is error. [%s]", m_cmhdr.print());
    	FrontThirdPartyLoginResult front_result;
        front_result.set_trans_id(front_request.trans_id());
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
		front_result.set_ret_msg("ThirdPartyLoginCmd, inpkg data format is not match, decode request failed");
		front_result.set_head(m_cmhdr);
        front_result.encode();
	    client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
		return ret;
	}

    coheader.cmd = m_cmhdr.cmd;
    coheader.seq = m_cmhdr.seq;
    coheader.head_len = sizeof(COHEADER);
    coheader.uid = m_cmhdr.uid;

    SrvLoginRequest &login_pb_msg = backend_request.mutable_msg();
    login_pb_msg.set_account(front_request.login_openid_);
    login_pb_msg.set_passwd(front_request.login_passwd_);
    login_pb_msg.set_accounttype(front_request.login_type_);
    login_pb_msg.set_nickname(front_request.login_nick_name_);
    login_pb_msg.set_avatar(front_request.login_avatar_);
    login_pb_msg.set_city(front_request.login_province_ + ", " + front_request.login_city_);
    login_pb_msg.set_country(front_request.login_country_);
    login_pb_msg.set_desc(front_request.login_desc_);
    login_pb_msg.set_email(front_request.login_email_);
    login_pb_msg.set_mobile(front_request.login_phone_);
    login_pb_msg.set_gender(front_request.login_sex_);
    login_pb_msg.set_token(front_request.login_token_);
    login_pb_msg.set_flag(front_request.login_flag_);
    login_pb_msg.set_transid(front_request.trans_id());
    login_pb_msg.set_time(front_request.time());
    backend_request.set_head(coheader);

	ONCE_LOOP_LEAVE

	ret = backend_request.encode();
	if (0 != ret) {
		LOG(ERROR)("ThirdPartyLoginCmd encode backend request failed. ret:%d", ret);
		return ret;
	}

    LOG(INFO)("send thirdparty login request to dbp, [%s]", coheader.print());
    ret = DbpSession::SendToDbp(backend_request.data(), backend_request.byte_size());
	if (0 != ret) {
		LOG(ERROR)("ThirdPartyLoginCmd send backend request failed. ret:%d", ret);
		return ret;
	}
	LOG(INFO)("done thirdparty login request. ret code:%d, [%s]", ret, m_cmhdr.print());
	return ret;
}



int WithdrawCmd::execute(void *)
{
	ClientSession *client_session = dynamic_cast<ClientSession *>(session_);
	if (NULL == client_session) {
		LOG(ERROR)("WithdrawCmd, client session cast failed.");
		return -1;
	}

	int ret = 0;
	FrontWithdrawRequest front_request(inpkg_);
    BackendWithdrawRequest backend_request;
    COHEADER coheader;
	ONCE_LOOP_ENTER
	ret = front_request.decode();
    m_cmhdr = front_request.get_head();
    LOG(INFO)("user request withdraw, [%s]", m_cmhdr.print());
	if (0 != ret) {
		LOG(ERROR)("WithdrawCmd decode request failed. inpkg is error. [%s]", m_cmhdr.print());
    	FrontWithdrawResult front_result;
        front_result.set_trans_id(front_request.trans_id());
		front_result.set_trade_no("");
		front_result.set_withdraw_type(front_request.withdraw_type());
		front_result.set_withdraw_cash(0);
        front_result.ret_code_ = PARAM_ERROR;
        front_result.ret_msg_ = "decode withdraw param is error.";
        front_result.encode();
	    client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
		return ret;
	}

    SrvWithdrawRequest &pb_request = backend_request.mutable_msg();
    pb_request.set_type(front_request.withdraw_type());
    pb_request.set_withdraw_cash(front_request.withdraw_cash());
    pb_request.set_id(front_request.trade_id());
    pb_request.set_open_id(front_request.open_id());
    pb_request.set_user_name(front_request.user_name());
    pb_request.set_desc(front_request.desc());
    pb_request.set_time(front_request.time());
    pb_request.set_transid(front_request.trans_id());

    coheader.cmd = m_cmhdr.cmd;
    coheader.seq = m_cmhdr.seq;
    coheader.head_len = sizeof(COHEADER);
    coheader.uid = m_cmhdr.uid;

	ONCE_LOOP_LEAVE

	ret = backend_request.encode();
	if (0 != ret) {
		LOG(ERROR)("WithdrawCmd encode backend request failed. ret:%d", ret);
		return ret;
	}

    LOG(INFO)("send withdraw request to usercenter, [%s]", coheader.print());
    ret = UserCenterSession::SendToUserCenter(coheader.uid, backend_request.data(), backend_request.byte_size());
	if (0 != ret) {
		LOG(ERROR)("WithdrawCmd send backend request failed. ret:%d", ret);
		return ret;
	}
	LOG(INFO)("done backend withdraw request. ret code:%d, [%s]", ret, m_cmhdr.print());
	return 0;
}

int BackGetIncomeCmd::execute(void *)
{
    COHEADER coheader;
    inpkg_.get_head(coheader);

    // 1. ½â°ü
	BackendIncomeResult backend_result(inpkg_);
    FrontIncomeResult front_result;
    int ret = backend_result.decode();
    if (0 != ret)
    {
        LOG(ERROR)("get income result decode failed. [%s]", coheader.print());
        return 0;
    }

    HEADER head;
    head.cmd = CMD_SYNC_DATA;
    head.seq = coheader.seq;
    head.head_len = sizeof(HEADER);
    head.uid = coheader.uid;
    front_result.set_head(head);

    // 2. »ñÈ¡session
    ClientSession *client_session = ServerApp::Instance()->get_uid_session(head.uid);
	if (NULL == client_session) {
		LOG(WARN)("client session not exist, maybe request income timeout. [%s]", coheader.print());
		return -1;
	}

    front_result.set_ret_value(backend_result.ret_value_);
    front_result.set_err_msg(backend_result.err_msg_);
    front_result.set_pre_cash(backend_result.pre_cash_);
    front_result.set_useable_cash(backend_result.useable_cash_);
    front_result.set_fetched_cash(backend_result.fetched_cash_);
    front_result.set_trans_id(backend_result.trans_id());
    
    // 4. »Ø¸´¿Í»§¶Ë
    LOG(INFO)("reply client, [%s]", head.print());
    ret = reply_client(client_session, front_result);

    return 0;
}

int BackGetIncomeCmd::reply_client(ClientSession *session, FrontIncomeResult &front_result)
{
    int ret = front_result.encode();

    if (0 != ret)
    {
        LOG(ERROR)("encode result failed. get income failed.");
        return ret;
    }

    LOG(DEBUG)("get income reply client.");
    return session->send_msg_to_client(front_result.data(), front_result.byte_size());
}

int BackGetTaskCmd::execute(void * )
{
    COHEADER coheader;
    inpkg_.get_head(coheader);

    // 1. ½â°ü
	BackendSyncResult backend_result(inpkg_);
    FrontSyncResult front_result;
    int ret = backend_result.decode();
    if (0 != ret)
    {
        LOG(ERROR)("sync task result decode failed. [%s]", coheader.print());
        return 0;
    }

    HEADER head;
    head.cmd = CMD_SYNC_DATA;
    head.seq = coheader.seq;
    head.head_len = sizeof(HEADER);
    head.uid = coheader.uid;
    front_result.set_head(head);

    // 2. »ñÈ¡session
    ClientSession *client_session = ServerApp::Instance()->get_uid_session(head.uid);
	if (NULL == client_session) {
		LOG(WARN)("client session not exist, maybe sync task timeout. [%s]", coheader.print());
		return -1;
	}

    front_result.set_ret_value(backend_result.ret_value_);
    front_result.set_err_msg(backend_result.err_msg_);
    front_result.set_trans_id(backend_result.trans_id());
	front_result.set_bus_type(backend_result.synctype_);
	front_result.set_continue_flag(backend_result.continueflag_);
    front_result.set_sync_value(backend_result.maxtaskid_);
    front_result.set_task_infos(backend_result.task_vec_);
    
    // 4. »Ø¸´¿Í»§¶Ë
    LOG(INFO)("reply client, [%s]", head.print());
    ret = reply_client(client_session, front_result);

    return 0;
}

int BackGetTaskCmd::reply_client(ClientSession *session, FrontSyncResult &front_result)
{
    int ret = front_result.encode();

    if (0 != ret)
    {
        LOG(ERROR)("encode result failed. sync task failed.");
        return ret;
    }

    LOG(DEBUG)("sync task reply client.");
    return session->send_msg_to_client(front_result.data(), front_result.byte_size());
}



int BackStartTaskCmd::execute(void *arg)
{
    COHEADER coheader;
    inpkg_.get_head(coheader);

    // 1. ½â°ü
	BackendStartTaskResult backend_result(inpkg_);
    FrontStartTaskResult front_result;
    int ret = backend_result.decode();
    if (0 != ret)
    {
        LOG(ERROR)("start task result decode failed. [%s]", coheader.print());
        return 0;
    }

    HEADER head;
    head.cmd = CMD_START_TASK;
    head.seq = coheader.seq;
    head.head_len = sizeof(HEADER);
    head.uid = coheader.uid;
    front_result.set_head(head);

    // 2. »ñÈ¡session
    ClientSession *client_session = ServerApp::Instance()->get_uid_session(head.uid);
	if (NULL == client_session) {
		LOG(WARN)("client session not exist, maybe request income timeout. [%s]", coheader.print());
		return -1;
	}

    front_result.ret_code_ = backend_result.ret_value();
    front_result.ret_msg_ = backend_result.ret_msg();
    front_result.task_id_ = backend_result.task_id();
    front_result.task_type_ = backend_result.task_type();
    front_result.set_trans_id(backend_result.trans_id());
    
    // 4. »Ø¸´¿Í»§¶Ë
    LOG(INFO)("reply client, [%s]", head.print());
    ret = reply_client(client_session, front_result);

    return 0;
}

int BackStartTaskCmd::reply_client(ClientSession *session, FrontStartTaskResult &front_result)
{
    int ret = front_result.encode();

    if (0 != ret)
    {
        LOG(ERROR)("encode result failed. start task failed.");
        return ret;
    }

    LOG(DEBUG)("start task reply client.");
    return session->send_msg_to_client(front_result.data(), front_result.byte_size());
}


int BackThirdPartyLoginCmd::execute(void *arg )
{
    COHEADER coheader;
    inpkg_.get_head(coheader);

    // 1. ½â°ü
	BackendThirdPartyLoginResult backend_result(inpkg_);
    FrontThirdPartyLoginResult front_result;
    int ret = backend_result.decode();
    if (0 != ret)
    {
        LOG(ERROR)("thirdparty login result decode failed. [%s]", coheader.print());
        return 0;
    }

    HEADER head;
    head.cmd = coheader.cmd;
    head.seq = coheader.seq;
    head.head_len = sizeof(HEADER);
    head.uid = coheader.uid;
    front_result.set_head(head);

    // 2. »ñÈ¡session
    ClientSession *client_session = ServerApp::Instance()->get_uid_session(head.uid);
	if (NULL == client_session) {
		LOG(WARN)("client session not exist, maybe request timeout. [%s]", coheader.print());
		return -1;
	}

    front_result.set_ret_value(backend_result.ret_value());
    front_result.set_ret_msg(backend_result.ret_msg());
    front_result.set_type(backend_result.type());
    front_result.set_trans_id(backend_result.trans_id());
    
    // 4. »Ø¸´¿Í»§¶Ë
    LOG(INFO)("reply client, [%s]", head.print());
    ret = reply_client(client_session, front_result);

    return 0;
}


int BackThirdPartyLoginCmd::reply_client(ClientSession *session, FrontThirdPartyLoginResult &front_result)
{
    int ret = front_result.encode();

    if (0 != ret)
    {
        LOG(ERROR)("encode result failed. get income failed.");
        return ret;
    }

    LOG(DEBUG)("thirdparty login reply client.");
    return session->send_msg_to_client(front_result.data(), front_result.byte_size());
}

int BackWithdrawCmd::execute(void *)
{
    COHEADER coheader;
    inpkg_.get_head(coheader);

    // 1. ½â°ü
	BackendWithdrawResult backend_result(inpkg_);
    FrontWithdrawResult front_result;
    int ret = backend_result.decode();
    if (0 != ret)
    {
        LOG(ERROR)("withdraw result decode failed. [%s]", coheader.print());
        return 0;
    }

    HEADER head;
    head.cmd = coheader.cmd;
    head.seq = coheader.seq;
    head.head_len = sizeof(HEADER);
    head.uid = coheader.uid;
    front_result.set_head(head);

    // 2. »ñÈ¡session
    ClientSession *client_session = ServerApp::Instance()->get_uid_session(head.uid);
	if (NULL == client_session) {
		LOG(WARN)("client session not exist, maybe request timeout. [%s]", coheader.print());
		return -1;
	}

    front_result.set_ret_value(backend_result.ret_value());
    front_result.set_ret_msg(backend_result.ret_msg());
    front_result.set_trade_no(backend_result.trade_no());
    front_result.set_withdraw_type(backend_result.withdraw_type());
    front_result.set_withdraw_cash(backend_result.withdraw_cash());
    front_result.set_trans_id(backend_result.trans_id());
    
    // 4. »Ø¸´¿Í»§¶Ë
    LOG(INFO)("reply client, [%s]", head.print());
    ret = reply_client(client_session, front_result);

    return 0;
}


int BackWithdrawCmd::reply_client(ClientSession *session, FrontWithdrawResult &front_result)
{
    int ret = front_result.encode();

    if (0 != ret)
    {
        LOG(ERROR)("encode result failed. withdraw failed.");
        return ret;
    }

    LOG(DEBUG)("withdraw reply client.");
    return session->send_msg_to_client(front_result.data(), front_result.byte_size());
}

