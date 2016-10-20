/**
* file: status_processor.h
* desc: ban liao, status_processor file. process cmd
* auth: bbwang
* date: 2015/3/3
*/
#ifndef STATUS_PROCESSOR_H_
#define STATUS_PROCESSOR_H_

#include "processor.h"

class Msg;

class StatusProcessor : public Processor
{
public:
    static StatusProcessor *Instance();
    virtual int ProcessData(const Param &param, Msg *msg) const;
    virtual int ProcessClose(const Param &param) const;
};

#endif //STATUS_PROCESSOR_H_
