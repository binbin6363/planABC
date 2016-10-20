
#include <cstring>
#include <memory.h>

#include "MemoryPool.h"

//using namespace std;

Cache *Cache::cacheList = NULL;

Cache::Cache(char* _desc,int size, int n)
{
	slab_count=0; 
	obj_count =0; 
	reclaim_count= 0;

	INIT_LIST_HEAD(&slabList);

	strncpy(desc,_desc,MAX_DESCRIPTION); 
	desc[MAX_DESCRIPTION]= '\0'; 
	objSize = size + sizeof(OBJ);
	numObjs = n;
	firstNotFull = &slabList;

	slabSize = sizeof(SLAB) + objSize * n;

	// Add it to the cache chain
	nextCache = cacheList;
	cacheList = this;
}

Cache::~Cache()
{
	struct list_head* pos, *next;
	list_for_each_safe(pos, next, &slabList)
	{
		list_del(pos);
		SLAB* slab = list_entry(pos, SLAB, item);
		free(slab);
	}
}

int Cache::reclaimAll()
{
	int ret = 0;

	for (Cache *p = cacheList; p; p = p->nextCache)
		ret += p->reclaim();
	return ret;
}

void Cache::printStatsAll()
{
	printf("\n");

	for (Cache *p = cacheList; p; p = p->nextCache)
		p->printStats();
		
	printf("\n");
}

void Cache::printVerboseAll()
{
	printf("\n");

	for (Cache *p = cacheList; p; p = p->nextCache)
		p->printVerbose();

	printf("\n");
}

void Cache::printStats()
{
	printf("cache %s objSize=%d slabSize=%d numObjs=%d slab_count=%d obj_count=%d\n",
		desc, objSize, slabSize, numObjs, slab_count, obj_count);
}

void Cache::printVerbose()
{
	list_head* pos;
	list_for_each(pos, &slabList)
	{
		SLAB* slab = list_entry(pos, SLAB, item);
		
		if(firstNotFull == &slab->item)
			printf("firstNotFull ");
		printf("slab %d inuse\n", slab->inuse);
	}
	if(firstNotFull == &slabList)
		printf("firstNotFull not exist\n");
}

/*
* Reclaim empty slabs in this cache
*/
int Cache::reclaim()
{
	//有策略的释放内存，当实际的obj数量小于总数量的1/2时重整内存
	if(slab_count < 10 || numObjs * slab_count <= obj_count * 2)
		return 0;

	printf("reclaim -> ");
	printStats();

	int n = 0;
	SLAB *slab;
	struct list_head *pos, *t = firstNotFull;
	while ((pos = slabList.prev) != &slabList) 
	{
		reclaim_count++; 
		slab = list_entry(pos, SLAB, item);
		if (slab->inuse)
			break;

		++n;
		if (firstNotFull == pos)
			firstNotFull = pos->prev;
		list_del(pos);
		slab_count--; 
		free(slab);
	}

	if (firstNotFull != t && firstNotFull != &slabList) {
		slab = list_entry(firstNotFull, SLAB, item);
		if (slab->inuse == numObjs)
			firstNotFull = &slabList;
	}

	printf("reclaim -> ");
	printStats();
	
	return n;
}

/*
* Alloc a new slab
*/
//////////////////////////////////////////////////////////////////////////
//
//
// slabmemory  =   | Slab | OBJ1 | OBJ2 | OBJ3 |
// OBJ = | OBJ struct |  really memory |
//
//////////////////////////////////////////////////////////////////////////
SLAB *Cache::newSlab()
{
	slab_count++; 
	SLAB *slab = (SLAB *) malloc(slabSize);
	if (!slab) 
	{
		reclaimAll();
		slab = (SLAB *) malloc(slabSize);
		if(!slab)
			return NULL;
	}
	
	slab->inuse = 0;

	OBJ *obj = (OBJ *) (slab + 1);
	slab->free = obj;
	for (int i = 0; i < numObjs - 1; ++i) 
	{
		OBJ *next = (OBJ *) ((char *) obj + objSize);
		obj->next = next;
		obj->slab = slab;
		obj = next;
	}
	obj->next = NULL;
	obj->slab = slab;

	list_add_tail(&slab->item, &slabList);
	firstNotFull = &slab->item;

	return slab;
}

/*
* Alloc an object from the cache
*/
void *Cache::allocObj()
{
	void *ret = NULL;
	SLAB *slab;

	obj_count ++;

	if (firstNotFull == &slabList)
		slab = newSlab();
	else
		slab = list_entry(firstNotFull, SLAB, item);

	if (slab)
	{
		if(slab->free)
		{
			slab->inuse++;
			ret = slab->free + 1;
			slab->free = slab->free->next;
		}

		if (!slab->free)
			firstNotFull = slab->item.next;
	}

	return ret;
}

/*
* Free an object in the cache
*/
void Cache::freeObj(void *p)
{
	obj_count--; 
#ifdef DEBUG
	memset(p, 0xdd, objSize - sizeof(OBJ));
#endif

	OBJ *obj = (OBJ *) p - 1;
	SLAB *slab = obj->slab;

	obj->next = slab->free;
	slab->free = obj;

	struct list_head *pos;
	if(slab->inuse-- == numObjs)
	{
		struct list_head *t = firstNotFull;
		pos = &slab->item;
		firstNotFull = pos;
		if (pos->next != t) 
		{
			list_del(pos);
			list_add_tail(pos, t);
		}
	}
	else if(slab->inuse == 0)
	{
		struct list_head *t = firstNotFull->prev;
		pos = &slab->item;

		list_del(pos);
		list_add_tail(pos, &slabList);
		if (firstNotFull == &slab->item)
			firstNotFull = t->next;
	}
}

