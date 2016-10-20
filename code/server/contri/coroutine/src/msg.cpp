/**
 * @filedesc: 
 * msg.cpp
 * @author: 
 *  bbwang
 * @date: 
 *  2015/1/7 16:02:41
 * @modify:
 *
**/


#include "msg.h"
#include <cxxabi.h>
//#include <sstream>
#include "libco_routine_manager.h"
#include "utils.h"
#include "option.h"

using namespace utils;

// static variable
ServiceFactoryMap MsgCreator::factories_;
strList SystemMsgFactory::types;


Msg::Msg()
    : len(0)
    , max_send_size(MAX_PACKET_LENGTH_FRAME) // 默认是4500,为发包制定
    , receiver_coid(0)
    , sender_coid(0)
    , trans_id(0)
    , transfer_str("")
{
    ++msg_counter;
}

Msg::Msg(const Msg &other)
{
    this->len = other.len;
    this->max_send_size = other.max_send_size;
    this->receiver_coid = other.receiver_coid;
    this->sender_coid = other.sender_coid;
    this->trans_id = other.trans_id;
    this->transfer_str = other.transfer_str;
    
    ++msg_counter;
}

Msg &Msg::operator=(const Msg &other)
{
    if (this == &other)
    {
        return *this;
    }
    else
    {
		this->len = other.len;
        this->max_send_size = other.max_send_size;
        this->receiver_coid = other.receiver_coid;
        this->sender_coid = other.sender_coid;
        this->trans_id = other.trans_id;
        this->transfer_str = other.transfer_str;
        
        ++msg_counter;
    }
    return *this;
}

Msg::~Msg()
{
    --msg_counter;
}

// msg
bool Msg::IsReply()
{ 
    return (0 != receiver_coid); 
}

bool Msg::IsSysError()
{
    return (GetTypeName() == SYSTEM_ERROR_MSG);
}


bool Msg::IsTimeout()
{
    return (GetTypeName() == SYSTEM_TIMEOUT_MSG);
}

uint32_t Msg::hashid() const
{
    LOG(ERROR)("base msg has no specified hashid.");
    return 0;
}

std::string Msg::server_id() const
{
    LOG(ERROR)("base msg has no specified server id.");
    return std::string("");
}

void Msg::SetReceiverCoid(uint32_t recv_coid)
{
    receiver_coid = recv_coid;
}


void Msg::SetRequestCoId()
{
    receiver_coid = 0;
    sender_coid = CoroutineMgr::Instance().GetCurCoId();
//    LOG(DEBUG)("request, receiver coid:0, sender coid:%u", sender_coid);
}

void Msg::SetReplyCoId()
{
    receiver_coid = sender_coid;
    sender_coid = 0;
    LOG(DEBUG)("response, receiver coid:%u, sender coid:0", receiver_coid);
}

int Msg::PutDataLength(char *data, uint32_t length) const
{
    int ret = 0;
    if (NULL == data || 0 == length)
    {
        LOG(ERROR)("PutDataLength, data is null, or length is 0");
        ret = -1;
        return ret;
    }
    uint32_t *plen = reinterpret_cast<uint32_t *>(data);
    if (NULL == plen)
    {
        LOG(ERROR)("PutDataLength, cast data to uint32_t failed.");
        return -2;
    }
    *plen = htonl(length);
    LOG(DEBUG)("PutDataLength, fill data len:%u", length);

    return ret;
}

int Msg::get_err_no() const
{
    return 0;
}   

string Msg::get_err_msg() const
{
    return "everything is ok.";
}

void Msg::set_err_msg(const string &msg_str) 
{
    (void)msg_str;
}

//////////////////////////////////////////////////////////////////////////

CoMsg::CoMsg()
:Msg()
{
}

CoMsg::~CoMsg()
{

}

uint32_t CoMsg::GetCmd() const
{
    return msg_header_.cmd;
}

void CoMsg::set_msg_header(const COHEADER &header)
{
    msg_header_ = header;
    sender_coid = msg_header_.sender_coid;
    receiver_coid = msg_header_.receiver_coid;
}



    
uint32_t CoMsg::hashid() const
{
    LOG(INFO)("CoMsg use header uid for hashid.");
    return msg_header_.uid;
}

