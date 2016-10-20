/**
* file: cmd.cpp
* desc: cmd file. handle command
* auth: bbwang
* date: 2015/11/4
*/
#include "cmd.h"
#include "client_processor.h"
#include "server_app.h"
#include "session_manager.h"
#include "client_session.h"
#include "dbp_processor.h"
#include "ldb_processor.h"
#include "string_util.h"
#include "constants.h"
#include "protocols.h"
#include "errcodes.h"
#include "json/json.h"
#include "json/config.h"
#include "json/value.h"
#include "json/writer.h"

using namespace common;
using namespace utils;


uint64_t MakeTaskId()
{
    static uint32_t auto_id = 0;
    ++auto_id;
    if (auto_id == 0X00FFFFFF) {
        auto_id = 0;
    }
    uint64_t now = time(NULL);
    uint32_t server_id = config.sys_id;

    return (now << 32 | server_id << 24 | auto_id);
}

Command::Command( const Processor *processor, const Param &param, Msg *msg )
    : processor_(processor)
    , param_(param)
    , first_request_msg_(msg)
    , err_code_(0)
    , err_msg_("")
{
}

Command::~Command()
{
}   


void Command::ErrMsg(const string &err_msg)
{
    err_msg_ = err_msg;
}


const string &Command::ErrMsg()
{
    return err_msg_;
}


void Command::ErrCode(int errcode)
{
    err_code_ = errcode;
}

int Command::ErrCode()
{
    return err_code_;
}



// ============================================================
// 0, beat
// ============================================================
BeatCmd::BeatCmd(const Processor *processor, const Param &param, Msg *msg)
: Command(processor, param, msg)
{
}

BeatCmd::~BeatCmd()
{
}

// 心跳
bool BeatCmd::Execute()
{
    // make reply msg
    BeatMsg *beat_msg = dynamic_cast<BeatMsg *>(first_request_msg_);
	if (NULL == beat_msg) {
        LOG(ERROR)("beat failed, cast beat msg failed.");
		return false;
	}
    BeatMsg reply_msg = *beat_msg;
    processor_->Reply(param_, *beat_msg, reply_msg);
    LOG(INFO)("client is alive. reply it. net id:%d, server id:%u, remote:%s"
        , param_.net_id, beat_msg->server_id, FromAddrTostring(param_.remote_addr).c_str());
    return true;
}


// ============================================================
// 2000, sync task
// ============================================================
SyncTaskCmd::SyncTaskCmd( const Processor *processor, const Param &param, Msg *msg )
: Command(processor, param, msg)
{

}

SyncTaskCmd::~SyncTaskCmd()
{
}


int SyncTaskCmd::RepylClient(const FrontSyncTaskMsg &front_request, BackSyncTaskMsg &front_result)
{
    // make SrvLoginResult
    //BackSyncTaskMsg back_sync_msg = *p_result;
    
    SrvSyncTaskResult &srv_sync_result = front_result.mutable_pb_msg();
    com::adv::msg::RetBase *ret_base = srv_sync_result.mutable_retbase();
    ret_base->set_retcode(ErrCode());
    ret_base->set_retmsg(ErrMsg());
    return processor_->Reply(param_, front_request, front_result);
}


bool SyncTaskCmd::Execute()
{
    bool ret = true;
    LOG(INFO)("SyncTaskCmd::Execute");
    FrontSyncTaskMsg *front_sync_msg = dynamic_cast<FrontSyncTaskMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_sync_msg), false, "sync data, cast request failed.");
    COHEADER coheader = front_sync_msg->msg_header();
    BackSyncTaskMsg back_task_info;
    back_task_info.set_transid(front_sync_msg->transid());
    back_task_info.set_synctype(front_sync_msg->synctype());
    back_task_info.set_continue_flag(NOT_CONTINUE);
    back_task_info.set_msg_header(coheader);
    const uint64_t &cli_sync_point = front_sync_msg->cli_sync_point();

    // 拉历史任务
    if (front_sync_msg->synctype() == SYNC_HISTORY_TASK)
    {
        return SyncHistoryTask(*front_sync_msg, back_task_info);
    }
    
    int ret_value = 0;
	Msg *result  = NULL;
    vector<uint64_t> task_ids;
    vector<uint64_t> used_out_task_ids;

    ONCE_LOOP_ENTER

	// 1. redis获取任务id
    LOG(INFO)("sync task, client sync point:%lu, get task id from redis. [%s]", cli_sync_point, coheader.print());
    ret_value = GetTaskIdsFromRedis(coheader.uid, task_ids);
    if (ret_value != 0)
    {
        LOG(INFO)("get task id, empty result, ret:%d", ret_value);
        ErrCode(0);
        ErrMsg("get task ids empty result.");
        break;
    }

    // 2. 检测任务是否被同步过，过滤已经同步过的任务
    FilterSyncedTaskIds(cli_sync_point, task_ids);
    LOG(INFO)("sync task, after filter by point:%lu, task id size:%zu, [%s]", cli_sync_point, task_ids.size(), coheader.print());

