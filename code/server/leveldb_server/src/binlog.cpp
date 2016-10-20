/*
Copyright (c) 2012-2014 The SSDB Authors. All rights reserved.
Use of this source code is governed by a BSD-style license that can be
found in the LICENSE file.
*/
// modify on ssdb source code, bbwang, 2015/8/18
#include "binlog.h"
#include <map>
#include <stdlib.h>
#include <unistd.h>
#include "string_util.h"
#include "constants.h"
#include "timecounter.h"

using namespace utils;
using namespace common;
/* Binlog */

// binlog记录保留的条数，最少10w条，太小会导致异常时频繁全量拷贝
int LOG_QUEUE_SIZE  = 100000;

Binlog::Binlog(uint64_t seq, char type, char cmd, const leveldb::Slice &key){
	buf.append((char *)(&seq), sizeof(uint64_t));
	buf.push_back(type);
	buf.push_back(cmd);
	buf.append(key.data(), key.size());
}

uint64_t Binlog::seq() const{
	return *((uint64_t *)(buf.data()));
}

char Binlog::type() const{
	return buf[sizeof(uint64_t)];
}

char Binlog::cmd() const{
	return buf[sizeof(uint64_t) + 1];
}

const std::string Binlog::key() const{
	return std::string(buf.data() + HEADER_LEN, buf.size() - HEADER_LEN);
}

int Binlog::load(const leveldb::Slice &s){
	if(s.size() < HEADER_LEN){
		return -1;
	}
	buf.assign(s.data(), s.size());
	return 0;
}

int Binlog::load(const std::string &s){
	if(s.size() < HEADER_LEN){
		return -1;
	}
	buf.assign(s.data(), s.size());
	return 0;
}

std::string Binlog::dumps() const{
	std::string str;
	if(buf.size() < HEADER_LEN){
		return str;
	}
	char buf[20];
	snprintf(buf, sizeof(buf), "%lu ", this->seq());
	str.append(buf);

	switch(this->type()){
		case BinlogType::NOOP:
			str.append("noop ");
			break;
		case BinlogType::SYNC:
			str.append("sync ");
			break;
		case BinlogType::MIRROR:
			str.append("mirror ");
			break;
		case BinlogType::COPY:
			str.append("copy ");
			break;
	}
	switch(this->cmd()){
		case BinlogCommand::NONE:
			str.append("none ");
			break;
		case BinlogCommand::KSET:
			str.append("set ");
			break;
		case BinlogCommand::KDEL:
			str.append("del ");
			break;
		case BinlogCommand::HSET:
			str.append("hset ");
			break;
		case BinlogCommand::HDEL:
			str.append("hdel ");
			break;
		case BinlogCommand::ZSET:
			str.append("zset ");
			break;
		case BinlogCommand::ZDEL:
			str.append("zdel ");
			break;
		case BinlogCommand::BEGIN:
			str.append("begin ");
			break;
		case BinlogCommand::END:
			str.append("end ");
			break;
		case BinlogCommand::QPUSH_BACK:
			str.append("qpush_back ");
			break;
		case BinlogCommand::QPUSH_FRONT:
			str.append("qpush_front ");
			break;
		case BinlogCommand::QPOP_BACK:
			str.append("qpop_back ");
			break;
		case BinlogCommand::QPOP_FRONT:
			str.append("qpop_front ");
		case BinlogCommand::QSET:
			str.append("qset ");
			break;
	}
	string b = this->key();
	str.append(b);
	return str;
}


/* SyncLogQueue */
// Binlog的key添加了一个特殊字符串来区分用户的kv和Binlog的kv
// 因为按key遍历同步的时候不希望把Binlog给slave同步过去
// modify by bbwang
static inline std::string encode_seq_key(uint64_t seq){
	seq = StringUtil::big_endian(seq);
	std::string ret;
    ret.push_back('~');
	ret.push_back(DataType::SYNCLOG);
	ret.append((char *)&seq, sizeof(seq));
	return ret;
}

static inline uint64_t decode_seq_key(const leveldb::Slice &key){
	uint64_t seq = 0;
	if(key.size() == (sizeof(uint64_t) + 2) 
        && key.data()[1] == DataType::SYNCLOG
        && key.data()[0] == '~') {
		seq = *((uint64_t *)(key.data() + 2));
		seq = StringUtil::big_endian(seq);
	} else {
        LOG(DEBUG)("decode seq key failed. key:%s", DATA2HEX_STR(key.data(), key.size()));
    }
	return seq;
}

