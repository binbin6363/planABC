/**
 * @filedesc: 
 * parser.cpp
 * @author: 
 *  bbwang
 * @date: 
 *  2015/4/26 14:02:41
 * @modify:
 *
**/


#include "parser.h"
namespace common {

int Parser::Parse(const std::string &key, const std::string &value)
{
    int ret = 0;
    key_ = key;
    ret = parse_key(key);

    ret |= parse_value(value);

    return ret;
}

//P2pMsgParser
P2pMsgParser::~P2pMsgParser()
{
}


int P2pMsgParser::parse_key()
{
    return 0;
}


int P2pMsgParser::parse_value()
{
    return 0;
}


//GroupMsgParser
GroupMsgParser::~GroupMsgParser()
{
}


int GroupMsgParser::parse_key()
{
    return 0;
}


int GroupMsgParser::parse_value()
{
    return 0;
}


//SystemMsgParser
SystemMsgParser::~SystemMsgParser()
{
}


int SystemMsgParser::parse_key(const std::string &key)
{
    return 0;
}


int SystemMsgParser::parse_value(const std::string &value)
{
    BinInputPacket<> inpkg(const_cast<char *>(value.c_str()), value.length());

    uint8_t tag = 0;
    uint8_t type = 0;    
    char value_str[MAX_SEND_BUFF_LEN] = {0};
    
    for (; !inpkg.eof() && inpkg.good(); )
    {
        memset(value_str, 0, sizeof(value_str));
        inpkg >> tag >> type;

        const std::string &tag_name = msg_tag_list[tag].field_name;
        switch (type)
        {
            case kTypeUint8:
            {
                uint8_t value = 0;
                inpkg >> value;
                snprintf(value_str, MAX_STACK_STR_LENGTH, "%u", value);
                result_[tag_name] = value_str;
            }
            break;
            case kTypeUint16:
            {
                uint16_t value = 0;
                inpkg >> value;
                snprintf(value_str, MAX_STACK_STR_LENGTH, "%u", value);
                result_[tag_name] = value_str;
            }
            break;
            case kTypeUint32:
            {
                uint32_t value = 0;
                inpkg >> value;
                snprintf(value_str, MAX_STACK_STR_LENGTH, "%u", value);
                result_[tag_name] = value_str;
            }
            break;
            case kTypeUint64:
            {
                uint64_t value = 0;
                inpkg >> value;
                snprintf(value_str, MAX_STACK_STR_LENGTH, "%lu", value);
                result_[tag_name] = value_str;
            }
            break;
            case kTypeString:
            {
                std::string value("");
                inpkg >> value;
                result_[tag_name] = value;
            }
            break;
            case kTypeInt8:
            {
                int8_t value = 0;
                inpkg >> value;
                snprintf(value_str, MAX_STACK_STR_LENGTH, "%d", value);
                result_[tag_name] = value_str;
            }
            break;
            case kTypeInt16:
            {
                int16_t value = 0;
                inpkg >> value;
                snprintf(value_str, MAX_STACK_STR_LENGTH, "%d", value);
                result_[tag_name] = value_str;
            }
            break;
            case kTypeInt32:
            {
                int32_t value = 0;
                inpkg >> value;
                snprintf(value_str, MAX_STACK_STR_LENGTH, "%d", value);
                result_[tag_name] = value_str;
            }
            break;
            case kTypeInt64:
            {
                int64_t value = 0;
                inpkg >> value;
                snprintf(value_str, MAX_STACK_STR_LENGTH, "%ld", value);
                result_[tag_name] = value_str;
            }
            break;
            default:
            {
                LOG(WARN)("unknow type.");
                ret = -2;
            }
            break;
               
        }
        LOG(DEBUG)("Debody, tag:%u, type:%u, value:%s", tag, type, value_str);
        if (!inpkg.good())
        {
            LOG(ERROR)("inpkg is error.");
            ret = -1;
            break;
        }
    
    }
    return 0;
}


//ContactParser
ContactParser::~ContactParser()
{
}


int ContactParser::parse_key()
{
    return 0;
}


int ContactParser::parse_value()
{
    return 0;
}



//ContactGroupParser
ContactGroupParser::~ContactGroupParser()
{
}


int ContactGroupParser::parse_key()
{
    return 0;
}


int ContactGroupParser::parse_value()
{
    return 0;
}



//GroupParser
GroupParser::~ContactParser()
{
}


int GroupParser::parse_key()
{
    return 0;
}


int GroupParser::parse_value()
{
    return 0;
}


}