//    ret_value = CheckTaskUsedOutFromRedis(coheader.uid, task_ids, used_out_task_ids);
//    if (ret_value != 0)
//    {
//        LOG(INFO)("check task used out failed, ret:%d", ret_value);
//        ErrCode(ret_value);
//        ErrMsg("check task used out failed.");
//        break;
//    }

    // 3. 获取所有的任务详情。若本地有缓存，就不用去leveldb获取
    vector<uint64_t> not_found_taskids;
    TaskCache &task_cache = TaskCache::Instance();
    uint32_t task_size = (uint32_t)task_ids.size();
    uint32_t now_time = time(NULL);
    for (uint32_t i = 0; i < task_size; ++i)
    {
        const uint64_t &taskid = task_ids[i];
        TaskInfoItem *plocal_task_info = task_cache.find(taskid);
        if (NULL != plocal_task_info)
        {
            if (now_time > plocal_task_info->tasketime) {
                SetInvaildFlag(plocal_task_info, used_out_task_ids);
            }
            back_task_info.add_task(plocal_task_info);
            LOG(INFO)("found task in local, task id:%lu", taskid);
            continue;
        } else {
            not_found_taskids.push_back(taskid);
            LOG(INFO)("not found task in local, task id:%lu", taskid);
        }
    }

    if (not_found_taskids.empty()) {
        LOG(INFO)("get all task id list ok from local, task size:%zu", not_found_taskids.size());
        ErrCode(0);
        ErrMsg("no task");
        break;
    }

    LdbBatchGetRequest ldb_batch_get_request;
    ldb_batch_get_request.biz_cmd_ = coheader.cmd;
    ldb_batch_get_request.trans_id_ = front_sync_msg->transid();
    ldb_batch_get_request.transfer_str_ = "get task info";
    coheader.cmd = LEVELDB_GET;
    ldb_batch_get_request.set_msg_header(coheader);

    for (uint32_t j = 0; j < (uint32_t)not_found_taskids.size(); ++j)
    {
        string key = makeTaskInfoKey(not_found_taskids[j]);
        ldb_batch_get_request.add_key(key);
    }
    result = NULL;
    LOG(INFO)("get task detail info from ldb. [%s]", coheader.print());
    ret_value = processor_->GetResult(LDB_SERVICE, ldb_batch_get_request, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_ERROR_BREAK((0 != ret_value), false, "get task info from ldb failed.");

    LdbBatchGetResult *back_batch_get_result = dynamic_cast<LdbBatchGetResult *>(result);
    CHECK_POINTER(back_batch_get_result, "cast get task info from ldb failed.");
    StrKVVec &datas = back_batch_get_result->kv_data();
    task_size = datas.size();
    TaskInfoItem local_task_item;
    for (uint32_t k = 0; k < task_size; ++k)
    {
        StrKV &kv = datas[k];
        ret_value = parseTaskInfoValue(kv.value.data(), kv.value.size(), local_task_item);
        if (ret_value != 0) 
        {
            continue;
        }
        SetInvaildFlag(&local_task_item, used_out_task_ids);
        back_task_info.add_task(&local_task_item);
        task_cache.insert(&local_task_item);
    }

    // 4. 将有效任务存储到mysql，为了上报任务时去修改执行步骤，派发金额
    //CMD_SAVE_TASK_INFO
//    InsertTaskRequest save_task_request;
//    coheader.cmd = CMD_SAVE_TASK_INFO;
//    save_task_request.set_msg_header(coheader);
//    save_task_request.set_transid(front_sync_msg->transid());
//    SetVaildTaskToRequest(save_task_request, back_task_info);
//    LOG(INFO)("record valid task to mysql. [%s]", coheader.print());
//    ret = processor_->GetResult(DBP_SERVICE, save_task_request, result);
//    ErrCode(result->get_err_no());
//    ErrMsg(result->get_err_msg());
//    CHECK_ERROR_BREAK((0 != ret), false, "record task from mysql failed.");
//    InsertTaskResult *task_request = dynamic_cast<InsertTaskResult *>(result);
//    CHECK_POINTER(task_request, "cast record task result from mysql failed.");

    // 5. 删除自己redis队列中已取到的任务列表
//    LOG(INFO)("remove task id from redis. [%s]", coheader.print());
//    ret_value = RemoveTaskIdsFromRedis(coheader.uid, task_ids);
//    if (ret_value != 0)
//    {
//        // 删除失败会导致拉倒重复的任务
//        LOG(ERROR)("remove task id failed, ret:%d", ret_value);
//        ErrCode(ret_value);
//        ErrMsg("remove task id failed.");
//        break;
//    }

    ONCE_LOOP_LEAVE
    // 3. 给客户端回复
    LOG(INFO)("reply client. [%s], ret:%u, err msg:%s"
        , coheader.print(), ErrCode(), ErrMsg().c_str());
    if (0 != RepylClient(*front_sync_msg, back_task_info))
    {
        LOG(ERROR)("reply client sync result failed.");
        ret = false;
    } else {
        LOG(INFO)("reply client sync result ok.");
    }

    return ret;
}


int SyncTaskCmd::GetTaskIdsFromRedis(uint32_t uid, vector<uint64_t> &ids)
{
    LOG(INFO)("get task ids, uid:%u", uid);
    RedisClient *redis_client = ServerApp::Instance()->GetRedisClient(uid);
    return redis_client->GetTaskIds(uid, ids);
}

int SyncTaskCmd::FilterSyncedTaskIds(uint64_t syncpoint, vector<uint64_t> &ids)
{
    vector<uint64_t> filtered_task_ids;
    uint32_t ids_size = (uint32_t)ids.size();
    for (uint32_t index = 0; index < ids_size; ++ids_size)
    {
        uint64_t &id = ids[index];
        if (id > syncpoint)
        {
            LOG(DEBUG)("select id:%lu, not synced.", id);
            filtered_task_ids.push_back(id);
        }
        else 
        {
            LOG(DEBUG)("filter id:%lu, sync already.", id);
        }
    }
    ids.swap(filtered_task_ids);
    return 0;
}


int SyncTaskCmd::CheckTaskUsedOutFromRedis(uint32_t uid, const vector<uint64_t> &ids, vector<uint64_t> &invalid_ids)
{
    LOG(INFO)("check task used out, uid:%u, task size:%zu", uid, ids.size());
    RedisClient *redis_client = ServerApp::Instance()->GetRedisClient(uid);
    return redis_client->CheckTaskUsedOut(uid, ids, invalid_ids);
}



int SyncTaskCmd::SetInvaildFlag(TaskInfoItem *local_task_info, vector<uint64_t> invalid_task_ids)
{
    uint32_t size = (uint32_t)invalid_task_ids.size();
    for (uint32_t index = 0; index < size; ++index) {
        const uint64_t &taskid = invalid_task_ids[index];
        if (local_task_info->taskid == taskid) {
            local_task_info->taskstatus = TASK_INVALID;
            LOG(INFO)("task invalid, task id:%lu", taskid);
            break;
        }
    }
    return 0;
}

int SyncTaskCmd::SetVaildTaskToRequest(InsertTaskRequest &request, const BackSyncTaskMsg &task)
{
    int ret = 0;
    int size = task.task_size();
    for (int i = 0; i < size; ++i)
    {
        const SrvTaskInfo &task_info = task.get(i);
        if ((uint32_t)task_info.taskstatus() == TASK_VALID) {
            request.add(task_info);
            LOG(DEBUG)("add task to request, task id:%lu", task_info.taskid());
        } else {
            LOG(DEBUG)("skip invalid task, task id:%lu", task_info.taskid());
        }
    }
    return ret;
}

// 同步历史任务数据
int SyncTaskCmd::SyncHistoryTask(const FrontSyncTaskMsg &front_request, BackSyncTaskMsg &front_result)
{
    // 1. 从db取数据
    GetTaskListFromMysql(front_request, front_result);

    // 2. 从leveldb批量获取任务信息
    GetTaskListFromLeveldb(front_request, front_result);

    // 3. 返回客户端
    return RepylClient(front_request, front_result);

}