int CoMsg::Decode(const char *data, uint32_t length)
{
    CHECK_ERROR(NULL == data, return -1, "co msg decode data is NULL.");

    CHECK_ERROR(0 == length, return -1, "co msg decode length is illegal.");

    BinInputPacket<> inpacket(const_cast<char *>(data), length);
    inpacket.offset_head(sizeof(COHEADER));

    inpacket.get_head(msg_header_);

    if( !inpacket.good() )
    {
        LOG(ERROR)("co msg decode msg failed. cmd:%u, type_name:%s",
            this->GetCmd(), this->GetTypeName().c_str());

        return -1;
    }

    len = msg_header_.len;
    receiver_coid = msg_header_.receiver_coid;
    sender_coid = msg_header_.sender_coid;

    LOG(DEBUG)("[header] co msg decode succeed. header:%s, type_Name:%s", 
        msg_header_.print(), this->GetTypeName().c_str());

    return 0;
}

int CoMsg::Encode(char *data, uint32_t &length) const
{
    LOCAL_UNREFERENCED_PARAMETER( data );
    LOCAL_UNREFERENCED_PARAMETER( length );

    msg_header_.head_len = sizeof(COHEADER);
    msg_header_.receiver_coid = receiver_coid;
    msg_header_.sender_coid = sender_coid;
    LOG(INFO)("call co msg encode, assign coid, receiver coid:%u, sender coid:%u"
        , receiver_coid, sender_coid);

    return 0;
}

//////////////////////////////////////////////////////////////////////////
//SystemTimeoutMsg::SystemTimeoutMsg *Instance()
//{
//    static SystemTimeoutMsg inst;
//    return &inst;
//}

SystemTimeoutMsg::SystemTimeoutMsg()
:Msg()
{
}

SystemTimeoutMsg::~SystemTimeoutMsg()
{
}

// time out msg
int SystemTimeoutMsg::Decode(const char *data, uint32_t length)
{
    LOCAL_UNREFERENCED_PARAMETER(data);
    LOCAL_UNREFERENCED_PARAMETER(length);
    return 0;
}

int SystemTimeoutMsg::Encode(char *data, uint32_t &length) const
{
    LOCAL_UNREFERENCED_PARAMETER(data);
    LOCAL_UNREFERENCED_PARAMETER(length);
    return 0;
}

uint32_t SystemTimeoutMsg::GetCmd() const
{
    return 0;
}

void SystemTimeoutMsg::set_curtime(time_t curtime)
{
    this->cur_time_ = curtime;
}

void SystemTimeoutMsg::set_lasttime(time_t lasttime)
{
    this->last_time_ = lasttime;
}

int SystemTimeoutMsg::get_err_no() const
{
    return SYSTEM_TIMEOUT_ERR;
}   

string SystemTimeoutMsg::get_err_msg() const
{
    return "backend timeout.";
}



// system error msg
//SystemErrorMsg::SystemErrorMsg *Instance()
//{
//    static SystemErrorMsg inst;
//    return &inst;
//}

SystemErrorMsg::SystemErrorMsg()
    :  CoMsg()
    , err_msg_("system error.")
{
}


SystemErrorMsg::~SystemErrorMsg()
{
}

int SystemErrorMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    LOG(INFO)("system error msg decode, just decode header.");
    return ret;
}

int SystemErrorMsg::get_err_no() const
{
    return SYSTEM_ERROR_ERR;
}   

string SystemErrorMsg::get_err_msg() const
{
    return err_msg_;
}

void SystemErrorMsg::set_err_msg(const string &msg)
{
    err_msg_ = msg;
}


// session not found msg
//SessionNotFoundErrorMsg::SessionNotFoundErrorMsg *Instance()
//{
//    static SessionNotFoundErrorMsg inst;
//    return &inst;
//}

SessionNotFoundErrorMsg::SessionNotFoundErrorMsg()
    :  CoMsg()
{

}

SessionNotFoundErrorMsg::~SessionNotFoundErrorMsg()
{
}

int SessionNotFoundErrorMsg::Decode(const char *data, uint32_t length)
{
    LOCAL_UNREFERENCED_PARAMETER(data);
    LOCAL_UNREFERENCED_PARAMETER(length);
    return 0;
}

int SessionNotFoundErrorMsg::Encode(char *data, uint32_t &length) const

{
    LOCAL_UNREFERENCED_PARAMETER(data);
    LOCAL_UNREFERENCED_PARAMETER(length);
    return 0;
}

int SessionNotFoundErrorMsg::get_err_no() const
{
    return SESSION_NOT_FOUND_ERR;
}

string SessionNotFoundErrorMsg::get_err_msg() const
{
    return "not found session.";
}


// BeatMsg
BeatMsg::BeatMsg()
    :Msg()
{
}
BeatMsg::~BeatMsg()
{
}

