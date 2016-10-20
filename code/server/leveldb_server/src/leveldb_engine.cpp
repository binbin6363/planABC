/**
 * @filedesc: 
 * leveldb_engine.cpp, handle client action
 * @author: 
 *  bbwang
 * @date: 
 *  2014/12/3 12:02:59
 * @modify:
 *
**/

#include "leveldb_engine.h"

#include "leveldb/write_batch.h"
#include "leveldb/env.h"
#include "leveldb/iterator.h"
#include "leveldb/cache.h"
#include "leveldb/filter_policy.h"

#include "serverconfig.h"
#include "log.h"
#include "binlog.h"
#include "nobinlog_iterator.h"


using namespace utils;

#define SAFE_CHECK_DB(DB) {\
    if (NULL == DB)\
    {\
        LOG(ERROR)("db is null.");\
        return LEVELDB_INNER_ERR;\
    }else if (db_opened_ == 0) {\
        LOG(ERROR)("db not open.");\
        return LEVELDB_INNER_ERR;\
    }\
}

LeveldbEngine::LeveldbEngine()
    : db_(NULL)
    , binlog_queue_(NULL)
    , log_type_(BinlogType::SYNC)
    , db_opened_(0)
{
    options_.block_cache = NULL;
    options_.filter_policy = NULL;
}

LeveldbEngine::~LeveldbEngine()
{
    closeDb();
    LOG(INFO)("delete leveldb.");
}



//  leveldb 的默认值:
//      create_if_missing(false),
//      error_if_exists(false),
//      paranoid_checks(false),
//      env(Env::Default()),
//      info_log(NULL),
//      write_buffer_size(4<<20),
//      max_open_files(1000),
//      block_cache(NULL),
//      block_size(4096),
//      block_restart_interval(16),
//      options_.block_cache = NULL;
//      options_.filter_policy = NULL;
//      options_.compression = leveldb::kNoCompression;
void LeveldbEngine::initCfg()
{
    // make sure reset default
    clearCfg();
    
    dbpath_ = utils::ServerConfig::Instance()->dbpath();
    if (!strcasecmp("false", utils::ServerConfig::Instance()->ldb_create_if_missing()))
    {
        // default false.
        LOG(INFO)("leveldb set config create_if_missing:false");
        options_.create_if_missing = false;
    }
    if (!strcasecmp("true", utils::ServerConfig::Instance()->ldb_error_if_exists()))
    {
        // default false.
        LOG(INFO)("leveldb set config error_if_exists:true");
        options_.error_if_exists = true;
    }
    if (!strcasecmp("true", utils::ServerConfig::Instance()->ldb_paranoid_checks()))
    {
        // default false.
        LOG(INFO)("leveldb set config paranoid_checks:true");
        options_.paranoid_checks = true;
    }
    if (0 != utils::ServerConfig::Instance()->ldb_write_buffer_size())
    {
        // default 4MB.
        options_.write_buffer_size = utils::ServerConfig::Instance()->ldb_write_buffer_size();
        LOG(INFO)("leveldb set config write_buffer_size:%zu", options_.write_buffer_size);
    }
    if (0 != utils::ServerConfig::Instance()->ldb_max_open_files())
    {
        // default 1000.
        options_.max_open_files = utils::ServerConfig::Instance()->ldb_max_open_files();
    }
    if (0 != utils::ServerConfig::Instance()->ldb_block_size())
    {
        // default 4KB.
        options_.block_size = utils::ServerConfig::Instance()->ldb_block_size();
    }
    if (0 != utils::ServerConfig::Instance()->ldb_block_restart_interval())
    {
        // default 16.
        options_.block_restart_interval = utils::ServerConfig::Instance()->ldb_block_restart_interval();
    }

    int bits_per_key = utils::ServerConfig::Instance()->ldb_bits_per_key();
    if (0 != bits_per_key)
    {
        // default 10.
     	options_.filter_policy = leveldb::NewBloomFilterPolicy(bits_per_key);
    }

    int cache_size = utils::ServerConfig::Instance()->ldb_cache_size();
    if (0 != cache_size)
    {
        // default 8m.
        options_.block_cache = leveldb::NewLRUCache(cache_size);
    }

//    compression type enum define as follows
//    kNoCompression     = 0x0,
//    kSnappyCompression = 0x1
//    default type: kNoCompression
    int compression_type = utils::ServerConfig::Instance()->ldb_compression_type();
    if (leveldb::kNoCompression == compression_type)
    {
		options_.compression = leveldb::kNoCompression;
    }
    else if (leveldb::kSnappyCompression == compression_type)
    {
		options_.compression = leveldb::kSnappyCompression;
    }
    else
    {
        LOG(WARN)("unsupported compression type: %d.", compression_type);
    }
    
    LOG(INFO)("load leveldb config.");
    
}


