//! @file pool_allocator.h


#ifndef _POOL_ALLOCATOR_H_
#define _POOL_ALLOCATOR_H_


#include "thread_mutex.h"
#include <stddef.h>
#include <assert.h>


//! @class Pool_Allocator_T
//! @brief 池分配器
//!
//! 预先分配一大块内存并切块, 用尽后直接new
//! 此类需要保证线程安全
template <typename T, int MAX_COUNT>
class Pool_Allocator_T
{
public:
	Pool_Allocator_T();
	~Pool_Allocator_T();

public:
	//! 分配内存
	//! @param size 要分配内存的长度
	//! @return 结果指针
	void* acquire(size_t size);

	//! 释放内存
	//! @param p 要释放内存的指针
	//! @param size 要释放内存的长度
	void release(void *p, size_t size);

	int size();
private:
	//! 数据长度(对齐到4)
	const int BLOCK_LENGTH;

	//! 内存块
	char *m_chunk;

	//! 空闲指针数组
	void *m_pool[MAX_COUNT];

	//! 当前空闲指针
	int m_index;

	//! 当前分配了多少个对象
	int m_currentSize;

	//! 线程锁
	utils::Thread_Mutex m_mutex;
};

template <typename T, int MAX_COUNT>
int Pool_Allocator_T<T, MAX_COUNT>::size()
{
	int length;
	m_mutex.acquire();
	length =  m_currentSize;
	m_mutex.release();
	return length;
}



template <typename T, int MAX_COUNT>
Pool_Allocator_T<T, MAX_COUNT>::Pool_Allocator_T() : BLOCK_LENGTH((sizeof(T) + 3) & ~3)
{
	m_chunk = new char[MAX_COUNT * BLOCK_LENGTH];

	for (int i = 0; i < MAX_COUNT; i++)
	{
		void *p = m_chunk + i * BLOCK_LENGTH;
		m_pool[i] = p;
	}

	m_index = MAX_COUNT - 1;
	m_currentSize = 0;
}


template <typename T, int MAX_COUNT>
Pool_Allocator_T<T, MAX_COUNT>::~Pool_Allocator_T()
{
	delete [] m_chunk;
}


template <typename T, int MAX_COUNT>
void* Pool_Allocator_T<T, MAX_COUNT>::acquire(size_t size)
{
	assert(size == sizeof(T));

	void *p;

	m_mutex.acquire();

	m_currentSize++;
	if (m_index >= 0)
	{
		p = m_pool[m_index];
		m_index--;

		m_mutex.release();
	}
	else
	{
		m_mutex.release();

		p = new char[sizeof(T)];
	}

	return p;
}


template <typename T, int MAX_COUNT>
void Pool_Allocator_T<T, MAX_COUNT>::release(void *p, size_t size)
{
	assert(size == sizeof(T));

	m_mutex.acquire();

	m_currentSize--;
	if ((p >= m_chunk) && (p < m_chunk + MAX_COUNT * BLOCK_LENGTH))
	{
		m_index++;
		m_pool[m_index] = p;

		assert(m_index < MAX_COUNT);
		assert(0 == (((char*)p - m_chunk) % BLOCK_LENGTH));

		m_mutex.release();

		return;
	}

	m_mutex.release();

	delete [] (char*)p;
	return;
}


//! @def DECLARE_ALLOCATOR
//! @brief 声明池配置器和重载的new, delete
#define DECLARE_ALLOCATOR(T, MAX_COUNT)	\
private:	\
	typedef Pool_Allocator_T<T, MAX_COUNT> Pool_Allocator;	\
	static Pool_Allocator m_pool_allocator;	\
public:	\
	void* operator new(size_t size);	\
	void operator delete(void *p, size_t size);


//! @def IMPLEMENT_ALLOCATOR
//! @brief 定义池配置器和重载的new, delete
#define IMPLEMENT_ALLOCATOR(T)	\
T::Pool_Allocator T::m_pool_allocator;	\
void* T::operator new(size_t size)	\
{	\
	return m_pool_allocator.acquire(size);	\
}	\
void T::operator delete(void *p, size_t size)	\
{	\
	return m_pool_allocator.release(p, size);	\
}


#endif // _POOL_ALLOCATOR_H_