int SyncTaskCmd::GetTaskListFromMysql(const FrontSyncTaskMsg &front_request, BackSyncTaskMsg &front_result)
{
    int ret = 0;
    FrontSyncTaskMsg backend_request;
    COHEADER header = front_request.msg_header();
    backend_request.set_msg_header(header);
    SrvSyncTaskRequest &backend_pb_request = backend_request.mutable_pb_msg();
    const SrvSyncTaskRequest &front_pb_request = front_request.pb_msg();
    backend_pb_request.CopyFrom(front_pb_request);
    
    ONCE_LOOP_ENTER
    Msg *result = NULL;
    LOG(INFO)("get task list from mysql, [%s]", header.print());
    ret = processor_->GetResult(DBP_SERVICE, backend_request, result);
    CHECK_RET(ret, "get task list from mysql failed.");

    BackSyncTaskMsg *backend_result = dynamic_cast<BackSyncTaskMsg *>(result);
    CHECK_ERROR_BREAK((NULL == backend_result), ERR_SERVER_UNKNOWN_ERROR, "cast result failed.");

    const SrvSyncTaskResult &pb_result = backend_result->pb_msg();
    front_result.mutable_pb_msg().CopyFrom(pb_result);
    const com::adv::msg::RetBase& base_ret = pb_result.retbase();
    ret = base_ret.retcode();
    const string &msg = base_ret.retmsg();
    LOG(INFO)("get task list from mysql, ret:%d, msg:%s", ret, msg.c_str());
    ONCE_LOOP_LEAVE

    return ret;
}

int SyncTaskCmd::GetTaskListFromLeveldb(const FrontSyncTaskMsg &front_request, BackSyncTaskMsg &front_result)
{
    int ret = 0;
    COHEADER coheader = front_request.msg_header();
    LdbBatchGetRequest ldb_batch_get_request;

    ONCE_LOOP_ENTER
    ldb_batch_get_request.biz_cmd_ = coheader.cmd;
    ldb_batch_get_request.trans_id_ = front_request.transid();
    ldb_batch_get_request.transfer_str_ = "batch get task list";
    coheader.cmd = LEVELDB_GET;
    ldb_batch_get_request.set_msg_header(coheader);
    const SrvSyncTaskResult &tasks_info = front_result.pb_msg();
    int task_size = tasks_info.taskinfos_size();
    if (task_size == 0)
    {
        LOG(INFO)("task size is 0 from mysql, do not get task list from leveldb.");
        break; 
    }
    for (int i = 0; i < task_size; ++i)
    {
        const com::adv::msg::TaskInfo& task_info = tasks_info.taskinfos(i);
        const uint64_t &task_id = task_info.taskid();
        ldb_batch_get_request.key_vec_.push_back(makeTaskInfoKey(task_id));
    }
    ldb_batch_get_request.SetMaxSize(_1MB);
    LOG(INFO)("get task list info from ldb, num:%d. [%s]"
        , coheader.print(), task_size);

    Msg *result = NULL;
    ret = processor_->GetResult(LDB_SERVICE, ldb_batch_get_request, result);
    ErrCode(result->get_err_no());
    CHECK_ERROR_BREAK((0 != ret), false, "get task list from ldb failed.");

    LdbBatchGetResult *back_batch_get_result = dynamic_cast<LdbBatchGetResult *>(result);
    CHECK_POINTER(back_batch_get_result, "cast get task list from ldb failed.");
    StrKVVec &datas = back_batch_get_result->kv_data_vec_;
    if (datas.empty() || result->get_err_no() != 0)
    {
        LOG(INFO)("not found task list, ret:%d", result->get_err_no());
    } else {
        ParseTaskList(datas, front_result);
    }

    ONCE_LOOP_LEAVE

    return ret;
}

int SyncTaskCmd::ParseTaskList(const StrKVVec &datas, BackSyncTaskMsg &front_result)
{
    int ret = 0;
    StrKVVCIter data_iter = datas.begin();
    StrKVVCIter end_iter = datas.end();

    TaskInfoItem task;
    for (; data_iter != end_iter; ++data_iter)
    {
        ret = parseTaskInfoValue(data_iter->value.data(), data_iter->value.size(), task);
        if (ret != 0) 
        {
            LOG(ERROR)("parse task data failed. task:%s", data_iter->value.data());
            continue;
        }

        front_result.set_task(&task);
    }

    ErrCode(0);
    ErrMsg("get history task list ok");

    return 0;
}



// ============================================================
// 3001, report data
// ============================================================
ReportDataCmd::ReportDataCmd( const Processor *processor, const Param &param, Msg *msg )
: Command(processor, param, msg)
, reward_(0)
{

}

ReportDataCmd::~ReportDataCmd()
{
}

// 3001, 上报任务完成情况
// 1. 验证任务有效性(mysql的任务信息表)
// 2. 拉取任务奖励办法(mysql的任务信息表)
// 3. 设置任务完成进度(mysql的用户任务信息表)
// 4. 按照奖励办法给用户奖励(mysql的用户任务信息表)
bool ReportDataCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
    LOG(INFO)("ReportDataCmd::Execute");
    FrontReportMsg *front_report_msg = dynamic_cast<FrontReportMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_report_msg), false, "report data, cast request failed.");
    COHEADER coheader = front_report_msg->msg_header();
    BackReportMsg back_report_msg;
    back_report_msg.set_transid(front_report_msg->transid());
    back_report_msg.set_task_type(front_report_msg->task_type());
    back_report_msg.set_taskid(front_report_msg->taskid());
    back_report_msg.set_time(time(NULL));
    back_report_msg.set_msg_header(coheader);

    ONCE_LOOP_ENTER

    // 任务校验
    ret_value = CheckTaskVaild(*front_report_msg, back_report_msg);
    if (ERR_MYSQL_ERROR == ret_value 
        || ERR_SERVER_UNKNOWN_ERROR == ret_value
        || ERR_TASK_INVALID == ret_value
        || ERR_TASK_EXPIRE == ret_value
        || ERR_TASK_OVERFLOW == ret_value
        || ERR_TASK_DONE == ret_value)
    {
        ReplyClient(*front_report_msg, back_report_msg);
        return ret;
    }

    // 2. 设置任务完成进度以及为用户奖励
    ret_value = SetTaskProgressAndReward(*front_report_msg);
    CHECK_RET(ret_value, "set task progress and reward failed.");

    ONCE_LOOP_LEAVE

    ReplyClient(*front_report_msg, back_report_msg);
    
    return ret;
}

