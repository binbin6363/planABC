/**
 * @filedesc: 
 * msg.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/1/7 16:02:41
 * @modify:
 *
**/

#ifndef MSG_H_
#define MSG_H_

#include <string>
#include <stdio.h>
#include <map>
#include <list>
#include "binpacket_wrap.h"

using namespace std;
using namespace utils;


#ifndef MAX_PACKET_LENGTH_FRAME
#define MAX_PACKET_LENGTH_FRAME 4500
#endif

class LibcoNetSession;
class MsgFactory;
// define some type
typedef std::map<std::string, MsgFactory*> MsgFactoryMap;
typedef MsgFactoryMap::iterator FactoryIter;
typedef MsgFactoryMap::const_iterator FactoryCIter;
typedef std::map<std::string, MsgFactoryMap> ServiceFactoryMap;
typedef ServiceFactoryMap::iterator ServiceFactoryIter;
typedef ServiceFactoryMap::const_iterator ServiceFactoryCIter;

typedef std::list<std::string> strList;
typedef strList::iterator strListIter;
typedef strList::const_iterator strListCIter;


enum kSelectServerStragey{
	MOD_HASH_STRATEGY = 0,  // 按模 HASH 策略
	POLL_STRATEGY = 1,      // 轮询 策略
	SPECIFIED_STRATEGY = 2, // 指定server id
	CONS_HASH_STRATEGY = 3, // 一致性 HASH 策略
	DYN_SCORE_STRATEGY = 4, // 动态得分策略
};


// 添加统计
extern uint32_t msg_counter;

// msg包含继承体系，不建议使用内存池，防止memset抹掉虚函数表，虚基表等
// 其子类也不要使用内存池
// TODO: bbwang, 2015/7/11
class Msg
{

public:

    Msg();
    Msg(const Msg &other);
    Msg &operator=(const Msg &other);
    virtual ~Msg();
	virtual int Decode(const char *data, uint32_t length) = 0;
	virtual int Encode(char *data, uint32_t &length) const = 0;
    virtual uint32_t GetCmd() const = 0;
    bool IsReply();
    bool IsSysError();
    bool IsTimeout();
	std::string GetTypeName() const;
    // 用于给服务端发送消息时选取哪个server的hash规则
    virtual uint32_t hashid() const;
    // 用于给客户端发送消息时选取哪个客户端发送
    virtual std::string server_id() const;
    void SetReceiverCoid(uint32_t recv_coid);
    int PutDataLength(char *data, uint32_t length) const;
    // 只有在需要发大包的时候才调用该接口扩大包长，否则无需调用
    void SetMaxSize(uint32_t size) { max_send_size = size; }
    uint32_t GetSize() const { return max_send_size; }

	void SetRequestCoId();
	void SetReplyCoId();

	virtual int get_err_no() const;
	virtual string get_err_msg() const;
	virtual void set_err_msg(const string &msg_str);

public:
    mutable uint32_t len;
    uint32_t max_send_size;
    mutable uint32_t receiver_coid;
    mutable uint32_t sender_coid;
    uint32_t trans_id;
    std::string transfer_str;
};

//////////////////////////////////////////////////////////////////////////

class CoMsg : public Msg
{
public:
    CoMsg();
    virtual ~CoMsg();

public:
    virtual int Decode(const char *data, uint32_t length);
    virtual int Encode(char *data, uint32_t &length) const;
    virtual uint32_t GetCmd() const;

    const COHEADER &msg_header() const { return msg_header_; }
    void set_msg_header(const COHEADER &header);
    virtual uint32_t hashid() const;

protected:
    mutable COHEADER msg_header_;
};

//////////////////////////////////////////////////////////////////////////

// time out
class SystemTimeoutMsg : public Msg
{
public:
	static SystemTimeoutMsg *Instance();

    // time1, curent time; time2, last time
    SystemTimeoutMsg();
    ~SystemTimeoutMsg();
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;
    virtual uint32_t GetCmd() const;
    void set_curtime(time_t curtime);
    void set_lasttime(time_t lasttime);

	int get_err_no() const;
	string get_err_msg() const;

private:
    time_t cur_time_;
    time_t last_time_;
};

