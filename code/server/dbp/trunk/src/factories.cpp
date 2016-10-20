/**
* file: factories.cpp
* desc: factories file.
* auth: bbwang
* date: 2015/11/3
*/

#include "factories.h"
#include "string_util.h"
#include "constants.h"
#include "client_msg.h"
#include "log.h"


using namespace utils;
using namespace common;


#define MESSAGE_REGISTER(value, name) \
		types.push_back(StringUtil::u32tostr(value));

#define MESSAGE_CREATE(value, name) \
		case value:                 \
			return new name();


/**********************************************************/
/* client msg factory.                                    */
/**********************************************************/
strList CliMsgFactory::types;
CliMsgFactory::CliMsgFactory()
{
	CLI_MESSAGE_MAP_LIST(MESSAGE_REGISTER)
}

CliMsgFactory::~CliMsgFactory()
{
    types.clear();
}

strList &CliMsgFactory::KnownTypes() const
{
    return types;
}


Msg* CliMsgFactory::Create(const char* type)
{
	uint32_t cmd = StringUtil::strtou32(type);
	LOG(DEBUG)("CliMsgFactory try create cmd:%s", type);
	switch(cmd)
	{
		CLI_MESSAGE_MAP_LIST(MESSAGE_CREATE)
		default:
			return NULL;
	}
}

void CliMsgFactory::Destroy(const Msg* msg)
{
    LOG(DEBUG)("[memory] CliMsgFactory destory msg:%p", msg);
    SAFE_DELETE(msg);
}


/**********************************************************/
/* dbp msg factory.                                       */
/**********************************************************/
strList DbagentMsgFactory::types;
DbagentMsgFactory::DbagentMsgFactory()
{
    DBAGENT_MESSAGE_MAP_LIST(MESSAGE_REGISTER)
}

DbagentMsgFactory::~DbagentMsgFactory()
{
    types.clear();
}


strList &DbagentMsgFactory::KnownTypes() const
{
    return types;
}
    
Msg* DbagentMsgFactory::Create(const char* type)
{
	uint32_t cmd = StringUtil::strtou32(type);
	LOG(DEBUG)("DbagentMsgFactory try create cmd:%s", type);
	switch(cmd)
	{
		DBAGENT_MESSAGE_MAP_LIST(MESSAGE_CREATE)
		default:
			return NULL;
	}
}


void DbagentMsgFactory::Destroy(const Msg* msg)
{
    LOG(DEBUG)("[memory] DbagentMsgFactory destory msg:%p", msg);
    SAFE_DELETE(msg);
}


/**********************************************************/
/* redis proxy msg factory.                                       */
/**********************************************************/
strList RedisProxyMsgFactory::types;
RedisProxyMsgFactory::RedisProxyMsgFactory()
{
    REDIS_PROXY_MESSAGE_MAP_LIST(MESSAGE_REGISTER)
}

RedisProxyMsgFactory::~RedisProxyMsgFactory()
{
    types.clear();
}


strList &RedisProxyMsgFactory::KnownTypes() const
{
    return types;
}
    
Msg* RedisProxyMsgFactory::Create(const char* type)
{
	uint32_t cmd = StringUtil::strtou32(type);
	LOG(DEBUG)("RedisProxyMsgFactory try create cmd:%s", type);
	switch(cmd)
	{
		DBAGENT_MESSAGE_MAP_LIST(MESSAGE_CREATE)
		default:
			return NULL;
	}
}


void RedisProxyMsgFactory::Destroy(const Msg* msg)
{
    LOG(DEBUG)("[memory] RedisProxyMsgFactory destory msg:%p", msg);
    SAFE_DELETE(msg);
}

