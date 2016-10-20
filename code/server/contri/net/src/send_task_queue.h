//! @file send_task_queue.h


#ifndef _SEND_TASK_QUEUE_H_
#define _SEND_TASK_QUEUE_H_


#include "types.h"
#include "cycle_buffer.h"
#include "send_task.h"
#include "config.h"
#include <list>

template <typename T>
class Queue
{
public:
        Queue()
        {
                max_count_ = 10000;
                size_ = 0;
        }
        Queue(int count)
        {
                max_count_ = count;
                size_ = 0;
        }
        bool read(T& data)
        {
                if(queue_.empty())
                        return false;
                data = queue_.front();
                queue_.pop_front();
                size_--;
                return true;
        }
        bool write(const T& data)
        {
                if(size_ >= max_count_)
                        return false;
                queue_.push_back(data);
                size_++;
                return true;
        }
        int size()
        {
                return size_;
        }

private:
        std::list<T> queue_;
        int size_;
        int max_count_;
};
//! @typedef Net_Send_Task_Queue
//! @brief 网络组件的发送任务队列
//!
//! 此队列部分线程安全
//! 队列容量为100万
typedef Cycle_Buffer_T<Send_Task*, MAX_NET_SEND_TASK_COUNT> Net_Send_Task_Queue;


//! @typedef Socket_Send_Task_Queue
//! @brief 某个网络通道的发送任务队列
//!
//! 此队列部分线程安全
//! 队列容量为1万
typedef Queue<Send_Task*> Socket_Send_Task_Queue;


#endif // _SEND_TASK_QUEUE_H_
