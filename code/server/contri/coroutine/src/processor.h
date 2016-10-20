/**
 * @filedesc: 
 * processor.h, other prcessor extends this
 * @author: 
 *  bbwang
 * @date: 
 *  2015/2/14 10:02:41
 * @modify:
 *
**/
#ifndef PROCESSOR_H_
#define PROCESSOR_H_
#include "libco_net_session.h"
#include "libco_data_type.h"
#include "param.h"

class Msg;
class Param;

class Processor
{
public:
    /*
     * 向某个服务请求数据，立即返回结果
     * const std::string &service_name为服务名称，由业务代码定义，用于区分服务
     * Msg &request，请求消息
     * Msg *&result,取得的结果，不一定是预期的，可能是框架抛出的系统超时消息
     * int strategy,同一服务多个实例的hash策略，暂未实现，默认轮询
    */
    int GetResult(const std::string &service_name, Msg &request, Msg *&result, int strategy = 0) const ;
    /*
     * 向某个服务发送数据，是否返回结果取决于服务端
     * const std::string &service_name为服务名称，由业务代码定义，用于区分服务
     * Msg &msg，请求消息
     * int strategy,同一服务多个实例的hash策略，暂未实现，默认轮询
    */
    int SendData(const std::string &service_name, Msg &msg, int strategy = 0) const;
    /*
     * 向客户端回复数据
     * const Param &param，取ProcessData的参数即可
     * const Msg &request，取ProcessData的Msg *msg，仅用于获得客户端的sender_coid
     * Msg &result，回复的数据
    */
    int Reply(const Param &param, const Msg &request, Msg &result) const;

    
    /*
     * 数据到达，处理业务逻辑，不同服务使用不同Processor
     * const Param &param，参数
     * Msg *msg，框架底层将网络数据转化之后的具体msg
    */
    virtual int ProcessData(const Param &param, Msg *msg) const;
    
    /*
     * 处理主动连接服务，框架实现，如果需要特殊操作，子类重载之
     * const Param &param，参数
    */
    virtual int ProcessConnect(const Param &param)const ;
    /*
     * 处理客户端的连接请求，框架实现，如果需要特殊操作，子类重载之
     * const Param &param，参数
    */
    virtual int ProcessAccept(const Param &param) const;
    /*
     * 处理关闭连接，框架实现，如果需要特殊操作，子类重载之
     * const Param &param，参数
    */
    virtual int ProcessClose(const Param &param) const;


    virtual ~Processor();
    
    LibcoNetSession *GetSessionByNetid(uint32_t netid) const;

    // 心跳处理
    virtual void KeepAlive(const Param &, Msg *) const;
protected:
    Processor();
    virtual LibcoNetSession *GetSession(const std::string &service_name, Msg &request, int strategy) const;
private:

    Processor(const Processor &);
    Processor operator =(const Processor &);

};


#endif //PROCESSOR_H_