void LeveldbEngine::clearCfg()
{    
    options_.create_if_missing = true;
    options_.error_if_exists = false;
    options_.paranoid_checks = false;
    options_.write_buffer_size = 0;
    options_.max_open_files = 0;
    options_.block_size = 0;
    options_.block_restart_interval = 0;
	SAFE_DELETE(options_.block_cache)
	SAFE_DELETE(options_.filter_policy)
    options_.compression = leveldb::kNoCompression;
}

int LeveldbEngine::reStartDb()
{
    int ret = 0;

    ret = closeDb();
    
    initCfg();
    
    ret |= openDb();
    
    LOG(INFO)("restart leveldb!");
    return ret;
}

int LeveldbEngine::startDb()
{
    int ret = 0;
    
    initCfg();
    
    ret = openDb();
    
    LOG(INFO)("start leveldb!");
    return ret;
}

int LeveldbEngine::openDb()
{
    int ret = 0;
    LOG(INFO)("start to open leveldb...");
    if (1 == db_opened_)
    {
        LOG(ERROR)("db already opened!");
        return -1;
    }
    if (!dbpath_.empty())
    {
        status_ = leveldb::DB::Open(options_
            , dbpath_, &db_);
        if(!status_.ok())
        {
            ret = -1;
            LOG(ERROR)("open level db file failed. db:%s, msg:%s"
                , dbpath_.c_str(), status_.ToString().c_str());
        }
        else
        {
            bool write_log = utils::ServerConfig::Instance()->write_log();
            LOG(INFO)("open level db file succeed. db:%s, msg:%s, binlog:%s"
                , dbpath_.c_str(), status_.ToString().c_str(), (write_log) ? "true" : "false");
            binlog_queue_ = new BinlogQueue(db_, write_log);
            db_opened_ = 1;
        }
    }
    return ret;
}
    
int LeveldbEngine::closeDb()
{
    LOG(INFO)("start to close leveldb...");
    
    db_opened_ = 0;
    
    SAFE_DELETE(db_);
    
    SAFE_DELETE(binlog_queue_);

	SAFE_DELETE(options_.block_cache);

	SAFE_DELETE(options_.filter_policy);
    LOG(INFO)("done close leveldb!");
    return 0;
}

int LeveldbEngine::BatchGet(const strvec &keys, strmap &kv)
{
    int ret = 0;
    SAFE_CHECK_DB(db_)
    int ok_num = 0;
    std::string result;
    svciter iter = keys.begin();
    svciter iter_end = keys.end();
    for ( ; iter != iter_end; ++iter)
    {
        status_ = db_->Get(leveldb::ReadOptions(), leveldb::Slice(*iter), &result);
        if (status_.ok())
        {
            LOG(DEBUG)("batch get, key:%s, value:", iter->c_str());
            LOG_HEX(result.c_str(), result.length(), utils::L_DEBUG);
            kv.insert(std::make_pair(*iter, result));
            ++ok_num;
            continue;
        }
        else if (status_.IsNotFound())
        {
            kv.insert(std::make_pair(*iter, ""));
            LOG(DEBUG)("batch get, ret:%d, key:%s, msg:%s", ret, iter->c_str(), status_.ToString().c_str());
            continue;
        }
        else if (status_.IsCorruption())
        {
            ret |= CORRUPTION;
        }
        else if (status_.IsIOError())
        {
            ret |= IOERROR;
        }
        else
        {
            ret |= OTHERERROR;
        }
        LOG(ERROR)("batch get, ret:%d, key:%s, msg:%s", ret, iter->c_str(), status_.ToString().c_str());
        kv.insert(std::make_pair(*iter, ""));

    }
    LOG(INFO)("batch get, ret:%d, get succeed num:%u, total num:%zu", ret, ok_num, keys.size());
    return ret;
}

