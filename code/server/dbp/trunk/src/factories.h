/**
* file: factories.h
* desc: factories file.
* auth: bbwang
* date: 2015/11/3
*/

#ifndef FACTORIES_H_
#define FACTORIES_H_
#include "msg.h"
#include "protocols.h"

using namespace common;

/**********************************************************/
/* message map table.                                     */
/**********************************************************/
// client request message map
#define CLI_MESSAGE_MAP_LIST(V)                           \
	V(CMD_KEEPALIVE,           BeatMsg)                   \
	V(CMD_SYNC_DATA,           FrontSyncTaskMsg)          \
	V(CMD_INNER_CHECK_USER,    FrontLoginMsg)          \
	V(CMD_INNER_INSERT_USER,   FrontLoginMsg)          \
	V(CMD_INNER_REPORT_TASK_INFO,FrontReportMsg)       \
	V(CMD_INNER_CHECK_TASK,    CheckTaskRequest)       \
	V(CMD_SAVE_TASK_INFO,      FrontSaveTaskMsg)       \
	V(CMD_THIRD_PARTY_LOGIN,   FrontThirdPartyLoginMsg)       \
	V(CMD_INNER_CLOSE_TASK,    CloseTaskRequest)       \
	V(CMD_QUERY_TASK_INFO,     QueryTaskInfoRequest)       \
	V(CMD_INNER_GET_INCOME,    FrontGetIncomeMsg) 

//V(CMD_INNER_WRITE_TASK,    CheckTaskRequest)	   \

// dbagent response message map
#define DBAGENT_MESSAGE_MAP_LIST(V)                        \
    V(CMD_KEEPALIVE,           BeatMsg)                    \
    V(CMD_DB_OPERATE,          BackQueryResult)            

/*
	V(CMD_INNER_CHECK_USER,    BackLoginMsg)          \
	V(CMD_INNER_INSERT_USER,   BackLoginMsg)
*/

// redis proxy response message map
#define REDIS_PROXY_MESSAGE_MAP_LIST(V)                        \
    V(CMD_KEEPALIVE,           BeatMsg)



/**********************************************************/
/* client msg factory.                                    */
/**********************************************************/
class CliMsgFactory : public MsgFactory
{
public:
    CliMsgFactory();
    virtual ~CliMsgFactory();
	virtual strList &KnownTypes() const;
	virtual Msg* Create(const char* type);
	virtual void Destroy(const Msg* msg);

private:
    static strList types;
};



/**********************************************************/
/* dbp msg factory.                                       */
/**********************************************************/
class DbagentMsgFactory : public MsgFactory
{

public:
    DbagentMsgFactory();
    virtual ~DbagentMsgFactory();
	virtual strList &KnownTypes() const;
	virtual Msg* Create(const char* type);
	virtual void Destroy(const Msg* msg);

private:
    static strList types;

};


/**********************************************************/
/* redis proxy msg factory.                               */
/**********************************************************/
class RedisProxyMsgFactory : public MsgFactory
{

public:
    RedisProxyMsgFactory();
    virtual ~RedisProxyMsgFactory();
	virtual strList &KnownTypes() const;
	virtual Msg* Create(const char* type);
	virtual void Destroy(const Msg* msg);

private:
    static strList types;

};


#endif //FACTORIES_H_
