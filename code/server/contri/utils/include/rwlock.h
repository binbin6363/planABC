/// @file rwlock.h

#ifndef THREAD_RWLOCK_H
#define THREAD_RWLOCK_H

namespace utils
{
    namespace thread
    {
        class rwlock_impl;

        /**
            读写锁的跨平台实现
         */
        class ReadWriteLock
        {
        public:
            ReadWriteLock();

            ~ReadWriteLock();

        public:
            /// 支持自动释放的读锁
            class ReadLock
            {
            public:
                ReadLock(ReadWriteLock & lock)
                    : lock_(lock)
                {
                    lock_.read_lock();
                }

                ~ReadLock()
                {
                    lock_.read_unlock();
                }

            private:
                ReadWriteLock & lock_;

            };

            /// 支持自动释放的写锁
            class WriteLock
            {
            public:
                WriteLock(ReadWriteLock & lock)
                    : lock_(lock)
                {
                    lock_.write_lock();
                }

                ~WriteLock()
                {
                    lock_.write_unlock();
                }

            private:
                ReadWriteLock & lock_;

            };

        public:
            ReadLock read_locker()
            {
                return ReadLock(*this);
            }

            WriteLock write_locker()
            {
                return WriteLock(*this);
            }

        public:
            // 初始化
            int init();

            /// 获取读锁
            int read_lock();

            /// 获取写锁
            int write_lock();

            /// 释放读锁
            int read_unlock();

            /// 释放写锁
            int write_unlock();

        private:
            rwlock_impl * impl_;

        };

    } // namespace thread

} // namespace utils

#endif // THREAD_CONDITION_H
