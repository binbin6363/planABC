/**
* file: dbp_processor.h
* desc: ban liao, dbp_processor file. process cmd
* auth: bbwang
* date: 2015/3/3
*/
#ifndef DBP_PROCESSOR_H_
#define DBP_PROCESSOR_H_

#include "processor.h"

class Msg;

class DbpProcessor : public Processor
{
public:
    static DbpProcessor *Instance();
    virtual int ProcessData(const Param &param, Msg *msg) const;

};

#endif //DBP_PROCESSOR_H_
