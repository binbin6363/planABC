/**
 * @filedesc: 
 * libco_net_session.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/1/7 16:02:41
 * @modify:
 *
**/
#ifndef LIBCO_NET_SESSION_H_
#define LIBCO_NET_SESSION_H_

//#include <string.h>
#include <string>
#include "net_handler.h"	
#include "net_session.h"	
#include "selector.h"	


class Msg;
class Processor;
class Option;


// 业务层直接从Libco_Net_Session派生session
class LibcoNetSession : public Net_Handler
{
public:
	LibcoNetSession();
	virtual ~LibcoNetSession();
	virtual int open(void *arg, const INET_Addr &remote_addr);
	virtual int on_receive_message(Msg &msg);

	//************************************
	// Method:    get_buffer
	// FullName:  Net_Session::get_buffer
	// Access:    virtual public 
	// Returns:   char *
	// Qualifier: 子类可实现该函数以自己提供任意buff
	//************************************
	virtual char *get_buffer();
	void session_type(Session_Type session_type);
    Session_Type &session_type();
    int SendRequest(Msg &request);
    int SendResponse(const Msg &msg, Msg &result);
	inline const std::string &session_id(){return session_id_;}
	inline void set_session_id(std::string session_id_){this->session_id_ = session_id_;}
	inline const INET_Addr &remote_addr()const {return remote_addr_;}
    void set_option(Option *option = NULL);
    Option *option() const;
    void update_time();
    time_t last_time();
    bool IsConnected();

    virtual void SetId(uint32_t id);
    virtual uint32_t GetId();
    virtual uint32_t GetId() const;

    virtual void SetScore(uint32_t score);
    virtual uint32_t GetScore();
    virtual uint32_t GetScore() const;
    
    virtual uint32_t NodeId(); // 虚拟节点id
    virtual void SetNodeId(uint32_t node_id);
	inline void remote_addr(const INET_Addr &remote_addr) {remote_addr_ = remote_addr;}
    inline const INET_Addr &remote_addr(){return remote_addr_;}


private:
	virtual int handle_connect(uint32_t new_handle, const INET_Addr &remote_addr);


private:
	
	INET_Addr     remote_addr_;
	Session_Type  session_type_;
	std::string   session_id_;
    Option        *option_;
protected:
    time_t    last_time_;
    bool      b_connected_;

};



class SessionGroup;
// cluster
class Cluster
{
public:

typedef map<uint32_t, SessionGroup *> GroupMap;
typedef GroupMap::const_iterator GroupCIter;
typedef GroupMap::iterator GroupIter;

    Cluster();
    ~Cluster();

    int AddGroup(SessionGroup *session_group);
    SessionGroup *GetGroup(uint32_t group_id);
    SessionGroup *GetGroup();
    void RemoveGroup(uint32_t group_id);

    GroupMap &Groups();


    void SetSelector(Selector<SessionGroup *> *selector);
    void SetHashId(uint32_t hash_id);

    Selector<SessionGroup *> *GetSelector();
    int CreateGroupSelector(int strategy);

private:
    int internal_add_group(SessionGroup *session_group);
    int internal_remove_group(uint32_t group_id);

private:
    Selector<SessionGroup *> *group_selector_;
    GroupMap                 groups_;
};



// sessionGroup
class SessionGroup
{
public:
typedef map<uint32_t, LibcoNetSession *> SessionMap;
typedef SessionMap::const_iterator SessionCIter;
typedef SessionMap::iterator SessionIter;
    
    SessionGroup();
    ~SessionGroup();

    int AddSession(LibcoNetSession *session);
    void RemoveSession(uint32_t session_id);
    LibcoNetSession *GetSession();
	LibcoNetSession *GetSession(uint32_t session_id);

    SessionMap &Sessions();

    uint32_t GetId();
    uint32_t GetId() const;
    void SetId(uint32_t group_id);

    uint32_t NodeId(); // 虚拟节点id
    void SetNodeId(uint32_t node_id);
    
    void SetSelector(Selector<LibcoNetSession *> *selector);
    Selector<LibcoNetSession *> *GetSelector();
    void SetHashId(uint32_t hash_id);
    int CreateSessionSelector(int strategy = -1);

private:
    int internal_add_session(LibcoNetSession *session);
    int internal_remove_session(uint32_t session_id);


private:
    uint32_t                    group_id_;
    uint32_t                    node_id_;
    Selector<LibcoNetSession *> *session_selector_;
    SessionMap                  sessions_;

};



#endif // LIBCO_NET_SESSION_H_

