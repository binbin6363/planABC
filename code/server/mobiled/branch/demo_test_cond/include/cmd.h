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

class ClientSession;
class FrontLoginResult;
class FrontIncomeResult;
class FrontSyncResult;

// =========================
// base cmd
// =========================
class Cmd
{
public:
    Cmd(Net_Session *session, BinInputPacket<> &inpkg);
    virtual ~Cmd();
    
    virtual int execute(void *arg = NULL) = 0;
	
protected:
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

class LcsBeatCmd : public Cmd
{
public:
	LcsBeatCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~LcsBeatCmd()
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
// 登录协议处理，cmd=1000
// =========================
class BackLoginCmd : public Cmd
{
public:
	BackLoginCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~BackLoginCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);

private:
	int reply_client(ClientSession *session, FrontLoginResult &log_result);
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

// =========================
// 同步数据协议，cmd=2000
// =========================
class SyncDataCmd : public Cmd
{
public:
	SyncDataCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~SyncDataCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);
	int sync_task(const uint64_t &point, uint32_t sync_type);
	int sync_income(const uint64_t &point);

private:
	HEADER m_cmhdr;
	uint32_t transid;
};

// =========================
// 同步数据协议的ack，cmd=2001
// =========================
class FrontSyncAckRequest;
class SyncDataAckCmd : public Cmd
{
public:
	SyncDataAckCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~SyncDataAckCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);
	int NotifyTaskCenter(FrontSyncAckRequest &front_request);

private:
	HEADER m_cmhdr;
	uint32_t transid;
};


// =========================
// 上报数据协议，cmd=3001
// =========================
class ReportDataCmd : public Cmd
{
public:
	ReportDataCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~ReportDataCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);

private:
	HEADER m_cmhdr;
	uint32_t transid;
};

// =========================
// 启动任务，cmd=4000
// =========================
class StartTaskCmd : public Cmd
{
public:
	StartTaskCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~StartTaskCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);

private:
	HEADER m_cmhdr;
};

// 使用第三方账户登陆本服务，类似于绑定第三方信息
// 直接转发请求到dbp，验证并绑定第三方信息
class ThirdPartyLoginCmd : public Cmd
{
public:
	ThirdPartyLoginCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~ThirdPartyLoginCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);

private:
	HEADER m_cmhdr;
};

// 提现协议
class WithdrawCmd : public Cmd
{
public:
	WithdrawCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~WithdrawCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);

private:
	HEADER m_cmhdr;
};


class BackGetIncomeCmd : public Cmd
{
public:
	BackGetIncomeCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~BackGetIncomeCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);
	int reply_client(ClientSession *session, FrontIncomeResult &front_result);

};


class BackGetTaskCmd : public Cmd
{
public:
	BackGetTaskCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~BackGetTaskCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);
	int reply_client(ClientSession *session, FrontSyncResult &front_result);

};

class FrontStartTaskResult;
class BackStartTaskCmd : public Cmd
{
public:
	BackStartTaskCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~BackStartTaskCmd()
	{
	}
	
	virtual int execute(void *arg = NULL);
	int reply_client(ClientSession *session, FrontStartTaskResult &front_result);

};


class FrontThirdPartyLoginResult;
class BackThirdPartyLoginCmd : public Cmd
{
public:
	BackThirdPartyLoginCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~BackThirdPartyLoginCmd()
	{
	}
	
	virtual int execute(void *arg = NULL);
	int reply_client(ClientSession *session, FrontThirdPartyLoginResult &front_result);
};


class FrontWithdrawResult;
class BackWithdrawCmd : public Cmd
{
public:
	BackWithdrawCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~BackWithdrawCmd()
	{
	}
	
	virtual int execute(void *arg = NULL);
	int reply_client(ClientSession *session, FrontWithdrawResult &front_result);
};

#endif // _CMD_H_

