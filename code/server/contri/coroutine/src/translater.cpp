
#include "translater.h"
#include "msg.h"
#include "binpacket_wrap.h"
#include "net_handle_manager.h"
#include "utils.h"
#include "net_event.h"
#include "option.h"
#include "net_packet.h"
#include "libco_net_session.h"
#include "libco_routine_manager.h"

using namespace utils;

const Option *GetServiceOption(Net_Event *ev)
{
    do {
        if (!ev) {
            LOG(ERROR)("GetServiceOption, net event is null.");
            break;
        }
        Net_Handler *net_handle = Net_Handle_Manager::Instance()->GetHandle(ev->id);
        if (!net_handle) {
            LOG(ERROR)("GetServiceOption, net handle not exist. ev id:%u, ev type:%u", ev->id, ev->net_event_type);
            break;
        }
        LibcoNetSession *session = dynamic_cast<LibcoNetSession *>(net_handle);
        if (!session) {
            LOG(ERROR)("GetServiceOption, get session failed.");
            break;
        }
        return session->option();
    } while (0);
    return NULL;
}

uint32_t DecodeCmd(Net_Event *ev)
{
    uint32_t cmd = 0;
    char *data = ev->packet.ptr();
    int len = ev->packet.length();
    if (NULL == data || (size_t)len < 5 * sizeof(uint32_t) )
    {
        LOG(ERROR)("input stream is error. len:%d", len);
        return cmd;
    }
    cmd = *(uint32_t *)(data + 2*sizeof(uint32_t));
    cmd = ntohl(cmd);
    return cmd;
}


// 对外提供的解msg的接口
Msg *interpret(Net_Event *ev)
{
    char *data = ev->packet.ptr();
    int len = ev->packet.length();
    // 0. check packet empty
    if (NULL == data)
    {
        LOG(DEBUG)("receive event, but packet is empty. netid:%u", ev->id);
        return NULL;
    }
    // 1. 获取服务配置
    const Option *service_option = GetServiceOption(ev);
    if (!service_option)
    {
        LOG(ERROR)("option is not initialized. interpret msg failed.");
        return NULL;
    }
    // 2. 获取translater，创建具体消息
    Translater *translater = service_option->translater;
    if (!translater)
    {
        LOG(ERROR)("Translater not specified. interpret msg failed.");
        return NULL;
    }
    Msg *msg = translater->Decode(data, len, service_option->service_name);
    return msg;
}



BinaryTranslater::BinaryTranslater()
{
}

BinaryTranslater::~BinaryTranslater()
{
}


// (uint32_t)len (uint32_t)cmd (uint32_t)seq (uint32_t)uid
Msg *BinaryTranslater::Decode(const char *data, uint32_t len, const std::string &service_name)
{
    Msg *msg = NULL;
    do {
	if(NULL == data || len <= 0)
	{
		LOG(ERROR)("[BinaryTranslater] decode failed. message data:%s, len=%d", data, len);
		break;
	}
  	BinInputPacket<> readstream(const_cast<char *>(data),len);
	uint32_t cmd = 0;
	uint32_t length = 0;
    readstream >> length >> cmd;
	if (!readstream.good())
	{
		LOG(WARN)("[BinaryTranslater] read cmd error");
		break;
	}
	LOG(DEBUG)("[BinaryTranslater] decode, data len:%u, pkg len:%u, service name:%s, cmd:%u", len, length, service_name.c_str(), cmd);
    LOG_HEX(data, len, utils::L_DEBUG);

    std::string cmd_str = ConvertToString(cmd);
	msg = MsgCreator::Create(service_name.c_str(), cmd_str.c_str());
	if (msg)
	{
		if (msg->Decode(data,len) != 0)
		{
			LOG(WARN)("[BinaryTranslater] message typename:%s, cmd:%d fail", msg->GetTypeName().c_str(), cmd);
			MsgCreator::Destory(service_name.c_str(),msg);
            msg = NULL;
			break;
		}
		LOG(DEBUG)("[BinaryTranslater] decode message header.typename:%s", msg->GetTypeName().c_str());
	}
	else
	{
        msg = new SystemErrorMsg;
        msg->set_err_msg("decode msg failed.");
		LOG(WARN)("[BinaryTranslater] create message fail, cmd:%d", cmd);
	}
    } while (0);
	return msg;
}

int BinaryTranslater::Encode(char* data, uint32_t len, Msg* msg, const std::string &service_name) const
{
    (void)service_name;
    if ( msg->Encode(data,len) != 0)
	{
	    len = ENCODE_ERROR;
		LOG(ERROR)("[BinaryTranslater] encode message fail typename=%s,write size=%d", msg->GetTypeName().c_str(), len);
    	return len;
	}
	LOG(DEBUG)("[BinaryTranslater] encode message header.typename=%s, cmd:%d", msg->GetTypeName().c_str(), msg->GetCmd());
    LOG_HEX(data, len, utils::L_DEBUG);
	return len;
}

CmdLineTranslater::CmdLineTranslater()
{
}

CmdLineTranslater::~CmdLineTranslater()
{
}


