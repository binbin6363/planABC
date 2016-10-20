#include "client_msg.h"

#include <google/protobuf/text_format.h> // header for TextFormat
#include <sstream>

#include "comm.h"
#include "log.h"

using namespace utils;
using namespace common;

void PrintPbData(const Message &message, const char *desc)
{
    // log protobuf info
    const char *tag = "print protobuf info:";
    if (NULL != desc)
    {
        tag = desc;
    }
    std::string printer;
    TextFormat::PrintToString(message, &printer);
    LOG(DEBUG)("%s\n%s", tag, printer.c_str());
}


/*
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
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    outpkg.offset_head(sizeof(COHEADER));
    outpkg.set_head(msg_header());
    if (!outpkg.good())
    {
        LOG(ERROR)("CliBeatMsg encode msg failed.");
        return -1;
    }
    length = outpkg.length();
    LOG(DEBUG)("CliBeatMsg encode msg succeed. %s", ToString().c_str());
    return 0;
}   
*/

FrontLoginMsg::FrontLoginMsg()
    : CoMsg()
    , hash_id_(0)
{
}

FrontLoginMsg::~FrontLoginMsg()
{
}

int FrontLoginMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontLoginMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontLoginMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontLoginMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    
}

int FrontLoginMsg::Encode(char *data, uint32_t &length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_request_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackLoginMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_request_, coheader.print());
    front_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;

}

uint32_t FrontLoginMsg::hashid() const
{
    return hash_id_;
}

void FrontLoginMsg::set_hashid(uint32_t id)
{
    hash_id_ = id;
}




//std::string FrontLoginMsg::Serialize() const
//{
//    std::ostringstream oss;
//	const string &token         = front_request_.token();    //登录的token
//	const string &deviceID      = front_request_.deviceid();    //设备唯一ID
//	const string &description   = front_request_.description();    //设备名称
//	int32   magicNum	  = front_request_.magicnum();    //固定值123456789
//	int32   status        = front_request_.status();	   //客户端状态
//	int32   deviceType    = front_request_.devicetype();    //客户端设备类型
//    int32   deviceVersion = front_request_.deviceversion();    //客户端版本号
//    int32   condid         = front_request_.condid();    //所在cond的id
//	// TODO: NEED ADD
//	oss << "FrontLoginMsg:{token:" << token << ", device id:" << deviceID 
//        << ", description:" << description << ", magic num:" << magicNum 
//        << ", status:" << status << ", device type:" << deviceType 
//        << ", device version:" << deviceVersion << ", cond id:" << condid 
//        << "}";
//    return oss.str();
//}


BackLoginMsg::BackLoginMsg()
    : CoMsg()
{
}

BackLoginMsg::~BackLoginMsg()
{
}

int BackLoginMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("BackLoginMsg get header failed.");
        return ret;
    }
    LOG(INFO)("BackLoginMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_result_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackLoginMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_result_, msg_header().print());
    return ret;    

}


int BackLoginMsg::Encode(char * data, uint32_t & length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackLoginMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, coheader.print());
    front_result_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}

UserKeepAliveMsg::UserKeepAliveMsg ()
    : CoMsg()
{
}

UserKeepAliveMsg::~UserKeepAliveMsg()
{
}

int UserKeepAliveMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("UserKeepAliveMsg get header failed.");
        return ret;
    }
    LOG(INFO)("UserKeepAliveMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("UserKeepAliveMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    
}


uint32_t UserKeepAliveMsg::uid()
{
    return front_request_.uid();
}

uint32_t UserKeepAliveMsg::cond_id()
{
    return front_request_.condid();
}

uint32_t UserKeepAliveMsg::device_type()
{
    return front_request_.devicetype();
}

uint32_t UserKeepAliveMsg::client_ver()
{
    return front_request_.version();
}

const string &UserKeepAliveMsg::device_id()
{
    return front_request_.deviceid();
}


FrontWithdrawMsg::FrontWithdrawMsg()
    : CoMsg()
    {
    }

FrontWithdrawMsg::~FrontWithdrawMsg()
{
}

int FrontWithdrawMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("FrontWithdrawMsg get header failed.");
        return ret;
    }
    LOG(INFO)("FrontWithdrawMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_request_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("FrontWithdrawMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_request_, msg_header().print());
    return ret;    
}


