

#ifndef _COND_DATA_TYPE_H_
#define _COND_DATA_TYPE_H_

#include "binpacket.h"
#include "net_event.h"
#include <string>
#include <stdint.h>
#include <vector>
#include <list>
#include <map>
#include <arpa/inet.h>

const static uint32_t MAX_NUM = 1024;
const static uint32_t STACK_STR_MAX_LEN = 1024;


#define DEF_PUSH_MSG_NUM				3//默认推送次数
#define DEF_RESEND_ONCE_SCAN_NUM		1000//每次重推扫描个数
#define DEF_RESEND_MSG_INTERVAL			10//重复推送时间
#define DEF_RESEND_MSG_TIMEER_INTERVAL	1//重复推送消息定时器的间隔
#define DEF_SESSION_TIMEOUT				10//会话超时时间
#define DEF_CONNECT_TIMEOUT				5//连接超时时间
#define DEF_RECONNECT_INTERVAL			1//重连间隔

#define DEF_MSG_NEED_WRITE_BACK			false//是否需要回写
//#define SAFE_DELETE(p)                  { if(p) { delete p; p=NULL; } }
//#define SAFE_DELETE_ARRAY(p)            { if(p) { delete[] (p); (p)=NULL; } }

//#define MAX_SEND_BUFF_LEN 100000




// add
#ifndef ONCE_LOOP_ENTER
#define ONCE_LOOP_ENTER          do {
#endif
#ifndef ONCE_LOOP_LEAVE
#define ONCE_LOOP_LEAVE          }while(0);
#endif

/*
#define CHECK_ERROR_BREAK(EXPRESSION, IN_RET, MSG) if ((EXPRESSION))\
{\
    ret = IN_RET;\
    LOG(ERROR)("%s", MSG);\
    break;\
}  

#define CHECK_WARN_BREAK(EXPRESSION, IN_RET, MSG) if ((EXPRESSION))\
{\
    ret = IN_RET;\
    LOG(WARN)("%s", MSG);\
    break;\
}  

enum
{
    NO_UPDATE   = 0,
    HIT_UPDATE  = 1,
    LESS_UPDATE = 2,
    DIFF_UPDATE = 3,
    MAX_UPDATE  = 4,
};
*/

enum M_ERROR_CODE
{
    // COMMON
    M_SUCCEED             = 0,

    // NET
    M_SERVER_NOT_EXIST    = 5001,
    M_SERVER_NOT_CONNECT  = 5002,

    // PACKET
    M_SERVER_PACKET_SEND_NOT_COMPLETELY = 6001,
    M_SERVER_PACKET_PARSE_ERROR         = 6002,
};

//生成msg_id高4位位消息中心id低28位为消息的id
#ifndef MAKE_SEQ_NUM
#define MAKE_SEQ_NUM(msg_id, msg_centerid) (((uint32_t)(msg_id))|(((uint32_t)(msg_centerid))<<28))
#endif //MAKE_SEQ_NUM
#ifndef MKAE_CENTER_ID
#define MKAE_CENTER_ID(seq) (((uint32_t)(seq))>>28)
#endif //MKAE_CENTER_ID
#ifndef MAKE_MSG_ID
#define MAKE_MSG_ID(seq) (((uint32_t)(seq)) & 0xfffffff)
#endif //MAKE_MSG_ID

// add leveldb
//#ifndef _1KB
//#define _1KB (1*1024)
//#endif
//#ifndef _1MB
//#define _1MB (1*1024*_1KB)
//#endif
//#ifndef _1GB
//#define _1GB (1*1024*_1MB)
//#endif


struct SingleMsgStatus
{
    uint32_t cid;
    uint32_t uid;
    uint32_t maxreadmsgid;

    SingleMsgStatus()
        : cid(0)
        , uid(0)
        , maxreadmsgid(0)
        {}

    void reset()
    {
        cid = 0;
        uid = 0;
        maxreadmsgid = 0;
    }
};

struct GroupMsgStatus
{
    uint32_t groupid;
    uint64_t maxreadmsgid;

    GroupMsgStatus()
        : groupid(0)
        , maxreadmsgid(0)
        {}
    void reset()
    {
        groupid = 0;
        maxreadmsgid = 0;
    }

};


// type define
typedef std::map<std::string, std::string> strmap;
typedef std::list<std::string> strlist;
typedef std::vector<std::string> strvec;
typedef std::map<std::string, uint32_t *> addressmap;
typedef std::list<uint32_t *> addresslist;
typedef std::vector<uint32_t *> addressvec;

typedef strmap::iterator smiter;
typedef strlist::iterator sliter;
typedef strvec::iterator sviter;
typedef addressmap::iterator amiter;
typedef addresslist::iterator aliter;
typedef addressvec::iterator aviter;

typedef strmap::const_iterator smciter;
typedef strlist::const_iterator slciter;
typedef strvec::const_iterator svciter;
typedef addressmap::const_iterator amciter;
typedef addresslist::const_iterator alciter;
typedef addressvec::const_iterator avciter;

typedef std::list<SingleMsgStatus> sstlist;
typedef std::list<GroupMsgStatus> gstlist;
typedef sstlist::iterator sstliter;
typedef gstlist::iterator gstliter;
typedef sstlist::const_iterator sstlciter;
typedef gstlist::const_iterator gstlciter;

#define LEVELDB_SUCCESS   0
#define LEVELDB_PKG_ERROR 100
#define LEVELDB_INNER_ERR 101
#define ZERO 0

#define VEC_DEFAULT_SIZE 10


using namespace std;

typedef struct UID_AND_TIME
{
    uint32_t  uid;
    time_t    times;
}UIDTIME;



typedef struct MSG_ID
{
    uint32_t msg_id;
    uint32_t msg_center_id;
    MSG_ID()
    {
        msg_id = 0;
        msg_center_id = 0;
    }

    MSG_ID(uint32_t msg_id, uint32_t center_id)
    {
        this->msg_id = msg_id;
        msg_center_id = center_id;
    }
}Msg_Id;




#endif
