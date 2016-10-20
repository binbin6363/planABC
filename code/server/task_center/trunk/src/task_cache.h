/**
* file: task_cache.h
* desc: task.
* auth: bbwang
* date: 2015/11/19
*/
#ifndef _TASK_CACHE_H_
#define _TASK_CACHE_H_
#include <string>
#include "list_hash.h"
#include "MemoryPool.h"

using namespace object_pool;
using namespace std;

class TaskInfoItem : public hash_base
{
DECLARE_SLAB(TaskInfoItem);
public:
    uint64_t  taskid; // 任务id号
    uint32_t  tasktype; // 任务类型
    uint32_t  tasksize; // 任务大小
    uint32_t  taskptstep; // 任务总步骤
    uint32_t  taskpcstep; // 任务当前处理进度
    uint32_t  taskstime;// 任务发布时间
    uint32_t  tasketime;// 任务截止时间
    string    taskname; // 任务名称
    string    tasklink; // 任务链接地址
    string    taskdesc; // 任务描述
    string    taskprice; // 任务价格
    string    taskpublisher; // 任务发布者
    uint32_t  taskstatus; // 任务状态
    string    taskicon; // 任务图标
    string    taskpkgname; // 任务名称
    uint32_t  tasknum;     // 任务总量
    uint32_t  taskusednum;  // 任务已经使用的量
    uint32_t  taskrewardnum;  // 任务奖励数
    
    uint32_t  last_time; // 另外加的，不是task必须的属性

	TaskInfoItem();
	~TaskInfoItem();
	TaskInfoItem(const TaskInfoItem &other);
	TaskInfoItem &operator=(const TaskInfoItem &other);
};


class TaskCache
{
public:
	static TaskCache &Instance();

	TaskInfoItem *find(const uint64_t &task_id);
	int insert(const TaskInfoItem *task);
	int remove(const uint64_t &task_id);
	list_head *begin();
	
private:
	TaskCache();
	~TaskCache();
	TaskCache(const TaskCache &);
	void operator=(const TaskCache &);


private:
	
	static uint32_t  max_cache_size_;
	list_lru_hash<TaskInfoItem> task_cache_list_;
};


#endif // _TASK_CACHE_H_