BackWithdrawMsg::BackWithdrawMsg()
{
}

BackWithdrawMsg::~BackWithdrawMsg()
{
}


int BackWithdrawMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("BackLoginMsg get header failed.");
        return ret;
    }
    LOG(INFO)("BackWithdrawMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().head_len;
    int len = length - msg_header().head_len;
    if (!front_result_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackWithdrawMsg ParsePartialFromArray failed.");
    }
    PrintPbData(front_result_, msg_header().print());
    return ret;    

}


int BackWithdrawMsg::Encode(char * data, uint32_t & length) const
{
    CoMsg::Encode(data, length);
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = front_result_.ByteSize();
    length = head_len + body_len;
    COHEADER coheader = msg_header();
    coheader.len = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackWithdrawMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result_, coheader.print());
    front_result_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}



#if 0
BatchNotifyStatusMsg::BatchNotifyStatusMsg()
    : CoMsg()
    , server_id_("")
{
    status_ret_.Clear();
    merged_user_.clear();
}

BatchNotifyStatusMsg::~BatchNotifyStatusMsg()
{

}

int BatchNotifyStatusMsg::ClassfyOneUser(int32_t condid, int32_t uid)
{
    LOG(DEBUG)("ClassfyOneUser, condid:%d, uid:%d", condid, uid);
    UserList &user_list = merged_user_[I2str(condid)];
    user_list.push_back(uid);
    return 0;
}


int BatchNotifyStatusMsg::Encode(char *data, uint32_t &length) const
{
    // check
    UserList &user_list = merged_user_[server_id_];
    if (user_list.empty())
    {
        LOG(WARN)("no user distributed in cond:%s", server_id_.c_str());
        length = 0;
        return -1;
    }
    LOG(INFO)("BatchNotifyStatusMsg, Encode status msg. condid:%s, user num:%zu"
        , server_id_.c_str(), user_list.size());
    
    // make result 
    SrvBatchStatusChangeNotice  my_status_notify_msg;
    SrvStatusRet *status = my_status_notify_msg.mutable_status();
    status->CopyFrom(status_ret_);

    for ( ; !user_list.empty(); )
    {
        my_status_notify_msg.add_touids(user_list.front());
        user_list.pop_front();
    }

    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = my_status_notify_msg.ByteSize();
    length = head_len + body_len;
    const COHEADER &header = msg_header();
    COHEADER coheader;
    coheader.uid = Str2i(server_id_.c_str());
    coheader.cmd = CMD_BATCH_USER_STATUS_CHANGE;
    coheader.seq = header.seq;
    coheader.len = header.len;
    coheader.pkglen = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("BackLoginMsg encode msg failed.");
        length = 0;
        return -1;
    }

    PrintPbData(my_status_notify_msg, coheader.print());
    my_status_notify_msg.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}   



void BatchNotifyStatusMsg::set_server_id(std::string condid)
{
    server_id_ = condid;
}

std::string BatchNotifyStatusMsg::server_id() const 
{
    return server_id_;
}

std::vector <std::string> BatchNotifyStatusMsg::cond_array()
{
    std::vector <std::string> cond_vec;
    CondUserListIter cond_list_iter = merged_user_.begin();
    CondUserListIter iter_end = merged_user_.end();

    for (; cond_list_iter != iter_end; ++cond_list_iter)
    {
        std::string condid = cond_list_iter->first;
        cond_vec.push_back(condid);
    }
    return cond_vec;
}

// TODO: NOT IMPLEMENTS
int BatchNotifyStatusMsg::ActiveEmit()
{
    // 遍历merged_user_，按照condid发送
    CondUserListIter cond_list_iter = merged_user_.begin();
    CondUserListIter iter_end = merged_user_.end();

    for (; cond_list_iter != iter_end; ++cond_list_iter)
    {
        std::string condid = cond_list_iter->first;
        server_id_ = condid;
        //UserList user_list = cond_list_iter->second;
        
    }
}


// TODO: NOT IMPLEMENTS
int BatchNotifyStatusMsg::FullAutoEmit(int32_t condid)
{
    return 0;
}