BinlogQueue::BinlogQueue(leveldb::DB *db, bool enabled){
	this->db = db;
	this->min_seq = 0;
	this->last_seq = 0;
	this->tran_seq = 0;
	this->capacity = LOG_QUEUE_SIZE;
	this->enabled = enabled;
	
	Binlog log;
    LOG(DEBUG)("==BinlogQueue, find last seq.");
	if(this->find_last(&log) == 0){
		this->last_seq = log.seq();
	}
	LOG(DEBUG)("==BinlogQueue, find min seq.");
	if(this->find_next_fuzzy(this->min_seq, &log) == 0){
		this->min_seq = log.seq();
	}

	if(this->enabled){
		LOG(INFO)("binlogs capacity: %d, min: %lu, max: %lu,", capacity, min_seq, last_seq);
	}

	// start cleaning thread
	if(this->enabled){
		thread_quit = false;
		pthread_t tid;
		int err = pthread_create(&tid, NULL, &BinlogQueue::log_clean_thread_func, this);
		if(err != 0){
			LOG(ERROR)("can't create thread: %s", strerror(err));
			exit(0);
		}
	}
}

BinlogQueue::~BinlogQueue(){
	if(this->enabled){
		thread_quit = true;
		for(int i=0; i<100; i++){
			if(thread_quit == false){
				break;
			}
			usleep(10 * 1000);
		}
	}
	db = NULL;
}

std::string BinlogQueue::stats() const{
	std::string s;
	s.append("    capacity : " + StringUtil::i32tostr(capacity) + "\n");
	s.append("    min_seq  : " + StringUtil::u64tostr(min_seq) + "\n");
	s.append("    max_seq  : " + StringUtil::u64tostr(last_seq) + "");
	return s;
}

void BinlogQueue::begin(){
	tran_seq = last_seq;
	batch.Clear();
}

void BinlogQueue::rollback(){
	tran_seq = 0;
}

leveldb::Status BinlogQueue::commit(){
	leveldb::WriteOptions write_opts;
	leveldb::Status s = db->Write(write_opts, &batch);
	if(s.ok()){
		last_seq = tran_seq;
		tran_seq = 0;
        LOG(DEBUG)("commit ok. last seq:%lu.", last_seq);
	} else {
        LOG(ERROR)("commit falied. last seq:%lu.", last_seq);
    }
	return s;
}

void BinlogQueue::add_log(char type, char cmd, const leveldb::Slice &key){
	if(!enabled){
		return;
	}
	tran_seq ++;
	Binlog log(tran_seq, type, cmd, key);
	batch.Put(encode_seq_key(tran_seq), log.repr());
}

void BinlogQueue::add_log(char type, char cmd, const std::string &key){
	if(!enabled){
		return;
	}
	leveldb::Slice s(key);
	this->add_log(type, cmd, s);
}

// leveldb put
void BinlogQueue::Put(const leveldb::Slice& key, const leveldb::Slice& value){
	batch.Put(key, value);
}

// leveldb delete
void BinlogQueue::Delete(const leveldb::Slice& key){
	batch.Delete(key);
}

// 精确查找下一条binlog，速度快，成功返回0，反之返回非0
int BinlogQueue::find_next(uint64_t next_seq, Binlog *log) const{
	int ret = 0;
	std::string key_str = encode_seq_key(next_seq);
    std::string value("");

	leveldb::ReadOptions iterate_options;
    iterate_options.fill_cache = false;
    leveldb::Status status = db->Get(iterate_options, leveldb::Slice(key_str), &value);
    if (status.ok())
    {
        LOG(DEBUG)("==found next key:%s.", DATA2HEX_STR(key_str.data(), key_str.size()));
		if(log->load(value) == -1) {
            // 找到了，加载失败，说明找错了
			ret = -1;
            LOG(DEBUG)("find next binlog, load binlog failed.");
		}
    } else {
    	// 没找到
        ret = 1;
        LOG(DEBUG)("find next binlog, not find the binlog.");
    }
	return ret;
}


// 模糊查找下一条binlog，速度慢，成功返回0，反之返回非0
int BinlogQueue::find_next_fuzzy(uint64_t next_seq, Binlog *log) const{
	int ret = 0;
	std::string key_str = encode_seq_key(next_seq);
	leveldb::ReadOptions iterate_options;
    iterate_options.fill_cache = false;
	leveldb::Iterator *it = db->NewIterator(iterate_options);
    LOG(DEBUG)("==find next binlog, seek key:%s, seq:%lu", DATA2HEX_STR(key_str.data(), key_str.size()), next_seq);
	it->Seek(key_str);
	if(it->Valid()) {
		leveldb::Slice key = it->key();
        LOG(DEBUG)("==found next key:%s.", DATA2HEX_STR(key.data(), key.size()));
		if(decode_seq_key(key) != 0) {
			leveldb::Slice val = it->value();
			if(log->load(val) == -1) {
                // 找到了，加载失败，说明找错了
				ret = -1;
                LOG(DEBUG)("find next binlog, load binlog failed.");
			}
		}
	} else {
    	// 没找到
        ret = 1;
        LOG(DEBUG)("find next binlog, not find the binlog.");
    }
	delete it;
	return ret;
}

