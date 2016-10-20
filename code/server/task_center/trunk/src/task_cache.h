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
    uint64_t  taskid; // ����id��
    uint32_t  tasktype; // ��������
    uint32_t  tasksize; // �����С
    uint32_t  taskptstep; // �����ܲ���
    uint32_t  taskpcstep; // ����ǰ�������
    uint32_t  taskstime;// ���񷢲�ʱ��
    uint32_t  tasketime;// �����ֹʱ��
    string    taskname; // ��������
    string    tasklink; // �������ӵ�ַ
    string    taskdesc; // ��������
    string    taskprice; // ����۸�
    string    taskpublisher; // ���񷢲���
    uint32_t  taskstatus; // ����״̬
    string    taskicon; // ����ͼ��
    string    taskpkgname; // ��������
    uint32_t  tasknum;     // ��������
    uint32_t  taskusednum;  // �����Ѿ�ʹ�õ���
    uint32_t  taskrewardnum;  // ��������
    
    uint32_t  last_time; // ����ӵģ�����task���������

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