FrontGetStatusMsg::FrontGetStatusMsg()
    : CoMsg()
{
}

FrontGetStatusMsg::~FrontGetStatusMsg()
{
}

// 按照uid进行hash，使用hash策略时此接口必须提供，否则比较混乱
uint32_t FrontGetStatusMsg::hashid() const
{
    return msg_header().uid;
}

// 组包发向status server
int FrontGetStatusMsg::Encode(char *data, uint32_t &length) const
{
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = getStatusRequest_.ByteSize();
    length = head_len + body_len;
    const COHEADER &header = msg_header();
    COHEADER coheader;
    coheader.uid = header.uid;
    coheader.cmd = header.cmd;
    coheader.seq = header.seq;
    coheader.len = header.len;
    coheader.pkglen = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("FrontGetStatusMsg encode msg failed.");
        return -1;
    }

    PrintPbData(getStatusRequest_, coheader.print());
    getStatusRequest_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;
}



BackGetStatusMsg::BackGetStatusMsg()
    : CoMsg()
{
}


BackGetStatusMsg::~BackGetStatusMsg()
{
}

int BackGetStatusMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("BackGetStatusMsg get header failed.");
        return ret;
    }
    LOG(INFO)("BackGetStatusMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().len;
    int len = length - msg_header().len;
    if (!back_status_result_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("BackGetStatusMsg ParsePartialFromArray failed.");
    }
    PrintPbData(back_status_result_, msg_header().print());

    // make user_info_
    int status_size = back_status_result_.status_size();
    if (0 == status_size)
    {
        LOG(INFO)("BackGetStatusMsg, no status data. size:%0");
        return ret;
    }
    for ( int i = 0; i < status_size; ++i )
    {
        UserDevInfo dev_info;
        const SrvStatusRet &status_ret = back_status_result_.status(i);
        user_info_.uid = status_ret.uid();
        int dev_size = status_ret.devstatus_size();
        for (int j = 0; j < dev_size; ++j )
        {
            const SrvDevStatus &dev_status = status_ret.devstatus(j);
            dev_info.condid = dev_status.condid();
            dev_info.device = dev_status.device();
            dev_info.status = dev_status.status();         
            LOG(DEBUG)("BackGetStatusMsg, print user device info:%s", dev_info.print());
            user_info_.userDevInfoList.push_back(dev_info);
        }
        break;
    }
    return ret;    
}


std::string BackGetStatusMsg::Serialize() const
{
    std::ostringstream oss;
    const BaseResult &base_ret = back_status_result_.retbase();
    int32_t err_code = base_ret.retcode();
    const std::string &err_msg = base_ret.retmsg();
    int size = back_status_result_.status_size();
    oss << "BackGetStatusMsg:{ret:" << err_code << ", err msg:" << err_msg 
        << ", status size:" << size << "}"; // 简单处理吧
    return oss.str();
}



// ======================================================
// cmd=4001,状态改变请求  CMD_USER_STATUS_CHANGE
// output: SrvStatusChangeRequest
// ======================================================
StatusChangeRequestMsg::StatusChangeRequestMsg()
    :CoMsg()
{
}

StatusChangeRequestMsg::~StatusChangeRequestMsg()
{
}

SrvStatusChangeRequest &StatusChangeRequestMsg::mutable_pb_msg()
{
    return status_change_request_;
}

int StatusChangeRequestMsg::Encode(char *data, uint32_t &length) const
{
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);

    int body_len = status_change_request_.ByteSize();
    length = head_len + body_len;
    const COHEADER &header = msg_header();
    COHEADER coheader;
    coheader.uid = header.uid;
    coheader.cmd = header.cmd;
    coheader.seq = header.seq;
    coheader.len = header.len;
    coheader.pkglen = length;
    outpkg.set_head(coheader);
    if (!outpkg.good())
    {
        LOG(ERROR)("StatusChangeRequestMsg encode msg failed.");
        return -1;
    }

    PrintPbData(status_change_request_, coheader.print());
    status_change_request_.SerializePartialToArray(outpkg.getCur(), outpkg.remainLength());
    return 0;

}   

StatusChangeResultMsg::StatusChangeResultMsg()
    :CoMsg()
{
}

StatusChangeResultMsg::~StatusChangeResultMsg()
{
}

