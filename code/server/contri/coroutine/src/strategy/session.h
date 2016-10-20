///////////////////////////////////////////////////////////
//  Session.h
//  Implementation of the Class Session
//  Created on:      26-����-2015 10:17:59
///////////////////////////////////////////////////////////

#if !defined(SESSION_H_)
#define SESSION_H_

#include <string>
using namespace std;

#include "sessiongroup.h"

class Session
{

public:
	Session();
	virtual ~Session();

    void SetId(int id);
    void SetName(const char *name);
    void SetSignature(const char *signature);
    int GetId();
    const char *GetName();
    const char *GetSignature();

    void ShowInfo();

private:
	int id_;              // ���ڹ�ϣ
	string name_;         // ��ʶ
    string signature_;    // ����һ���Թ�ϣ

};
#endif // !defined(SESSION_H_)


