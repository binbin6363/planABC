#include "libco_data_type.h"

#ifndef LOG
#define LOG(LEVEL)
#endif

#if 0 
namespace libco_src
{

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
    value.pkglen = ntohl(value.pkglen);
    value.len = ntohl(value.len);
    value.cmd = ntohl(value.cmd);
    value.seq = ntohl(value.seq);
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

    value.pkglen = htonl(value.pkglen);
    value.len = htonl(value.len);
    value.cmd = htonl(value.cmd);
    value.seq = htonl(value.seq);
    value.uid = htonl(value.uid);
    value.sender_coid = htonl(value.sender_coid);
    value.receiver_coid = htonl(value.receiver_coid);
    *reinterpret_cast<COHEADER*>(m_begin) = value;
}

}
#endif

