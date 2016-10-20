/********************************************************************
	created:	2013/06/27
	filename: 	list_hash.h
	file base:	list_hash
	file ext:	h
	author:		pengshuai
	
	purpose:	
*********************************************************************/
#ifndef list_hash_h__
#define list_hash_h__
#include "List.h"

// add log for look up error
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


using namespace object_pool;
#define DEF_MAX_HASH_NUM 100007

class hash_base
{
	//friend class list_hash<hash_base>;
public:
	hash_base()
	{
		INIT_LIST_HEAD(&item_);
		INIT_LIST_HEAD(&item_key_);
	};

	virtual ~hash_base(){}

	list_head item_;
	list_head item_key_;
	uint32_t  key_;

private:
	hash_base(const hash_base &base_);

	void operator = (const hash_base &base_);	
};

static inline uint32_t hashfn(uint32_t unKey, uint32_t unHashSize)
{
	unKey ^= (unKey >> 16);
	unKey ^= (unKey >> 8);
	return (unKey % unHashSize);
}

static inline uint64_t hashfn_64(const uint64_t &val, const uint64_t &unHashSize)
{
    uint64_t hash = val;

    /*  Sigh, gcc can't optimise this alone like it does for 32 bits. */
    uint64_t n = hash;
    n <<= 18;
    hash -= n;
    n <<= 33;
    hash -= n;
    n <<= 3;
    hash += n;
    n <<= 3;
    hash -= n;
    n <<= 4;
    hash += n;
    n <<= 2;
    hash += n;

    return (hash % unHashSize);
}


template <class type_>
class list_lru_hash
{
public:
	list_lru_hash(void);
	~list_lru_hash(void){};

	bool insert( uint32_t key_, type_ *value_);
	type_ *find(uint32_t key_);
	bool move_to_tail(type_ *value_);
	bool remove(uint32_t key_);
	bool remove(type_ * value_);
	inline list_head * begin(){return &list_item_;};

	// for 64 bit key
	bool remove(const uint64_t &key_);
	type_ *find(const uint64_t &key_);
	bool insert(const uint64_t &key_, type_ *value_);

private:
	list_head headBucket_[DEF_MAX_HASH_NUM];
	list_head list_item_;
};


template <class type_>
bool list_lru_hash<type_>::remove( uint32_t key_ )
{
	type_ * value_ = find(key_);
	if (value_ != NULL)
	{
		list_del(&value_->item_);
		list_del(&value_->item_key_);

		return true;
	}

	return false;
}

template <class type_>
bool list_lru_hash<type_>::remove(type_ * value_)
{
	if (value_ != NULL)
	{
		list_del(&value_->item_);
		list_del(&value_->item_key_);

		return true;
	}

	return false;
}


template <class type_>
bool list_lru_hash<type_>::move_to_tail( type_ *value_ )
{
	list_del(&value_->item_);
	list_add_tail(&value_->item_, &list_item_);

	return true;
}

template <class type_>
type_ * list_lru_hash<type_>::find( uint32_t key_ )
{
	list_head* pPos;
	uint32_t unIndex = hashfn(key_, DEF_MAX_HASH_NUM);
	list_head* pHead = &headBucket_[unIndex];
	list_for_each(pPos, pHead)
	{
		if (pPos == NULL) 
		{
			return NULL;
		}
		type_* p = list_entry(pPos, type_, item_key_);
		if (p->key_ == key_)
		{
			return p;
		}
	}
	return NULL;
}

template <class type_>
bool list_lru_hash<type_>::insert( uint32_t key, type_ *value_ )
{
	uint32_t unIndex = hashfn(key, DEF_MAX_HASH_NUM);
	list_head* pHead = &headBucket_[unIndex];
	list_add_tail(&(value_->item_key_), pHead);

	value_->key_ = key;

	move_to_tail(value_);
	return true;
}

// for 64 bit key
template <class type_>
bool list_lru_hash<type_>::remove( const uint64_t &key_ )
{
	type_ * value_ = find(key_);
	if (value_ != NULL)
	{
		list_del(&value_->item_);
		list_del(&value_->item_key_);

		return true;
	}

	return false;
}

