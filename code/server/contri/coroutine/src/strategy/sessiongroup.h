///////////////////////////////////////////////////////////
//  SessionGroup.h
//  Implementation of the Class SessionGroup
//  Created on:      26-ÎåÔÂ-2015 9:42:26
///////////////////////////////////////////////////////////

#if !defined(SESSION_GROUP_H_)
#define SESSION_GROUP_H_
#include <string>
#include <vector>
#include <stdint.h>
#include "selector.h"

using namespace std;

class Session;

class SessionGroup
{

public:
	SessionGroup();
	virtual ~SessionGroup();

	Session * GetSession();


//    void SetGroupId(uint32_t gid);
//    void SetGroupName(const char *name);
    void AddSession(Session *session);
    void DestroySession();
    void SetSelector(Selector<Session *> *selector);
    const vector<Session *> &SessionArray() const;

    uint32_t GetId()
        {
        return id_;
        }
    void SetId(uint32_t id)
        {
        id_ = id;
        }
    void SetSignature(const char *signature)
        {
        signature_ = signature;
        }
    const char *GetSignature()
        {
        return signature_.c_str();
        }
    void SetSelectTarget(uint32_t target_id)
        {
        selector_->SetElementId(target_id);
        }


private:
    string signature_;
    uint32_t id_;
    vector<Session *> session_arrays_;
    Selector<Session *> *selector_;
};





class Cluster
{
public:
    Cluster()
        : selector_(NULL)
        {
        cluster_cell_.clear();
        }
    ~Cluster()
        {}

    void AddSessionGroup(SessionGroup *sessionGroup);
    void SetSelector(Selector<SessionGroup *> *selector);
    void SetSelectTarget(uint32_t target_id);

    SessionGroup *GetSessionGroup();

private:
    vector<SessionGroup *>    cluster_cell_;
    Selector<SessionGroup *> *selector_;
};




#endif // !defined(SESSION_GROUP_H_)