// (uint32_t)len (uint32_t)cmd (uint32_t)seq (uint32_t)uid
Msg *CmdLineTranslater::Decode(const char *data, uint32_t len, const std::string &service_name)
{
    Msg *msg = NULL;
	LOG(DEBUG)("[CmdLineTranslater] decode, data len:%u, service name:%s", len, service_name.c_str());
    LOG_HEX(data, len, utils::L_DEBUG);
    do {
	if(NULL == data || len <= 0)
	{
		LOG(ERROR)("[CmdLineTranslater] decode failed. message data:%s, len=%d", data, len);
		break;
	}

	msg = new CmdLineMsg;//MsgCreator::Create(service_name.c_str(), cmd_str.c_str());
	if (msg)
	{
		if (msg->Decode(data,len) != 0)
		{
			LOG(WARN)("[CmdLineTranslater] message typename:%s fail", msg->GetTypeName().c_str());
			//MsgCreator::Destory(service_name.c_str(),msg);
			delete msg;
            msg = NULL;
			break;
		}
		LOG(DEBUG)("[CmdLineTranslater] decode message header.typename:%s", msg->GetTypeName().c_str());
	}
	else
	{
		LOG(WARN)("[CmdLineTranslater] create cmd line message fail");
	}
    } while (0);
	return msg;
}

int CmdLineTranslater::Encode(char* data, uint32_t len, Msg* msg, const std::string &service_name) const
{ 
    (void)service_name;
    if ( msg->Encode(data,len) != 0)
	{
	    len = ENCODE_ERROR;
		LOG(ERROR)("[CmdLineTranslater] encode message fail typename=%s,write size=%d", msg->GetTypeName().c_str(), len);
    	return len;
	}
	LOG(DEBUG)("[CmdLineTranslater] encode message header.typename=%s", msg->GetTypeName().c_str());
    LOG_HEX(data, len, utils::L_DEBUG);
	return len;
}

JsonTranslater::JsonTranslater()
{
}

JsonTranslater::~JsonTranslater()
{
}


// (uint32_t)len (uint32_t)cmd (uint32_t)seq (uint32_t)uid
Msg *JsonTranslater::Decode(const char *data, uint32_t len, const std::string &service_name)
{
    (void)data;
    (void)len;
    (void)service_name;
	return NULL;
}

int JsonTranslater::Encode(char* data, uint32_t len, Msg* msg, const std::string &service_name) const
{ 
    (void)data;
    (void)len;
    (void)msg;
    (void)service_name;
	return 0;
}



// (uint32_t)len (uint32_t)cmd (uint32_t)seq (uint32_t)head_len
Msg *ProtocTranslater::Decode(const char *data, uint32_t len, const std::string &service_name) 
{
    Msg *msg = NULL;
    do {
	if(NULL == data || len <= 0)
	{
		LOG(ERROR)("[ProtocTranslater] decode failed. message data:%s, len:%d", data, len);
		break;
	}
  	BinInputPacket<> readstream(const_cast<char *>(data),len);
	uint32_t cmd = 0;
	uint32_t length = 0;
    readstream >> length >> cmd;
	if (!readstream.good())
	{
		LOG(WARN)("[ProtocTranslater] read cmd error");
		break;
	}
	LOG(DEBUG)("[ProtocTranslater] decode, data len:%u, pkg len:%u, service name:%s, cmd:%u", len, length, service_name.c_str(), cmd);
    LOG_HEX(data, len, utils::L_DEBUG);

    std::string cmd_str = ConvertToString(cmd);
	msg = MsgCreator::Create(service_name.c_str(), cmd_str.c_str());
	if (msg)
	{
		if (msg->Decode(data,len) != 0)
		{
        	LOG(WARN)("[ProtocTranslater] length:%d, cmd:%d. service:%s decode msg typename:%s failed."
                , length, cmd, service_name.c_str(), msg->GetTypeName().c_str());
			MsgCreator::Destory(service_name.c_str(),msg);
            msg = NULL;
			break;
		}
    	LOG(DEBUG)("[ProtocTranslater] length:%d, cmd:%d. service:%s create and decode msg typename:%s ok."
            , length, cmd, service_name.c_str(), msg->GetTypeName().c_str());
	}
	else
	{
        msg = new SystemErrorMsg;
        msg->set_err_msg("decode msg failed.");
    	LOG(WARN)("[ProtocTranslater] length:%d, cmd:%d. service:%s create msg failed."
            , length, cmd, service_name.c_str());
	}
    } while (0);
	return msg;
}


// 返回编码后的长度
int ProtocTranslater::Encode(char* data, uint32_t len, Msg* msg, const std::string &service_name) const 
{
    (void)service_name;
    if ( msg->Encode(data,len) != 0)
	{
	    len = ENCODE_ERROR;
		LOG(ERROR)("[ProtocTranslater] encode message fail typename=%s,write size:%d", msg->GetTypeName().c_str(), len);
    	return len;
	}
	LOG(DEBUG)("[ProtocTranslater] encode message header.typename=%s, cmd:%d", msg->GetTypeName().c_str(), msg->GetCmd());
    LOG_HEX(data, len, utils::L_DEBUG);
	return len;
}

ProtocTranslater::ProtocTranslater()
{
}

ProtocTranslater::~ProtocTranslater()
{
}