// get key-value by prefix or whole key range
int LeveldbEngine::GetByRange(const std::string &start, const std::string &end, strmap &kv, uint32_t &end_flag, unsigned int limit)
{
    int ret = 0;
    end_flag = 1;
    unsigned int cnt = 0;
    SAFE_CHECK_DB(db_)
    // 针对大块文件的读写遍历等需求，为了避免读入的块把之前的热数据都淘汰掉，
    // 在ReadOptions里设置不需要进cache
    leveldb::ReadOptions options;
    options.fill_cache = false;
    leveldb::Iterator* it = db_->NewIterator(options);
    for (it->Seek(start); it->Valid() && it->key().ToString() < end; it->Next()) 
    {
        if (cnt >= limit) // 超过limit就需要下次再请求
        {
            end_flag = 0;
            LOG(INFO)("GetByRange, total result cnt is larger than limit:%u, need continue request.", limit);
            break;
        }
        LOG(DEBUG)("get key:%s", it->key().data());
        kv.insert(std::make_pair(it->key().ToString(), it->value().ToString()));
        ++cnt;
    }
    if (kv.empty())
    {
        LOG(WARN)("not found key-value in range [%s, %s)", start.c_str(), end.c_str());
    }
    if (!it->status().ok())
    {
        LOG(ERROR)("something error occur during get by range.");
        ret = LEVELDB_INNER_ERR;
    }
    delete it;
    return ret;
}

// get key-value by prefix or whole key range in reverse
// assert(start < end)
int LeveldbEngine::GetByRangeInReverse(const std::string &start, const std::string &end, strmap &kv, uint32_t &end_flag, unsigned int limit)
{
    int ret = 0;
    end_flag = 1;
    unsigned int cnt = 0;
    SAFE_CHECK_DB(db_)
    // 针对大块文件的读写遍历等需求，为了避免读入的块把之前的热数据都淘汰掉，
    // 在ReadOptions里设置不需要进cache
    leveldb::ReadOptions options;
    options.fill_cache = false;
    leveldb::Iterator* it = db_->NewIterator(options);
    // Seek
    // Position at the first key in the source that at or past target
    // The iterator is Valid() after this call iff the source contains
    // an entry that comes at or past target.
    for (it->Seek(end); it->Valid() && (it->key().ToString() > start); it->Prev()) 
    {
        if (it->key().ToString() > end)
        {
            LOG(DEBUG)("skip value:%s which past target:%s.", it->key().data(), end.c_str());
            continue;
        }
        if (cnt >= limit) // 超过limit就需要下次再请求
        {
            end_flag = 0;
            LOG(INFO)("GetByRangeInReverse, total result cnt is larger than limit:%u, need continue request.", limit);
            break;
        }
        LOG(DEBUG)("get key:%s", it->key().data());
        kv.insert(std::make_pair(it->key().ToString(), it->value().ToString()));
        ++cnt;
    }
    if (kv.empty())
    {
        LOG(WARN)("not found key-value in range [%s, %s)", start.c_str(), end.c_str());
    }
    if (!it->status().ok())
    {
        LOG(ERROR)("something error occur during get by range.");
        ret = LEVELDB_INNER_ERR;
    }
    delete it;
    return ret;
}


int LeveldbEngine::DelByRange(const std::string &start, const std::string &end)
{
    int ret = 0;
    uint32_t total_count = 0;
    bool stop = false;

    string key_iter = start;

    leveldb::Slice del_start = leveldb::Slice(start);
    leveldb::Slice del_end = leveldb::Slice(end);

    while(key_iter < end){
        uint32_t count = 0;
        leveldb::ReadOptions options;
        options.fill_cache = false;
        leveldb::Iterator* it = db_->NewIterator(options);
        // 边查边删就是要格外注意迭代器失效的问题
        // must be Seek(key_iter)!!!
        for (it->Seek(key_iter); it->Valid() && (it->key().compare(del_end)< 0) && (count < 1000); it->Next(), ++count) 
        {
            del_start = it->key();
            // copy it to string
            key_iter = del_start.ToString();
            binlog_queue_->Delete(del_start);
            binlog_queue_->add_log(log_type_, BinlogCommand::KDEL, del_start);
            LOG(INFO)("del key:'%s'", key_iter.c_str());
        }
        total_count += count;
        // 迭代器判断需要在commit之前，防止commit之后迭代器失效
        if (!it->Valid())
        {
            LOG(DEBUG)("range del, iterator invalid, stop del.");
            stop = true;
        }
        delete it;
        it = NULL;
        
        leveldb::Status s = binlog_queue_->commit();
        if(!s.ok()){
            LOG(DEBUG)("range del, commit status not ok, stop del.");
            ret = -1;
            stop = true;
        }
        
        if (stop)
        {
            LOG(DEBUG)("range del, break active.");
            break;
        }
	}

    LOG(INFO)("done range del, delete num:%u, ret:%d, key range:[%s, %s)"
        , total_count, ret, key_iter.c_str(), end.c_str());
	return ret;

}


