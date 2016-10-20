/**
* file: cmd.h
* desc: cmd file. handle command
* auth: bbwang
* date: 2015/11/4
*/

#ifndef _CMD_H_
#define _CMD_H_
#include "comm.h"
#include "client_msg.h"
#include "task_base.h"
#include "inner_common_msg.h"


using namespace utils;

class Processor;
class Param;

class Command
{
public:
	Command( const Processor *processor, const Param &param, Msg *msg );
	virtual ~Command();

	virtual bool Execute() = 0;

	void ErrMsg(const string &err_msg);
	const string &ErrMsg();
    void ErrCode(int errcode);
    int  ErrCode();

protected:
    const Processor *processor_;
    const Param &param_;
    Msg *first_request_msg_;

private:
    int     err_code_;
    string  err_msg_;
};



// 服务器内部心跳
class BeatCmd : public Command
{
public:
    BeatCmd(const Processor *processor, const Param &param, Msg *msg);
    virtual ~BeatCmd();
	virtual bool Execute();

};


// ============================================================
// 2000, sync task
// ============================================================
class SyncTaskCmd : public Command
{
public:
    SyncTaskCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~SyncTaskCmd();
    
	virtual bool Execute();

    int RepylClient(const FrontSyncTaskMsg &front_request, BackSyncTaskMsg &front_result);
    int GetTaskIdsFromRedis(uint32_t uid, vector<uint64_t> &ids);
	int FilterSyncedTaskIds(uint64_t syncpoint, vector<uint64_t> &ids);
    int CheckTaskUsedOutFromRedis(uint32_t uid, const vector<uint64_t> &ids, vector<uint64_t> &valid_ids);
	int RemoveTaskIdsFromRedis(uint32_t uid, vector<uint64_t> &ids);
	int SetInvaildFlag(TaskInfoItem *local_task_info, vector<uint64_t> invalid_task_ids);
	int SetVaildTaskToRequest(InsertTaskRequest &request, const BackSyncTaskMsg &task);
	int SyncHistoryTask(const FrontSyncTaskMsg &front_request, BackSyncTaskMsg &front_result);
	int GetTaskListFromMysql(const FrontSyncTaskMsg &front_request, BackSyncTaskMsg &front_result);
	int GetTaskListFromLeveldb(const FrontSyncTaskMsg &front_request, BackSyncTaskMsg &front_result);
	int ParseTaskList(const StrKVVec &datas, BackSyncTaskMsg &front_result);

};


class FrontReportMsg;
// ============================================================
// 2002, report data
// ============================================================
class ReportDataCmd : public Command
{
public:
    ReportDataCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~ReportDataCmd();
    
	virtual bool Execute();

private:
	int CheckTaskVaild(const FrontReportMsg &front_request, BackReportMsg &front_result);
	
	int SetTaskProgressAndReward(const FrontReportMsg &front_request);
	int ReplyClient(const FrontReportMsg &front_request, BackReportMsg &report_result);
//    int RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result);


	uint32_t reward_; // 服务端返回的任务奖励金额
};


// ============================================================
// 2001, report data
// ============================================================
class SyncDataAckCmd : public Command
{
public:
    SyncDataAckCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~SyncDataAckCmd();
    
	virtual bool Execute();
	int RemoveTaskIdsFromRedis(uint32_t uid, const SrvAckRequest &request);

};

// ============================================================
// 5000, web publish task
// ============================================================
class WebCommonRequest;
class PublishTaskCmd : public Command
{
public:
    PublishTaskCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~PublishTaskCmd();
    
	virtual bool Execute();

private:
    int WriteTaskToLeveldb(const WebCommonRequest &front_request);
	int PrepareAndNotifyUser(const WebCommonRequest &front_request, uint64_t taskid);
    int ReplyClient(const WebCommonRequest &front_request, FrontPublishTaskResult &publish_result);

	string MakePushMsg(const WebCommonRequest &front_request);
	int PushMsg(uint32_t uid, const string &push_msg);

private:
	uint64_t task_id_;
};

class QueryTaskListCmd : public Command
{
public:
    QueryTaskListCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~QueryTaskListCmd();
    
	virtual bool Execute();
	int GetTaskListFromLeveldb(const WebCommonRequest &front_request, FrontQueryTaskListResult &front_result);
	int ParseTaskList(const StrKVVec &datas, FrontQueryTaskListResult &front_result);
	int ReplyClient(const WebCommonRequest &front_request, FrontQueryTaskListResult &front_result);

private:
	uint32_t time_start_;
	uint32_t time_end_;
	
};


class FrontQueryTaskInfoResult;
class QueryTaskInfoCmd : public Command
{
public:
    QueryTaskInfoCmd( const Processor *processor, const Param &param, Msg *msg );
    virtual ~QueryTaskInfoCmd();
    
	virtual bool Execute();

private:
	int GetTaskInfoFromLeveldb(const WebCommonRequest &front_request, FrontQueryTaskInfoResult &front_result);
	int GetTaskPayInfo(const WebCommonRequest &front_request, FrontQueryTaskInfoResult &front_result);
	int ReplyClient(const WebCommonRequest &front_request, FrontQueryTaskInfoResult &publish_result);

private:
	uint64_t task_id_;
};


class StartTaskCmd : public Command
{
public:
	StartTaskCmd( const Processor *processor, const Param &param, Msg *msg );
	virtual ~StartTaskCmd();
	
	virtual bool Execute();

private:
	int CheckTaskVaild(const StartTaskRequest &front_request, StartTaskResult &front_result);
	int WriteTaskInfoToDb(const StartTaskRequest &front_request);
	int DecrTaskNum(const StartTaskRequest &front_request);
	int WriteTaskOverflow(const StartTaskRequest &front_request);
	int ReplyClient(const StartTaskRequest &front_request, StartTaskResult &front_result);
};

#endif //_CMD_H_

