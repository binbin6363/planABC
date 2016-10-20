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
#ifndef _BASE_TYPE_H_
#define _BASE_TYPE_H_



struct Decoder
{
    virtual int Decode(char *data, int len) = 0;
};


struct Encoder
{
    virtual int Encode(char *data, int &len) = 0;
};

struct Translater
{
    virtual int Translate(char *data, int len) = 0;
};

struct Param
{
    int net_id;              // 唯一标示一个连接
    int task_id;             // 标示一个请求任务，也作为协程id
    Decoder *decoder;        // 解包器
    Encoder *encoder;        // 封包器
    Translater  *translater; // 协议解释器
    void *args;              // 额外参数
};

#endif // _BASE_TYPE_H_