// 查找最后一条binlog，成功返回0，反之返回非0
// binlog的key是以'~'开头的，而'~'是可显示字符的最大值(leveldb所有key按字典序排序)
// ，所以SeekToLast就是binlog的最后一条
int BinlogQueue::find_last(Binlog *log) const{
	int ret = 0;
	leveldb::ReadOptions iterate_options;
    iterate_options.fill_cache = false;
	leveldb::Iterator *it = db->NewIterator(iterate_options);
    it->SeekToLast();
	if(it->Valid()) {
		leveldb::Slice key = it->key();
        LOG(DEBUG)("==find_last, seek to last key:%s.", DATA2HEX_STR(key.data(), key.size()));
		if(decode_seq_key(key) != 0) {
			leveldb::Slice val = it->value();
			if(log->load(val) == -1) {
                // 找到了，加载失败，说明找错了
				ret = -1;
                LOG(DEBUG)("find last binlog, load binlog failed.");
			}
		} else {
            // 找到了，decode失败，说明找错了
            ret = -1;
            LOG(DEBUG)("find last binlog, found error binlog.");
        }
	} else {
    	// 没找到
        ret = 1;
        LOG(DEBUG)("find last binlog, do not find the binlog.");
    }
	delete it;
	return ret;
}

int BinlogQueue::get(uint64_t seq, Binlog *log) const{
	std::string val;
	leveldb::Status s = db->Get(leveldb::ReadOptions(), encode_seq_key(seq), &val);
	if(s.ok()){
		if(log->load(val) != -1){
			return 1;
		}
	}
	return 0;
}

int BinlogQueue::update(uint64_t seq, char type, char cmd, const std::string &key){
	Binlog log(seq, type, cmd, key);
	leveldb::Status s = db->Put(leveldb::WriteOptions(), encode_seq_key(seq), log.repr());
	if(s.ok()){
		return 0;
	}
	return -1;
}

int BinlogQueue::del(uint64_t seq){
	leveldb::Status s = db->Delete(leveldb::WriteOptions(), encode_seq_key(seq));
	if(!s.ok()){
		return -1;
	}
	return 0;
}

void BinlogQueue::flush(){
	del_range(this->min_seq, this->last_seq);
}

int BinlogQueue::del_range(uint64_t start, uint64_t end){
	while(start <= end){
		leveldb::WriteBatch batch;
		for(int count = 0; start <= end && count < 1000; start++, count++){
			batch.Delete(encode_seq_key(start));
		}
		leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);
		if(!s.ok()){
			return -1;
		}
	}
	return 0;
}

void* BinlogQueue::log_clean_thread_func(void *arg){
	BinlogQueue *logs = (BinlogQueue *)arg;
	LOG(INFO)("enter binlog clean_thread. keep binlog size:%d", LOG_QUEUE_SIZE);
	
	while(!logs->thread_quit){
		if(!logs->db){
			break;
		}
		usleep(100 * 1000);
        
		if(logs->last_seq < logs->min_seq) {
            LOG(ERROR)("last_seq < min_seq, exit clean binlog thread.");
            break;
        }

		if(logs->last_seq - logs->min_seq < LOG_QUEUE_SIZE * 1.1){
			continue;
		}
		
		uint64_t start = logs->min_seq;
		uint64_t end = logs->last_seq - LOG_QUEUE_SIZE;
		logs->del_range(start, end);
		logs->min_seq = end + 1;
		LOG(INFO)("clean %lu logs[%lu ~ %lu], %lu left, max: %lu",
			end-start+1, start, end, logs->last_seq - logs->min_seq + 1, logs->last_seq);
	}
	LOG(DEBUG)("binlog clean_thread quit");
	
	logs->thread_quit = false;
	return (void *)NULL;
}

// TESTING, slow, so not used
void BinlogQueue::merge(){
	std::map<std::string, uint64_t> key_map;
	uint64_t start = min_seq;
	uint64_t end = last_seq;
	int reduce_count = 0;
	int total = 0;
	total = end - start + 1;
	(void)total; // suppresses warning
	LOG(INFO)("merge begin");
	for(; start <= end; start++){
		Binlog log;
		if(this->get(start, &log) == 1){
			if(log.type() == BinlogType::NOOP){
				continue;
			}
			std::string key = log.key();
			std::map<std::string, uint64_t>::iterator it = key_map.find(key);
			if(it != key_map.end()){
				uint64_t seq = it->second;
				this->update(seq, BinlogType::NOOP, BinlogCommand::NONE, "");
				reduce_count ++;
			}
			key_map[key] = log.seq();
		}
	}
	LOG(INFO)("merge reduce %d of %d binlogs", reduce_count, total);
}
