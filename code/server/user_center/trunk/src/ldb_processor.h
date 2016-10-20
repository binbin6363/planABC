/**
* file: ldb_processor.h
* desc: ban liao, ldb_processor file. process cmd
* auth: bbwang
* date: 2015/3/3
*/
#ifndef LDB_PROCESSOR_H_
#define LDB_PROCESSOR_H_

#include "processor.h"

class Msg;

class LdbProcessor : public Processor
{
public:
    static LdbProcessor *Instance();
    virtual int ProcessData(const Param &param, Msg *msg) const;
    virtual int ProcessClose(const Param &param) const;
};

#endif //LDB_PROCESSOR_H_
