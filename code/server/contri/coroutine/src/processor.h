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
     * ��ĳ�������������ݣ��������ؽ��
     * const std::string &service_nameΪ�������ƣ���ҵ����붨�壬�������ַ���
     * Msg &request��������Ϣ
     * Msg *&result,ȡ�õĽ������һ����Ԥ�ڵģ������ǿ���׳���ϵͳ��ʱ��Ϣ
     * int strategy,ͬһ������ʵ����hash���ԣ���δʵ�֣�Ĭ����ѯ
    */
    int GetResult(const std::string &service_name, Msg &request, Msg *&result, int strategy = 0) const ;
    /*
     * ��ĳ�����������ݣ��Ƿ񷵻ؽ��ȡ���ڷ����
     * const std::string &service_nameΪ�������ƣ���ҵ����붨�壬�������ַ���
     * Msg &msg��������Ϣ
     * int strategy,ͬһ������ʵ����hash���ԣ���δʵ�֣�Ĭ����ѯ
    */
    int SendData(const std::string &service_name, Msg &msg, int strategy = 0) const;
    /*
     * ��ͻ��˻ظ�����
     * const Param &param��ȡProcessData�Ĳ�������
     * const Msg &request��ȡProcessData��Msg *msg�������ڻ�ÿͻ��˵�sender_coid
     * Msg &result���ظ�������
    */
    int Reply(const Param &param, const Msg &request, Msg &result) const;

    
    /*
     * ���ݵ������ҵ���߼�����ͬ����ʹ�ò�ͬProcessor
     * const Param &param������
     * Msg *msg����ܵײ㽫��������ת��֮��ľ���msg
    */
    virtual int ProcessData(const Param &param, Msg *msg) const;
    
    /*
     * �����������ӷ��񣬿��ʵ�֣������Ҫ�����������������֮
     * const Param &param������
    */
    virtual int ProcessConnect(const Param &param)const ;
    /*
     * ����ͻ��˵��������󣬿��ʵ�֣������Ҫ�����������������֮
     * const Param &param������
    */
    virtual int ProcessAccept(const Param &param) const;
    /*
     * ����ر����ӣ����ʵ�֣������Ҫ�����������������֮
     * const Param &param������
    */
    virtual int ProcessClose(const Param &param) const;


    virtual ~Processor();
    
    LibcoNetSession *GetSessionByNetid(uint32_t netid) const;

    // ��������
    virtual void KeepAlive(const Param &, Msg *) const;
protected:
    Processor();
    virtual LibcoNetSession *GetSession(const std::string &service_name, Msg &request, int strategy) const;
private:

    Processor(const Processor &);
    Processor operator =(const Processor &);

};


#endif //PROCESSOR_H_

