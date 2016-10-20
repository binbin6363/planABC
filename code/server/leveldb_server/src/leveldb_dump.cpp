// Copyright (c) 2012 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "leveldb_dump.h"
#include <stdio.h>
#include <errno.h>
#include <sstream>
#include "db/dbformat.h"
#include "db/filename.h"
#include "db/log_reader.h"
#include "db/version_edit.h"
#include "db/write_batch_internal.h"
#include "leveldb/env.h"
#include "leveldb/iterator.h"
#include "leveldb/options.h"
#include "leveldb/status.h"
#include "leveldb/table.h"
#include "leveldb/write_batch.h"
#include "util/logging.h"
#include "comm.h"
#include "leveldb_engine.h"
#include "log.h"


using namespace utils;

namespace leveldb {


bool GuessType(const std::string& fname, FileType* type) {
  size_t pos = fname.rfind('/');
  std::string basename;
  if (pos == std::string::npos) {
    basename = fname;
  } else {
    basename = std::string(fname.data() + pos + 1, fname.size() - pos - 1);
  }
  uint64_t ignored;
  return ParseFileName(basename, &ignored, type);
}

// Notified when log reader encounters corruption.
class CorruptionReporter : public log::Reader::Reporter {
 public:
  virtual void Corruption(size_t bytes, const Status& status) {
    LOG(DEBUG)("corruption: %d bytes; %s\n",
            static_cast<int>(bytes),
            status.ToString().c_str());
  }
};

// Print contents of a log file. (*func)() is called on every record.
bool PrintLogContents(Env* env, const std::string& fname,
                      void (*func)(Slice, FILE *), 
                      const string &tfpath) {
  SequentialFile* file;
  Status s = env->NewSequentialFile(fname, &file);
  if (!s.ok()) {
    LOG(ERROR)( "%s\n", s.ToString().c_str());
    return false;
  }
  CorruptionReporter reporter;
  log::Reader reader(file, &reporter, true, 0);
  Slice record;
  std::string scratch;

  FILE *fp = fopen(tfpath.c_str(), "w+");
  if (!fp)
    {
    LOG(ERROR)("dump, target file open failed. [%s] err msg:%s", tfpath.c_str(), strerror(errno));
    //fp = stdout;
    }
  else
    {
    LOG(INFO)("dump, target file open succeed.");
    }
  while (reader.ReadRecord(&record, &scratch)) {
    fprintf(fp, "--- offset %llu; \n",
           static_cast<unsigned long long>(reader.LastRecordOffset()));
    (*func)(record, fp);
  }
  if (fp)
    {
      fclose(fp);
      fp = NULL;
    }
  delete file;
  return true;
}

// Called on every item found in a WriteBatch.
class WriteBatchItemPrinter : public WriteBatch::Handler {
 public:
  uint64_t offset_;
  uint64_t sequence_;
  FILE *fp_;
  
  WriteBatchItemPrinter()
    :fp_ (stdout) {}
  WriteBatchItemPrinter(FILE *fp)
    : fp_(fp)
    {
        if (!fp_)
        {
            LOG(ERROR)("dump, target file not open.");
        }
    }

  ~WriteBatchItemPrinter()
    {
    fp_ = NULL;
    }
  
