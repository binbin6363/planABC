#ifndef _UTILS_H
#define _UTILS_H
#include "inet_addr.h"
#include <string>
using namespace std;
string FromAddrTostring(const INET_Addr& addr);
string FromAddrToIpAddr(const INET_Addr& addr);
int FromStringToAddr(const char* addr,INET_Addr &inetaddr);

const char * GetTimeColor( double _t );
timespec operator-(const timespec & lhs, const timespec & rhs);


#define _TIME_RED_ 		"\033[0;32;31m"
#define _TIME_LRED_ 	"\033[1;31m"
#define _TIME_YELLOW_ 	"\033[1;33m"
#define _TIME_WHITE_ 	"\033[1;37m"
#define _TIME_BLUE_ 	"\033[1;34m"
#define _TIME_PURPLE 	"\033[1;35m"

#define RED(x)  	"\033[1;31m" #x "\033[0m"  //error
#define YELLOW(x) 	"\033[1;33m" #x "\033[0m"	//warn
#define GREEN(x) 	"\033[1;32m" #x "\033[0m"	//info
#endif