int ReportDataCmd::CheckTaskVaild(const FrontReportMsg &front_request, BackReportMsg &front_result)
{
    int ret = 0;
    StartTaskRequest check_start_request;
    SrvInnerCommonMsg &common_request = check_start_request.mutable_common_msg();
    //common_request.mutable_pb_msg().CopyFrom(front_request.common_msg().pb_msg());
    common_request.AddTagValue(TAG_TYPE, front_request.task_type());
    common_request.AddTagValue(TAG_TASK_ID, front_request.taskid());
    common_request.SetTransid(front_request.transid());
    common_request.SetTime(time(NULL));
    COHEADER header = front_request.msg_header();
    header.cmd = CMD_INNER_CHECK_TASK;
    check_start_request.set_msg_header(header);

    ONCE_LOOP_ENTER
    Msg *result = NULL;
    LOG(INFO)("check task valid, [%s]", header.print());
    ret = processor_->GetResult(DBP_SERVICE, check_start_request, result);
    CHECK_RET(ret, "check task start flag from dbp failed.");

    StartTaskResult *backend_result = dynamic_cast<StartTaskResult *>(result);
    CHECK_ERROR_BREAK((NULL == backend_result), ERR_SERVER_UNKNOWN_ERROR, "cast result failed.");
    ret = backend_result->RetCode();
    if (-1 == ret)
    {
        LOG(ERROR)("backend server maybe error. ret:%d", ret);
        ret = ERR_SERVER_UNKNOWN_ERROR;
        ErrCode(ret);
        ErrMsg("unknown server error.");
    } else {
        ErrCode(ret);
        ErrMsg(backend_result->RetMsg());
        LOG(INFO)("done check task. ret:%d, msg:%s", ret, ErrMsg().c_str());
    }
    ONCE_LOOP_LEAVE
    return ret;
}


int ReportDataCmd::SetTaskProgressAndReward(const FrontReportMsg &front_request)
{
    int ret = 0;
    COHEADER coheader = front_request.msg_header();
    LOG(INFO)("set task progress and reward to mysql. [%s]", coheader.print());
    FrontReportMsg report_task_request;// = front_request;
    report_task_request.mutable_pb_msg().CopyFrom(front_request.pb_msg());

    Msg *result = NULL;
    ONCE_LOOP_ENTER
        // 1. 汇报进度
        coheader.cmd = CMD_INNER_REPORT_TASK_INFO;
        report_task_request.set_msg_header(coheader);
        ret = processor_->GetResult(DBP_SERVICE, report_task_request, result);
        ErrCode(result->get_err_no());
        ErrMsg(result->get_err_msg());
        CHECK_ERROR_BREAK((0 != ret), false, "report task progress to mysql failed.");
        BackReportMsg *back_report_result = dynamic_cast<BackReportMsg *>(result);
        CHECK_ERROR_BREAK((NULL == back_report_result), -1, "cast report result failed.");
        reward_ = back_report_result->get_reward();
        // 检测返回结果是否正常，不正常就回滚，退出返回
    ONCE_LOOP_LEAVE
    return ret;
}

int ReportDataCmd::ReplyClient(const FrontReportMsg &front_request, BackReportMsg &report_result)
{
    LOG(INFO)("reply client report data, ret:%u, ret msg:%s"
        , report_result.get_err_no(), report_result.get_err_msg().c_str());
    SrvReportTaskResult &request = report_result.mutable_pb_msg();
    com::adv::msg::RetBase *ret_base = request.mutable_retbase();
    ret_base->set_retcode(ErrCode());
    ret_base->set_retmsg(ErrMsg());
    request.set_done_step(front_request.pb_msg().taskinfo().taskpcstep());
    request.set_taskid(front_request.taskid());
    request.set_tasktype(front_request.task_type());
    request.set_transid(front_request.transid());
    request.set_report_time(time(NULL));
    request.set_reward(StringUtil::u32tostr(reward_));
    return processor_->Reply(param_, front_request, report_result);
}



// ============================================================
// 2001, sync data ack
// ============================================================
SyncDataAckCmd::SyncDataAckCmd( const Processor *processor, const Param &param, Msg *msg )
: Command(processor, param, msg)
{

}


SyncDataAckCmd::~SyncDataAckCmd()
{

}

    
bool SyncDataAckCmd::Execute()
{
    bool ret = true;
    LOG(INFO)("SyncDataAckCmd::Execute");
    FrontAckMsg *front_sync_msg = dynamic_cast<FrontAckMsg *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_sync_msg), false, "ack data, cast request failed.");
    COHEADER coheader = front_sync_msg->msg_header();

    RemoveTaskIdsFromRedis(coheader.uid, front_sync_msg->pb_msg());
    return true;
}

int SyncDataAckCmd::RemoveTaskIdsFromRedis(uint32_t uid, const SrvAckRequest &request)
{
    uint32_t size = (uint32_t)request.id_size();
    LOG(INFO)("remove task from redis, uid:%u, task size:%u", uid, size);
    for (uint32_t i = 0; i < size; ++i)
    {
        // 按taskid进行hash选择redis，按taskid进行hash选择db
        RedisClient *redis_client = ServerApp::Instance()->GetRedisClient(request.id(i));
        redis_client->RemoveTaskIds(uid, request.id(i));
    }
    return 0;
}


// web publish task
PublishTaskCmd::PublishTaskCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
    , task_id_(0)
{

}

PublishTaskCmd::~PublishTaskCmd()
{

}