int LeveldbEngine::Put(const string &k, const string &v)
{
    int ret = 0;
    SAFE_CHECK_DB(db_)
	Transaction trans(binlog_queue_);
    
	binlog_queue_->Put(leveldb::Slice(k), leveldb::Slice(v));
	binlog_queue_->add_log(log_type_, BinlogCommand::KSET, leveldb::Slice(k));

    leveldb::Status s = binlog_queue_->commit();
	if(!s.ok()){
		LOG(ERROR)("Put error: %s", s.ToString().c_str());
        ret = LEVELDB_INNER_ERR;
	}
    return ret;
}

int LeveldbEngine::Delete(const string &k)
{
    int ret = 0;
    SAFE_CHECK_DB(db_)
	Transaction trans(binlog_queue_);
    
	binlog_queue_->Delete(leveldb::Slice(k));
	binlog_queue_->add_log(log_type_, BinlogCommand::KDEL, leveldb::Slice(k));

    leveldb::Status s = binlog_queue_->commit();
	if(!s.ok()){
		LOG(ERROR)("Delete error: %s", s.ToString().c_str());
        ret = LEVELDB_INNER_ERR;
	}
    LOG(INFO)("del key:'%s'", k.c_str());
    return ret;
}

int LeveldbEngine::BatchPut(const strmap &kv)
{
    int ret = 0;
    SAFE_CHECK_DB(db_)
	Transaction trans(binlog_queue_);

    smciter iter = kv.begin();
    smciter iter_end = kv.end();
    leveldb::Slice key_slice, value_slice;
    for ( ; iter != iter_end; ++iter)
    {
        key_slice = leveldb::Slice(iter->first.c_str(), iter->first.length());
        value_slice = leveldb::Slice(iter->second.c_str(), iter->second.length());
    	binlog_queue_->Put(key_slice, value_slice);
    	binlog_queue_->add_log(log_type_, BinlogCommand::KSET, key_slice);
    }
	leveldb::Status s = binlog_queue_->commit();
	if(!s.ok()){
		LOG(ERROR)("BatchPut error: %s", s.ToString().c_str());
        ret = LEVELDB_INNER_ERR;
	}
    return ret;
}


int LeveldbEngine::BatchDelete(const strvec &keys)
{
    int ret = 0;
    SAFE_CHECK_DB(db_)
	Transaction trans(binlog_queue_);

    svciter iter = keys.begin();
    svciter iter_end = keys.end();
    leveldb::Slice key_slice;
    for ( ; iter != iter_end; ++iter)
    {
        key_slice = leveldb::Slice(*iter);
    	binlog_queue_->Delete(key_slice);
    	binlog_queue_->add_log(log_type_, BinlogCommand::KDEL, key_slice);
    }
	leveldb::Status s = binlog_queue_->commit();
	if(!s.ok()){
		LOG(ERROR)("BatchDelete error: %s", s.ToString().c_str());
        ret = LEVELDB_INNER_ERR;
	}
    return ret;
}

