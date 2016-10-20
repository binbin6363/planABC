//! @file linux_util.h
//! @brief linux���е�һЩ����


#ifndef _LINUX_UTIL_H_
#define _LINUX_UTIL_H_
#include <time.h>
#include <string>

using namespace std;

namespace utils {


//! linux���е�һЩ����������
class LinuxUtil
{
public:

    static void daemon();

    static int lock_wait(const char * fname);

    static void partner(const char * lockname, char* argv[]);  // argv ��������������

    static void groupd_partner(const char * lockname, char*argv[], const char* cShellPath);

  	static string getTime();//YYYYMMDDHHmmSS  	
  	
  	static string getTime1(); //YYYY-mm-DD HH:MM:SS  	
  	
  	static string getTime(time_t now);//YYYYMMDDHHmmSS  	
  	
  	static string getTime1(time_t now); //YYYY-mm-DD HH:MM:SS  	

};

} // namespace utils

#endif // _LINUX_UTIL_H_
