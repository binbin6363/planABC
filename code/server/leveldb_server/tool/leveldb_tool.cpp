/**
 * @filedesc: 
 * leveldb_tool.cpp, tool for db
 * @author: 
 *  bbwang
 * @date: 
 *  2015/9/13 9:02:59
 * @modify:
 *
**/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include "leveldb/db.h"
#include "leveldb/iterator.h"
#include "leveldb/cache.h"

using namespace std;

/**********************************************************************
db数据一致性比较工具,需要停机做比较
**********************************************************************/
int CompareDbAndShowDiff(int argc, char *argv[]);

void initOptions();
void help(int argc, char *argv[]);
void skipNext(leveldb::Iterator *it);
int compareIter(leveldb::Iterator *it1, leveldb::Iterator *it2);
void recordIt(const string &path, leveldb::Iterator *it, FILE *fp);
    
leveldb::Options options;

int main(int argc, char *argv[])
{
    int ret = 0;
    if (3 != argc) {
        help(argc, argv);
        return ret;
    }
    ret = CompareDbAndShowDiff(argc, argv);
    return ret;
}

void initOptions()
{
    options.create_if_missing = false;
    options.error_if_exists = false;
    options.max_open_files = 10;
}


void help(int argc, char *argv[])
{
    fprintf(stdout, "leveldb db tool usage:\n");
    fprintf(stdout, "\t%s db1 db2\n", argv[0]);
}

int CompareDbAndShowDiff(int argc, char *argv[])
{
    int ret = 0;
    string result_file("leveldb_compare_result.txt");
    leveldb::DB *db1 = NULL;
    leveldb::DB *db2 = NULL;
    leveldb::Iterator *it1 = NULL;
    leveldb::Iterator *it2 = NULL;
    leveldb::ReadOptions read_options;
    read_options.fill_cache = false;
    string db1_path = argv[1];
    string db2_path = argv[2];
    FILE *fp = NULL;
    uint64_t cmp_record_cnt = 0;

    do {
        // 打开对比结果文件
        fp = fopen(result_file.c_str(), "a+");
        if (NULL == fp) {
            fprintf(stderr, "open result file failed.\n");
            ret = 1000;
            break;
        }
        
        char content[1024] = {0};
        uint32_t now = time(NULL);
        int wn = sprintf(content, "leveldb tool write this file at:%u\n", now);
        fwrite(content, wn, 1, fp);
        fflush(fp);
        
        // 打开db
        leveldb::Status status1 = leveldb::DB::Open(options, db1_path, &db1);
        leveldb::Status status2 = leveldb::DB::Open(options, db2_path, &db2);
        if (!status1.ok() || !status2.ok()) {
            fprintf(stderr, "open db failed. db1 status:%s, db2 status:%s, exit ...\n"
                , status1.ToString().c_str(), status2.ToString().c_str());
            ret = 2000;
            break;
        }
        it1 = db1->NewIterator(read_options);
        it2 = db2->NewIterator(read_options);
        // SeekToFirst take times
        it1->SeekToFirst();
        it2->SeekToFirst();
        
        // compare,分别向后遍历
        for (; ; ) {
            ++cmp_record_cnt;
            if (cmp_record_cnt % 10000 == 0) {
                fprintf (stdout, "already compare record:%lu\n", cmp_record_cnt);
                fflush(stdout);
            }
            if (!it1->Valid() && !it2->Valid()) {
                fprintf(stdout, "done write diff data, all data cnt:%lu.\n", cmp_record_cnt);
                break;
            }
            int cv = compareIter(it1, it2);
            if (cv == 0) {
                skipNext(it1);
                skipNext(it2);
            } else if (cv > 0) {
                // it1比it2大，说明db2数据多了这条，记录这条多的
                fprintf(stdout, "db2 more a record.\n");
                recordIt(db2_path, it2, fp);
                skipNext(it2);
            } else {
                // it1比it2小，说明db1数据多了这条，记录这条多的
                fprintf(stdout, "db1 more a record.\n");
                recordIt(db1_path, it1, fp);
                skipNext(it1);
            }
        }
        
    }while(0);
    
    // clear mem
    if (fp) {
        fclose(fp);
    }
    if (it1) {
        delete it1;
    }
    if (it2) {
        delete it2;
    }
    if (db1) {
        delete db1;
    }
    if (db2) {
        delete db2;
    }
}


void skipNext(leveldb::Iterator *it)
{
    if (NULL == it) {
        return ;
    }
    while (true) {
        it->Next();
        if (it->Valid() && it->key()[0] == '~') {
            continue;
        }
        break;
    }
}

int compareIter(leveldb::Iterator *it1, leveldb::Iterator *it2)
{
    if (it1->Valid() && it2->Valid()) {
        return (it1->key().compare(it2->key()));
    }
    if (!it1->Valid()) {
        return 1;
    } else if (!it2->Valid()) {
        return -1;
    }
}


void recordIt(const string &path, leveldb::Iterator *it, FILE *fp)
{
    string content("+");
    content.append(path);
    content.append(" : ");
    content.append(it->key().data(), it->key().size());
    content.append(" -> ");
    content.append(it->value().data(), it->value().size());
    content.append("\n");
    fwrite(content.c_str(), content.size(), 1, fp);
    fflush(fp);
}
