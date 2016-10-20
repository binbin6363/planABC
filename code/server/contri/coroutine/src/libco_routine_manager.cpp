/**
 * @filedesc: 
 * co_routine_manager.cpp
 * @author: 
 *  bbwang
 * @date: 
 *  2015/1/6 16:02:41
 * @modify:
 *
**/
#include "libco_routine_manager.h"

#include "co_routine_inner.h"
#include "co_routine.h"
#include "comm.h"
#include "translater.h"
#include "libco_data_type.h"
#include "msg.h"
#include "processor.h"
#include "net_handler.h"
#include "net_handle_manager.h"
#include "option.h"
#include "libco_net_session.h"
#include "timecounter.h"

//using namespace object_pool;


// 添加统计
uint32_t msg_counter = 0;
uint32_t task_counter = 0;
uint32_t co_counter = 0;
// 可以启的最大协程数目,默认20000，可配
uint32_t max_co_num = 20000;


IMPLEMENT_SLAB("CoRoutineTask", CoRoutineTask, 1000);
IMPLEMENT_SLAB("Param", Param, 1000);


CoRoutineTask::CoRoutineTask(uint32_t coid)
    : flag_(0)
    , coid_(coid)
    , last_time_(0)
    , co_(NULL)
{
    msg_list_.clear();
    ++task_counter;
}

CoRoutineTask::~CoRoutineTask() 
{
    if (co_)
    {
        if (co_->cEnd)
        {
            --co_counter;
            co_release(co_);
            co_ = NULL;
        }
        else
        {
            LOG(WARN)("release co failed. co not end.");
            co_->cEnd = 1;
            co_release(co_);
        }
    }
    else
    {
        LOG(ERROR)("no release routine.");
    }
    DestoryMsgList();
    --task_counter;
}
    
CoRoutineTask *CoRoutineTask::Create()
{
    uint32_t coid = GetNewId();
    CoRoutineTask *task = new CoRoutineTask(coid);
    return task;
}

void CoRoutineTask::Destory(CoRoutineTask *task)
{
    SAFE_DELETE(task);
}

int CoRoutineTask::CreateCo()
{
    ++co_counter;
    return co_create( &(this->co_), NULL, libco_handler, (void *)&co_param_);
}

void CoRoutineTask::StartCo()
{
    last_time_ = time(NULL);
    if (NULL== co_ || co_->cEnd)
    {
        LOG(ERROR)("coroutine is null or run end.");
        return;
    }
    co_resume( co() );
}

uint32_t CoRoutineTask::coid()
{
    return coid_;
}
void CoRoutineTask::PushBackMsg(Msg *msg)
{
    msg_list_.push_back(msg);
}

Msg *CoRoutineTask::BackMsg()
{
    return msg_list_.back();
}

void CoRoutineTask::set_param(Net_Event *ev)
{
    co_param_.coid = coid();
    co_param_.net_event = ev;
}

stCoRoutine_t *CoRoutineTask::co()
{
    return co_;
}

void CoRoutineTask::UpdateTime()
{
    last_time_ = time(NULL);
}
        

time_t CoRoutineTask::LastTime()
{
    return last_time_;
}

// 设置运行结束标志
void CoRoutineTask::RunEnd()
{
    flag_ = 1;
}

bool CoRoutineTask::IsRunEnd()
{
    return (flag_ == 1);
}
// 超时标志
void CoRoutineTask::RunTimeOut()
{
    flag_ = 2;
}

bool CoRoutineTask::IsTimeOut()
{
    return (flag_ == 2);
}

void CoRoutineTask::DestoryMsgList()
{
    for (; !msg_list_.empty(); )
    {
        Msg *msg = msg_list_.back();
        msg_list_.pop_back();
        if (NULL == msg)
        {
            continue;
        }
        MsgCreator::Destory("", msg);
    }
}


// =============CoroutineMgr=================

CoroutineMgr::CoroutineMgr(void)
    : current_task_(NULL)
    , timeout_(5)
{
    LOG(INFO)("start CoroutineMgr, register timeout, check interval:%us, stCoRoutine_t size:%zu", timeout_, sizeof(stCoRoutine_t));
    
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(timeout_), NULL);
}



CoroutineMgr::~CoroutineMgr(void)
{
    timeout_ = 0;
    // clear map
    list_head *co_head = co_time_check_list_.begin();
	list_head *pPos = NULL;
	list_head *next = NULL;
	list_for_each_safe(pPos, next, co_head)
	{
		CoRoutineTask* task = list_entry(pPos, CoRoutineTask, item_);
        DestoryTask(task);
	}

    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
}