int BeatMsg::Decode(const char *data, uint32_t length)
{
    CHECK_ERROR(NULL == data, return -1, "BeatMsg decode data is NULL.");

    BinInputPacket<> inpkg(const_cast<char *>(data), length);
    COHEADER coheader;
    uint32_t head_len = sizeof(COHEADER);
    inpkg.offset_head(head_len);
    inpkg.get_head(coheader);
    len = coheader.len;
    cmd = coheader.cmd;
    seq = coheader.seq;
    receiver_coid = coheader.receiver_coid;
    sender_coid = coheader.sender_coid;
	inpkg >> server_id;
    if(!inpkg.good())
    {
        LOG(ERROR)("BeatMsg decode failed.");
        return -1;
    }
    LOG(DEBUG)("decode beat msg. [%s], serverid:%u", coheader.print(), server_id);
    return 0;
}

int BeatMsg::Encode(char *data, uint32_t &length) const 
{
    CHECK_ERROR(NULL == data, return -1, "BeatMsg encode data is NULL.");

    BinOutputPacket<> outpkg(data, length);
    uint32_t head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);
    COHEADER coheader;
    coheader.cmd = cmd;
    coheader.seq = seq;
    coheader.head_len = head_len;
    coheader.receiver_coid = receiver_coid;
    coheader.sender_coid = sender_coid;
    outpkg.set_head(coheader);
	outpkg << server_id ;
    length = len = outpkg.length();
    // fill length
    uint32_t *plen = (uint32_t *)outpkg.getData();
    *plen = htonl(len);
    LOG(DEBUG)("encode beat msg. [%s], serverid:%u", coheader.print(), server_id);
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);
    return 0;
}
uint32_t BeatMsg::GetCmd() const
{
    return cmd;
}


CliBeatMsg::CliBeatMsg()
    :CoMsg()
{
}


CliBeatMsg::~CliBeatMsg()
{
}

int CliBeatMsg::Decode(const char *data, uint32_t length)
{
    return CoMsg::Decode(data, length);
}

int CliBeatMsg::Encode(char *data, uint32_t &length) const 
{
    BinOutputPacket<> outpkg(data, length);
    outpkg.offset_head(sizeof(COHEADER));
    outpkg.set_head(msg_header());
    if (!outpkg.good())
    {
        LOG(ERROR)("CliBeatMsg encode msg failed.");
        return -1;
    }
    length = outpkg.length();
    LOG(DEBUG)("CliBeatMsg encode msg succeed.");
    return 0;
}   






// cmd line msg
CmdLineMsg::CmdLineMsg()
{
}

CmdLineMsg::~CmdLineMsg()
{
}

int CmdLineMsg::Decode(const char *data, uint32_t length)
{
    cmd_str_ = string(data, length);
    LOG(INFO)("cmd line msg decode:%s", cmd_str_.c_str());
    return 0;
}

int CmdLineMsg::Encode(char *data, uint32_t &length) const
{
    uint32_t str_len = cmd_str_.length();
    memcpy(data, cmd_str_.c_str(), str_len);
    length = str_len;
    LOG(INFO)("cmd line msg encode, length:%u", str_len);
    return 0;
}

string &CmdLineMsg::get_requset_str()
{
    return cmd_str_;
}

void CmdLineMsg::set_result_str(const string &str)
{
    cmd_str_ = str;
}

uint32_t CmdLineMsg::GetCmd() const
{
    LOG(WARN)("call CmdLineMsg GetCmd!!!");
    return (1 << 31);
}


// 返回全名，包括命名空间
std::string Msg::GetTypeName() const
{
	char buffer[128];
	size_t buflen = sizeof(buffer);
	int status;
	char* unmangled = abi::__cxa_demangle(typeid(*this).name(), buffer, &buflen, &status);
	if (status) return typeid(*this).name();
	return unmangled;
}

MsgCreator* MsgCreator::GetGlobalMsgFactory()
{
	static MsgCreator g_msg_creator;
	return &g_msg_creator;
}


MsgCreator::~MsgCreator()
{
    MsgCreator::UnregisterFactory(SYSTEM, sys_factory_);
    SAFE_DELETE(sys_factory_);
}

MsgCreator::MsgCreator()
{
    sys_factory_ = new SystemMsgFactory;
    MsgCreator::RegisterFactory(SYSTEM, sys_factory_);
}

