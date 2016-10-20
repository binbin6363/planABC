/**
 * @filedesc: 
 * libco_routine_manager.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/1/6 16:02:41
 * @modify:
 *
**/
#ifndef LIBCO_ROUTINE_MANAGER_H_
#define LIBCO_ROUTINE_MANAGER_H_


#include "timeoutmanager.h"
#include "libco_data_type.h"
#include "net_event.h"
#include "param.h"

using namespace libco_src;

extern uint32_t max_co_num;

// unit s
// TODO: from config file
const uint32_t ROUTINE_TIMEOUT = 5;

class Msg;
class CoRoutineTask;
struct stCoRoutine_t;

typedef std::map<uint32_t, CoRoutineTask*> CoRtMap;
typedef CoRtMap::iterator                  CoRtiter;
typedef CoRtMap::const_iterator            CoRtCiter;
typedef std::list<CoRoutineTask*>          TaskList;

typedef std::list<Msg *> MsgList;
typedef MsgList::const_iterator MsgLCIter;
typedef MsgList::iterator MsgLIter;


class CoRoutineTask : public hash_base
{
DECLARE_SLAB(CoRoutineTask);
public:

    static CoRoutineTask *Create();
    static void Destory(CoRoutineTask *task);
    int CreateCo();
    void StartCo();
    uint32_t coid();
    void PushBackMsg(Msg *msg);
    Msg *BackMsg();
    void set_param(Net_Event *ev);
    stCoRoutine_t *co();

    void UpdateTime();
    time_t LastTime();
    // �������н�����־
    void RunEnd();
    bool IsRunEnd();
    // ��ʱ��־
    void RunTimeOut();
    bool IsTimeOut();
    ~CoRoutineTask();

private:
    CoRoutineTask(const CoRoutineTask &);
    CoRoutineTask operator =(const CoRoutineTask &);
    
    explicit CoRoutineTask(uint32_t coid);
    void DestoryMsgList();

private:
    uint8_t       flag_;       // 1,run end; 2,timeout
    uint32_t      coid_;       // co_routine id
    time_t        last_time_;  // active time
    stCoRoutine_t *co_;
    MsgList       msg_list_;
    CoParam       co_param_;

};


class CoroutineMgr : public TimeoutEvent
{
public:
    ~CoroutineMgr(void);
    static CoroutineMgr &Instance()
    {
        static CoroutineMgr inst_;
        return inst_;
    }

    void SetTimeout(uint32_t timeout);
	virtual void handle_timeout(int id,void *userData);
    int CoroutineRun(Net_Event *ev);
    int Run(void *arg); // TODO: should be just a proxy
    int CoroutineYieldCt();       // TODO: should be just a proxy
    int CoroutineResume(uint32_t);
    int CoroutineRelease(stCoRoutine_t *co); // TODO: should be just a proxy

    CoRoutineTask *CreateTask() ;
    int DestoryTask(CoRoutineTask *task);
    int PutTask(CoRoutineTask *task);
    CoRoutineTask *GetTask(uint32_t coid);

    Msg *GetCurMsg();
    uint32_t GetCurCoId();
    void StoreDelTaskBy(uint32_t coid);
    void TaskRunEnd(uint32_t coid);

    void CheckDelCurTask();
private:
    // make sure non-copy or non-constr
    CoroutineMgr(void);
    CoroutineMgr &operator=(const CoroutineMgr &);
    CoroutineMgr(const CoroutineMgr &);
    stCoRoutine_t *GetCurCo();
    
private:

    list_lru_hash<CoRoutineTask> co_time_check_list_;
    CoRoutineTask *current_task_; // ���ڼ�¼��ǰtask���������֮������ɾ��task����������������;
    uint32_t timeout_;
    
};

#define CoroutineMgrInst CoroutineMgr::Instance()



// ֻ��data�¼��Ż����˻ص����������¼�����Ĭ�ϻص�������
void *libco_handler(void *arg);


#endif //LIBCO_ROUTINE_MANAGER_H_