void CoroutineMgr::SetTimeout(uint32_t timeout)
{
    timeout_ = timeout;
}


Msg *CoroutineMgr::GetCurMsg()
{
    uint32_t cur_coid = GetCurCoId();
    CoRoutineTask *task = GetTask(cur_coid);
    if (!task)
    {
        LOG(ERROR)("get task failed, task is null. task id:%u", cur_coid);
        return NULL;
    }
    return task->BackMsg();
}

uint32_t CoroutineMgr::GetCurCoId()
{
    uint32_t cur_coid = 0;
    stCoRoutine_t *co = GetCurCo();
    // 目前心跳不在协程中发送
    // 非协程的coid为0
    // TODO: 可以将心跳也放在协程中，后续实现
    if (NULL == co)
    {
        //LOG(DEBUG)("GetCurCoId. current is not a coroutine. task id:0");
        return cur_coid;
    }
    // 主协程的arg是空的,且主协程的coid为0
    if (co->cIsMain)
    {
        //LOG(DEBUG)("GetCurCoId. current is main rotinue. cur task id:0");
        return cur_coid;
    }
    // 如果是子协程，但是参数为空，就是出错了
    CoParam *co_param = static_cast<CoParam *>(co->arg);
    if (!co_param)
    {
        LOG(ERROR)("GetCurCoId. current is sub rotinue, param is null. cur task id:0");
        return cur_coid;
    }
    cur_coid = co_param->coid;
//    LOG(DEBUG)("CoroutineMgr::GetCurCoId task id:%u", cur_coid);
    return cur_coid;
}


void CoroutineMgr::TaskRunEnd(uint32_t coid)
{
    CoRoutineTask *task = GetTask(coid);
    if (NULL == task)
    {
        LOG(ERROR)("task leak! task id:%u, task addr:%p", coid, task);
        return ;
    }
    if (GetCurCo() != task->co())
    {
        task->RunEnd();
        LOG(ERROR)("task is not equal to current task, task id:%u, task addr:%p", coid, task);
        return ;
    }

    // 唯一赋值点,记录刚刚完成的task，马上删除
    current_task_ = task;
    current_task_->RunEnd();
}

stCoRoutine_t *CoroutineMgr::GetCurCo()
{
    return co_self();
}


CoRoutineTask *CoroutineMgr::CreateTask()
{
    CoRoutineTask *task = CoRoutineTask::Create();
    if (task != NULL)
    {
        // 创建co，并放到超时检测队列中
        task->CreateCo();
        PutTask(task);
        return task;
    } else {
        LOG(ERROR)("CoRoutineTask create failed, task id:%u", task->coid());
        return NULL;
    }
}

int CoroutineMgr::PutTask(CoRoutineTask *task)
{
    if (NULL == task)
    {
        LOG(ERROR)("put task to time check list, task is null");
        return -1;
    }
    co_time_check_list_.insert(task->coid(), task);
    return 0;
}

CoRoutineTask *CoroutineMgr::GetTask(uint32_t coid)
{
    CoRoutineTask *task = co_time_check_list_.find(coid);
    if (NULL == task)
    {
        LOG(ERROR)("did not find the co with id:%u", coid);
        return NULL;
    }

    // 每次来取的时候表示激活，更新时间，并移动到队列末尾
    task->UpdateTime();
    co_time_check_list_.move_to_tail(task);
    return task;
}


int CoroutineMgr::DestoryTask(CoRoutineTask *task)
{
    if (NULL == task)
    {
        LOG(WARN)("destory task, task is null.");
        return 0;
    }

    // 从超时队列删除
    co_time_check_list_.remove(task->coid());
    // 销毁任务
    CoRoutineTask::Destory(task);
    current_task_ = NULL;
    
    return 0;
}


// 仅仅只有数据请求会到达此处，其余直接走ev->handler
int CoroutineMgr::CoroutineRun(Net_Event *ev)
{
    int ret = 0;
    uint32_t coid = 0;
    CoRoutineTask *task = NULL;

    // 1. decode msg
    Msg *msg = interpret(ev);
    
    // 2. 启动协程处理，若是回包，存在就取出使用，否则就创建
    if(msg && msg->IsReply())
    {
        coid = msg->receiver_coid;
        task = GetTask(coid);
        delete ev;
        LOG(INFO)("[response] cmd:%u, msg:%s, task id:%u", msg->GetCmd(), msg->GetTypeName().c_str(), coid);
        if(!task) {
            LOG(ERROR)("is a reply pkg, but no co task match it. maybe timeout. task id:%u.", coid);
            MsgCreator::Destory("", msg);
            return ret;
        }
        task->PushBackMsg(msg);
        // 恢复协程，进入协程函数libco_handler继续上次的操作
        task->StartCo();
    } else {
        task = CreateTask();
        // 判空
        if (NULL == task)
        {
            LOG(ERROR)("[request] create task failed");
            delete ev;
            return ret;
        }
        coid = task->coid();
        if (NULL != msg)
        {
            msg->SetReceiverCoid(coid);
            LOG(INFO)("[request] cmd:%u, msg:%s, task id:%u", msg->GetCmd(), msg->GetTypeName().c_str(), coid);
        } else {
            LOG(WARN)("[request] task id:%u, msg:null", coid);
        }
        task->PushBackMsg(msg);
        task->set_param(ev);
        // 启动协程，进入协程函数libco_handler
        task->StartCo();
        delete ev;
    }

    return ret;
}