class BeatMsg  : public Msg 
{
public:
    BeatMsg();
    virtual ~BeatMsg();
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const ;
    virtual uint32_t GetCmd() const;
    
public:
    // head
    uint32_t cmd;
    uint32_t seq;
    uint32_t server_id;
//    uint32_t uid;
    // data, empty
};

class CliBeatMsg : public CoMsg
{
public:
    CliBeatMsg();
    virtual ~CliBeatMsg();
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;

};



class SystemErrorMsg : public CoMsg 
{
public:
	static SystemErrorMsg *Instance();
	
    SystemErrorMsg();
    virtual ~SystemErrorMsg();
	virtual int Decode(const char *data, uint32_t length);

	virtual int get_err_no() const;
	virtual string get_err_msg() const;
	virtual void set_err_msg(const string &msg);

private:
	string err_msg_;
};


class SessionNotFoundErrorMsg : public CoMsg 
{
public:
	static SessionNotFoundErrorMsg *Instance();

    SessionNotFoundErrorMsg();
    virtual ~SessionNotFoundErrorMsg();
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;

	int get_err_no() const;
	string get_err_msg() const;

};



// for cmd line request, eg. nc client
class CmdLineMsg : public Msg
{
public:
    CmdLineMsg();
    virtual ~CmdLineMsg();
	virtual int Decode(const char *data, uint32_t length);
	virtual int Encode(char *data, uint32_t &length) const;

    string &get_requset_str();
    void set_result_str(const string &str);
    virtual uint32_t GetCmd() const;

private:
    string cmd_str_;
    
};

class SystemMsgFactory;
// 创建Msg
class MsgCreator
{
private:
    MsgCreator();
    MsgCreator(const MsgCreator &other);
    MsgCreator &operator=(const MsgCreator &other);
    
public:
    static MsgCreator *GetGlobalMsgFactory();
    virtual ~MsgCreator();
    static int RegisterFactory(const std::string& service_name,MsgFactory *factory);
	static void UnregisterFactory(const std::string& service_name,MsgFactory* factory);
    static Msg *Create(const char* service_name,const char* type);
    static void Destory(const char* service_name,const Msg* msg);

private:
    static ServiceFactoryMap factories_;
    SystemMsgFactory *sys_factory_;
};

#define GlobalFactoryRegister MsgCreator::GetGlobalMsgFactory()->RegisterFactory
#define GlobalFactoryUnregister MsgCreator::GetGlobalMsgFactory()->UnregisterFactory


// Msg工厂
struct MsgFactory
{
	MsgFactory(){}
	virtual ~MsgFactory() {}
	virtual strList &KnownTypes() const = 0;
	virtual Msg* Create(const char* type) = 0;
	virtual void Destroy(const Msg* msg) = 0;
    
private:
	MsgFactory(const MsgFactory&);
	MsgFactory& operator=(const MsgFactory&);
};


#define SYSTEM                       "SYSTEM"
#define SYSTEM_TIMEOUT_MSG           "SystemTimeoutMsg"
#define SYSTEM_ERROR_MSG             "SystemErrorMsg"
#define SESSION_NOT_FOUND_ERROR_MSG  "SessionNotFoundErrorMsg"

#define SYSTEM_OK_ERR         0
#define SYSTEM_TIMEOUT_ERR    5000
#define SYSTEM_ERROR_ERR      6000
#define SESSION_NOT_FOUND_ERR 7000

#define SYSTEM_MESSAGE_MAP_LIST(V)                \
    V(SYSTEM_TIMEOUT_MSG,          SystemTimeoutMsg)   \
    V(SYSTEM_ERROR_MSG,            SystemErrorMsg)     \
    V(SESSION_NOT_FOUND_ERROR_MSG, SessionNotFoundErrorMsg)

struct SystemMsgFactory : public MsgFactory
{
    SystemMsgFactory();
	virtual ~SystemMsgFactory() {}
	virtual strList &KnownTypes() const;
	virtual Msg* Create(const char* type);
	virtual void Destroy(const Msg* msg);

private:
    static strList types;
};

#endif //MSG_H_

