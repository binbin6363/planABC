/**
* file: udp_cmdline_processor.h
* desc: ban liao, udp_cmdline_processor file. process cmd
* auth: bbwang
* date: 2015/7/8
*/
#ifndef UDP_CMDLINE_PROCESSOR_H_
#define UDP_CMDLINE_PROCESSOR_H_

#include "processor.h"
#include "param.h"
class Msg;

class UdpCmdLineProcessor : public Processor
{
public:
    static UdpCmdLineProcessor *Instance();
    virtual int ProcessData(const Param &param, Msg *msg) const;

private:
    UdpCmdLineProcessor();
    ~UdpCmdLineProcessor();
    int call_cmd(const Param &param, const string &cmd) const;

    void initCmd() const;
    int parse_args(string &cmd) const;
    string parsecmd(string &strUdp) const;



};


#endif //UDP_CMDLINE_PROCESSOR_H_
