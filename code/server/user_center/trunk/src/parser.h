/**
 * @filedesc: 
 * parser.h, leveldb 结果解析器
 * 
 * @author: 
 *  bbwang
 * @date: 
 *  2015/4/26 14:02:41
 * @modify:
 *
**/

#ifndef PARSER_H_
#define PARSER_H_

namespace common {


class Parser
{
public:
    Parser();
    virtual ~Parser();

    std::map<std::string, std::string> GetResult();
    int Parse(const std::string &key, const std::string &value);
    
private:

    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);

    
private:
    std::string key_;
    std::map<std::string, std::string> result_;
};


// 单人消息解析器
class P2pMsgParser : public Parser
{
public:
    virtual ~P2pMsgParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};


// 群消息解析器
class GroupMsgParser : public Parser
{
public:
    virtual ~GroupMsgParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};


// 系统消息解析器
class SystemMsgParser : public Parser
{
public:
    virtual ~SystemMsgParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};


// 联系人解析器
class ContactParser : public Parser
{
public:
    virtual ~ContactParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};



// 联系人分组解析器
class ContactGroupParser : public Parser
{
public:
    virtual ~ContactGroupParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};



// 群解析器
class GroupParser : public Parser
{
public:
    virtual ~GroupParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};


}


#endif // PARSER_H_


