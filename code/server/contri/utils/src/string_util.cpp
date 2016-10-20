
#include "string_util.h"
#include <stdlib.h>
#include <stdio.h>


namespace utils {


// class StringUtil
string StringUtil::upper(const string &str)
{
	string result = "";

	for (string::const_iterator c = str.begin(); c != str.end(); ++c)
	{
		result += toupper(*c);
	}

	return result;
}

void StringUtil::upper(char* str)
{
	int i = 0;
	while(str[i] != '\0' )
	{
		str[i] = toupper(str[i]);
		i++;
	}
}

string StringUtil::lower(const string &str)
{
	string result = "";

	for (string::const_iterator c = str.begin(); c != str.end(); ++c)
	{
		result += tolower(*c);
	}

	return result;
}

void StringUtil::lower(char *str)
{
	int i = 0;
	while(str[i] != '\0')
	{
		str[i] = tolower(str[i]);
		i++;
	}
}

string StringUtil::ltrim(const string &str, const string &skip)
{
	string::size_type pos;
	for (pos = 0; pos < str.length(); pos++)
	{
		if (string::npos == skip.find(str[pos]))
			break;
	}
	return str.substr(pos);
}

void StringUtil::ltrim(char *str, const char *skip)
{
	char s[2];
	s[1] = 0;

	size_t i;
	for (i = 0; i < strlen(str); i++)
	{
		s[0] = str[i];
		if (NULL == strstr(skip, s))
		{
			break;
		}
	}

	int j = 0;
	for (size_t p = i; p < strlen(str) + 1; p++)
	{
		str[j++] = str[p];
	}
}

string StringUtil::rtrim(const string &str, const string &skip)
{
	string::size_type pos;
	for (pos = str.length() - 1; pos >= 0; pos--)
	{
		if (string::npos == skip.find(str[pos]))
			break;
	}
	return str.substr(0, pos + 1);
}

void StringUtil::rtrim(char *str, const char *skip)
{
	char s[2];
	s[1] = 0;

	for (int i = (int)strlen(str); i >= 0; i--)
	{
		s[0] = str[i];
		if (NULL == strstr(skip, s))
		{
			break;
		}
		else
		{
			str[i] = 0;
		}
	}
}

string StringUtil::trim(const string &str, const string &skip)
{
	return rtrim(ltrim(str, skip), skip);
}

void StringUtil::trim(char *str, const char *skip)
{
	rtrim(str, skip);
	ltrim(str, skip);
}


int StringUtil::split(const string &str, std::vector<std::string> & dest, const string &dem /*= ";"*/)
{
    std::string::size_type idx, idx1;

    for( idx=0; idx<str.size(); idx=idx1+dem.size() )
    {
        idx1 = str.find(dem, idx);
        if (idx == idx1)
        {
            continue; //如果是分隔符，则略过
        }
        if (idx1 == std::string::npos)
        {
            idx1 = str.size(); //如果没有找到,则取剩余的所有字符
        }
        dest.push_back(str.substr(idx, idx1-idx));
    }
    return 0;

}

int StringUtil::split(const char *cstr, std::vector<std::string> & dest, const string &dem/* = ";"*/)
{
    return split(string(cstr), dest, dem);
}

//! 字符串相等比较
int StringUtil::is_equal(const char *s1, const char *s2)
{
    if (NULL == s1 && NULL == s2) {
        return 0;
    } else if (NULL != s1 && NULL == s2) {
        return 1;
    } else if (NULL == s1 && NULL != s2) {
        return -1;
    }
    return is_equal(string(s1), string(s2));
}

int StringUtil::is_equal(const string &s1, const string &s2)
{
    return s1.compare(s2);
}


//! 忽略大小写的字符串比较
int StringUtil::strcasecmp(const char* str1, const char * str2)
{
	#ifdef WIN32
	return ::stricmp(str1, str2);
	#else
	return ::strcasecmp(str1, str2);
	#endif
}

//! 忽略大小写的字符串比较
int StringUtil::strcasecmp(const string &str1, const string &str2)
{
	return strcasecmp(str1.c_str(), str2.c_str());
}

//! double字符串转换为数值
double StringUtil::strtod(const char * str)
{
	return ::strtod(str, 0);
}

//! double字符串转换为数值
double StringUtil::strtod(const string &str)
{
	return ::strtod(str.c_str(), 0);
}

//! float字符串转换为数值
float StringUtil::strtof(const char * str)
{
	double val = ::strtod(str, 0);
	return (float)val;
}

//! float字符串转换为数值
float StringUtil::strtof(const string &str)
{
	double val = ::strtod(str.c_str(), 0);
	return (float)val;
}

//! bool字符串转换为bool值
bool StringUtil::strtobool(const char * str, bool def)
{
	if ( str == 0 )
		return def;
	if ( strcasecmp(str, "true") == 0 )
		return true;
	if ( strcasecmp(str, "false") == 0 )
		return false;
	if ( strcmp(str, "0") == 0 )
		return false;
	if ( strtoi32(str) != 0 )
		return true;
	return def;
}

//! bool字符串转换为bool值
bool StringUtil::strtobool(const string &str, bool def)
{
    return strtobool(str.c_str(), def);
}

//! 整型字符串转换为32位数值
int32_t StringUtil::strtoi32(const char * str)
{
	return (int32_t)strtol(str, 0, 10);
}

//! 整型字符串转换为32位数值
int32_t StringUtil::strtoi32(const string &str)
{
	return (int32_t)strtol(str.c_str(), 0, 10);
}

//! 无符号整型字符串转换为32位无符号数值
uint32_t StringUtil::strtou32(const char * str)
{
	return strtoul(str, 0, 10);
}

//! 无符号整型字符串转换为32位无符号数值
uint32_t StringUtil::strtou32(const string &str)
{
	return strtoul(str.c_str(), 0, 10);
}

//! 整型字符串转换为64位数值
int64_t StringUtil::strtoi64(const char * str)
{
	return strtoll(str, 0, 10);
}

//! 整型字符串转换为64位数值
int64_t StringUtil::strtoi64(const string &str)
{
	return strtoll(str.c_str(), 0, 10);
}

//! 整型字符串转换为64位无符号数值
uint64_t StringUtil::strtou64(const char * str)
{
	return strtoull(str, 0, 10);
}

//! 整型字符串转换为64位无符号数值
uint64_t StringUtil::strtou64(const string &str)
{
	return strtoull(str.c_str(), 0, 10);
}


//! 32位整型数值转换为字符串
//! fmt 标识转换到哪种格式，二进制，八进制，十进制，十六进制等
const char * StringUtil::i32tostr(int32_t val, std::string & str, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%d", val);
	str = buf;
	return str.c_str();
}

//! 32位整型数值转换为字符串
//! fmt 标识转换到哪种格式，二进制，八进制，十进制，十六进制等
std::string StringUtil::i32tostr(int32_t val, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%d", val);
	return buf;
}

//! 64位整型数值转换为字符串
//! fmt 标识转换到哪种格式，二进制，八进制，十进制，十六进制等
const char * StringUtil::i64tostr(int64_t val, std::string & str, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%ld", val);
	str = buf;
	return str.c_str();
}

//! 64位整型数值转换为字符串
//! fmt 标识转换到哪种格式，二进制，八进制，十进制，十六进制等
std::string StringUtil::i64tostr(int64_t val, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%ld", val);
	return buf;
}


//! 32位无符号整型数值转换为字符串
//! fmt 标识转换到哪种格式，二进制，八进制，十进制，十六进制等
const char * StringUtil::u32tostr(uint32_t val, std::string & str, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%u", val);
	str = buf;
	return str.c_str();
}

//! 32位无符号整型数值转换为字符串
//! fmt 标识转换到哪种格式，二进制，八进制，十进制，十六进制等
std::string StringUtil::u32tostr(uint32_t val, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%u", val);
	return buf;
}


//! 32位整型数值转换为字符串
//! fmt 标识转换到哪种格式，二进制，八进制，十进制，十六进制等
const char * StringUtil::u64tostr(uint64_t val, std::string & str, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%lu", val);
	str = buf;
	return str.c_str();
}

//! 32位整型数值转换为字符串
//! fmt 标识转换到哪种格式，二进制，八进制，十进制，十六进制等
std::string StringUtil::u64tostr(uint64_t val, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%lu", val);
	return buf;
}


//! float数值转换为字符串
//! fmt 自定义格式输出
const char * StringUtil::ftostr(double val, std::string & str, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%6.3f", val);
	str = buf;
	return str.c_str();
}

//! float数值转换为字符串
//! fmt 自定义格式输出
std::string StringUtil::ftostr(double val, const char * fmt)
{
	char buf[1024];
	sprintf(buf, fmt ? fmt : "%6.3f", val);
	return buf;
}

} // namepsace utils

