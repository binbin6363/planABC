#include <stdlib.h>
#include <string.h>

#include "phpAgent_splitter.h"
#include "os.h"
#include "config.h"
#include <stdio.h>
#include <log.h>

// class String_Splitter
PhpAgentSplit::~PhpAgentSplit()
{
}
#if 0
int PhpAgentSplit::split(const char *buf, int len, int& packet_begin, int& packet_len)
{
	if (len < 4) 
	{
		return 0;
	}
	packet_begin = 0;
	packet_len = len;
	return 1;
}
#else


int PhpAgentSplit::split(const char *buf, int len, int& packet_begin, int& packet_len)
{
	if (len < 10) 
	{
		return 0;
	}
	int pack_len = 0;
	unsigned short * plength = (unsigned short* )buf;
	unsigned short length = *plength;
	length = ntohs(length);
	if(len >= length + 10){
		packet_begin = 0;
		packet_len = length + 10;
		return 1;
	}

	packet_begin = 0;
	packet_len = length + 10;
	
	LOG_DEBUG("Packet Not Complete, len=%d pack_len=%d", len, pack_len);
	
	return 0;

}
#endif
