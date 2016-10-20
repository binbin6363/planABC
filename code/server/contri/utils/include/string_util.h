//! @file string_util.h
//! @brief �ַ���������


#ifndef _STRING_UTIL_H_
#define _STRING_UTIL_H_

#include <string.h>
#include <string>
#include <vector>
#include <stdint.h>

using namespace std;


namespace utils {


//! �ַ���������
class StringUtil
{
public:
	//! ת��Ϊ��д
	static string upper(const string &str);

	//! ת��Ϊ��д
	static void upper(char* str);

	//! ת��ΪСд
	static string lower(const string &str);

	//! ת��ΪСд
	static void lower(char *str);

	//! ȥ������skip�е��ַ�
	static string ltrim(const string &str, const string &skip = " \t");

	//! ȥ������skip�е��ַ�
	static void ltrim(char *str, const char *skip = " \t");

	//! ȥ���Ҳ��skip�е��ַ�
	static string rtrim(const string &str, const string &skip = " \t");

	//! ȥ���Ҳ��skip�е��ַ�
	static void rtrim(char *str, const char *skip = " \t");

	//! ȥ�������skip�е��ַ�
	static string trim(const string &str, const string &skip = " \t");

	//! ȥ�������skip�е��ַ�
	static void trim(char *str, const char *skip = " \t");

	//! �ַ����ָ�
	static int split(const string &str, std::vector<std::string> & dest, const string &dem = ";");
	static int split(const char *cstr, std::vector<std::string> & dest, const string &dem = ";");

	//! �ַ�����ȱȽ�
	static int is_equal(const char *s1, const char *s2);
	static int is_equal(const string &s1, const string &s2);

	//! ���Դ�Сд���ַ����Ƚ�
    static int strcasecmp(const char* str1, const char * str2);

    //! ���Դ�Сд���ַ����Ƚ�
    static int strcasecmp(const string &str1, const string &str2);

    //! double�ַ���ת��Ϊ��ֵ
	static double strtod(const char * str);

    //! double�ַ���ת��Ϊ��ֵ
	static double strtod(const string &str);

    //! float�ַ���ת��Ϊ��ֵ
	static float strtof(const char * str);

    //! float�ַ���ת��Ϊ��ֵ
	static float strtof(const string &str);

    //! bool�ַ���ת��Ϊboolֵ
	static bool strtobool(const char * str, bool def = false);

    //! bool�ַ���ת��Ϊboolֵ
	static bool strtobool(const string &str, bool def = false);

    //! �����ַ���ת��Ϊ��ֵ
    static int32_t strtoi32(const char * str);

    //! �����ַ���ת��Ϊ��ֵ
    static int32_t strtoi32(const string &str);

    //! �޷��������ַ���ת��Ϊ32λ�޷�����ֵ
	static uint32_t strtou32(const char * str);

    //! �޷��������ַ���ת��Ϊ32λ�޷�����ֵ
	static uint32_t strtou32(const string &str);

    //! �����ַ���ת��Ϊ64λ��ֵ
    static int64_t strtoi64(const char * str);

    //! �����ַ���ת��Ϊ64λ��ֵ
    static int64_t strtoi64(const string &str);

    //! �����ַ���ת��Ϊ64λ�޷�����ֵ
	static uint64_t strtou64(const char * str);

    //! �����ַ���ת��Ϊ64λ�޷�����ֵ
	static uint64_t strtou64(const string &str);

    //! 32λ������ֵת��Ϊ�ַ���
	static const char * i32tostr(int32_t val, std::string & str, const char * fmt = 0);

    //! 32λ������ֵת��Ϊ�ַ���
	static std::string i32tostr(int32_t val, const char * fmt = 0);

    //! 64λ������ֵת��Ϊ�ַ���
	static const char * i64tostr(int64_t val, std::string & str, const char * fmt = 0);

    //! 64λ������ֵת��Ϊ�ַ���
	static std::string i64tostr(int64_t val, const char * fmt = 0);

    //! 32λ�޷���������ֵת��Ϊ�ַ���
	static const char * u32tostr(uint32_t val, std::string & str, const char * fmt = 0);

    //! 32λ�޷���������ֵת��Ϊ�ַ���
	static std::string u32tostr(uint32_t val, const char * fmt = 0);

    //! 64λ�޷���������ֵת��Ϊ�ַ���
	static const char * u64tostr(uint64_t val, std::string & str, const char * fmt = 0);

    //! 64λ�޷���������ֵת��Ϊ�ַ���
	static std::string u64tostr(uint64_t val, const char * fmt = 0);

    //! float��ֵת��Ϊ�ַ���
	static const char * ftostr(double val, std::string & str, const char * fmt = 0);

    //! float��ֵת��Ϊ�ַ���
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
