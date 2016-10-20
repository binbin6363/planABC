/**
* file: client_processor.h
* desc: client_processor file. process cmd
* auth: bbwang
* date: 2015/11/4
*/
#ifndef CLIENT_PROCESSOR_H_
#define CLIENT_PROCESSOR_H_

#include "processor.h"
#include "param.h"
class Msg;

class ClientProcessor : public Processor
{
public:
    static ClientProcessor *Instance();
    virtual int ProcessData(const Param &param, Msg *msg) const;
	virtual void KeepAlive(const Param &param, Msg *msg) const;
};


#endif //CLIENT_PROCESSOR_H_
