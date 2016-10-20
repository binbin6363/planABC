
#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#ifndef WIN32
#include <unistd.h>
#else
#include <Windows.h>
#endif

#include <list>

#include <cstdio>
#include <cstdlib>
#include <string>
#include "List.h"
using namespace object_pool;
//using namespace std;

#define MAX_DESCRIPTION  20
#define DEFAULT_ALLOC_COUNT 256
struct SLAB;

struct OBJ 
{
	OBJ *next;
	SLAB *slab;
};

struct SLAB 
{
	struct list_head item;
	int inuse;
	OBJ *free;
};

class Cache 
{
public:
	Cache(char* desc,int size, int n);
	~Cache();

	void *allocObj();
	void freeObj(void *p);

	static int reclaimAll();
	static void printStatsAll();
	static void printVerboseAll();

	int slab_count; 
	int obj_count; 
	int reclaim_count;
	char desc[MAX_DESCRIPTION+1];

	static Cache *cacheList;
	Cache *nextCache;
private:
	SLAB *newSlab();
	int reclaim();
	void printStats();
	void printVerbose();

	struct list_head slabList;
	struct list_head* firstNotFull;

	int objSize;
	int numObjs;
	int slabSize;
};


/*    pool allocator, stl compatible    */
template <class T> class pool_allocator
{
public:
	typedef T                 value_type;
	typedef value_type*       pointer;
	typedef const value_type* const_pointer;
	typedef value_type&       reference;
	typedef const value_type& const_reference;
	typedef std::size_t       size_type;
	typedef std::ptrdiff_t    difference_type;

	template <class U> 
	struct rebind
	{ 
		typedef pool_allocator<U> other; 
	};

	pool_allocator() {}
	pool_allocator(const pool_allocator&) {}
	template <class U> pool_allocator(const pool_allocator<U>&) {}
	~pool_allocator() {}

	pointer address(reference x) const { return &x; }
	const_pointer address(const_reference x) const { return x; }

	pointer allocate(size_type n, const_pointer = 0) 
	{
		pointer ret = 0;
		if(n == 1)
		{
#ifdef DEBUG
            printf("pool_allocator allocate size_type = %u\n", n);
#endif
			return reinterpret_cast<pointer>(m_cache.allocObj());
		}
		else /* stl中有可能会一次申请多个内存区，比如vector，调用缺省的内存分配器 */
		{
			return reinterpret_cast<pointer>(std::malloc(n * sizeof(T)));
		}
	}

	void deallocate(pointer p, size_type n) 
	{
		if(n == 1)
		{
#ifdef DEBUG
			printf("pool_allocator deallocate pointer = 0x%08x size_type = %u\n", p,n);
#endif
			m_cache.freeObj(p);
		}
		else /* stl中有可能一次申请多个内存区，比如vector，调用缺省的内存分配器 */
		{
			std::free(p);
		}
	}

	size_type max_size() const 
	{
		return static_cast<size_type>(-1) / sizeof(T);
	}

	void construct(pointer p, const value_type& x) 
	{
		new(p) value_type(x); 
	}

	void destroy(pointer p) 
	{ 
		p->~value_type(); 
	}

private:
	void operator=(const pool_allocator&);
	static Cache m_cache;
};

template<class T>
Cache pool_allocator<T>::m_cache("stl", sizeof(T), DEFAULT_ALLOC_COUNT);

template<>
class pool_allocator<void>
{
	typedef void        value_type;
	typedef void*       pointer;
	typedef const void* const_pointer;

	template <class U> 
	struct rebind { typedef pool_allocator<U> other; };
};


template <class T>
inline bool operator==(const pool_allocator<T>&, const pool_allocator<T>&) 
{
	return true;
}

template <class T>
inline bool operator!=(const pool_allocator<T>&, const pool_allocator<T>&)
{
	return false;
}


#define DECLARE_SLAB(type)		\
private:	\
	static Cache type##_cache;	\
public:	\
	void *operator new(size_t) {	\
	return type##_cache.allocObj();	\
	}	\
	void operator delete(void *p) {	\
	type##_cache.freeObj(p);	\
	}

#define IMPLEMENT_SLAB(desc,type, num)	\
	Cache type::type##_cache(desc,sizeof(type), num);

#endif
