/**
* file: dbagent_processor.h
* desc: dbp_processor file. process cmd
* auth: bbwang
* date: 2015/11/3
*/
#ifndef _MYSQL_PROCESSOR_H_
#define _MYSQL_PROCESSOR_H_

#include "processor.h"

class Msg;

class DbagentProcessor : public Processor
{
public:
    static DbagentProcessor *Instance();
    virtual int ProcessData(const Param &param, Msg *msg) const;
};

#endif //_MYSQL_PROCESSOR_H_


