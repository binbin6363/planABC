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
    , tasktype(0) // ��������
    , tasksize(0) // �����С
    , taskptstep(0) // ������
    , taskpcstep(0) // ����ǰ�������
    , taskstime(0) // ���񷢲�ʱ��
    , tasketime(0) // �����ֹʱ��
    , taskname("") // ��������
    , tasklink("") // �������ӵ�ַ
    , taskdesc("") // ��������
    , taskprice("") // ����۸�
    , taskpublisher("") // ���񷢲���
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
    taskid = other.taskid;// ����id
    tasktype = other.tasktype ; // ��������
    tasksize = other.tasksize; // �����С
    taskptstep = other.taskptstep; // ������
    taskpcstep = other.taskpcstep; // ����ǰ�������
    taskstime = other.taskstime;// ���񷢲�ʱ��
    tasketime = other.tasketime;// �����ֹʱ��
    taskname = other.taskname; // ��������
    tasklink = other.tasklink; // �������ӵ�ַ
    taskdesc = other.taskdesc; // ��������
    taskprice = other.taskprice; // ����۸�
    taskpublisher = other.taskpublisher; // ���񷢲���
    taskstatus = other.taskstatus; // ����״̬
    taskicon = other.taskicon; // ����ͼ��
    taskpkgname = other.taskpkgname; // �������
    tasknum = other.tasknum; // ��������
    taskusednum = other.taskusednum; // �����Ѿ�ʹ�õ���
    taskrewardnum = other.taskrewardnum; // ��������
    
    last_time = other.last_time; // ����ӵģ�����task���������

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

