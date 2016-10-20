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
#include "server_app.h"
#include "client_session.h"
#include "hex_string.h"

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

int CmdThread::execute(void *arg)
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
// 客户端的心跳，cmd=0
// =========================
int BeatCmd::execute(void *arg)
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
	// 加密
	ret = client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
	// 不加密
//	ret = client_session->send_msg(front_result.data(), front_result.byte_size());
	if (0 != ret) {
		LOG(ERROR)("LoginCmd send response failed. ret:%d", ret);
		return ret;
	}
	LOG(INFO)("done beat request. ret code:%d", ret);
	return ret;
}


// =========================
// 登录协议处理，cmd=1000
// =========================
int LoginCmd::execute(void *arg)
{
	ClientSession *client_session = dynamic_cast<ClientSession *>(session_);
	if (NULL == client_session) {
		LOG(ERROR)("client session cast failed.");
		return -1;
	}
	
	int ret = 0;
	char random_key[CLIENT_PASSWD_LENGTH] = {0};
	// TODO: for debug, ramdom key set permant value
	MakeKeyRand(random_key);
	client_session->session_key(random_key, CLIENT_PASSWD_LENGTH);
    LOG(DEBUG)("show session key:");
    LOG_HEX(client_session->session_key(), CLIENT_PASSWD_LENGTH, utils::L_DEBUG);

    HEADER cmhdr;
    inpkg_.get_head(cmhdr);
    char client_random_key[CLIENT_PASSWD_LENGTH] = {0};
    inpkg_.read(client_random_key, CLIENT_PASSWD_LENGTH);

    if (!inpkg_.good() || inpkg_.eof())
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

    LOG(DEBUG)("[login] before decrypt pkg body:");
    LOG_HEX(inpkg_.getCur(), cmhdr.len - sizeof(HEADER) - 
	CLIENT_PASSWD_LENGTH, utils::L_DEBUG);
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

	FrontLoginRequest front_request(inpkg_);
	FrontLoginResult front_result;
	char random_key_str[2 * CLIENT_PASSWD_LENGTH] = {0};
	int len = 0;
	HexString::bin2asc(random_key, 
	CLIENT_PASSWD_LENGTH, random_key_str, 2 * CLIENT_PASSWD_LENGTH, len);
	if (2 * CLIENT_PASSWD_LENGTH != len) {
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
		LOG(ERROR)("randomkey convert error.");
		return ret;
	}
	front_result.key_ = string(random_key_str, 2*CLIENT_PASSWD_LENGTH);
	
	ONCE_LOOP_ENTER
	ret = front_request.decode();
	if (0 != ret) {
		LOG(ERROR)("LoginCmd decode request failed.");
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
		front_result.ret_msg_ = "inpkg data format is not match, LoginCmd decode request failed";
		front_result.set_head(front_request.get_head());
		break;
	}

	// do something
	char pasd[CLIENT_PASSWD_LENGTH] = {0};
	HexString::asc2bin(front_request.passwd_.data(), 
	front_request.passwd_.size(), pasd, CLIENT_PASSWD_LENGTH, len);
	if (CLIENT_PASSWD_LENGTH != len) {
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
		LOG(ERROR)("passwd convert error. inpkg data passwd is not 32 length ascii char.");
		return ret;
	}
	client_session->passwd(string(pasd, CLIENT_PASSWD_LENGTH));
	front_result.ret_code_ = 0;
	front_result.ret_msg_ = "log ok!";
	front_result.uid_ = 12345678;
	front_result.time_ = time(NULL);
	front_result.set_head(front_request.get_head());
	ONCE_LOOP_LEAVE

	//front_result;
	ret = front_result.encode();
	if (0 != ret) {
		LOG(ERROR)("LoginCmd encode result failed. ret:%d", ret);
		return ret;
	}

	LOG(DEBUG)("show client result head [%s]", front_result.get_head().print());
	// 加密
	ret = client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
	// 不加密
	if (0 != ret) {
		LOG(ERROR)("LoginCmd send response failed. ret:%d", ret);
		return ret;
	}
	LOG(INFO)("done login request. ret code:%d", ret);
	return ret;
}


// =========================
// 下线协议处理，cmd=1001
// =========================
int LogoutCmd::execute(void *arg)
{
	LOG(WARN)("LogoutCmd not support.");
	return 0;
}


// =========================
// 同步数据协议，cmd=2000
// =========================
int SyncDataCmd::execute(void * arg)
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
	if (0 != ret) {
		LOG(ERROR)("SyncDataCmd decode request failed.");
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
		front_result.ret_msg_ = "inpkg data format is not match, SyncDataCmd decode request failed";
		front_result.set_head(front_request.get_head());
		break;
	}

	front_result.ret_code_ = ERR_SUCCESS;
	front_result.ret_msg_ = "sync success.";
	front_result.set_head(front_request.get_head());
	ONCE_LOOP_LEAVE

	//front_result;
	ret = front_result.encode();
	if (0 != ret) {
		LOG(ERROR)("SyncDataCmd encode result failed. ret:%d", ret);
		return ret;
	}

	LOG(DEBUG)("show client result head [%s]", front_result.get_head().print());
	// 加密
	ret = client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
	if (0 != ret) {
		LOG(ERROR)("SyncDataCmd send response failed. ret:%d", ret);
		return ret;
	}
	LOG(INFO)("done sync request. ret code:%d", ret);
	return ret;
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
	FrontReportResult front_result;
	ONCE_LOOP_ENTER
	ret = front_request.decode();
	if (0 != ret) {
		LOG(ERROR)("SyncDataCmd decode request failed.");
		front_result.ret_code_ = ERR_PROCOL_NOMATCH;
		front_result.ret_msg_ = "inpkg data format is not match, ReportDataCmd decode request failed";
		front_result.set_head(front_request.get_head());
		break;
	}

	front_result.ret_code_ = ERR_SUCCESS;
	front_result.ret_msg_ = "report task data success.";
	front_result.set_head(front_request.get_head());
	ONCE_LOOP_LEAVE

	//front_result;
	ret = front_result.encode();
	if (0 != ret) {
		LOG(ERROR)("ReportDataCmd encode result failed. ret:%d", ret);
		return ret;
	}

	LOG(DEBUG)("show client result head [%s]", front_result.get_head().print());
	ret = client_session->send_msg_to_client(front_result.data(), front_result.byte_size());
	if (0 != ret) {
		LOG(ERROR)("ReportDataCmd send response failed. ret:%d", ret);
		return ret;
	}
	LOG(INFO)("done report task data request. ret code:%d", ret);
	return ret;
}

