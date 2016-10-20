///////////////////////////////////////////////////////////
//  Session.cpp
//  Implementation of the Class Session
//  Created on:      26-ÎåÔÂ-2015 10:17:59
///////////////////////////////////////////////////////////

#include "session.h"


Session::Session()
    : id_(0)
    , name_("")
    , signature_("")
{

}



Session::~Session(){

}


void Session::SetId(int id)
{
    id_ = id;
}
void Session::SetName(const char *name)
{
    name_ = name;
}
void Session::SetSignature(const char *signature)
{
    signature_ = signature;
}
int Session::GetId()
{
    return id_;
}
const char *Session::GetName()
{
    return name_.c_str();
}
const char *Session::GetSignature()
{
    return signature_.c_str();
}


void Session::ShowInfo()
{
    fprintf(stdout, 
        "show session info:\n"\
        "\tid:%d, name:%s\n"
    , id_, name_.c_str()
    );
}



