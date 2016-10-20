///////////////////////////////////////////////////////////
//  Session.h
//  Implementation of the Class Session
//  Created on:      26-五月-2015 10:17:59
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
	int id_;              // 用于哈希
	string name_;         // 标识
    string signature_;    // 用于一致性哈希

};
#endif // !defined(SESSION_H_)


