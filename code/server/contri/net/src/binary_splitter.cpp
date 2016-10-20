

#include "binary_splitter.h"
#include "os.h"
#include "config.h"

#ifndef PACK_LEN_BYTE
#define PACK_LEN_BYTE 4
#endif
#ifndef MAX_PACK_LEN
#define MAX_PACK_LEN (10 * 1024 * 1024)
#endif


// class Binary_Splitter
Binary_Splitter::~Binary_Splitter()
{
}

int Binary_Splitter::split(const char *buf, int len, int& packet_begin, int& packet_len)
{
	if (len < PACK_LEN_BYTE) {
		return 0 ;
	}

	int pack_len = 0;
	
	if(PACK_LEN_BYTE == 4)
	{
		pack_len = ntohl(*(uint32_t*)buf);
	}
	else//len == 2 
	{
		pack_len = ntohs(*(uint16_t*)buf);
	}
	if (pack_len < PACK_LEN_BYTE) {
		return -1;
	}
	if (pack_len > MAX_PACK_LEN) {
		return -1;
	}

	if (len >= pack_len) {
		packet_begin = 0;
		packet_len = pack_len;
		return 1;
	}

	if(len < pack_len)	//缓冲区中的数据不足一个包
	{
		packet_begin = 0;		
		packet_len = pack_len;
	}

	return 0;
}

