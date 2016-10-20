#ifndef TRANSLATER_H_
#define TRANSLATER_H_

#include <stdint.h>
#include <string>

class Msg;
class Net_Event;


// �����ṩ�Ľ�msg�Ľӿ�
Msg *interpret(Net_Event *ev);

// ���ڴ������н��ͳ���Ϣ
class Translater
{
public:
    virtual Msg *Decode(const char *data, uint32_t len, const std::string &service_name) = 0;
    // ���ر����ĳ���
	virtual int Encode(char* data, uint32_t len, Msg* msg, const std::string &service_name) const = 0;
    virtual ~Translater() {}
};

// ����������Ľ���
class BinaryTranslater : public Translater
{
public:
    virtual ~BinaryTranslater();
    virtual Msg *Decode(const char *data, uint32_t len, const std::string &service_name);
    // ���ر����ĳ���
	virtual int Encode(char* data, uint32_t len, Msg* msg, const std::string &service_name) const;
	static BinaryTranslater *Instance()
	{
		static BinaryTranslater inst_;
		return &inst_;
	}

private:
    // non copy
    BinaryTranslater();
    BinaryTranslater(const BinaryTranslater&);
    BinaryTranslater &operator=(const BinaryTranslater&);
};



class CmdLineTranslater : public Translater
{
public:
    virtual ~CmdLineTranslater();
    virtual Msg *Decode(const char *data, uint32_t len, const std::string &service_name);
    // ���ر����ĳ���
	virtual int Encode(char* data, uint32_t len, Msg* msg, const std::string &service_name) const;
	static CmdLineTranslater *Instance()
	{
		static CmdLineTranslater inst_;
		return &inst_;
	}

private:
    // non copy
    CmdLineTranslater();
    CmdLineTranslater(const CmdLineTranslater&);
    CmdLineTranslater &operator=(const CmdLineTranslater&);
};


class JsonTranslater : public Translater
{
public:
    virtual ~JsonTranslater();
    virtual Msg *Decode(const char *data, uint32_t len, const std::string &service_name);
    // ���ر����ĳ���
	virtual int Encode(char* data, uint32_t len, Msg* msg, const std::string &service_name) const;
	static JsonTranslater *Instance()
	{
		static JsonTranslater inst_;
		return &inst_;
	}

private:
    // non copy
    JsonTranslater();
    JsonTranslater(const JsonTranslater&);
    JsonTranslater &operator=(const JsonTranslater&);
};

// protobuf��ʽ������
class ProtocTranslater : public Translater
{
public:
    virtual Msg *Decode(const char *data, uint32_t len, const std::string &service_name);
    // ���ر����ĳ���
	virtual int Encode(char* data, uint32_t len, Msg* msg, const std::string &service_name) const;
    virtual ~ProtocTranslater();
	static ProtocTranslater *Instance()
	{
		static ProtocTranslater inst_;
		return &inst_;
	}

private:
    // non copy
    ProtocTranslater();
    ProtocTranslater(const ProtocTranslater&);
    ProtocTranslater &operator=(const ProtocTranslater&);
};


#endif //TRANSLATER_H_

