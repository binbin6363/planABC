#ifndef __Runlet_MESSAGE_QUEUE_H__
#define __Runlet_MESSAGE_QUEUE_H__

#include "condition.h"
#include <deque>

namespace utils 
{
    template <typename T>
	class MessageQueue
	{
	    public:
            MessageQueue(
                T invalid_value = T(), 
                std::size_t max_size = 50000, 
                int pop_wait = 500, 
                int push_wait = 500)
                : invalid_value_(invalid_value)
                , max_size_(max_size)
                , pop_wait_(pop_wait)
                , push_wait_(push_wait)
            {
                cond_.init();
            }

            ~MessageQueue()
            {
            }

            T pop()
            {
                cond_.lock();
                if (size() == 0) {
	             if (pop_wait_ == 0) {
		          cond_.unlock();
                        return invalid_value_;
	             }
                    cond_.timed_wait(pop_wait_);
                }
                if (size() == 0) {
                    cond_.unlock();
                    return invalid_value_;
                }
                T t = queue_.front();
                if (size() == max_size_)
                    cond_.broadcast();	
                queue_.pop_front();
                cond_.unlock();
                return t;
            }

            bool push(T t)
            {
                cond_.lock();
                if (queue_.size() >= max_size_) {
                    if (push_wait_ == 0) {
                        cond_.unlock();
                        return false;
                    }
                    cond_.timed_wait(push_wait_);
                }
                if (queue_.size() >= max_size_) {
                    cond_.unlock();
                    return false;
                }
                queue_.push_back(t);
                if(queue_.size() == 1) {
                    cond_.broadcast();	
                }
                cond_.unlock();
                return true;
            }

            std::size_t size() const
            {
                return queue_.size();
            }

	    private:
            std::deque<T> queue_;
            thread::Condition cond_;
            const T invalid_value_;
            const std::size_t max_size_;
		    const int pop_wait_; ///< 读取消息等待时间，毫秒
		    const int push_wait_; ///< 写入消息等待时间，毫秒
	};

} // namespace utils 

#endif
