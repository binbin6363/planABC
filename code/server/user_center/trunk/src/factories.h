/**
* file: factories.h
* desc: ban liao, factories file.
* auth: bbwang
* date: 2015/3/3
*/

#ifndef FACTORIES_H_
#define FACTORIES_H_
#include "msg.h"
#include "protocols.h"

using namespace common;

/**********************************************************/
/* message map table.                                     */
/**********************************************************/
// client request message map
#define CLI_MESSAGE_MAP_LIST(V)                           \
	V(CMD_KEEPALIVE,           BeatMsg)                   \
	V(CMD_LOGIN,               FrontLoginMsg)             \
	V(CMD_USER_KEEPALIVE,	   UserKeepAliveMsg)


// dbp response message map
#define DBP_MESSAGE_MAP_LIST(V)                         \
    V(CMD_KEEPALIVE,           BeatMsg)            \
	V(CMD_LOGIN,               BackLoginMsg) \
	V(CMD_INNER_INSERT_USER,   BackLoginMsg) \
	V(CMD_INNER_CHECK_USER,    BackLoginMsg) 



/**********************************************************/
/* client msg factory.                                    */
/**********************************************************/
class CliMsgFactory : public MsgFactory
{
public:
    CliMsgFactory();
    virtual ~CliMsgFactory();
	virtual strList &KnownTypes() const;
	virtual Msg* Create(const char* type);
	virtual void Destroy(const Msg* msg);

private:
    static strList types;
};



/**********************************************************/
/* dbp msg factory.                                       */
/**********************************************************/
class DbpMsgFactory : public MsgFactory
{

public:
    DbpMsgFactory();
    virtual ~DbpMsgFactory();
	virtual strList &KnownTypes() const;
	virtual Msg* Create(const char* type);
	virtual void Destroy(const Msg* msg);

private:
    static strList types;

};
#endif //FACTORIES_H_
