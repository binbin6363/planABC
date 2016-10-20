//! @file string_util.h
//! @brief 字符串工具类


#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <string.h>
#include <string>
#include <vector>
#include <stdint.h>

using namespace std;


namespace utils {


//! 字符串工具类
class StringUtil
{
public:
	//! 转换为大写
	static string upper(const string &str);

	//! 转换为大写
	static void upper(char* str);

	//! 转换为小写
	static string lower(const string &str);

	//! 转换为小写
	static void lower(char *str);

	//! 去掉左侧的skip中的字符
	static string ltrim(const string &str, const string &skip = " \t");

	//! 去掉左侧的skip中的字符
	static void ltrim(char *str, const char *skip = " \t");

	//! 去掉右侧的skip中的字符
	static string rtrim(const string &str, const string &skip = " \t");

	//! 去掉右侧的skip中的字符
	static void rtrim(char *str, const char *skip = " \t");

	//! 去掉两侧的skip中的字符
	static string trim(const string &str, const string &skip = " \t");

	//! 去掉两侧的skip中的字符
	static void trim(char *str, const char *skip = " \t");

	//! 字符串分割
	static int split(const string &str, std::vector<std::string> & dest, const string &dem = ";");
	static int split(const char *cstr, std::vector<std::string> & dest, const string &dem = ";");

	//! 字符串相等比较
	static int is_equal(const char *s1, const char *s2);
	static int is_equal(const string &s1, const string &s2);

	//! 忽略大小写的字符串比较
    static int strcasecmp(const char* str1, const char * str2);

    //! 忽略大小写的字符串比较
    static int strcasecmp(const string &str1, const string &str2);

    //! double字符串转换为数值
	static double strtod(const char * str);

    //! double字符串转换为数值
	static double strtod(const string &str);

    //! float字符串转换为数值
	static float strtof(const char * str);

    //! float字符串转换为数值
	static float strtof(const string &str);

    //! bool字符串转换为bool值
	static bool strtobool(const char * str, bool def = false);

    //! bool字符串转换为bool值
	static bool strtobool(const string &str, bool def = false);

    //! 整型字符串转换为数值
    static int32_t strtoi32(const char * str);

    //! 整型字符串转换为数值
    static int32_t strtoi32(const string &str);

    //! 无符号整型字符串转换为32位无符号数值
	static uint32_t strtou32(const char * str);

    //! 无符号整型字符串转换为32位无符号数值
	static uint32_t strtou32(const string &str);

    //! 整型字符串转换为64位数值
    static int64_t strtoi64(const char * str);

    //! 整型字符串转换为64位数值
    static int64_t strtoi64(const string &str);

    //! 整型字符串转换为64位无符号数值
	static uint64_t strtou64(const char * str);

    //! 整型字符串转换为64位无符号数值
	static uint64_t strtou64(const string &str);

    //! 32位整型数值转换为字符串
	static const char * i32tostr(int32_t val, std::string & str, const char * fmt = 0);

    //! 32位整型数值转换为字符串
	static std::string i32tostr(int32_t val, const char * fmt = 0);

    //! 64位整型数值转换为字符串
	static const char * i64tostr(int64_t val, std::string & str, const char * fmt = 0);

    //! 64位整型数值转换为字符串
	static std::string i64tostr(int64_t val, const char * fmt = 0);

    //! 32位无符号整型数值转换为字符串
	static const char * u32tostr(uint32_t val, std::string & str, const char * fmt = 0);

    //! 32位无符号整型数值转换为字符串
	static std::string u32tostr(uint32_t val, const char * fmt = 0);

    //! 64位无符号整型数值转换为字符串
	static const char * u64tostr(uint64_t val, std::string & str, const char * fmt = 0);

    //! 64位无符号整型数值转换为字符串
	static std::string u64tostr(uint64_t val, const char * fmt = 0);

    //! float数值转换为字符串
	static const char * ftostr(double val, std::string & str, const char * fmt = 0);

    //! float数值转换为字符串
	static std::string ftostr(double val, const char * fmt = 0);


	static inline
	uint16_t big_endian(uint16_t v){
		return (v>>8) | (v<<8);
	}

	static inline
	uint32_t big_endian(uint32_t v){
		return (v >> 24) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000) | (v << 24);
	}

	static inline
	uint64_t big_endian(uint64_t v){
		uint32_t h = v >> 32;
		uint32_t l = v & 0xffffffffull;
		return big_endian(h) | ((uint64_t)big_endian(l) << 32);
	}



};


} // namepsace utils


#endif // _STRING_UTIL_H_
