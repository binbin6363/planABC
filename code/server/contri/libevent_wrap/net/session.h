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
#ifndef _SESSION_H_
#define _SESSION_H_

class Session
{
public:
    Session();
    ~Session();

    int Connect();
    int DisConnect();
    int SendRequest();
    int GetResult();
    int Reply();
};


#endif //_SESSION_H_
