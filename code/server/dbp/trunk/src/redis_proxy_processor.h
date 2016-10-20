/**
* file: redis_proxy_processor.h
* desc: redis_proxy_processor file. process cmd
* auth: bbwang
* date: 2015/11/3
*/
#ifndef _REDIS_PROXY_PROCESSOR_H_
#define _REDIS_PROXY_PROCESSOR_H_

#include "processor.h"

class Msg;

class RedisProxyProcessor : public Processor
{
public:
    static RedisProxyProcessor *Instance();
    virtual int ProcessData(const Param &param, Msg *msg) const;
};

#endif //_REDIS_PROXY_PROCESSOR_H_


