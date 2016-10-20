///////////////////////////////////////////////////////////
//  sessiongroup.cpp
//  Implementation of the Class SessionGroup
//  Created on:      26-ÎåÔÂ-2015 9:42:26
///////////////////////////////////////////////////////////

#include "sessiongroup.h"
#include "session.h"
#include "selector.h"



SessionGroup::SessionGroup()
    : signature_("")
    , id_(0)
    , selector_(NULL)
{

}



SessionGroup::~SessionGroup()
{

}



Session * SessionGroup::GetSession()
{

    return selector_->Select();
}


//void SessionGroup::SetGroupId(uint32_t gid){
//    group_id_ = gid;
//}

//void SessionGroup::SetGroupName(const char *name){
//    group_name_ = name;
//}

void SessionGroup::AddSession(Session *session){
    session_arrays_.push_back(session);
}

void SessionGroup::DestroySession(){
    int size = session_arrays_.size();
    for (int i = 0; i < size; ++i)
    {
        delete session_arrays_[i];
    }
    session_arrays_.clear();
}

void SessionGroup::SetSelector(Selector<Session *> *selector)
{
    if (NULL == selector)
    {
        return ;
    }

    selector_ = selector;
    selector_->SetElements(SessionArray());
}

const vector<Session *> &SessionGroup::SessionArray() const
{
    return session_arrays_;
}


//
void Cluster::AddSessionGroup(SessionGroup *sessionGroup)
{
    cluster_cell_.push_back(sessionGroup);
}

void Cluster::SetSelector(Selector<SessionGroup *> *selector)
{
    if (NULL == selector)
    {
        return ;
    }

    selector_ = selector;
    selector_->SetElements(cluster_cell_);
}

void Cluster::SetSelectTarget(uint32_t target_id)
{
    selector_->SetElementId(target_id);
}

SessionGroup *Cluster::GetSessionGroup()
{
    return selector_->Select();
}