int MsgCreator::RegisterFactory(const std::string& service_name,MsgFactory* factory)
{
    LOG(INFO)("RegisterFactory, service name:%s, factory:%p", service_name.c_str(), factory);
	const strList &types = factory->KnownTypes();

	ServiceFactoryIter serviceFactory = factories_.find(service_name);
	if (serviceFactory != factories_.end())
	{
		// check name not registered
		MsgFactoryMap &tmpFactory = serviceFactory->second;
		for (strListCIter it = types.begin();
			it != types.end(); it ++)
		{
			LOG(DEBUG)("register factory: %s -> %p", it->c_str(), factory);
			FactoryCIter fit = tmpFactory.find(*it);
			if (fit != tmpFactory.end()) {
				if (fit->second != factory) {
					LOG(WARN)("Msg factory already exists: %s", it->c_str());
					return -1;
				}
				LOG(DEBUG)("re-register factory: %s", it->c_str());
			}
		}

		for (strListCIter it = types.begin();
			it != types.end(); it ++)
		{
			tmpFactory.insert(std::make_pair(*it, factory));
		}
	}
	else
	{
		MsgFactoryMap serviceFactory;
		for (strListCIter it = types.begin(); it != types.end(); it ++)
		{
			serviceFactory.insert(std::make_pair(*it, factory));
			LOG(DEBUG)("servicename=%s,register factory: %s -> %p",service_name.c_str(),it->c_str(), factory);
		}
		factories_.insert(make_pair(service_name,serviceFactory));
	}
	return 0;
}

void MsgCreator::UnregisterFactory(const std::string& service_name,MsgFactory* factory)
{
    (void)factory;
	ServiceFactoryIter it = factories_.find(service_name);

    LOG(DEBUG)("unregister factory: %s -> %p", service_name.c_str(), factory);
	if (it == factories_.end())
	{
        LOG(ERROR)("unregister factory failed: %s -> %p", service_name.c_str(), factory);
		return;
	}
	factories_.erase(it);
}


Msg* MsgCreator::Create(const char* service_name,const char* type)
{
	ServiceFactoryIter it = factories_.find(service_name);
    LOG(DEBUG)("create msg, service name:%s, cmd:%s", service_name, type);
	if (it == factories_.end()) {
		LOG(WARN)("create Msg fail: %s, not found service name:%s in factory.", type, service_name);
		return NULL;
	}
	
//	LOG(DEBUG)("create Msg,type=%s,servicename=%s",type,service_name);
	MsgFactoryMap& serviceFactory  = it->second;
	FactoryIter it1 = serviceFactory.find(type);

	if (it1 == serviceFactory.end()) {
		LOG(WARN)("create Msg fail: %s. not found msg type in factory, throw system error message.", type);
        return GetGlobalMsgFactory()->sys_factory_->Create(SYSTEM_ERROR_MSG);
	}

	return it1->second->Create(type);
}


void MsgCreator::Destory(const char* service_name,const Msg* msg)
{
	if (!msg) return;
	
	ServiceFactoryIter it = factories_.find(service_name);

	if (it == factories_.end()) {
		LOG(DEBUG)("MsgCreator destroy msg, cmd:%u, msg addr:%p", msg->GetCmd(), msg);
        delete msg;
		return;
	}
	
	MsgFactoryMap& serviceFactory = it->second;
    std::string str_value = ConvertToString(msg->GetCmd());
	
	FactoryIter it1 = serviceFactory.find(str_value);
	if (it1 == serviceFactory.end()) {
		LOG(WARN)("destroy Msg fail: %s,service_name: %s", str_value.c_str(),service_name);
		return;
	}
	return it1->second->Destroy(msg);
}



#define SYSTEM_MESSAGE_REGISTER(value, name) \
		types.push_back(value);


#define SYSTEM_MESSAGE_CREATE(value, name) \
		if (0 == strcmp(value, type))     \
			return new name();


SystemMsgFactory::SystemMsgFactory()
{
	SYSTEM_MESSAGE_MAP_LIST(SYSTEM_MESSAGE_REGISTER)
}

strList &SystemMsgFactory::KnownTypes() const
{
    return types;
}

Msg* SystemMsgFactory::Create(const char* type)
{
	LOG(INFO)("[frame] SystemMsgFactory try create cmd:%s", type);
	SYSTEM_MESSAGE_MAP_LIST(SYSTEM_MESSAGE_CREATE)
        
	return new SystemErrorMsg;
}


void SystemMsgFactory::Destroy(const Msg* msg)
{
    LOG(DEBUG)("[memory] SystemMsgFactory destory msg:%p", msg);
    SAFE_DELETE(msg);
}


// option implements
Option::Option()
    : translater(NULL)
    , processor(NULL)
    , group_selector(NULL)
{
}
Option::~Option()
{
    translater = NULL;
    processor = NULL;
    group_selector = NULL;
}


