/**
 * @filedesc: 
 * session_manager.h, singleton, manage client and server session.
 * include connect, accept, get, delete ...
 * @author: 
 *  bbwang
 * @date: 
 *  2015/2/26 11:18:41
 * @modify:
 *
**/
#ifndef SESSION_MANAGER_H_
#define SESSION_MANAGER_H_
#include "utils.h"

#include "net_connector.h"
#include "net_acceptor.h"
#include "libco_data_type.h"
#include "binpacket_wrap.h"
#include "timeoutmanager.h"
#include "directselector.h"


using namespace libco_src;
using namespace utils;

#define CLIENT_CLUSTER_NAME "CLIENT"
class ClientSession;
class ServerSession;
class INET_Addr;
class LibcoNetSession;
class Option;
class Msg;
class Cluster;
class SessionGroup;

template<class T>
class Selector;


class ClientAcceptor : public Net_Acceptor<ClientSession, TCP_SESSION>
{
	typedef Net_Acceptor<ClientSession, TCP_SESSION> super;
public:
	ClientAcceptor(void);
	virtual ~ClientAcceptor(void);

	virtual int make_svc_handler(ClientSession *&sh);
	virtual int activate_svc_handler(ClientSession *svc_handler, const INET_Addr &remote_addr);

    void set_option(Option *option);
    
private:
    Option *option_;
};

class UdpAcceptor : public Net_Acceptor<ClientSession, UDP_SESSION>
{
	typedef Net_Acceptor<ClientSession, UDP_SESSION> super;
public:
	UdpAcceptor(void);
	virtual ~UdpAcceptor(void);

	virtual int make_svc_handler(ClientSession *&sh);
	virtual int activate_svc_handler(ClientSession *svc_handler, const INET_Addr &remote_addr);

    void set_option(Option *option);
    
private:
    Option *option_;
};

typedef Net_Connector<ServerSession> ServerConnector;

// 客户端最大包长为1m
const static uint32_t CLIENT_MAX_PKG_SIZE = (1024 * 1024 * 1);
// spliter
class ClientSplitter : public Packet_Splitter
{
    virtual int split(const char *buf, int len, int &packet_begin, int &packet_len);
};

// 服务端最大包长为1m
const static uint32_t SERVER_MAX_PKG_SIZE = (1024 * 1024 * 1);
class ServerSplitter : public Packet_Splitter
{
    virtual int split(const char *buf, int len, int &packet_begin, int &packet_len);
};


typedef std::vector<LibcoNetSession*> SessionVec;
typedef SessionVec::iterator SessionVIter;
typedef SessionVec::const_iterator SessionVCIter;

typedef std::map<std::string, SessionVec > SessionVecMap;
typedef SessionVecMap::iterator SessionVMIter;
typedef SessionVecMap::const_iterator SessionVMCIter;

typedef map<uint32_t, LibcoNetSession *> SessionMap;
typedef SessionMap::const_iterator SessionCIter;
typedef SessionMap::iterator SessionIter;

typedef std::map<std::string, Cluster*> Clusters; // name->cluster
typedef Clusters::iterator ClustersIter;
typedef Clusters::const_iterator ClustersCIter;


// 加timeout的目的是为client session连接做超时检测
class SessionManager : public TimeoutEvent// : public ImoSingleton<SessionManager>
{
public:
    static SessionManager &Instance();

    void net_manager(Net_Manager *net_manager);
    int ConnectServer(const Server_Info &server_info, Option *option);
    int ReConnectServer(ServerSession *server_session);
    int  OpenAcceptor(const Server_Info &server_info, Option *option);
    int  ReOpenAcceptor(const ClientSession *client_session);
    int  OpenUdpAcceptor(const Server_Info &server_info, Option *option);

    bool IsExistSession(const char *cluster_name, uint32_t group_id, uint32_t session_id);
    int AddClientSession(ClientSession *client_session);
    int RemoveAndCloseClientSession(const char *cluster_name, const std::string &server_id);
    ClientSession *GetClientSession(const std::string &server_id);
    LibcoNetSession *GetSession(const std::string &name, Msg &msg, int strategy);
    virtual void handle_timeout(int ,void *);
    void BindId(uint32_t myself_id);
    int AddServerSession(ServerSession *server_session);
    int RemoveServerSession(ServerSession *session);

    
private:
    int AddToCluster(const Server_Info &server_info, LibcoNetSession *session);
    SessionManager();
    ~SessionManager();    

    bool IsSelectorTypeEqual(const Selector<SessionGroup *> &s1, const Selector<SessionGroup *> &s2);

private:
    ClientSplitter              client_splitter_;
    ServerSplitter              server_splitter_;
    ClientAcceptor              client_acceptor_;
    UdpAcceptor                 udp_acceptor_;
    ServerConnector             server_connector_;
    Net_Manager                 *net_manager_;

    Clusters                    session_clusters_;    // name -> cluster
    SessionMap                  cli_sessions_map_;    // 客户端的session，服务id为键
};





#endif // SESSION_MANAGER_H_

