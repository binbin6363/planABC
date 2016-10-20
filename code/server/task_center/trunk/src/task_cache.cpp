/**
* file: task_cache.h
* desc: task.
* auth: bbwang
* date: 2015/11/19
*/
#include "task_cache.h"
#include "constants.h"
#include "log.h"


IMPLEMENT_SLAB("TaskInfoItem", TaskInfoItem, 1000);

TaskInfoItem::TaskInfoItem()
    : taskid(0)
    , tasktype(0) // 任务类型
    , tasksize(0) // 任务大小
    , taskptstep(0) // 任务步骤
    , taskpcstep(0) // 任务当前处理进度
    , taskstime(0) // 任务发布时间
    , tasketime(0) // 任务截止时间
    , taskname("") // 任务名称
    , tasklink("") // 任务链接地址
    , taskdesc("") // 任务描述
    , taskprice("") // 任务价格
    , taskpublisher("") // 任务发布者
    , taskstatus(0)
    , taskicon("")
    , taskpkgname("")
    , tasknum(0)
    , taskusednum(0)
    , last_time(0)
    , taskrewardnum(0)
{
}

TaskInfoItem::~TaskInfoItem()
{
}

TaskInfoItem::TaskInfoItem(const TaskInfoItem &other)
{
    *this = other;
}

TaskInfoItem &TaskInfoItem::operator=(const TaskInfoItem &other)
{
    if (&other == this)
        return *this;
    taskid = other.taskid;// 任务id
    tasktype = other.tasktype ; // 任务类型
    tasksize = other.tasksize; // 任务大小
    taskptstep = other.taskptstep; // 任务步骤
    taskpcstep = other.taskpcstep; // 任务当前处理进度
    taskstime = other.taskstime;// 任务发布时间
    tasketime = other.tasketime;// 任务截止时间
    taskname = other.taskname; // 任务名称
    tasklink = other.tasklink; // 任务链接地址
    taskdesc = other.taskdesc; // 任务描述
    taskprice = other.taskprice; // 任务价格
    taskpublisher = other.taskpublisher; // 任务发布者
    taskstatus = other.taskstatus; // 任务状态
    taskicon = other.taskicon; // 任务图标
    taskpkgname = other.taskpkgname; // 任务包名
    tasknum = other.tasknum; // 任务总量
    taskusednum = other.taskusednum; // 任务已经使用的量
    taskrewardnum = other.taskrewardnum; // 任务奖励数
    
    last_time = other.last_time; // 另外加的，不是task必须的属性

    return *this;
}


TaskCache::TaskCache()
{
}

TaskCache::~TaskCache()
{
}

TaskCache &TaskCache::Instance()
{
    static TaskCache inst;
    return inst;
}

TaskInfoItem *TaskCache::find(const uint64_t &task_id)
{
    TaskInfoItem *item = task_cache_list_.find(task_id);
    if (NULL != item) {
        item->last_time = time(NULL);
        task_cache_list_.move_to_tail(item);
    }
    return item;
}

int TaskCache::insert(const TaskInfoItem *task)
{
    if (NULL == task)
    {
        return -1;
    }

    if (NULL != find(task->taskid))
    {
        LOG(WARN)("task cache exist this task already, id:%lu", task->taskid);
        return 0;
    }
    
    TaskInfoItem *local_task_info = new TaskInfoItem(*task);

    local_task_info->last_time = time(NULL);
    if (task_cache_list_.insert(local_task_info->taskid, local_task_info))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int TaskCache::remove(const uint64_t &task_id)
{
    TaskInfoItem *local_task_info = task_cache_list_.find(task_id);
    if (NULL == local_task_info)
    {
        LOG(ERROR)("remove task failed, not found it.");
        return -1;
    }
    if (task_cache_list_.remove(local_task_info))
    {
        SAFE_DELETE(local_task_info);
        return 0;
    }
    else
    {
        LOG(ERROR)("remove task failed.");
        return -1;
    }
}

list_head *TaskCache::begin()
{
    return task_cache_list_.begin();
}

