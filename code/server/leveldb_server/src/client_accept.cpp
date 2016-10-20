#include "client_accept.h"
using namespace utils;


ClientAccept::ClientAccept(void)
{
}

ClientAccept::~ClientAccept(void)
{
}

int ClientAccept::make_svc_handler( ClientSession *&sh )
{
    sh = new ClientSession;
    return 0;
}

int ClientAccept::activate_svc_handler( ClientSession *svc_handler, const INET_Addr &remote_addr )
{
    LOG(INFO)("new session connected, handle:%u, clientip:%s.", svc_handler->handle(), FromAddrTostring(remote_addr).c_str());
    if (!ServerConfig::Instance()->IsAllow(FromAddrToIpAddr(remote_addr)))
    {
        LOG(WARN)("the ip not allowed connect, ip:%s.", FromAddrTostring(remote_addr).c_str());
        return -1;
    }

    return super::activate_svc_handler(svc_handler, remote_addr);
}
