/**
 * @filedesc: 
 * parser.h, leveldb ���������
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


// ������Ϣ������
class P2pMsgParser : public Parser
{
public:
    virtual ~P2pMsgParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};


// Ⱥ��Ϣ������
class GroupMsgParser : public Parser
{
public:
    virtual ~GroupMsgParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};


// ϵͳ��Ϣ������
class SystemMsgParser : public Parser
{
public:
    virtual ~SystemMsgParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};


// ��ϵ�˽�����
class ContactParser : public Parser
{
public:
    virtual ~ContactParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};



// ��ϵ�˷��������
class ContactGroupParser : public Parser
{
public:
    virtual ~ContactGroupParser();
private:
    virtual int parse_key(const std::string &key);
    virtual int parse_value(const std::string &value);
};



// Ⱥ������
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


