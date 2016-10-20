/**
* file: factories.cpp
* desc: ban liao, factories file.
* auth: bbwang
* date: 2015/3/3
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
strList DbpMsgFactory::types;
DbpMsgFactory::DbpMsgFactory()
{
    DBP_MESSAGE_MAP_LIST(MESSAGE_REGISTER)
}

DbpMsgFactory::~DbpMsgFactory()
{
    types.clear();
}


strList &DbpMsgFactory::KnownTypes() const
{
    return types;
}

Msg* DbpMsgFactory::Create(const char* type)
{
	uint32_t cmd = StringUtil::strtou32(type);
	LOG(DEBUG)("DbpMsgFactory try create cmd:%s", type);
	switch(cmd)
	{
		DBP_MESSAGE_MAP_LIST(MESSAGE_CREATE)
		default:
			return NULL;
	}
}


void DbpMsgFactory::Destroy(const Msg* msg)
{
    LOG(DEBUG)("[memory] DbpMsgFactory destory msg:%p", msg);
    SAFE_DELETE(msg);
}


