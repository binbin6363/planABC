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
    int net_id;              // Ψһ��ʾһ������
    int task_id;             // ��ʾһ����������Ҳ��ΪЭ��id
    Decoder *decoder;        // �����
    Encoder *encoder;        // �����
    Translater  *translater; // Э�������
    void *args;              // �������
};

#endif // _BASE_TYPE_H_

