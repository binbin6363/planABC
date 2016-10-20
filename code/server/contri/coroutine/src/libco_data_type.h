
#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

//#include <ostream>
#include <string>
#include <vector>
#include <map>
#include <list>

#include "list_hash.h"
using namespace object_pool;

#include "MemoryPool.h"
#include "co_routine_inner.h"
#include "log.h"
#include "net_event.h"
#include "msg.h"

using namespace std;

// TODO: desprate log
//#ifndef LOG
//#define LOG(level) printf
//#endif

namespace libco_src
{

// 框架服务的id，由应用设置,使用时截断至8位
extern uint16_t service_id;

/*
 * 需要保证其他服务主动请求的coid与本服务发出的coid不一样
 * 否则会导致协程被错误唤醒
 * 目前的做法是在coid中带上本服务的id以区别，要求就是所有的服务id不能相同，略显山寨
 *  (uint8_t)service_id + (uint24_t) auto_incr_num
 */
inline uint32_t GetNewId()
{
    static uint32_t u32_id = 0;
    uint32_t id = 0;
    
    ++u32_id;
    if(0xFFFFFF == u32_id)
        u32_id = 1;
    
    id = ((uint32_t)service_id << 24) | u32_id;
    
    return id;
}




// 框架内部错误码
const static int TIME_OUT_ERROR       = -1;
const static int NET_LOST_ERROR       = -2;
const static int COID_ERROR           = -3;
const static int ENCODE_ERROR         = -4;
const static int NET_NOT_INIT_ERROR   = -5;
const static int CONFIG_NOT_INIT_ERROR= -6;
const static int NET_ABNORMAL_ERROR   = -7;
const static int NOT_COROUTINE_ERROR   = -8;
const static int MSG_CREATE_ERROR     = -9;
const static int SESSION_NOT_FOUND_ERROR = -10;
const static int UNKNOWN_ERROR        = -100;

// 数据单位定义
const static uint32_t _1K = 1 * 1024;
const static uint32_t _1M = 1024 * _1K;
const static uint32_t _1G = 1024 * _1M;

#define CHAR_BUFF_256 256
#define CHAR_BUFF_1024 1024
#define CHAR_BUFF_4096 4096
#define CHAR_BUFF_5120 5120

#if 0
#pragma pack(1)

// !!!do not use memset on CoheaderBase and its inherited class, this operation may broke virtual table
struct CoheaderBase
{
    CoheaderBase()
        :sender_coid(0)
        ,receiver_coid(0)
    {
    }

    virtual ~CoheaderBase()
    {
    }
    
    void ReverseCoid()
    {
        int32_t coid = sender_coid;
        sender_coid = receiver_coid;
        receiver_coid = coid;
    }

    int32_t sender_coid;
    int32_t receiver_coid;
};

struct COHEADER : public CoheaderBase
{
    COHEADER()
        :pkglen(0)
        ,len(0)
        ,cmd(-1)
        ,seq(0)
        ,uid(0)
    {
    }

    virtual ~COHEADER()
    {
    }

    const char * print()
    {
        static char print_buf[CHAR_BUFF_256];
        snprintf(print_buf, sizeof(print_buf),
            " co hdrinfo, pkglen:%d, headlen:%d, cmd:%d, seq:%d, uid:%d, sender_coid:%d, receiver_coid:%d "
            , pkglen, len, cmd, seq, uid, sender_coid, receiver_coid);
        return print_buf;
    }

    const char * print() const
    {
            static char print_buf[CHAR_BUFF_256];
            snprintf(print_buf, sizeof(print_buf),
                " co hdrinfo, pkglen:%d, headlen:%d, cmd:%d, seq:%d, uid:%d, sender_coid:%d, receiver_coid:%d "
                , pkglen, len, cmd, seq, uid, sender_coid, receiver_coid);
            return print_buf;
    }

    int32_t pkglen; // sizeof(coheader) + sizeof(body)
    int32_t len;    //  sizeof(coheader)
    int32_t cmd;
    int32_t seq;
    int32_t uid;

}; // LIBCO FRAME HEADER

#pragma pack()

// libco frame HEADER
template<>
template<>
void BinInputPacket<true>::get_head<COHEADER>(COHEADER& value);

template<>
template<>
void BinOutputPacket<true>::set_head<COHEADER>(COHEADER value);

#endif

#if 0
typedef struct SERVER_CONFIG_INFO
{
    std::string server_ip;//对端地址
    std::string server_port;
    std::string server_id;    //id
    std::string service_name; // 服务名称
    uint32_t  time_out;//超时时间
    uint32_t  heart_beat_interval;//心跳间隔
    uint32_t  reconnect_interval;//重连间隔
    uint32_t  conn_time_out; //连接超时
    uint32_t  max_buf_size; // 服务接收的包最大长度

    const char *print() const
    {
        static char szBuf[CHAR_BUFF_1024] = {0};
        snprintf(szBuf, sizeof(szBuf), "addr:%s:%s, server_id:%s, service_name:%s, time_out:%u, heart_beat_interval:%u, reconnected_interval:%u, conn_time_out:%u, max_buf_size:%u"
            ,server_ip.c_str(), server_port.c_str(), server_id.c_str(), service_name.c_str(), time_out, heart_beat_interval, reconnect_interval, conn_time_out, max_buf_size);

        return szBuf;
    }
}Server_Info;

typedef std::vector<Server_Info> ServerInfoVec;
#endif

// type define
typedef std::map<std::string, std::string > KVData;
typedef KVData::const_iterator KVDCIter;
typedef KVData::iterator KVDIter;


}

#endif // __DATA_TYPE_H__

