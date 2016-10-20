/**
 * @filedesc: 
 * cmd.h, handle command
 * @author: 
 *  bbwang
 * @date: 
 *  2015/11/13 20:02:59
 * @modify:
 *
**/
#ifndef _CMD_H_
#define _CMD_H_

#include "comm.h"
#include "data_type.h"
#include "binpacket_wrap.h"

using namespace utils;

class Net_Session;

// =========================
// base cmd
// =========================
class Cmd
{
public:
    Cmd(Net_Session *session, BinInputPacket<> &inpkg)
		: session_(session)
		, inpkg_(inpkg)
		, arg_(NULL)
		{}
    virtual ~Cmd()
    	{}
    
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
// 操作数据库，cmd = 10000
// =========================
class MysqlOperateCmd : public Cmd
{
public:
	MysqlOperateCmd(Net_Session* session, BinInputPacket<>& inpkg)
		: Cmd(session, inpkg)
	{
	}
	~MysqlOperateCmd()
	{
	}
	
    virtual int execute(void *arg = NULL);
};




#endif // _CMD_H_

