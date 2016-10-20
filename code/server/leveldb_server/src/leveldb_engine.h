/**
 * @filedesc: 
 * leveldb_engine.h, handle client action
 * @author: 
 *  bbwang
 * @date: 
 *  2014/12/3 12:02:59
 * @modify:
 *
**/
#ifndef __LEVELDB_ENGINE_H__
#define __LEVELDB_ENGINE_H__

#include <string>
#include "leveldb/db.h"
#include "data_type.h"


using namespace std;
using namespace utils;

class NoBinlogIterator;
class BinlogQueue;

// ldb error code enum, bitwise
enum
{
    OK         = 0X00,
    CORRUPTION = (0X01 << 1), // 2
    IOERROR    = (0X01 << 2), // 4
    OTHERERROR = (0X01 << 3), // 8
};


class LeveldbEngine
{

private:
    LeveldbEngine();
    ~LeveldbEngine();
    LeveldbEngine operator =(const LeveldbEngine& other);
    void initCfg();

    void clearCfg();

    int openDb();


public:
    static LeveldbEngine &inst() 
    {
        static LeveldbEngine inst_;
        return inst_;
    }

    int closeDb();
    int startDb();
    int reStartDb();
    int BatchGet(const strvec &keys, strmap &kv);
    int Put(const string &k, const string &v);
    int Delete(const string &k);
    int BatchPut(const strmap &kv);
    int BatchDelete(const strvec &keys);
    int GetProperty(const std::string& property, std::string* value);
    int GetByRange(const std::string &start, const std::string &end, strmap &kv, uint32_t &end_flag, unsigned int limit = 20);
	int GetByRangeInReverse(const std::string &start, const std::string &end, strmap &kv, uint32_t &end_flag, unsigned int limit = 20);
    int DelByRange(const std::string &start, const std::string &end);
    int Flushdb();
    
    int raw_set(const string &key, const string &val);
    int raw_del(const string &key);
    int raw_get(const string &key, string *val);
    
    leveldb::DB *GetDb() {return db_;}
    NoBinlogIterator *iterator(const std::string &start, const std::string &end, uint64_t limit);

    BinlogQueue *Binlogs(){return binlog_queue_;}

    uint64_t get_min_seq();
    uint64_t get_lastest_seq();
private:
    std::string         dbpath_;
    leveldb::DB         *db_;
    leveldb::Status     status_;
    leveldb::Options    options_;
	BinlogQueue         *binlog_queue_;
    uint8_t             log_type_;
    uint8_t             db_opened_;

};


#define LDBINST LeveldbEngine::inst()

#endif //__LEVELDB_ENGINE_H__
