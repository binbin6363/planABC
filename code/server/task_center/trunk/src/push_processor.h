/**
* file: push_processor.h
* desc: push_processor file. process cmd
* auth: bbwang
* date: 2015/3/3
*/
#ifndef PUSH_PROCESSOR_H_
#define PUSH_PROCESSOR_H_

#include "processor.h"

class Msg;

class PushProcessor : public Processor
{
public:
    static PushProcessor *Instance();
    virtual int ProcessData(const Param &param, Msg *msg) const;

};

#endif //PUSH_PROCESSOR_H_
