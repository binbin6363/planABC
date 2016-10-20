#ifndef BINPACKET_WRAP_H_
#define BINPACKET_WRAP_H_

#include <ostream>
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <stdio.h>
#include "binpacket.h"

using namespace std;

namespace utils
{
#define CHAR_BUFF_1024 1024

typedef struct SERVER_CONFIG_INFO
{
    std::string server_ip;    //对端地址
    std::string server_port;
    std::string server_id;    //id
    std::string service_name; // 服务名称
    uint32_t  time_out;//超时时间
    uint32_t  heart_beat_interval;//心跳间隔
    uint32_t  reconnect_interval;//重连间隔
    uint32_t  conn_time_out; //连接超时
    uint32_t  max_buf_size; // 服务接收的包最大长度
    // TODO: 访问策略有修改
    uint32_t  group_strategy;     // 会话组的选择策略，哈希，一致性哈希，轮询，动态得分
    uint32_t  session_strategy;   // 会话的选择策略，哈希，一致性哈希，轮询，动态得分
    uint8_t   group_enabled; // 是否有对服务进行分组
    uint32_t  group_id;      // 若有进行分组，那所属的分组id是多少

    SERVER_CONFIG_INFO()
        : server_ip("")
        , server_port("")
        , server_id("")
        , service_name("")
        , time_out(40)
        , heart_beat_interval(10)
        , reconnect_interval(2)
        , conn_time_out(20)
        , max_buf_size(0)
        , group_strategy(-1)   // max unknown stragety
        , session_strategy(-1) // max unknown stragety
        , group_enabled(0)
        , group_id(0)
    {
    }


    ~SERVER_CONFIG_INFO()
    {
    }



    const char *print() const
    {
        static char szBuf[CHAR_BUFF_1024] = {0};
        snprintf(szBuf, sizeof(szBuf), "addr:%s:%s, server_id:%s, service_name:%s,"\
            "time_out:%u, heart_beat_interval:%u, reconnected_interval:%u, "\
            "conn_time_out:%u, max_buf_size:%u, group_enabled:%u, group_id:%u"\
            ", group_strategy:%d, session_strategy:%d"
            ,server_ip.c_str(), server_port.c_str(), server_id.c_str()
            , service_name.c_str(), time_out, heart_beat_interval, reconnect_interval
            , conn_time_out, max_buf_size, group_enabled, group_id, (int)group_strategy, (int)session_strategy);

        return szBuf;
    }
}Server_Info;

typedef std::vector<Server_Info> ServerInfoVec;


#pragma pack(1)

typedef struct header_
{
    header_()
        :len(0)
        ,cmd(0)
        ,seq(0)
        ,head_len(0)
        ,uid(0)
    {
    }

    ~header_()
    {
    }

    const char * print() const
    {
        static char print_buf[CHAR_BUFF_1024];
        snprintf(print_buf, sizeof(print_buf),
            " hdrinfo, len:%u, headlen:%u, cmd:%u, uid:%u, seq:%u ", len, head_len, cmd, uid, seq);
    	return print_buf;
    }

    uint32_t len;    // sizeof(header) + sizeof(body)
    uint32_t cmd;
    uint32_t seq;
    uint32_t head_len; // sizeof(header) 
    uint32_t uid;

}HEADER;


typedef struct coheader_
{
    coheader_()
        :len(0)
        ,cmd(-1)
        ,seq(0)
        ,head_len(0)
        ,uid(0)
        ,sender_coid(0)
        ,receiver_coid(0)
    {
    }

    ~coheader_()
    {
    }
	
    const char * print()
    {
        static char print_buf[1024];
        snprintf(print_buf, sizeof(print_buf),
            " co hdrinfo, len:%u, headlen:%u, cmd:%u, uid:%u, seq:%u, sender_coid:%u, receiver_coid:%u "
            , len, head_len, cmd, uid, seq, sender_coid, receiver_coid);
        return print_buf;
    }

    const char * print() const
    {
            static char print_buf[1024];
            snprintf(print_buf, sizeof(print_buf),
                " co hdrinfo, pkglen:%u, headlen:%u, cmd:%u, uid:%u, seq:%u, sender_coid:%u, receiver_coid:%u "
                , len, head_len, cmd, uid, seq, sender_coid, receiver_coid);
            return print_buf;
    }

    void ReverseCoid()
    {
        uint32_t coid = sender_coid;
        sender_coid = receiver_coid;
        receiver_coid = coid;
    }


    uint32_t len;    //  sizeof(coheader) + sizeof(body)
    uint32_t cmd;
    uint32_t seq;
    uint32_t head_len; // sizeof(coheader)
    uint32_t uid;
    uint32_t sender_coid;
    uint32_t receiver_coid;

}COHEADER; // LIBCO FRAME HEADER


#if 0


// 前端的包头,包含cid
typedef struct BIN_HDR
{
    uint32_t len;
    uint32_t cmd;
    uint32_t seq;
    uint32_t cid;
    uint32_t uid;

    const char *print() const
    {
        static char print_buf[CHAR_BUFF_1024];
        snprintf(print_buf, sizeof(print_buf),
            " hdrinfo, cmd:%u, seq:%u, cid:%u, uid:%u, len:%u", 
            cmd, seq, cid, uid, len);
        return print_buf;
    }
}BHDR;




// 通用的包头
typedef struct CO_BIN_HDR
{
    uint32_t len;          // 整个包的长度
    uint32_t cmd;          // 协议号
    uint32_t seq;          // 请求的序列号
    uint32_t head_len;     // 包头长度=sizeof(COBHDR)
    uint32_t sender_coid;  // 发送方的coid
    uint32_t receiver_coid;// 接收方的coid
    uint32_t server_id;    // 前端的服务id

    void ReverseCoid()
    {
        uint32_t coid = sender_coid;
        sender_coid = receiver_coid;
        receiver_coid = coid;
    }

    const char *print() const
    {
        static char print_buf[CHAR_BUFF_1024];
        snprintf(print_buf, sizeof(print_buf),
            " cobin hdrinfo, len:%u, cmd:%u, seq:%u, head_len:%u, snd_coid:%u, rcv_coid:%u, srv_id:%u", 
            len, cmd, seq, head_len, sender_coid, receiver_coid, server_id);
        return print_buf;
    }
}COBHDR;
#endif

#pragma pack()

// HEADER
template<>
template<>
void BinInputPacket<true>::get_head<HEADER>(HEADER& value);

template<>
template<>
void BinOutputPacket<true>::set_head<HEADER>(HEADER value);

// libco frame HEADER
template<>
template<>
void BinInputPacket<true>::get_head<COHEADER>(COHEADER& value);

template<>
template<>
void BinOutputPacket<true>::set_head<COHEADER>(COHEADER value);


#if 0

// BIN HEADER
template<>
template<>
void BinInputPacket<true>::get_head<BHDR>(BHDR& value);

template<>
template<>
void BinOutputPacket<true>::set_head<BHDR>(BHDR value);

// libco BIN HEADER
template<>
template<>
void BinInputPacket<true>::get_head<COBHDR>(COBHDR& value);

template<>
template<>
void BinOutputPacket<true>::set_head<COBHDR>(COBHDR value);

#endif
} // namespace utils
#endif