// 清空db数据，危险等级，极高
// 亲，这个接口不是闹着玩的，执行之后你将一无所有
int LeveldbEngine::Flushdb()
{
    LOG(WARN)("flush db! you will have nothing after this operation!");
	int ret = 0;
	bool stop = false;
	while(!stop){
		leveldb::Iterator *it;
		leveldb::ReadOptions iterate_options;
		iterate_options.fill_cache = false;
		leveldb::WriteOptions write_opts;

		it = db_->NewIterator(iterate_options);
		it->SeekToFirst();
		for(int i=0; i < 10000; i++){
			if(!it->Valid()){
				stop = true;
				break;
			}
			leveldb::Status s = db_->Delete(write_opts, it->key());
			if(!s.ok()){
				LOG(ERROR)("del error: %s", s.ToString().c_str());
				stop = true;
				ret = -1;
				break;
			}
			it->Next();
		}
		delete it;
	}
	binlog_queue_->flush();
	return ret;
}

int LeveldbEngine::GetProperty(const std::string& property, std::string* value)
{
    int ret = 0;
    SAFE_CHECK_DB(db_)
    std::string status("ok");
    leveldb::Slice slice_property(property);
    if (!db_->GetProperty(slice_property, value))
    {
        ret = -1;
        status = "failed";
    }
    LOG(DEBUG)("get property:%s %s.", slice_property.data(), status.c_str());
    return ret;
}

// ===============================    
// raw operate, no operate log
// ===============================    
int LeveldbEngine::raw_set(const string &key, const string &val){
    SAFE_CHECK_DB(db_)

	leveldb::WriteOptions write_opts;
	leveldb::Status s = db_->Put(write_opts, leveldb::Slice(key), leveldb::Slice(val));
	if(!s.ok()){
		LOG(ERROR)("set error: %s", s.ToString().c_str());
		return -1;
	}
	return 0;
}

int LeveldbEngine::raw_del(const string &key){
    SAFE_CHECK_DB(db_)
	leveldb::WriteOptions write_opts;
	leveldb::Status s = db_->Delete(write_opts, leveldb::Slice(key));
	if(!s.ok()){
		LOG(ERROR)("del error: %s", s.ToString().c_str());
		return -1;
	}
	return 0;
}

int LeveldbEngine::raw_get(const string &key, string *val){
    SAFE_CHECK_DB(db_)
	leveldb::ReadOptions opts;
	opts.fill_cache = false;
	leveldb::Status s = db_->Get(opts, leveldb::Slice(key), val);
	if(s.IsNotFound()){
		LOG(DEBUG)("raw_get, not found key:%s", key.c_str());
		return 1;
	}
	if(!s.ok()){
		LOG(ERROR)("get error: %s", s.ToString().c_str());
		return -1;
	}
	return 0;
}

// seek到大于start的第一个位置
NoBinlogIterator *LeveldbEngine::iterator(const std::string &start, const std::string &end, uint64_t limit)
{
	leveldb::Iterator *it;
	leveldb::ReadOptions iterate_options;
	iterate_options.fill_cache = false;
	it = db_->NewIterator(iterate_options);
    if (start.empty()) {
        LOG(INFO)("start is empty, seek to first in the db.");
        it->SeekToFirst();
        return new NoBinlogIterator(it, end, limit);
    }
	it->Seek(start);
    // TODO:这块不知道原作者要闹哪样
//	if(it->Valid() && it->key() == start){
//		it->Next();
//	}
    return new NoBinlogIterator(it, end, limit);
}


uint64_t LeveldbEngine::get_min_seq()
{
    if (NULL == binlog_queue_ ) {
        return 0;
    }
    return binlog_queue_->get_min_seq();
}

uint64_t LeveldbEngine::get_lastest_seq()
{
    if (NULL == binlog_queue_ ) {
        return 0;
    }
    return binlog_queue_->get_last_seq();
}



#ifdef TEST
int main(int argc, char *argv[])
{
    int ret = 0;
    if (argc != 4)
    {
        printf("usage: %s cfg.xml\n", argv[0]);
        return 0;
    }
    utils::ServerConfig::Instance()->load_file(argv[1]);
    utils::ServerConfig::Instance()->init_log();
    utils::ServerConfig::Instance()->load_config();
    std::string start = argv[2];
    std::string end = argv[3];
    strmap kv;
    uint32_t endflag = 0;
    LeveldbEngine::inst().GetByRange(start, end, kv, endflag);
    smiter start_iter = kv.begin();
    smiter end_iter = kv.end();
    for (; start_iter != end_iter; ++start_iter)
    {
        fprintf(stdout, "show get key:%s", start_iter->first.c_str());
    }
    
    return ret;
}
#endif



