#include "utils.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#define ADDR_PREFIX_INET "inet://"
#define ADDR_PREFIX_LOCAL "local://"


string FromAddrTostring(const INET_Addr& inetAddr)
{
	char buffer[256] = {0};
	uint16_t port = inetAddr.get_port();
	in_addr addrTmp;
	addrTmp.s_addr  = inetAddr.get_addr();
	const char *addr = inet_ntoa(addrTmp);
	snprintf(buffer,sizeof(buffer),"%s:%d",addr,port);
	return buffer;
}

string FromAddrToIpAddr(const INET_Addr& inetAddr)
{
	char buffer[256] = {0};
	in_addr addrTmp;
	addrTmp.s_addr  = inetAddr.get_addr();
	const char *addr = inet_ntoa(addrTmp);
	snprintf(buffer,sizeof(buffer),"%s",addr);
	return buffer;
}


static int StringToInetAddr(const char* addr,INET_Addr &inetaddr)
{
	const char* host = addr;
	unsigned short port = 0;

	const char* pos = NULL;
	if ((pos = (const char*)strstr(addr, ":")) != NULL) {
		port = atoi(pos + 1);
		string strHost(host,pos);
		in_addr_t inAddr = inet_addr(strHost.c_str());
		if (inAddr == INADDR_NONE)
		{
			return -1;
		}
		inetaddr.set_addr(inAddr);
		inetaddr.set_port(htons(port));
	}
	return 0;
}


int FromStringToAddr(const char* addr,INET_Addr &inetaddr)
{
	if (strncmp(addr, ADDR_PREFIX_INET, strlen(ADDR_PREFIX_INET)) == 0) {
		addr += strlen(ADDR_PREFIX_INET);
		return StringToInetAddr(addr,inetaddr);
	}
	return StringToInetAddr(addr,inetaddr);
}

const char * GetTimeColor( double _t ) {
	return ( _t > 1.0 ) ? _TIME_RED_ :
		( ( _t > 0.5 ) ? _TIME_LRED_ :
		( (_t > 0.1) ? _TIME_PURPLE : 
		( ( _t > 0.05 ) ? _TIME_YELLOW_ : 
		( (_t > 0.01) ? _TIME_BLUE_ : _TIME_WHITE_)
		)
		)
		);
}

timespec operator-(const timespec & lhs, const timespec & rhs)
{
	timespec result;

	result.tv_sec = lhs.tv_sec - rhs.tv_sec;
	result.tv_nsec = lhs.tv_nsec - rhs.tv_nsec;

	if ( result.tv_nsec < 0 ) {
		result.tv_nsec += 1000000000;
		--result.tv_sec;	
	}

	return result;
}
