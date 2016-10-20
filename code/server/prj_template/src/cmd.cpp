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
	LOG(WARN)("BeatCmd not support.");
	return 0;
}


// =========================
// 登录协议处理，cmd=1000
// =========================
int LoginCmd::execute(void *arg)
{
	LOG(WARN)("LoginCmd not support.");
	return 0;
}


// =========================
// 下线协议处理，cmd=1001
// =========================
int LogoutCmd::execute(void *arg)
{
	LOG(WARN)("LogoutCmd not support.");
	return 0;
}



