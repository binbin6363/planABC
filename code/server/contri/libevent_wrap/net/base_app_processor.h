/**
 * @filedesc: 
 * 
 * @author: 
 *  bbwang
 * @date: 
 *  2016/8/4 20:02:59
 * @modify:
 *
**/

#ifndef _BASE_APP_PROCESSOR_H_
#define _BASE_APP_PROCESSOR_H_


// app��processor�̳д˻��࣬ʵ��ҵ���߼�����
class BaseProcessor
{
public:
    BaseProcessor();
    virtual ~BaseProcessor();

	// ���յ���������
    virtual int OnRecvRequest(const Param &param, char *data, int len);

protected:
    Session *GetSession(int net_id);
};

#endif // _BASE_APP_PROCESSOR_H_
