///////////////////////////////////////////////////////////
//  Test.cpp
//  Implementation of the Class Test
//  Created on:      26-五月-2015 9:48:04
//  Original author: imo
///////////////////////////////////////////////////////////

#include "test.h"
#include "session.h"
#include "sessiongroup.h"
#include "hashselector.h"
#include "conshashselector.h"
#include <stdio.h>


Cluster *Test::cluster_;


Test::Test(){
}



Test::~Test(){
}


Test &Test::Inst()
{
    static Test inst_;
    return inst_;
}


int Test::TestHashGet()
{
    fprintf(stdout, "TestHashGet =======\n");
    ModHashSelector<SessionGroup *> *groupSelector = new ModHashSelector<SessionGroup *>;
    cluster_->SetSelector(groupSelector);

    ModHashSelector<Session *> *sessionSelector = new ModHashSelector<Session *>;
    
    for (int i = 0; i < 30; ++i)
    {
        groupSelector->SetElementId(i);
        groupSelector->ShowInfo();
        SessionGroup *sessionGroup = cluster_->GetSessionGroup();
        if (NULL == sessionGroup)
            continue;

        sessionGroup->SetSelector(sessionSelector);
        sessionSelector->SetElementId(i+100);
        Session *session = sessionGroup->GetSession();
        if (NULL == session)
            continue;
        session->ShowInfo();
    }
    return 0;
}

int Test::TestConsHashGet()
{
    fprintf(stdout, "TestConsHashGet =======\n");
    ConsHashSelector<SessionGroup *> *groupSelector = new ConsHashSelector<SessionGroup *>;
    cluster_->SetSelector(groupSelector);

    ConsHashSelector<Session *> *sessionSelector = new ConsHashSelector<Session *>;
    
    for (int i = 0; i < 10; ++i)
    {
        groupSelector->SetElementId(i+100000);
        groupSelector->ShowInfo();
        SessionGroup *sessionGroup = cluster_->GetSessionGroup();
        if (NULL == sessionGroup)
            continue;

        sessionGroup->SetSelector(sessionSelector);
        sessionSelector->SetElementId(i+1000000);
        Session *session = sessionGroup->GetSession();
        if (NULL == session)
            continue;
        session->ShowInfo();
    }
    return 0;
}

int Test::TestDirectGet()
{
    fprintf(stdout, "TestDirectGet =======\n");
    return 0;
}


void Test::SetCluster(Cluster *cluster)
{
    cluster_ = cluster;
}





//

string makeRandomSignature()
{
    static int id = 1;
    const static int CLIENT_PASSWD_LENGTH = 17;
    char signature[CLIENT_PASSWD_LENGTH] = {0};
    id = 2*id + 10;
    srand(time(NULL) + id);
    for (int i = 0; i < CLIENT_PASSWD_LENGTH; ++i)
    {
        signature[i] = (rand() % 0x4A) + 0x30; // 可显示的字符
    }
    signature[CLIENT_PASSWD_LENGTH-1] = 0;
    return signature;
}



void addSession(SessionGroup *sessionGroup, uint32_t id)
{   
    if (NULL == sessionGroup)
        {
        LOG(ERROR)("sessionGroup is null.");
        return ;
        }
    Session *session = new Session;
    string sessionSignature = makeRandomSignature(); // char[16]
    session->SetId(id+1);
    session->SetSignature(sessionSignature.c_str());
    sessionGroup->AddSession(session);
}

void addSessionGroup(Cluster *cluster)
{
    if (NULL == cluster)
        {
        LOG(ERROR)("cluster is null.");
        return ;
        }
    static int id = 1;
    ++id;

    SessionGroup *sessionGroup = new SessionGroup;
    uint32_t group_id = id + 100;
    string groupSignature = makeRandomSignature(); // char[16]
    sessionGroup->SetSignature(groupSignature.c_str());
    sessionGroup->SetId(group_id);

    cluster->AddSessionGroup(sessionGroup);
    for (int j = 0; j < 5; ++j)
    {
        addSession(sessionGroup, j);
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;

    Cluster *cluster = new Cluster;

    for (int i = 0; i < 10; ++i)
    {
        addSessionGroup(cluster);
    }

    Test::SetCluster(cluster);

    Test::TestHashGet();

    Test::TestConsHashGet();

    Test::TestDirectGet();

    delete cluster;

    return ret;
}