bool PublishTaskCmd::Execute()
{
    bool ret = true;
    LOG(INFO)("PublishTaskCmd::Execute");
    WebCommonRequest *front_request = dynamic_cast<WebCommonRequest *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_request), false, "publish task, cast request failed.");
    COHEADER coheader = front_request->msg_header();
    const string &task_sn = front_request->get_task_sn();

    task_id_ = MakeTaskId();
    LOG(INFO)("publish task, task id:%lu", task_id_);
    front_request->set_task_id(task_id_);
    RedisClient *redis_client = NULL;
    FrontPublishTaskResult publish_result;

    ONCE_LOOP_ENTER

    if (StringUtil::strcasecmp(task_sn.c_str(), "") == 0)
    {
        LOG(ERROR)("task serial number is empty.");
        ret = false;
        ErrCode(ERR_TASK_SN_EMPTY);
        ErrMsg("task serial number is empty.");
        break;
    }

    redis_client = ServerApp::Instance()->GetRedisClient(task_id_);
    // 1. 根据任务id去重,仅仅对某个时间段内的任务id去重。因为id是全局递增的
    // 去重的目的是对同一个操作可能发生的两次相同请求做去重，id保存时间为10min
    bool task_exist = redis_client->ExistKey(0, task_sn);
    if (task_exist) {
        LOG(ERROR)("task is repeated. skip this publish. task sn:%s", task_sn.c_str());
        ret = false;
        ErrCode(ERR_TASK_REPEATED);
        ErrMsg(task_sn + ", task is repeated.");
        break;
    }
    
    // 2. 任务简介写入leveldb
    if (WriteTaskToLeveldb(*front_request) != 0) {
        LOG(ERROR)("write task info to leveldb failed. taskid:%lu", task_id_);
        ErrCode(ERR_SAVE_TASK_FAILED);
        ErrMsg("save task to leveldb failed");
        ret = false;
        break;
    }

    // 3. 写任务数量到redis,task_reward_num
    string key = MakeTaskLimitKey(task_id_);
    string limit_value = front_request->get_task_limit();
    int expire = front_request->get_task_expire();
    if (expire <= 0) {
        LOG(ERROR)("task end time must bigger than start time.");
        ret = false;
        ErrCode(ERR_TASK_TIME_INVALID);
        ErrMsg("task end time must bigger than start time.");
        break;
    }
    int re_value = redis_client->SetKeyValue(task_id_, key, limit_value, expire);
    if (0 != re_value) {
        LOG(ERROR)("write task limit num failed. taskid:%lu", task_id_);
        ret = false;
        ErrCode(re_value);
        ErrMsg("write task limit num failed");
        break;
    }

    // 4. 回复ack给web
    LOG(INFO)("done publish task ok, reply client.");
    publish_result.set_msg_header(coheader);
    publish_result.set_transid(front_request->transid());
    publish_result.set_err_no(0);
    publish_result.set_err_msg("publish task ok.");
    publish_result.set_task_id(task_id_);
    ReplyClient(*front_request, publish_result);

    // 5. 任务id存redis，以备后续去重
    //expire = config.key_expire_time;
    uint32_t now_time = time(NULL);
    string chk_repeat_value = StringUtil::u64tostr(task_id_)+"_"+StringUtil::u32tostr(now_time);
    redis_client->SetKeyValue(task_id_, task_sn, chk_repeat_value, expire);
    if (0 != re_value) {
        LOG(WARN)("write task name for repeat check failed. taskid:%lu, task sn:%s"
            , task_id_, task_sn.c_str());
    }

    /*
    // 6. 从db获取用户
      目前不筛选用户，直接广播给所有用户
      vector<uint32_t> userList;
      SelectUsersFromDbp(userList);
      */

    // 7. 分别为每个用户写入种子数据，并推push
    PrepareAndNotifyUser(*front_request, task_id_);

    ONCE_LOOP_LEAVE

    if (!ret) {
        LOG(ERROR)("publish task failed, reply client.");
        // 8. 出错回复ack给web
        publish_result.set_msg_header(coheader);
        publish_result.set_transid(front_request->transid());
        publish_result.set_err_no(ErrCode());
        publish_result.set_err_msg(ErrMsg());
        publish_result.set_task_id(task_id_);
        ReplyClient(*front_request, publish_result);
    }
    
    return ret;
}


int PublishTaskCmd::WriteTaskToLeveldb(const WebCommonRequest &front_request)
{
    int ret = 0;
    COHEADER coheader = front_request.msg_header();
    const StrMap &task_params = front_request.get_params();
    string task_key = makeTaskInfoKey(task_id_);
    string task_value = makeTaskInfoValue(task_params);
    ONCE_LOOP_ENTER
    LdbBatchPutRequest ldb_batch_put_request;
    ldb_batch_put_request.biz_cmd_ = coheader.cmd;
    ldb_batch_put_request.transfer_str_ = "write task info to leveldb";
    ldb_batch_put_request.trans_id_ = front_request.transid();
    ldb_batch_put_request.AddKV(task_key, task_value);
    ldb_batch_put_request.set_msg_header(coheader);
    Msg *result = NULL;
    LOG(INFO)("put task detail info to ldb. [%s]", coheader.print());
    ret = processor_->GetResult(LDB_SERVICE, ldb_batch_put_request, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_ERROR_BREAK((0 != ret), -1, "put task info to ldb failed.");
    ONCE_LOOP_LEAVE

    LOG(INFO)("put task detail info to leveldb ok. ret:%d", ret);
    return ret;
}


int PublishTaskCmd::PrepareAndNotifyUser(const WebCommonRequest &front_request, uint64_t taskid)
{
    int ret = 0;
    const StrVec &uid_list = front_request.get_uid_list();

    RedisClient *redis_client = NULL;
    StrVCIter uid_iter = uid_list.begin();
    StrVCIter uid_iter_end = uid_list.end();

    // 1. 为用户的个人队列写入taskid
    for (; uid_iter != uid_iter_end; ++uid_iter )
    {
        uint32_t uid = StringUtil::strtou32(*uid_iter);
        redis_client = ServerApp::Instance()->GetRedisClient(uid);
        int ret_value = redis_client->AddTaskToUser(uid, taskid);
        if (0 != ret_value) {
            LOG(ERROR)("write task to user list failed. uid:%u, taskid:%lu, ret:%d."
                , uid, taskid, ret_value);
            continue;
        } else {
            LOG(INFO)("write task to user list ok. uid:%u, taskid:%lu", uid, taskid);
        }
    }
    LOG(INFO)("done add task id to user task id list");

    // 2. 通知推push消息
    uid_iter = uid_list.begin();
    string push_msg = MakePushMsg(front_request);
    for (; uid_iter != uid_iter_end; ++uid_iter )
    {
        uint32_t uid = StringUtil::strtou32(*uid_iter);
        int ret_value = PushMsg(uid, push_msg);
        if (0 != ret_value) {
            LOG(ERROR)("push msg to user failed. uid:%u, taskid:%lu, ret:%d."
                , uid, taskid, ret_value);
            continue;
        } else {
            LOG(INFO)("push msg to user ok. uid:%u, taskid:%lu", uid, taskid);
        }
    }

    // 3. 最后写push公共队列,list

    
    LOG(INFO)("done push msg to user.");
    
    return ret;
}


/**
{
    "uid": 500001,
    "pushid": "${taskid}_${autoid}",
    "platform": 1,
    "push_type": 3,
    "token": "08602750202301622000000525000001",
    "title": "dsp",
    "msgcontent": "你收到一条新任务"
}
*/
string PublishTaskCmd::MakePushMsg(const WebCommonRequest &front_request)
{
    return "";
}

int PublishTaskCmd::PushMsg(uint32_t uid, const string &push_msg)
{
    return 0;
}

int PublishTaskCmd::ReplyClient(const WebCommonRequest &front_request, FrontPublishTaskResult &publish_result)
{
    LOG(INFO)("reply client publish task, ret:%u, ret msg:%s"
        , publish_result.get_err_no(), publish_result.get_err_msg().c_str());
    return processor_->Reply(param_, front_request, publish_result);
}


StartTaskCmd::StartTaskCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
{
}

StartTaskCmd::~StartTaskCmd()
{
}


/**
// 检测任务状态以及修改任务可用数，
// 如果是首次启动任务，就校验任务状态，
// 并且在任务状态ok的情况下将任务池中的本任务的可
// 用任务数减一，反之之前已经启动过，则仅仅是检验
// 任务状态。
 1. 去mysql的任务表校验任务状态，任务失效就直接返回
 2. 如果是首次启动有效的任务，dbp会将任务写进用户的任务列表,就去任务池减一操作
**/
bool StartTaskCmd::Execute()
{
    bool ret = true;
    int ret_value = 0;
    StartTaskRequest *front_msg = dynamic_cast<StartTaskRequest *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_msg), false, "start task, cast request failed.");
    COHEADER coheader = front_msg->msg_header();
    StartTaskResult front_result;
    front_result.set_msg_header(coheader);
    uint64_t taskid = front_msg->GetTaskId();
    LOG(INFO)("start task, taskid:%lu, [%s]", taskid, coheader.print());

    // 1. 检测任务状态
    ret_value = CheckTaskVaild(*front_msg, front_result);
    if (ERR_MYSQL_ERROR == ret_value || ERR_SERVER_UNKNOWN_ERROR == ret_value)
    {
        ReplyClient(*front_msg, front_result);
        return ret;
    }
    
    // 2. 首次点击任务就将全局任务池任务减一
    if (ERR_TASK_FIRST_START == ret_value)
    {
        ret_value = DecrTaskNum(*front_msg);
        if (ERR_TASK_OVERFLOW == ret_value)
        {
            LOG(INFO)("task overflow, taskid:%lu. [%s]", taskid, coheader.print());
            // 极其重要!!!
            WriteTaskOverflow(*front_msg);
            ErrCode(ERR_TASK_OVERFLOW);
            ErrMsg("task already overflow.");
            ReplyClient(*front_msg, front_result);
            return ret;
        } else if (0 != ret_value) {
            LOG(ERROR)("desc task num failed. [%s]", coheader.print());
            ReplyClient(*front_msg, front_result);
            return ret;
        }
    }

    LOG(INFO)("done start task, reply client. [%s]", coheader.print());
    ReplyClient(*front_msg, front_result);
    return ret;
}


