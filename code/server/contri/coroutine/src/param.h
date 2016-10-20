/**
 * @filedesc: 
 * param.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/2/14 10:02:41
 * @modify:
 *
**/
#ifndef PARAM_H_
#define PARAM_H_

class INET_Addr;
using std::string;


// 保存运行时的参数
class Param
{
DECLARE_SLAB(Param);

public:
    const char *service_name;  // service name
    uint32_t net_id;           // net_id
    INET_Addr remote_addr;      // remot addr
};

class CoParam
{
public:
    uint32_t coid;
    Net_Event *net_event;
};


#endif // PARAM_H_


