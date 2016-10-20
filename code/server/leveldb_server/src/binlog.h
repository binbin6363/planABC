/*
Copyright (c) 2012-2014 The SSDB Authors. All rights reserved.
Use of this source code is governed by a BSD-style license that can be
found in the LICENSE file.
*/
// modify on ssdb source code, bbwang, 2015/8/18

#ifndef LEVELDB_BINLOG_H_
#define LEVELDB_BINLOG_H_

#include <string>
#include "leveldb/db.h"
#include "leveldb/options.h"
#include "leveldb/slice.h"
#include "leveldb/status.h"
#include "leveldb/write_batch.h"
#include "constants.h"
#include "const.h"
#include "log.h"
#include "thread_mutex.h"


using namespace common;
using namespace utils;



class Binlog
{

public:
	Binlog(){}
	Binlog(uint64_t seq, char type, char cmd, const leveldb::Slice &key);
		
	int load(const leveldb::Slice &s);
	int load(const std::string &s);

	uint64_t seq() const;
	char type() const;
	char cmd() const;
	const std::string key() const;

	const char* data() const{
		return buf.data();
	}
	int size() const{
		return (int)buf.size();
	}
	const std::string repr() const{
		return this->buf;
	}
	std::string dumps() const;

private:
	std::string buf;
	static const unsigned int HEADER_LEN = sizeof(uint64_t) + 2;

};

// circular queue
class BinlogQueue
{
public:
//  这个的调用本身是单线程的，去掉互斥锁
//	Thread_Mutex mutex;

	BinlogQueue(leveldb::DB *db, bool enabled=true);
	~BinlogQueue();
	void begin();
	void rollback();
	leveldb::Status commit();
	// leveldb put
	void Put(const leveldb::Slice& key, const leveldb::Slice& value);
	// leveldb delete
	void Delete(const leveldb::Slice& key);
	void add_log(char type, char cmd, const leveldb::Slice &key);
	void add_log(char type, char cmd, const std::string &key);
		
	int get(uint64_t seq, Binlog *log) const;
	int update(uint64_t seq, char type, char cmd, const std::string &key);
		
	void flush();
		
	/** @returns
	 1 : log.seq greater than or equal to seq
	 0 : not found
	 -1: error
	 */
	int find_next(uint64_t seq, Binlog *log) const;
    int find_next_fuzzy(uint64_t next_seq, Binlog *log) const;
	int find_last(Binlog *log) const;

    // 对外暴露seq值，不可修改
    const uint64_t &get_min_seq() const { return min_seq;}
    const uint64_t &get_last_seq() const { return last_seq;}
		
	std::string stats() const;

private:
	leveldb::DB *db;
	uint64_t min_seq;
	uint64_t last_seq;
	uint64_t tran_seq;
	int capacity;
	leveldb::WriteBatch batch;

	volatile bool thread_quit;
	static void* log_clean_thread_func(void *arg);
	int del(uint64_t seq);
	// [start, end] includesive
	int del_range(uint64_t start, uint64_t end);
		
	void merge();
	bool enabled;
};



class Transaction
{
public:
	Transaction(BinlogQueue *logs){
		this->logs = logs;
        // 这个的调用本身是单线程的，去掉互斥锁
		//logs->mutex.acquire();
		logs->begin();
	}
	
	~Transaction(){
		// it is safe to call rollback after commit
		logs->rollback();
		//logs->mutex.release();
	}

private:
	BinlogQueue *logs;
};



#endif