// 
// input: arg, Net_Event
// start coroutine or resume coroutine
int CoroutineMgr::Run(void *arg)
{
    int ret = 0;

    Net_Event *ev = static_cast<Net_Event *>(arg);
    if (!ev)
    {
        LOG(ERROR)("run, net event is null.");
        return ret;
    }

    // 单个请求的性能统计
    static char handle_str[CHAR_BUFF_256];
    snprintf(handle_str, CHAR_BUFF_256, "%u", ev->id);
    TimeCounterAssistant tca1(handle_str);
    
    // 判断handle是否存在，若不存在就没必要继续执行
    Net_Handler *net_handle = Net_Handle_Manager::Instance()->GetHandle(ev->id);
    if (!net_handle) {
        LOG(ERROR)("net handle not exist. ev id:%u, ev type:%u", ev->id, ev->net_event_type);
    	delete ev;
        return ret;
    }
    
    switch (ev->net_event_type)
    {
	    case TYPE_DATA:
        {
            if (co_counter > max_co_num)
            {
                LOG(ERROR)("coroutine is more than %u, lost event. handle:%u", max_co_num, ev->id);
            	delete ev;
                break;
            }
            ret = CoroutineRun(ev);
        }
        break;
        // connection, close, exception hit this
        default:
        {
            LOG(DEBUG)("call handler directly. not in coroutine.");
            ev->handler(*ev);
        	delete ev;
        }
        break;
    }
    
    return ret;
}


int CoroutineMgr::CoroutineResume(uint32_t coid)
{
    int ret = 0;
    CoRoutineTask *cur_task = GetTask(coid);
    if (NULL == cur_task)
    {
        LOG(ERROR)("resume, current task is null.");
        ret = -1;
        return ret;
    }
    stCoRoutine_t *co = cur_task->co();
    if (NULL != co)
    {
        LOG(DEBUG)("CoroutineMgr::CoroutineResume, task id:%u", coid);
        co_resume(co);
    }
    else
    {
        LOG(ERROR)("resume, did not find the co");
        ret = -1;
    }
    return ret;
}

int CoroutineMgr::CoroutineYieldCt()
{
//    LOG(DEBUG)("CoroutineMgr::CoroutineYieldCt");
    co_yield_ct();
    return 0;
}

int CoroutineMgr::CoroutineRelease(stCoRoutine_t *co)
{
//    LOG(DEBUG)("CoroutineMgr::CoroutineRelease, addr:%p", co);
    co_release(co);
    return 0;
}

//检测并删除当前的任务
void CoroutineMgr::CheckDelCurTask()
{
    if (NULL == current_task_)
    {
        LOG(DEBUG)("del check, task is null.");
        return;
    }
    if (current_task_->IsRunEnd())
    {
        LOG(DEBUG)("delete done task, task id:%u, task addr:%p", current_task_->coid(), current_task_);
        co_time_check_list_.remove(current_task_->coid());
        CoRoutineTask::Destory(current_task_);
        // 删除之后就置空，唯一置空点
        current_task_ = NULL;
    }
}