template <class type_>
type_ * list_lru_hash<type_>::find( const uint64_t &key_ )
{
	list_head* pPos;
	uint64_t unIndex = hashfn_64(key_, DEF_MAX_HASH_NUM);
	list_head* pHead = &headBucket_[unIndex];
	list_for_each(pPos, pHead)
	{
		type_* p = list_entry(pPos, type_, item_key_);
		if (p->key_ == key_)
		{
			return p;
		}
	}
	return NULL;
}

template <class type_>
bool list_lru_hash<type_>::insert( const uint64_t &key, type_ *value_ )
{
	uint64_t unIndex = hashfn_64(key, DEF_MAX_HASH_NUM);
	list_head* pHead = &headBucket_[unIndex];
	list_add_tail(&(value_->item_key_), pHead);

	value_->key_ = key;

	move_to_tail(value_);
	return true;
}



template <class type_>
list_lru_hash<type_>::list_lru_hash( void )
{
	for (uint64_t i = 0; i < DEF_MAX_HASH_NUM; i++)
	{
		INIT_LIST_HEAD(headBucket_ + i);
	}

	INIT_LIST_HEAD(&list_item_);
}



template <class type_>
class list_hash
{
public:
	list_hash(void);
	~list_hash(void){};

	bool insert( uint32_t key_, type_ *value_);
	type_ *find(uint32_t key_);
	bool remove(uint32_t key_);

	// for 64 bit key
	bool insert(const uint64_t &key_, type_ *value_);
	type_ *find(const uint64_t &key_);
	bool remove(const uint64_t &key_);

private:
	list_head headBucket_[DEF_MAX_HASH_NUM];
};

template <class type_>
bool list_hash<type_>::remove( uint32_t key_ )
{
	type_ * value_ = find(key_);
	if (value_ != NULL)
	{
		list_del(&value_->item_key_);
		return true;
	}

	return false;
}

template <class type_>
type_ * list_hash<type_>::find( uint32_t key_ )
{
	list_head* pPos;
	uint32_t unIndex = hashfn(key_, DEF_MAX_HASH_NUM);
	list_head* pHead = &headBucket_[unIndex];
	list_for_each(pPos, pHead)
	{
		type_* p = list_entry(pPos, type_, item_key_);
		if (p->key_ == key_)
		{
			return p;
		}
	}

	return NULL;
}

template <class type_>
bool list_hash<type_>::insert( uint32_t key_, type_ *value_ )
{
	uint32_t unIndex = hashfn(key_, DEF_MAX_HASH_NUM);
	list_add_tail(&(value_->item_key_), &(headBucket_[unIndex]));

	value_->key_ = key_;

	return true;
}

// for 64 bit key
template <class type_>
bool list_hash<type_>::remove( const uint64_t &key_ )
{
	type_ * value_ = find(key_);
	if (value_ != NULL)
	{
		list_del(&value_->item_key_);
		return true;
	}

	return false;
}

template <class type_>
type_ * list_hash<type_>::find( const uint64_t &key_ )
{
	list_head* pPos;
	uint64_t unIndex = hashfn_64(key_, DEF_MAX_HASH_NUM);
	list_head* pHead = &headBucket_[unIndex];
	list_for_each(pPos, pHead)
	{
		type_* p = list_entry(pPos, type_, item_key_);
		if (p->key_ == key_)
		{
			return p;
		}
	}

	return NULL;
}

template <class type_>
bool list_hash<type_>::insert( const uint64_t &key_, type_ *value_ )
{
	uint64_t unIndex = hashfn_64(key_, DEF_MAX_HASH_NUM);
	list_add_tail(&(value_->item_key_), &(headBucket_[unIndex]));

	value_->key_ = key_;

	return true;
}


template <class type_>
list_hash<type_>::list_hash( void )
{
	for (uint64_t i = 0; i < DEF_MAX_HASH_NUM; i++)
	{
		INIT_LIST_HEAD(headBucket_ + i);
	}
}


#endif // msg_manager_h__