int StartTaskCmd::CheckTaskVaild(const StartTaskRequest &front_request, StartTaskResult &front_result)
{
    int ret = 0;
    StartTaskRequest check_start_request;
    SrvInnerCommonMsg &common_request = check_start_request.mutable_common_msg();
    common_request.mutable_pb_msg().CopyFrom(front_request.common_msg().pb_msg());
    COHEADER header = front_request.msg_header();
    header.cmd = CMD_INNER_CHECK_TASK;
    check_start_request.set_msg_header(header);

    ONCE_LOOP_ENTER
    Msg *result = NULL;
    LOG(INFO)("check task whether start, [%s]", header.print());
    ret = processor_->GetResult(DBP_SERVICE, check_start_request, result);
    CHECK_RET(ret, "check task start flag from dbp failed.");

    StartTaskResult *backend_result = dynamic_cast<StartTaskResult *>(result);
    CHECK_ERROR_BREAK((NULL == backend_result), ERR_SERVER_UNKNOWN_ERROR, "cast result failed.");
    ret = backend_result->RetCode();
    if (-1 == ret)
    {
        LOG(ERROR)("backend server maybe error. ret:%d", ret);
        ret = ERR_SERVER_UNKNOWN_ERROR;
        ErrCode(ret);
        ErrMsg("unknown server error.");
    } else {
        ErrCode(ret);
        ErrMsg(backend_result->RetMsg());
        LOG(INFO)("done check task. ret:%d, msg:%s", ret, ErrMsg().c_str());
    }
    ONCE_LOOP_LEAVE
    return ret;
}

int StartTaskCmd::DecrTaskNum(const StartTaskRequest &front_request)
{
    COHEADER header = front_request.msg_header();
    uint64_t task_id = front_request.GetTaskId();
    RedisClient *redis_client = ServerApp::Instance()->GetRedisClient(task_id);
    if (NULL == redis_client)
    {
        LOG(ERROR)("get redis client failed.");
        ErrCode(ERR_SERVER_UNKNOWN_ERROR);
        ErrMsg("unknown error.");
        return ERR_SERVER_UNKNOWN_ERROR;
    }
    return redis_client->DecrTaskNum(task_id, task_id);
}

int StartTaskCmd::WriteTaskOverflow(const StartTaskRequest &front_request)
{
    StartTaskRequest check_start_request;
    SrvInnerCommonMsg &common_request = check_start_request.mutable_common_msg();
    common_request.mutable_pb_msg().CopyFrom(front_request.common_msg().pb_msg());
    COHEADER header = front_request.msg_header();
    header.cmd = CMD_INNER_CLOSE_TASK;
    check_start_request.set_msg_header(header);
    
    int ret = 0;
    ONCE_LOOP_ENTER
    Msg *result = NULL;
    LOG(INFO)("close task, [%s]", header.print());
    ret = processor_->GetResult(DBP_SERVICE, check_start_request, result);
    CHECK_RET(ret, "write close task to db failed.");

    StartTaskResult *backend_result = dynamic_cast<StartTaskResult *>(result);
    CHECK_ERROR_BREAK((NULL == backend_result), ERR_SERVER_UNKNOWN_ERROR, "cast result failed.");
    ret = backend_result->RetCode();
    if (-1 == ret)
    {
        LOG(ERROR)("backend server maybe error. ret:%d", ret);
        ret = ERR_SERVER_UNKNOWN_ERROR;
        ErrCode(ret);
        ErrMsg("unknown error.");
    } else {        
        ErrCode(ret);
        ErrMsg(backend_result->RetMsg());
    }
    ONCE_LOOP_LEAVE
    return ret;
}

int StartTaskCmd::ReplyClient(const StartTaskRequest &front_request, StartTaskResult &front_result)
{
    LOG(INFO)("reply client start task, ret:%u, ret msg:%s"
        , ErrCode(), ErrMsg().c_str());
    SrvInnerCommonMsg &request = front_result.mutable_common_msg();
    request.AddTagValue(TAG_RET_CODE, (uint32_t)ErrCode());
    request.AddTagValue(TAG_RET_MSG, ErrMsg());
    request.AddTagValue(TAG_TYPE, front_request.GetTaskType());
    request.AddTagValue(TAG_TASK_ID, front_request.GetTaskId());
    request.SetTransid(front_request.GetTransId());
    request.SetTime(time(NULL));
    return processor_->Reply(param_, front_request, front_result);
}

// 系统接口，不对外提供
// 根据时间段查询系统已发表的任务列表
QueryTaskListCmd::QueryTaskListCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
    , time_start_(0)
    , time_end_(0)
{
}

QueryTaskListCmd::~QueryTaskListCmd()
{}