const SrvStatusChangeResult &StatusChangeResultMsg::pb_msg()
{
    return status_change_result_;
}


int StatusChangeResultMsg::Decode(const char *data, uint32_t length)
{
    int ret = CoMsg::Decode(data, length);
    if (0 != ret)
    {
        LOG(ERROR)("StatusChangeResultMsg get header failed.");
        return ret;
    }
    LOG(INFO)("StatusChangeResultMsg, Decode. [%s]", msg_header().print());

    const char *pbody = data + msg_header().len;
    int len = length - msg_header().len;
    if (!status_change_result_.ParsePartialFromArray(pbody, len))
    {
        ret = 2;
        LOG(ERROR)("StatusChangeResultMsg ParsePartialFromArray failed.");
    }
    PrintPbData(status_change_result_, msg_header().print());
    return ret;    
}

// ======================================================
// cmd=200,从dbp拉取联系人列表的请求消息
// output: (int32_t)uid
// ======================================================
DbpGetContactorMsg::DbpGetContactorMsg  ()
    : CoMsg()
    , uid_(0)
{
}

DbpGetContactorMsg::~DbpGetContactorMsg()
{
}

int DbpGetContactorMsg::Encode(char *data, uint32_t &length) const
{
    BinOutputPacket<> outpkg(data, MAX_SEND_BUFF_LEN);
    outpkg.offset_head(sizeof(COHEADER));

    outpkg << uid_;

    const COHEADER &header = msg_header();
    COHEADER coheader;
    coheader.uid = header.uid;
    coheader.seq = header.seq;
    coheader.cmd = GET_CONTACT_LIST;
    coheader.len = header.len;
    coheader.pkglen = outpkg.length();
    length = outpkg.length();

    outpkg.set_head(coheader);
    
    if (!outpkg.good())
    {
        LOG(ERROR)("DbpGetContactorMsg, inpkg is error.");
        return -1;
    }

    LOG(DEBUG)("DbpGetContactorMsg encode msg succeed. %s", ToString().c_str());
    return 0;
}

std::string DbpGetContactorMsg::Serialize() const
{
    std::ostringstream oss;
    oss << "DbpGetContactorMsg:{uid:" << uid_ << "}"; 
    return oss.str();
}

// ======================================================
// cmd=200,从dbp拉取联系人列表的结果消息
// input: (int32_t)uid + (uint32_t)unRet + (uint32_t)unNum + [(int32_t)contactoruid + (int32_t)contactoruid + ...]
// ======================================================
BackDbpGetContactorMsg::BackDbpGetContactorMsg()
    : CoMsg()
{
}

BackDbpGetContactorMsg::~BackDbpGetContactorMsg()
{
}

int BackDbpGetContactorMsg::Decode(const char *data, uint32_t length)
{
    if (0 != CoMsg::Decode(data, length))
    {
        LOG(ERROR)("BackDbpGetContactorMsg, base decode failed.");
        return 2;
    }

    int head_len = msg_header().len;
    const char *body = data + head_len;
    int len = length - head_len;
    BinInputPacket<> inpkg(const_cast<char *>(body), len);
    int32_t uid = 0;
    uint32_t unret = 0;
    int32_t contactor_uid = 0;
    uint32_t uid_size = 0;
    uint32_t num = 0;
    inpkg >> uid >> unret >> num;
    LOG(INFO)("BackDbpGetContactorMsg decode, uid:%d, ret:%u, contactor num:%u"
        , uid, unret, num);
    // TODO: num 暂时做4096的限制，可能引起bug
    if (num > 4096)
    {
        LOG(ERROR)("BackDbpGetContactorMsg, num > 4096.");
        return 2;
    }

    user_array_.reserve(num);
    for (int i = 0; i < num; ++i )
    {
        inpkg >> contactor_uid;
        user_array_.push_back(contactor_uid);
    }

    if (!inpkg.good())
    {
        LOG(ERROR)("BackDbpGetContactorMsg, inpkg is error");
        return 2;
    }

    return 0;
}

const UserArray &BackDbpGetContactorMsg::user_array() const 
{
    return user_array_;
}

const UserArray &BackDbpGetContactorMsg::user_array() 
{
    return user_array_;
}
#endif

