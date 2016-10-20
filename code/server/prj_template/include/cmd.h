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
// �����̳д���ģ�ֻ��ʵ��svc���ɣ�execute������Ų��svc��
// =========================
class CmdThread : public Task_Base
{
public:

    CmdThread(Net_Session* session, BinInputPacket<>& inpkg);
    virtual ~CmdThread();

    int execute(void *arg = NULL);

	//! �����߳�
	int start();
	//! ֹͣ�߳�
	int stop();
    bool is_stop(){return m_notify_stop;}
private:
    // start ֮ǰ���߳�����֮ǰ��������
    virtual int doPreStart();

	//! �̺߳���
	virtual int svc();

private:
	//! ����״̬
	bool m_is_run;

	//! ֹ֪ͣͨ
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
// �ͻ��˵�������cmd=0
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
// ��¼Э�鴦��cmd=1000
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
// ����Э�鴦��cmd=1001
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