bool QueryTaskListCmd::Execute()
{
    bool ret = true;
    LOG(INFO)("QueryTaskListCmd::Execute");
    WebCommonRequest *front_request = dynamic_cast<WebCommonRequest *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_request), false, "query task list, cast request failed.");
    COHEADER coheader = front_request->msg_header();
    
    FrontQueryTaskListResult front_result;
    front_result.set_msg_header(coheader);

    GetTaskListFromLeveldb(*front_request, front_result);
    
    ReplyClient(*front_request, front_result);

    return ret;
}

int QueryTaskListCmd::GetTaskListFromLeveldb(const WebCommonRequest &front_request, FrontQueryTaskListResult &front_result)
{
    int ret = 0;
    COHEADER coheader = front_request.msg_header();
    LdbRangeGetRequest ldb_range_get_request;
    const StrMap &params = front_request.get_params();

    ONCE_LOOP_ENTER
        
    StrMapCIter end_iter = params.end();
    StrMapCIter task_start_iter = params.find("task_stime");
    StrMapCIter task_end_iter = params.find("task_etime");
    if ((end_iter == task_start_iter) || (end_iter == task_end_iter))
    {
        ErrCode(PARAM_ERROR);
        ErrMsg("get task list start or end condition is error.");
        break;
    }

    uint64_t time_start = StringUtil::strtou64(task_start_iter->second);
    uint64_t time_end = StringUtil::strtou64(task_end_iter->second);
    // 默认获取一个月的历史记录
    time_start = (time_start == 0) ? (time(NULL) - (3600 * 24 * 30)) : time_start;
    time_end = ((time_end == 0) && (time_end > time_start)) ? time(NULL) : time_end;

    string range_start = makeTaskInfoKey((time_start << 32));
    string range_end = makeTaskInfoKey((time_end << 32));
    
    ldb_range_get_request.biz_cmd_ = coheader.cmd;
    ldb_range_get_request.trans_id_ = front_request.transid();
    ldb_range_get_request.transfer_str_ = "web query task list";
    coheader.cmd = LEVELDB_RANGE_GET;
    ldb_range_get_request.set_msg_header(coheader);
    ldb_range_get_request.set_key_start(range_start);
    ldb_range_get_request.set_key_end(range_end);
    ldb_range_get_request.SetMaxSize(_1MB);
    LOG(INFO)("get task list info from ldb, range:[%s, %s). [%s]"
        , range_start.c_str(), range_end.c_str(), coheader.print());
    Msg *result = NULL;
    ret = processor_->GetResult(LDB_SERVICE, ldb_range_get_request, result);
    ErrCode(result->get_err_no());
    CHECK_ERROR_BREAK((0 != ret), false, "get task list from ldb failed.");

    LdbRangeGetResult *back_range_get_result = dynamic_cast<LdbRangeGetResult *>(result);
    CHECK_POINTER(back_range_get_result, "cast get task list from ldb failed.");
    StrKVVec &datas = back_range_get_result->kv_data_vec_;
    if (datas.empty() || result->get_err_no() != 0)
    {
        LOG(INFO)("not found task list in range:[%s, %s), ret:%d"
            , range_start.c_str(), range_end.c_str(), result->get_err_no());
    } else {
        ParseTaskList(datas, front_result);
    }

    ErrCode(0);
    ErrMsg("query task list ok");

    ONCE_LOOP_LEAVE
    
    return ret;

}

int QueryTaskListCmd::ParseTaskList(const StrKVVec &datas, FrontQueryTaskListResult &front_result)
{
    int ret = 0;
    StrKVVCIter data_iter = datas.begin();
    StrKVVCIter end_iter = datas.end();

    Json::StyledWriter writer;
    TaskInfoItem task;
    for (; data_iter != end_iter; ++data_iter)
    {
        ret = parseTaskInfoValue(data_iter->value.data(), data_iter->value.size(), task);
        if (ret != 0) 
        {
            LOG(ERROR)("parse task data failed. task:%s", data_iter->value.data());
            continue;
        }
        Json::Value root;
        // js 不支持u64类型，故转城字符串
        root[TAG_TASK_ID] = StringUtil::u64tostr(task.taskid);
        root[TAG_NAME] = task.taskname;
        root[TAG_PAY] = task.taskprice;
        root[TAG_PUBLR_NAME] = task.taskpublisher;
        root[TAG_REWARD_NUM] = task.taskrewardnum;
        root[TAG_STIME] = task.taskstime;
        root[TAG_ETIME] = task.tasketime;
        root[TAG_TOTAL_NUM] = task.tasknum;
        root[TAG_TOTAL_STEP] = task.taskptstep;
        root[TAG_TYPE] = task.tasktype;

        front_result.AddTask(writer.write(root));
        
    }

    return 0;
}

int QueryTaskListCmd::ReplyClient(const WebCommonRequest &front_request, FrontQueryTaskListResult &front_result)
{
    LOG(INFO)("reply client start task, ret:%u, ret msg:%s"
        , ErrCode(), ErrMsg().c_str());
    front_result.set_err_msg(ErrMsg());
    front_result.set_err_no(ErrCode());
    return processor_->Reply(param_, front_request, front_result);
}

// 系统接口，不对外提供
// 根据任务id查询任务详细信息，包括详情
QueryTaskInfoCmd::QueryTaskInfoCmd( const Processor *processor, const Param &param, Msg *msg )
    : Command(processor, param, msg)
    , task_id_(0)
{
}

QueryTaskInfoCmd::~QueryTaskInfoCmd()
{
}

bool QueryTaskInfoCmd::Execute()
{
    bool ret = true;
    LOG(INFO)("QueryTaskInfoCmd::Execute");
    WebCommonRequest *front_request = dynamic_cast<WebCommonRequest *>(first_request_msg_);
    CHECK_ERROR_RETURN((NULL == front_request), false, "query task info, cast request failed.");
    COHEADER coheader = front_request->msg_header();
    FrontQueryTaskInfoResult front_result;
    front_result.set_msg_header(coheader);
    front_result.trans_id = front_request->transid();

    ONCE_LOOP_ENTER

    const StrMap &params = front_request->get_params();
    StrMapCIter taskid_iter = params.find(TAG_TASK_ID);
    StrMapCIter iter_end = params.end();
    if (taskid_iter == iter_end)
    {
        LOG(ERROR)("QueryTaskInfoCmd param is error, missing id field.");
        ErrCode(PARAM_ERROR);
        ErrMsg("param is error, missing id field");
        break;
    }
    task_id_ = StringUtil::strtou64(taskid_iter->second);

    // 获取任务详细
    if (0 != GetTaskInfoFromLeveldb(*front_request, front_result))
    {
        ErrCode(ERR_TASK_INVALID);
        ErrMsg(string("task info not exist with id ")+StringUtil::u64tostr(task_id_));
        break;
    }

    // 获取任务奖励信息
    if (0 != GetTaskPayInfo(*front_request, front_result))
    {
        ErrCode(10);
        ErrMsg(string("get task pay info failed with id ")+StringUtil::u64tostr(task_id_));
        break;
    }
    
    ONCE_LOOP_LEAVE

    ReplyClient(*front_request, front_result);

    return ret;
}


