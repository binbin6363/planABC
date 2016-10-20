/**
* file: factories.h
* desc: factories file.
* auth: bbwang
* date: 2015/11/4
*/

#ifndef FACTORIES_H_
#define FACTORIES_H_
#include "msg.h"
#include "protocols.h"

using namespace common;

/**********************************************************/
/* message map table.                                                                   */
/**********************************************************/
// client request message map
#define CLI_MESSAGE_MAP_LIST(V)                      \
	V(CMD_KEEPALIVE,           BeatMsg)              \
	V(CMD_SYNC_DATA,           FrontSyncTaskMsg)     \
	V(CMD_REPORT_DATA,         FrontReportMsg)       \
	V(CMD_START_TASK,          StartTaskRequest)       \
	V(CMD_PUBLISH_TASK,        WebCommonRequest) \
	V(CMD_QUERY_TASK_LIST,     WebCommonRequest) \
	V(CMD_QUERY_TASK_INFO,     WebCommonRequest)

// ldb response message map
#define LDB_MESSAGE_MAP_LIST(V)                    \
    V(CMD_KEEPALIVE,           BeatMsg)            \
	V(CMD_SYNC_DATA,           BackSyncTaskMsg)     \
	V(LEVELDB_GET,             LdbBatchGetResult)   \
	V(LEVELDB_RANGE_GET,       LdbRangeGetResult)   \
	V(LEVELDB_PUT,             LdbBatchPutResult)  
//	V(CMD_REPORT_DATA,         BackReportMsg) 

// dbp response message map
#define DBP_MESSAGE_MAP_LIST(V)                    \
    V(CMD_KEEPALIVE,           BeatMsg)            \
	V(CMD_INNER_GET_TASK,      BackSyncTaskMsg)    \
	V(CMD_INNER_CHECK_TASK,    StartTaskResult)    \
	V(CMD_INNER_WRITE_TASK,    StartTaskResult)    \
	V(CMD_QUERY_TASK_INFO,     QueryTaskInfoResult)  \
	V(CMD_SYNC_DATA,           BackSyncTaskMsg)  \
    V(CMD_INNER_REPORT_TASK_INFO,BackReportMsg)
//	V(CMD_REPORT_DATA,         BackReportMsg) 

// push response message map
#define PUSH_MESSAGE_MAP_LIST(V)                   \
    V(CMD_KEEPALIVE,           BeatMsg)            \
	V(CMD_INNER_GET_TASK,      BackSyncTaskMsg)

/**********************************************************/
/* client msg factory.                                                                      */
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
/* leveldb msg factory.                                       */
/**********************************************************/
class LdbMsgFactory : public MsgFactory
{

public:
    LdbMsgFactory();
    virtual ~LdbMsgFactory();
	virtual strList &KnownTypes() const;
	virtual Msg* Create(const char* type);
	virtual void Destroy(const Msg* msg);

private:
    static strList types;

};




/**********************************************************/
/* dbp msg factory.                                                                        */
/**********************************************************/
class DbpMsgFactory : public MsgFactory
{

public:
    DbpMsgFactory();
    virtual ~DbpMsgFactory();
	virtual strList &KnownTypes() const;
	virtual Msg* Create(const char* type);
	virtual void Destroy(const Msg* msg);

private:
    static strList types;

};

/**********************************************************/
/* push msg factory.                                                                      */
/**********************************************************/
class PushMsgFactory : public MsgFactory
{

public:
    PushMsgFactory();
    virtual ~PushMsgFactory();
	virtual strList &KnownTypes() const;
	virtual Msg* Create(const char* type);
	virtual void Destroy(const Msg* msg);

private:
    static strList types;

};

#endif //FACTORIES_H_
