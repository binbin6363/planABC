
#ifndef _LEVELDB_COMMNAD_H_
#define _LEVELDB_COMMNAD_H_
#include "comm.h"
#include "data_type.h"
#include "binpacket_wrap.h"

using namespace common;
using namespace utils;


class Net_Session;

class Command 
{
public:
	Command( Net_Session * pSession, BinInputPacket<>& inpkg )
	  : m_session(pSession), m_inpkg( inpkg )
	{ }

	virtual ~Command(){ }
	virtual bool execute() = 0;

public:
	Net_Session * m_session;
	BinInputPacket<>& m_inpkg;

	uint32_t m_unRet;
    string m_transfer_str;
};


class AliveCommand : public Command
{
public:
	AliveCommand(Net_Session* session,BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~AliveCommand(){}
	bool execute();
};

// msg content
class GetSingleChatMsg : public Command
{
public:
	GetSingleChatMsg(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~GetSingleChatMsg(){}
	bool execute();
};


class GetQgroupChatMsg : public Command
{
public:
	GetQgroupChatMsg(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~GetQgroupChatMsg(){}
	bool execute();
};


class GetNgroupChatMsg : public Command
{
public:
	GetNgroupChatMsg(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~GetNgroupChatMsg(){}
	bool execute();
};


class SetSingleChatMsg : public Command
{
public:
	SetSingleChatMsg(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket)
		, tuid_(0)
        , fcid_(0)
        , fuid_(0)
        , srvmsgid_(0)
        , msg_("")
        , msgtime_(0)
        {}
	~SetSingleChatMsg(){}
	bool execute();

private:
    int decode();

    uint32_t tuid_;
    uint32_t fcid_;
    uint32_t fuid_;
    uint64_t srvmsgid_;
    string   msg_;
    uint32_t msgtime_;

};


class SetNgroupChatMsg : public Command
{
public:
	SetNgroupChatMsg(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~SetNgroupChatMsg(){}
	bool execute();
private:
    int decode();

    uint32_t tngroupid_;
    uint32_t fcid_;
    uint32_t fuid_;
    uint64_t srvmsgid_;
    string   msg_;
    uint32_t msgtime_;
};


class SetQgroupChatMsg : public Command
{
public:
	SetQgroupChatMsg(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~SetQgroupChatMsg(){}
	bool execute();
private:
    int decode();

    uint32_t tqgroupid_;
    uint32_t fcid_;
    uint32_t fuid_;
    uint64_t srvmsgid_;
    string   msg_;
    uint32_t msgtime_;
};

// common cmd, put, get, del
class BatchSetValueCmd : public Command
{
public:
	BatchSetValueCmd(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~BatchSetValueCmd(){}
	bool execute();

};

class BatchGetValueCmd : public Command
{
public:
	BatchGetValueCmd(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~BatchGetValueCmd(){}
	bool execute();

};

class RangeGetValueCmd : public Command
{
public:
	RangeGetValueCmd(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~RangeGetValueCmd(){}
	bool execute();

};



class BatchDelCmd : public Command
{
public:
	BatchDelCmd(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~BatchDelCmd(){}
	bool execute();

};

// 先获取值，然后覆盖写入
class GetAndPutValueCmd : public Command
{
public:
	GetAndPutValueCmd(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~GetAndPutValueCmd(){}
	bool execute();

};

// mutlikey and same value
class BatchSetSameValueCmd : public Command
{
public:
	BatchSetSameValueCmd(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~BatchSetSameValueCmd(){}
	bool execute();

};

class RangeDelValueCmd : public Command
{
public:
    RangeDelValueCmd(Net_Session* session, BinInputPacket<>& inpacket)
        :Command(session,inpacket){}
    ~RangeDelValueCmd(){}
    bool execute();
};


class ReverseRangeGetValueCmd : public Command
{
public:
	ReverseRangeGetValueCmd(Net_Session* session, BinInputPacket<>& inpacket)
		:Command(session,inpacket){}
	~ReverseRangeGetValueCmd(){}
	bool execute();
};


class Record;
class Binlog;
class RecvSyncDataCmd : public Command
{
public:
    RecvSyncDataCmd(Net_Session* session, BinInputPacket<>& inpacket)
        :Command(session,inpacket){}
    ~RecvSyncDataCmd(){}

    bool execute();


private:
    int HandOneRecord(const Record &rcd);
    int ProcCopy(Binlog &log, const Record &rcd);
    int ProcSync(Binlog &log, const Record &rcd);

};


#endif //_LEVELDB_COMMNAD_H_