int QueryTaskInfoCmd::GetTaskInfoFromLeveldb(const WebCommonRequest &front_request, FrontQueryTaskInfoResult &front_result)
{
    int ret = 0;
    COHEADER coheader = front_request.msg_header();
    LdbBatchGetRequest ldb_batch_get_request;

    ONCE_LOOP_ENTER
        
    ldb_batch_get_request.biz_cmd_ = coheader.cmd;
    ldb_batch_get_request.trans_id_ = front_request.transid();
    ldb_batch_get_request.transfer_str_ = "web query task info";
    coheader.cmd = LEVELDB_GET;
    ldb_batch_get_request.set_msg_header(coheader);
    string key = makeTaskInfoKey(task_id_);
    ldb_batch_get_request.add_key(key);
    Msg *result = NULL;
    LOG(INFO)("get task detail info from ldb. [%s]", coheader.print());
    ret = processor_->GetResult(LDB_SERVICE, ldb_batch_get_request, result);
    ErrCode(result->get_err_no());
    ErrMsg(result->get_err_msg());
    CHECK_ERROR_BREAK((0 != ret), false, "get task info from ldb failed.");

    LdbBatchGetResult *back_batch_get_result = dynamic_cast<LdbBatchGetResult *>(result);
    CHECK_POINTER(back_batch_get_result, "cast get task info from ldb failed.");
    StrKVVec &datas = back_batch_get_result->kv_data();
    if (!datas.empty())
    {
        front_result.SetTaskInfo(datas[0].value);
    } else {
        LOG(ERROR)("can not find task info with id:%lu", task_id_);
        ret -1;
    }

    ONCE_LOOP_LEAVE
    
    return ret;

}


// 从dbp获取 Ftask_status,Fpre_cash,Faward_cash,Ffetched_cash
int QueryTaskInfoCmd::GetTaskPayInfo(const WebCommonRequest &front_request, FrontQueryTaskInfoResult &front_result)
{
    int ret = 0;
    QueryTaskInfoRequest backend_request;
    SrvInnerCommonMsg &common_request = backend_request.mutable_common_msg();
    common_request.SetTransid(front_request.transid());
    common_request.SetTime(time(NULL));
    COHEADER header = front_request.msg_header();
    header.cmd = CMD_QUERY_TASK_INFO;
    backend_request.set_msg_header(header);

    ONCE_LOOP_ENTER

    const StrMap &params = front_request.get_params();
    StrMapCIter task_id_iter = params.find(TAG_TASK_ID);
    if (task_id_iter != params.end())
    {
        const string &task_id_str = task_id_iter->second;
        common_request.AddTagValue(TAG_TASK_ID, StringUtil::strtou64(task_id_str));
    } else {
        ErrCode(PARAM_ERROR);
        ErrMsg("param is error, id is missing");
        break;
    }

    Msg *result = NULL;
    LOG(INFO)("query task pay info, [%s]", header.print());
    ret = processor_->GetResult(DBP_SERVICE, backend_request, result);
    CHECK_RET(ret, "query task pay info from dbp failed.");

    QueryTaskInfoResult *backend_result = dynamic_cast<QueryTaskInfoResult *>(result);
    CHECK_ERROR_BREAK((NULL == backend_result), ERR_SERVER_UNKNOWN_ERROR, "cast result failed.");
    ret = backend_result->get_err_no();
    if (-1 == ret)
    {
        LOG(ERROR)("backend server maybe error. ret:%d", ret);
        ret = ERR_SERVER_UNKNOWN_ERROR;
        ErrCode(ret);
        ErrMsg("unknown server error.");
        break;
    } else {
        ErrCode(ret);
        ErrMsg(backend_result->get_err_msg());
    }

    LOG(INFO)("done query task pay info. ret:%d, msg:%s", ret, ErrMsg().c_str());
    front_result.SetPrePay(backend_result->GetPrePay());
    front_result.SetAwardPay(backend_result->GetAwardPay());
    front_result.SetTaskStatus(backend_result->GetTaskStatus());
    ONCE_LOOP_LEAVE
    return ret;
}


int QueryTaskInfoCmd::ReplyClient(const WebCommonRequest &front_request, FrontQueryTaskInfoResult &front_result)
{
    front_result.set_err_no(ErrCode());
    front_result.set_err_msg(ErrMsg());
    LOG(INFO)("reply client query task info, ret:%u, ret msg:%s"
        , front_result.get_err_no(), front_result.get_err_msg().c_str());
    return processor_->Reply(param_, front_request, front_result);
}



#if 0
int ReportDataCmd::RepylClient(FrontLoginMsg *p_request, BackLoginMsg *p_result)
{
    // make SrvLoginResult
    BackLoginMsg back_login_msg;
    
    SrvLoginResult &srv_login_result = back_login_msg.mutable_pb_msg();
    srv_login_result = p_result->mutable_pb_msg();
    COHEADER coheader = p_request->msg_header();
    coheader.uid = srv_login_result.uid();
    back_login_msg.set_msg_header(coheader);
    processor_->Reply(param_, *p_request, back_login_msg);
    return 0;
}


bool ReportDataCmd::Execute()
{
    bool ret = true;
    LOG(INFO)("LoginCmd::Execute");
    FrontLoginMsg *front_login_msg = dynamic_cast<FrontLoginMsg *>(first_request_msg_);
    BackLoginMsg *back_login_result = NULL;
    
    int ret_value = 0;
	Msg *result  = NULL;
    ONCE_LOOP_ENTER
	// 1. db校验
    ret_value = processor_->GetResult(LDB_SERVICE, *front_login_msg, result);
    CHECK_RET(ret_value, "check user from db failed.");

    back_login_result = dynamic_cast<BackLoginMsg *>(result);
    CHECK_POINTER(back_login_result, "cast login result from db failed.");
    if (0 == back_login_result->mutable_pb_msg().retbase().retcode())
    {
        LOG(INFO)("check user ok, user exist.");
        break;
    }

    ONCE_LOOP_LEAVE
    if (0 != ret_value)
    {
        ErrCode(result->get_err_no());
        ErrMsg(result->get_err_msg());
    }
    // 3. 给客户端回复
    if (0 != RepylClient(front_login_msg, back_login_result))
    {
        LOG(ERROR)("reply client login result failed.");
        return false;
    }

    return ret;
}
#endif