  virtual void Put(const Slice& key, const Slice& value) 
  {
    if (fp_)
    {
    fprintf(fp_, "  put '%s' '%s'\n",
           EscapeString(key).c_str(),
           EscapeString(value).c_str());
    }
  }
  virtual void Delete(const Slice& key)
  {
    if (fp_)
    {
    fprintf(fp_, "  del '%s'\n", EscapeString(key).c_str());
    }
  }

};


// Called on every log record (each one of which is a WriteBatch)
// found in a kLogFile.
static void WriteBatchPrinter(Slice record, FILE *fp) {
  if (record.size() < 12) {
    LOG(DEBUG)("log record length %d is too small\n",
           static_cast<int>(record.size()));
    return;
  }
  WriteBatch batch;
  WriteBatchInternal::SetContents(&batch, record);
  LOG(DEBUG)("sequence %llu\n",
         static_cast<unsigned long long>(WriteBatchInternal::Sequence(&batch)));
  WriteBatchItemPrinter batch_item_printer(fp);
  Status s = batch.Iterate(&batch_item_printer);
  if (!s.ok()) {
    LOG(DEBUG)("  error: %s\n", s.ToString().c_str());
  }
}

bool DumpLog(Env* env, const std::string& fname, const std::string& tfname) {
    bool ret = true;
  ret = PrintLogContents(env, fname, WriteBatchPrinter, tfname);
  return ret;
}

// Called on every log record (each one of which is a WriteBatch)
// found in a kDescriptorFile.
static void VersionEditPrinter(Slice record, FILE *fp) {
  VersionEdit edit;
  Status s = edit.DecodeFrom(record);
  if (!s.ok()) {
    //LOG(DEBUG)("%s\n", s.ToString().c_str());
    fprintf(fp, "%s\n", s.ToString().c_str());
    return;
  }
  fprintf(fp, "%s\n", s.ToString().c_str());
  //LOG(DEBUG)("%s", edit.DebugString().c_str());
}

bool DumpDescriptor(Env* env, const std::string& fname, const std::string& tfname) {
  return PrintLogContents(env, fname, VersionEditPrinter, tfname);
}

bool DumpTable(Env* env, const std::string& fname, const std::string& tfname) {
  uint64_t file_size;
  RandomAccessFile* file = NULL;
  Table* table = NULL;
  Status s = env->GetFileSize(fname, &file_size);
  if (s.ok()) {
    s = env->NewRandomAccessFile(fname, &file);
  }
  if (s.ok()) {
    // We use the default comparator, which may or may not match the
    // comparator used in this database. However this should not cause
    // problems since we only use Table operations that do not require
    // any comparisons.  In particular, we do not call Seek or Prev.
    s = Table::Open(Options(), file, file_size, &table);
  }
  if (!s.ok()) {
    LOG(ERROR)( "%s\n", s.ToString().c_str());
    delete table;
    delete file;
    return false;
  }

  FILE *fp = fopen(tfname.c_str(), "w+");
  if (!fp)
    {
    LOG(ERROR)("dump, open target file failed. [%s], msg:%s"
        , tfname.c_str(), strerror(errno));
    //fp = stdout;
    }
  else
    {
    LOG(INFO)("dump, open target file succeed. [%s]", tfname.c_str());
    }

  ReadOptions ro;
  ro.fill_cache = false;
  Iterator* iter = table->NewIterator(ro);
  for (iter->SeekToFirst(); iter->Valid(); iter->Next()) {
    ParsedInternalKey key;
    if (!ParseInternalKey(iter->key(), &key)) {
      fprintf(fp, "badkey '%s' => '%s'\n",
             EscapeString(iter->key()).c_str(),
             EscapeString(iter->value()).c_str());
    } else {
      char kbuf[20];
      const char* type;
      if (key.type == kTypeDeletion) {
        type = "del";
      } else if (key.type == kTypeValue) {
        type = "val";
      } else {
        snprintf(kbuf, sizeof(kbuf), "%d", static_cast<int>(key.type));
        type = kbuf;
      }
      fprintf(fp, "'%s' @ %8llu : %s => '%s'\n",
             EscapeString(key.user_key).c_str(),
             static_cast<unsigned long long>(key.sequence),
             type,
             EscapeString(iter->value()).c_str());
    }
  }
  s = iter->status();
  if (!s.ok()) {
    LOG(DEBUG)("iterator error: %s\n", s.ToString().c_str());
  }

  if (fp)
  {
  fclose(fp);
  }
  delete iter;
  delete table;
  delete file;
  return true;
}

bool DumpFile(Env* env, const std::string& fname, const std::string& tfname) {
  FileType ftype;
  if (!GuessType(fname, &ftype)) {
    LOG(ERROR)( "%s: unknown file type\n", fname.c_str());
    return false;
  }
  switch (ftype) {
    case kLogFile:         return DumpLog(env, fname, tfname);
    case kDescriptorFile:  return DumpDescriptor(env, fname, tfname);
    case kTableFile:       return DumpTable(env, fname, tfname);

    default: {
      LOG(DEBUG)( "%s: not a dump-able file type\n", fname.c_str());
      break;
    }
  }
  return false;
}

bool HandleDumpCommand(Env* env, char** files, int num) {
  bool ok = true;
  for (int i = 0; i < num; i++) {
    ok &= DumpFile(env, files[i], string(""));
  }
  return ok;
}

bool DumpProperty(const std::string &dump_dir)
{
    stringstream ss;
    std::string spliter = "=====================================================================";
    char property[STACK_STR_MAX_LEN] = {0};
    const static char *PERFIX = "leveldb.";
    const static char *LEVEL_FILES_NUM = "num-files-at-level";
    const static char *STATS = "stats";
    const static char *SSTABLES = "sstables";
    const static char *PROPERTY_NAME = "property";
    std::string value;
    ss << spliter << std::endl;
    // 1. dump leveldb.num-files-at-level<N>
    for (int i = 0; i < leveldb::config::kNumLevels; ++i)
    {
        sprintf(property, "%s%s%u", PERFIX, LEVEL_FILES_NUM, i);
        std::string files_property(property);
        LeveldbEngine::inst().GetProperty(files_property, &value);
        ss << files_property << std::endl;
        ss << value << std::endl;
        value.clear();
    }

    // 2. dump leveldb.stats
    sprintf(property, "%s%s", PERFIX, STATS);
    std::string stats(property);
    LeveldbEngine::inst().GetProperty(stats, &value);
    ss << std::endl;
    ss << stats << std::endl;
    ss << value << std::endl;
    value.clear();

    // 3. dump leveldb.sstables
    sprintf(property, "%s%s", PERFIX, SSTABLES);
    std::string sstables(property);
    LeveldbEngine::inst().GetProperty(sstables, &value);
    ss << std::endl;
    ss << sstables << std::endl;
    ss << value << std::endl;
    value.clear();
    ss << spliter << std::endl;

    // 4. write dump file
    string property_path = dump_dir + "/" + PERFIX + PROPERTY_NAME;
    FILE *fp = fopen(property_path.c_str(), "w+");
    if (!fp)
    {
        LOG(ERROR)("dump, open property file failed. [%s], msg:%s"
            , property_path.c_str(), strerror(errno));
        return false;
    }
    else
    {
        LOG(INFO)("dump, open property file succeed. [%s]", property_path.c_str());
    }
    uint32_t write_len = fwrite(ss.str().c_str(), ss.str().size(), 1, fp);
    LOG(INFO)("write property length:%u", write_len);
    fclose(fp);
    fp = NULL;
    return true;
}

}  // namespace leveldb


#if 0
static void Usage() {
  LOG(ERROR)(
      "Usage: leveldbutil command...\n"
      "   dump files...         -- dump contents of specified files\n"
      );
}

int main(int argc, char** argv) {
  leveldb::Env* env = leveldb::Env::Default();
  bool ok = true;
  if (argc < 2) {
    Usage();
    ok = false;
  } else {
    std::string command = argv[1];
    if (command == "dump") {
      ok = leveldb::HandleDumpCommand(env, argv+2, argc-2);
    } else {
      Usage();
      ok = false;
    }
  }
  return (ok ? 0 : 1);
}
#endif

