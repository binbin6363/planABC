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


// app的processor继承此基类，实现业务逻辑处理
class BaseProcessor
{
public:
    BaseProcessor();
    virtual ~BaseProcessor();

	// 接收到数据请求
    virtual int OnRecvRequest(const Param &param, char *data, int len);

protected:
    Session *GetSession(int net_id);
};

#endif // _BASE_APP_PROCESSOR_H_
