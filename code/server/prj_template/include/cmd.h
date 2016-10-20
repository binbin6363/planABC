/**
 * @filedesc: 
 * cmd.h, handle command
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/13 20:02:59
 * @modify:
 *
**/
#ifndef _CMD_H_
#define _CMD_H_

#include "headers.h"

using namespace utils;


// =========================
// base cmd
// =========================
class Cmd
{
public:
    Cmd(Net_Session *session, BinInputPacket<> &inpkg);
    virtual ~Cmd();
    
    virtual int execute(void *arg = NULL) = 0;
	
private:
    Net_Session *session_;
	BinInputPacket<> &inpkg_;
    void *arg_;
    
private:
    Cmd(const Cmd &);
    void operator=(const Cmd &);
};


// =========================
// base cmd which running in separete thread
// 后续继承此类的，只需实现svc即可，execute的内容挪到svc中
// =========================
class CmdThread : public Task_Base
{
public:

    CmdThread(Net_Session* session, BinInputPacket<>& inpkg);
    virtual ~CmdThread();

    int execute(void *arg = NULL);

	//! 启动线程
	int start();
	//! 停止线程
	int stop();
    bool is_stop(){return m_notify_stop;}
private:
    // start 之前，线程启动之前做的事情
    virtual int doPreStart();

	//! 线程函数
	virtual int svc();

private:
	//! 运行状态
	bool m_is_run;

	//! 停止通知
	volatile bool m_notify_stop;

private:
    CmdThread(const CmdThread &);
    void operator=(const CmdThread &);

protected:
	Net_Session *session_;
	BinInputPacket<> &inpkg_;
    void *arg_;

};



// =========================
// 客户端的心跳，cmd=0
// =========================
class BeatCmd : public Cmd
{
public:
	BeatCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~BeatCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);

};


// =========================
// 登录协议处理，cmd=1000
// =========================
class LoginCmd : public Cmd
{
public:
	LoginCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~LoginCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);

};


// =========================
// 下线协议处理，cmd=1001
// =========================
class LogoutCmd : public Cmd
{
public:
	LogoutCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~LogoutCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);

};


#endif // _CMD_H_