void CoroutineMgr::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;
    LOG(INFO)("dump CoroutineMgr, msg size:%u, task size:%u, co size:%u"
        ,  msg_counter, task_counter, co_counter);
    time_t curTime = time(NULL);

    // 扫描超时队列
    list_head *co_head = co_time_check_list_.begin();
	list_head *pPos = NULL;
	list_head *next = NULL;
	list_for_each_safe(pPos, next, co_head)
	{
		CoRoutineTask* task = list_entry(pPos, CoRoutineTask, item_);
        if (NULL == task)
        {
            LOG(WARN)("timeout check, task is null.");
            continue;
        }

        // 如果运行结束，销毁task
        if (task->IsRunEnd())
        {
            DestoryTask(task);
            continue;
        }
        
        if (curTime - task->LastTime() > timeout_)
        {
            // throw timeout msg
            SystemTimeoutMsg *msg = dynamic_cast<SystemTimeoutMsg *>(MsgCreator::Create(SYSTEM, SYSTEM_TIMEOUT_MSG));
            msg->set_curtime(curTime);
            msg->set_lasttime(task->LastTime());
            msg->receiver_coid = task->coid();
            task->PushBackMsg(msg);
            LOG(WARN)("timeout, throw timeout msg and destory task. task id:%u", task->coid());
            task->RunTimeOut(); // 这个标志暂时没什么作用
            CoroutineResume(task->coid());
            // destory task
            DestoryTask(task);
        }
        else
        {
            // 达到未到超时的节点，说明后续未超时，跳出循环
            break;
        }
	}

    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(timeout_), NULL);
}


// 只有data事件才会进入此回调处理，其他事件进入默认回调处理函数
void *libco_handler(void *arg)
{
    CoParam *co_param = (CoParam *)arg;
    if (NULL == co_param)
    {
        LOG(ERROR)("enter handler. param is null, return.");
        return NULL;
    }
    Net_Event *ev = co_param->net_event;
    if (!ev)
    {
        LOG(ERROR)("enter handler. event is null.");
        return NULL;
    }

    Net_Handler *handle = Net_Handle_Manager::Instance()->GetHandle(ev->id);
    if (!handle)
    {
        LOG(ERROR)("handle not exist! netid:%u", ev->id);
        return NULL;
    }
    LibcoNetSession *session = dynamic_cast<LibcoNetSession *>(handle);
    if (!session)
    {
        LOG(ERROR)("cast session failed. netid:%u", ev->id);
        return NULL;
    }
    // 更新时间
    session->update_time();
    // 如果是udp，就要保存对端的ip地址
    if (session->session_type() == UDP_SESSION)
    {
        session->remote_addr(ev->remote_addr);
    }
    
    const Option *option = session->option();
    if (!option)
    {
        LOG(ERROR)("session option is null. netid:%u", ev->id);
        return NULL;
    }
    const Processor *processor = option->processor;
    if (!processor)
    {
        LOG(ERROR)("session processor is null! netid:%u", ev->id);
        return NULL;
    }

    // make param, 内存池
    Param *psession_param = new Param;
    psession_param->net_id = ev->id;
    psession_param->service_name = option->service_name.c_str();
    psession_param->remote_addr = ev->remote_addr;
    const string remote_addr = FromAddrTostring(psession_param->remote_addr);
    uint32_t cmd = 0;
    
	switch ( ev->net_event_type )
	{
	case TYPE_DATA:
		{
            Msg *msg = CoroutineMgr::Instance().GetCurMsg();
            if (!msg)
            {
                LOG(ERROR)("get cur co msg failed. handle:%u, coid:%u", ev->id, CoroutineMgr::Instance().GetCurCoId());
                break;
            }
            cmd = msg->GetCmd();
            LOG(INFO)("start task, name:%s, handle:%u, task id:%u, cmd:%u, remote_addr:%s"
                , psession_param->service_name, psession_param->net_id, co_param->coid, msg->GetCmd(), remote_addr.c_str());
            // 客户端上来的心跳
            if (0 == cmd)
            {
                processor->KeepAlive(*psession_param, msg);
                break;
            }
            processor->ProcessData(*psession_param, msg);
		}break;	
    // TODO: 以下逻辑暂时未走到，直接走net_session
//	case TYPE_ACCEPT:
//		{
//            processor->ProcessAccept(*psession_param);
//		}
//		break;
//	case TYPE_CONNECT:
//		{
//            processor->ProcessConnect(*psession_param);
//		}
//		break;
//	case TYPE_EXCEPTION:
//	case TYPE_SYS_ERROR:
//	case TYPE_CLOSE:
//	case TYPE_TIMEOUT:
//		{
//            processor->ProcessClose(*psession_param);
//			Net_Handle_Manager::Instance()->RemoveHandle(ev->id);
//		}break;
	default:
		{
			LOG(WARN)("process error, unknown net_event_type:%d", ev->net_event_type);
		}break;
	}

    LOG(INFO)("done task, name:%s, handle:%u, task id:%u, cmd:%u, remote_addr:%s"
        , psession_param->service_name, psession_param->net_id, co_param->coid, cmd, remote_addr.c_str());
    SAFE_DELETE(psession_param);

    // 设置任务结束
    CoroutineMgr::Instance().TaskRunEnd(co_param->coid);
    return 0;
}



