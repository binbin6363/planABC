/**
 * @filedesc: 
 * leveldb_dump.h, handle client action
 * @author: 
 *  bbwang
 * @date: 
 *  2014/12/3 12:02:59
 * @modify:
 *
**/
#ifndef __LEVELDB_DUMP_H__
#define __LEVELDB_DUMP_H__

#include <string>

#define LEVELDB_PLATFORM_POSIX

namespace leveldb {
class Env;

bool DumpFile(Env* env, const std::string& fname, const std::string& tfname);
bool HandleDumpCommand(Env* env, char** files, int num);

bool DumpProperty(const std::string &dump_dir);

}

#endif //__LEVELDB_DUMP_H__

