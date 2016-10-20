/********************************************************************
	created:	2013/10/29
	created:	29:10:2013   10:14
	filename: 	imo_net\comm.h
	file base:	comm
	file ext:	h
	author:		pengshuai
	
	purpose:	
*********************************************************************/
#ifndef __comm_h__
#define __comm_h__
#ifdef WIN32
	#include "Windows.h"
	#include "winsock2.h"
#else
#include <sys/socket.h>
       #include <netinet/in.h>
       #include <arpa/inet.h>
#endif

#include "inet_addr.h"
#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
using namespace std;

#define bzero(a, b) memset(a, 0, b)

#define ADDR_PREFIX_INET "inet://"
#define ADDR_PREFIX_LOCAL "local://"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p) { if(p) { delete p; p=NULL; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p); (p)=NULL; } }
#endif

#define DEF_CONNECTE_TIME_OUT  5

#define TIME_SPEC timeval
#define ACE_INT64_LITERAL(n) n ## ll
#ifdef WIN32
#define  FILETIME_to_timval_skew ((DWORDLONG)ACE_INT64_LITERAL(0x19db1ded53e8000))
#else
#define sprintf_s snprintf
#endif


inline TIME_SPEC gettimeofday (void)
{
	timeval tv;

#if defined (WIN32)
	FILETIME   tfile;
	::GetSystemTimeAsFileTime (&tfile);

	ULARGE_INTEGER _100ns;
	_100ns.LowPart = tfile.dwLowDateTime;
	_100ns.HighPart = tfile.dwHighDateTime;

	_100ns.QuadPart -= FILETIME_to_timval_skew;

	// Convert 100ns units to seconds;
	tv.tv_sec = (time_t) (_100ns.QuadPart / (10000 * 1000));
	// Convert remainder to microseconds;
	tv.tv_usec = (long) ((_100ns.QuadPart % (10000 * 1000)) / 10);
#else
	timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	tv.tv_sec = ts.tv_sec;
	tv.tv_usec = ts.tv_nsec / 1000L;  // timespec has nsec, but timeval has usec
#endif // !defined (ACE_WIN32)

	return tv;
};

inline string FromAddrTostring(const INET_Addr& inetAddr)
{
	char buffer[256] = {0};
	uint16_t port = inetAddr.get_port();
	in_addr addrTmp;
	addrTmp.s_addr  = inetAddr.get_addr();
	const char *addr = inet_ntoa(addrTmp);
	sprintf_s(buffer,sizeof(buffer),"%s:%d",addr, ntohs(port));
	return buffer;
}

inline string FromAddrToIpAddr(const INET_Addr& inetAddr)
{
	char buffer[256] = {0};
	in_addr addrTmp;
	addrTmp.s_addr  = inetAddr.get_addr();
	const char *addr = inet_ntoa(addrTmp);
	sprintf_s(buffer,sizeof(buffer),"%s",addr);
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

inline int FromStringToAddr(const char* addr,INET_Addr &inetaddr)
{
	if (strncmp(addr, ADDR_PREFIX_INET, strlen(ADDR_PREFIX_INET)) == 0) {
		addr += strlen(ADDR_PREFIX_INET);
		return StringToInetAddr(addr,inetaddr);
	}

	return StringToInetAddr(addr,inetaddr);
}

inline int FromIpToAddr(const char* addr, uint16_t port, INET_Addr &inetaddr)
{
	in_addr_t inAddr = inet_addr(addr);
	if (inAddr == INADDR_NONE)
	{
		return -1;
	}

	inetaddr.set_addr(inAddr);
	inetaddr.set_port(htons(port));

	return 0;
};

#ifdef WIN32

inline string get_module_name()
{
	char this_exe[MAX_PATH + 2];
	if (GetModuleFileName (0, this_exe, MAX_PATH) == 0)
		return "";

	char *module_name = strrchr(this_exe, '\\');
	if (!module_name)
		return "";

	char *name = strchr(module_name, '.');

	if (!name)
		return "";

	return string(module_name+1, name - module_name-1);
}
#endif

#endif // __comm_h__
