#include "binpacket_wrap.h"

#ifndef LOG
#define LOG(LEVEL)
#endif

namespace utils
{

template<>
template<>
void BinInputPacket<true>::get_head<HEADER>(HEADER& value)
{
    if (sizeof(HEADER) != m_head_len)
    {
        m_good = false;
        LOG(ERROR)("get_head failed.");
        return;
    }

    value = *reinterpret_cast<HEADER*>(m_begin);
    value.len = ntohl(value.len);
    value.cmd = ntohl(value.cmd);
    value.seq = ntohl(value.seq);
    value.head_len = ntohl(value.head_len);
    value.uid = ntohl(value.uid);
}

template<>
template<>
void BinOutputPacket<true>::set_head<HEADER>(HEADER value)
{
    if (sizeof(HEADER) != m_head_len)
    {
        m_good = false;
        LOG(ERROR)("set_head failed.");
        return;
    }

    value.len = htonl(value.len);
    value.cmd = htonl(value.cmd);
    value.seq = htonl(value.seq);
    value.head_len = htonl(value.head_len);
    value.uid = htonl(value.uid);
    *reinterpret_cast<HEADER*>(m_begin) = value;
}

template<>
template<>
void BinInputPacket<true>::get_head<COHEADER>(COHEADER& value)
{
    if (sizeof(COHEADER) != m_head_len)
    {
        m_good = false;
        LOG(ERROR)("get_head failed.");
        return;
    }

    value = *reinterpret_cast<COHEADER*>(m_begin);
    value.len = ntohl(value.len);
    value.cmd = ntohl(value.cmd);
    value.seq = ntohl(value.seq);
    value.head_len = ntohl(value.head_len);
    value.uid = ntohl(value.uid);
    value.sender_coid = ntohl(value.sender_coid);
    value.receiver_coid = ntohl(value.receiver_coid);
}

template<>
template<>
void BinOutputPacket<true>::set_head<COHEADER>(COHEADER value)
{
    if (sizeof(COHEADER) != m_head_len)
    {
        m_good = false;
        LOG(ERROR)("set_head failed.");
        return;
    }

    value.len = htonl(value.len);
    value.cmd = htonl(value.cmd);
    value.seq = htonl(value.seq);
    value.head_len = htonl(value.head_len);
    value.uid = htonl(value.uid);
    value.sender_coid = htonl(value.sender_coid);
    value.receiver_coid = htonl(value.receiver_coid);
    *reinterpret_cast<COHEADER*>(m_begin) = value;
}

#if 0

template<>
template<>
void BinInputPacket<true>::get_head<BHDR>(BHDR& value)
{
    if (sizeof(BHDR) != m_head_len)
    {
        m_good = false;
        LOG(ERROR)("get_head failed.");
        return;
    }

    value = *reinterpret_cast<BHDR*>(m_begin);
    value.len = ntohl(value.len);
    value.cmd = ntohl(value.cmd);
    value.seq = ntohl(value.seq);
    value.cid = ntohl(value.cid);
    value.uid = ntohl(value.uid);
}

template<>
template<>
void BinOutputPacket<true>::set_head<BHDR>(BHDR value)
{
    if (sizeof(BHDR) != m_head_len)
    {
        m_good = false;
        LOG(ERROR)("set_head failed.");
        return;
    }

    value.len = htonl(value.len);
    value.cmd = htonl(value.cmd);
    value.seq = htonl(value.seq);
    value.cid = htonl(value.cid);
    value.uid = htonl(value.uid);
    *reinterpret_cast<BHDR*>(m_begin) = value;
}


template<>
template<>
void BinInputPacket<true>::get_head<COBHDR>(COBHDR& value)
{
    if (sizeof(COBHDR) != m_head_len)
    {
        m_good = false;
        LOG(ERROR)("get_head failed.");
        return;
    }

    value = *reinterpret_cast<COBHDR*>(m_begin);
    value.len = ntohl(value.len);
    value.cmd = ntohl(value.cmd);
    value.seq = ntohl(value.seq);
    value.head_len = ntohl(value.head_len);
    value.sender_coid = ntohl(value.sender_coid);
    value.receiver_coid = ntohl(value.receiver_coid);
    value.server_id = ntohl(value.server_id);
}


template<>
template<>
void BinOutputPacket<true>::set_head<COBHDR>(COBHDR value)
{
    if (sizeof(COBHDR) != m_head_len)
    {
        m_good = false;
        LOG(ERROR)("set_head failed.");
        return;
    }

    value.len = htonl(value.len);
    value.cmd = htonl(value.cmd);
    value.seq = htonl(value.seq);
    value.head_len = htonl(value.head_len);
    value.sender_coid = htonl(value.sender_coid);
    value.receiver_coid = htonl(value.receiver_coid);
    value.server_id = htonl(value.server_id);
    *reinterpret_cast<COBHDR*>(m_begin) = value;
}
#endif

} // namespace utils

