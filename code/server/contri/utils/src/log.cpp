// log.cpp

#include "log.h"
#include <stdint.h>

#ifdef _MULTI_THREAD
#include "thread_mutex.h"
namespace utils
{
    class Log::Lock : public Thread_Mutex {};
}
#else
namespace utils
{
    /// 一个空互斥锁
    class Log::Lock
    {
    public:
        void acquire() {};
        void release() {};
    };
}
#endif

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <sys/stat.h>

#ifdef WIN32

#if _MSC_VER <= 1400
#define vsnprintf _vsnprintf
#define snprintf _snprintf
#endif

#else

#include <unistd.h>

#endif

namespace utils
{

#if WIN32

#define DATE_START  0
    char Log::level_str_[][64] = {
        "2008-11-07 09:35:00 FATAL ", 
        "2008-11-07 09:35:00 ERROR ", 
        "2008-11-07 09:35:00 WARN  ", 
        "2008-11-07 09:35:00 INFO  ", 
//	        "2008-11-07 09:35:00 TRACE ", 
        "2008-11-07 09:35:00 DEBUG ",
		"2008-11-07 09:35:00 TRACE ",
    };

	char Log::level_str_usec_[][64] = {
	"\033[1;31m2008-11-07 09:35:00.000000 FATAL ", 
	"\033[1;33m2008-11-07 09:35:00.000000 ERROR ", 
	"\033[1;35m2008-11-07 09:35:00.000000 WARN	", 
	"\033[1;32m2008-11-07 09:35:00.000000 INFO	", 
//		"\033[0;00m2008-11-07 09:35:00.000000 TRACE ", 
	"\033[0;00m2008-11-07 09:35:00.000000 DEBUG ",
	"\033[0;00m2008-11-07 09:35:00.000000 TRACE ",

#else

#define DATE_START  7
    char Log::level_str_[][64] = {
        "\033[1;31m2008-11-07 09:35:00 FATAL ", 
        "\033[1;33m2008-11-07 09:35:00 ERROR ", 
        "\033[1;35m2008-11-07 09:35:00 WARN  ", 
        "\033[1;32m2008-11-07 09:35:00 INFO  ", 
//	        "\033[0;00m2008-11-07 09:35:00 TRACE ", 
        "\033[0;00m2008-11-07 09:35:00 DEBUG ",
		"\033[0;00m2008-11-07 09:35:00 TRACE ",
    };

	char Log::level_str_usec_[][64] = {
	"\033[1;31m2008-11-07 09:35:00.000000 FATAL ", 
	"\033[1;33m2008-11-07 09:35:00.000000 ERROR ", 
	"\033[1;35m2008-11-07 09:35:00.000000 WARN  ", 
	"\033[1;32m2008-11-07 09:35:00.000000 INFO  ", 
//		"\033[0;00m2008-11-07 09:35:00.000000 TRACE ", 
	"\033[0;00m2008-11-07 09:35:00.000000 DEBUG ",
	"\033[0;00m2008-11-07 09:35:00.000000 TRACE ",
};

#endif

#define TIME_START  (DATE_START + 11)

    Log::Log()
    {
        memset(file_name_, 0, sizeof(file_name_));
        max_size_ = 500 * 1024 * 1024;  // 500M
        lock_ = new Lock;
        file_ = NULL;
        max_level_ = L_INFO;
		enable_usec = false;
		enable_pack_print = true;
    }

    Log::~Log()
    {
        close();
        delete lock_;
        lock_ = NULL;
    }

    int Log::set_file_name(const char *filename)
    {
        // 已经打开，不能设置了
        if (file_ != NULL) {
            return -1;
        }

        if (NULL == filename){
            return -1;
        }

        size_t name_len = sizeof(file_name_)/sizeof(char);
        memset(file_name_, 0, name_len);
        strncpy(file_name_, filename, name_len - 1);
        return 0;
    }

    int Log::set_max_size(size_t maxsize)
    {
        max_size_ = maxsize;
        // 不立即生效
        return 0;
    }

    int Log::set_max_level(LogLevel level)
    {
        if (level < L_LEVEL_MAX) {
            max_level_ = level;
            return 0;
        }
        return 1;
    }
	
    int Log::set_pack_print(bool in_enable_pack_print)
    {
        enable_pack_print = in_enable_pack_print;
        return 0;
    }

	int Log::set_usec(bool in_enable_usec)
    {
//	    	printf("in_enable_usec:%d set.\n", in_enable_usec);
    	enable_usec = in_enable_usec;
        return 1;
    }

    int Log::open()
    {
        if (file_ != NULL) {
            return -1;
        }


        char name[MAX_PATH];
        size_t len = 0;

        strncpy(name, file_name_, MAX_PATH);
        len = strlen(name);

        time_t t;
        time(&t);
        struct tm lt = *localtime(&t);
        strftime(name + len, MAX_PATH - len, "-%Y%m%d-%H%M%S.log", &lt);

        file_ = fopen(name, "a+");
        if (NULL == file_)
            return -1;

        // 填写日志记录中的日期，在一天之内就不用填写了
        strftime(name, 12, "%Y-%m-%d", &lt);
        for (int i = 0; i < L_LEVEL_MAX; i++) {
            memcpy(level_str_[i] + DATE_START, name, 10);
        }
		
        for (int i = 0; i < L_LEVEL_MAX; i++) {
            memcpy(level_str_usec_[i] + DATE_START, name, 10);
        }

        lt.tm_hour = lt.tm_min = lt.tm_sec = 0;
        mid_night_ = mktime(&lt);
        
        return 0;
    }

    int Log::close()
    {
        if (NULL == file_) {
            return -1;
        }

        fclose(file_);
        file_ = NULL;

        return 0;
    }

    int Log::log(LogLevel level, const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(level, fmt, ap); // not safe
        va_end(ap);
        return ret;
    }

    int Log::log_fatal(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_FATAL, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_error(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_ERROR, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_warn(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_WARN, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_info(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_INFO, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_trace(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_TRACE, fmt, ap);
        va_end(ap);
        return ret;
    }

    int Log::log_debug(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int ret = vlog(L_DEBUG, fmt, ap);
        va_end(ap);
        return ret;
    }

	int Log::vlog(int level, const char * fmt, va_list ap)
	{
		if (level > max_level_ || NULL == file_)
			return -1;

		lock_->acquire();

		struct tm tm_now;
		struct timeval tv;
		struct timezone tz;
		gettimeofday(&tv, &tz);
		time_t now = tv.tv_sec;

		int t_diff = (int)(now - mid_night_);
		if (t_diff > 24 * 60 * 60) {
			close();
			open();
			t_diff -= 24 * 60 * 60;
		}
		
		localtime_r(&now, &tm_now);
		if(enable_usec)
		{
//				printf("%s enableusec:%d, -----\n", 
//						level_str_[level], enable_usec, tv.tv_usec, tv.tv_usec);
			sprintf(((char*)level_str_usec_[level]+TIME_START), "%02d:%02d:%02d.%06ld",					
				tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec, tv.tv_usec);
			level_str_usec_[level][strlen(level_str_usec_[level])] = ' ';
//				printf("%s enableusec:%d, %06ld, %ld\n", 
//						level_str_usec_[level], enable_usec, tv.tv_usec, tv.tv_usec);
			fputs(level_str_usec_[level], file_);
		}
		else
		{
			sprintf(((char*)level_str_[level]+TIME_START), "%02d:%02d:%02d",					
				tm_now.tm_hour, tm_now.tm_min, tm_now.tm_sec);
			level_str_[level][strlen(level_str_[level])] = ' ';
//	 			printf("%s enableusec:%d, %06ld, %ld\n", 
//	 				level_str_[level], enable_usec, tv.tv_usec, tv.tv_usec);
			fputs(level_str_[level], file_);
		}
		// strftime消耗大，改为直接格式化

		//time_t t;
		//struct tm lt;
		//char time_str[32];
		//time(&t);
		//lt = *localtime(&t);
		//strftime(time_str, 10, "%H:%M:%S", &lt);
		//memcpy(level_str_[level] + TIME_START, time_str, 8);

//			register int hour = t_diff / 3600;
//			register int minute = t_diff % 3600;
//			register int second = minute % 60;
//			minute /= 60;
//	
//			level_str_[level][TIME_START] = hour / 10 + '0';
//			level_str_[level][TIME_START + 1] = hour % 10 + '0';
//			level_str_[level][TIME_START + 3] = minute / 10 + '0';
//			level_str_[level][TIME_START + 4] = minute % 10 + '0';
//			level_str_[level][TIME_START + 6] = second / 10 + '0';
//			level_str_[level][TIME_START + 7] = second % 10 + '0';
		//// set color
		//fputs(color_str_[level], file_);
		//// write time
		//fputs(time_str, file_);
		//// write level
		// write msg
		vfprintf(file_, fmt, ap);
		// reset color
		//fputs(color_str_[L_LEVEL_MAX], file_);
		if (fmt[strlen(fmt) - 1] != '\n')
			fputc('\n', file_);

		/*
		char buf[2048];
		snprintf(buf, sizeof(buf), "%s%s%s%s\n",color_str_[level] , time_str, level_str_[level], fmt);
		vfprintf(file_, buf, ap);
		*/

        // bug, if ftell error, return -1, then convert -1 to 4294967295(32bit machine)
        // , that bigger than max_size_! so ... ~_~
        int file_len = ftell(file_);
		if (-1 != file_len && (size_t)file_len > max_size_) {
			close();
			open();
		}

		lock_->release();

		return 0;
	}


//	    int Log::vlog(int level, const char * fmt, va_list ap)
//	    {
//	        if (level > max_level_ || NULL == file_)
//	            return -1;
//	
//	        lock_->acquire();
//	
//	        int t_diff = (int)(time(NULL) - mid_night_);
//	        if (t_diff > 24 * 60 * 60) {
//	            close();
//	            open();
//	            t_diff -= 24 * 60 * 60;
//	        }
//	
//	        // strftime消耗大，改为直接格式化
//	
//	        //time_t t;
//	        //struct tm lt;
//	        //char time_str[32];
//	        //time(&t);
//	        //lt = *localtime(&t);
//	        //strftime(time_str, 10, "%H:%M:%S", &lt);
//	        //memcpy(level_str_[level] + TIME_START, time_str, 8);
//	
//	        register int hour = t_diff / 3600;
//	        register int minute = t_diff % 3600;
//	        register int second = minute % 60;
//	        minute /= 60;
//	
//	        level_str_[level][TIME_START] = hour / 10 + '0';
//	        level_str_[level][TIME_START + 1] = hour % 10 + '0';
//	        level_str_[level][TIME_START + 3] = minute / 10 + '0';
//	        level_str_[level][TIME_START + 4] = minute % 10 + '0';
//	        level_str_[level][TIME_START + 6] = second / 10 + '0';
//	        level_str_[level][TIME_START + 7] = second % 10 + '0';
//	
//	        //// set color
//	        //fputs(color_str_[level], file_);
//	        //// write time
//	        //fputs(time_str, file_);
//	        //// write level
//	        fputs(level_str_[level], file_);
//	        // write msg
//	        vfprintf(file_, fmt, ap);
//	        // reset color
//	        //fputs(color_str_[L_LEVEL_MAX], file_);
//	        if (fmt[strlen(fmt) - 1] != '\n')
//	            fputc('\n', file_);
//	
//	        /*
//	        char buf[2048];
//	        snprintf(buf, sizeof(buf), "%s%s%s%s\n",color_str_[level] , time_str, level_str_[level], fmt);
//	        vfprintf(file_, buf, ap);
//	        */
//	
//	        if ((size_t)ftell(file_) > max_size_) {
//	            close();
//	            open();
//	        }
//	
//	        lock_->release();
//	
//	        return 0;
//	    }

    static const char chex[] = "0123456789ABCDEF";

	int Log::log_hex_prefix(
		unsigned char * prefix,
        unsigned char * data, 
        size_t len, 
        LogLevel level)
    {
		log(level, "%s", prefix);
		return log_hex(data, len, level);
	}
	
    int Log::log_hex(
        unsigned char * data, 
        size_t len, 
        LogLevel level)
    {
        size_t i, j, k, l;

        if(level > max_level_ ||NULL == data|| NULL == file_)
        {
            return -1;
        }
        
        //DON'T disable hex_print when level is  l_info, l_warn....
        if (!enable_pack_print && level > L_INFO)
            return -1;
/*
[00000] 00 00 01 28  00 00 1F 82  00 00 00 57  00 00 00 2B  |...(.......W...+|
address|                       binary                                         |         char
*/
        char msg_str[128] = {0};           // every line have 128 chars

        msg_str[0] = '[';                       // address column, begin, + 4
        msg_str[5] = '0';                      // address column, default
        msg_str[6] = ']';                      // address column, end
        msg_str[59] = ' ';                     // binary column, last ' '
        msg_str[60] = '|';                    // char column, begin
        msg_str[77] = '|';                    // char column, end
        msg_str[78] = 0;                      // every single line's end, '\0'
        k = 6;
        for (j = 0; j < 16; j++)             // binary column
        {
            if ((j & 0x03) == 0)              // 4 element a group, every group has 4 0x00
            {
                msg_str[++k] = ' ';         // every group's space have 2 SPACE ' '
            }
            k += 3;
            msg_str[k] = ' ';                 // on binary column, every 0x00's space has 1 SPACE
        }
        for (i = 0; i < len / 16; i++)                    // calc how many 4*0x00 rows, and paint everyone on loop, absolutly 16's multiple
        {
        	msg_str[1] = chex[i >> 12];		 // 0x00 * 16 * 16 * 16 * 16
            msg_str[2] = chex[(i >> 8)&0x0F];      // 0x00 * 16 * 16 * 16
            msg_str[3] = chex[(i >>4)&0x0F];       // 0x00 * 16 * 16
            msg_str[4] = chex[i &0x0F];               // mod(%) 0x00 * 16
            k = 7;
            l = i * 16;                                         // every row has 0x00 * 16 about binary desc.
            memcpy(msg_str + 61, data + l, 16); 
            for (j = 0; j < 16; j++)                      // trans binary to char(0-9, A-F) at binary column, char column remain the same
            {
                if ((j & 0x03) == 0)                       // every 0x00 * 4 hit, paint stored SPACE ' '
                {
                    k++;
                }
                msg_str[k++] = chex[data[l] >> 4];         // char as 2^8, and split it to 2 group of 2^4, high group
                msg_str[k++] = chex[data[l++] & 0x0F];  // low group, like this, above can express as (data[l] & 0xF0, then >> 4, so bored)
                k++;
                if (!isgraph(msg_str[61 + j]))                   // char column, char * 16
                    msg_str[61 + j]= '.';
            }
			msg_str[127] = 0;                               // '\0'
			lock_->acquire();
            fprintf(file_, "# %s\n", msg_str);
			lock_->release();
        }
        
    	msg_str[1] = chex[i >> 12];                              // remain byte, can not divide by 16, %16
        msg_str[2] = chex[(i >> 8)&0x0F];
        msg_str[3] = chex[(i >>4)&0x0F];
        msg_str[4] = chex[i &0x0F];
        
        k = 7;
        l = i * 16;
        memcpy(msg_str + 61, data + l, len % 16);
        for (j = 0; j < len % 16; j++)
        {
            if ((j & 0x03) == 0)
            {
                k++;
            }
            msg_str[k++] = chex[data[l] >> 4];
            msg_str[k++] = chex[data[l++] & 0x0F];
            k++;
            if (!isgraph(msg_str[61 + j]))
                msg_str[61 + j]= '.';
        }
        for (; j < 16; j++)
        {
            if ((j & 0x03) == 0)
            {
                k++;
            }
            msg_str[k++] = ' ';
            msg_str[k++] = ' ';
            k++;
            msg_str[61 + j]= ' ';
        }
		msg_str[127] = 0;
        lock_->acquire();
        fprintf(file_, "# %s\n", msg_str);
        lock_->release();

        return 0;
    }

 
    /*
        string convert to hex string, double copy, use it as little as possible
    */
    std::string Log::data2hex_str(
        const char * data, 
        size_t len)        
        {
            std::string hexstr;
			char msg_str[3] = {0};
			for (uint32_t i = 0; i < len; ++i)
			{
				sprintf(msg_str, "%02x", static_cast<unsigned char>(data[i]));
                hexstr.append(msg_str);
			}
            return hexstr;
        }

    Log Log::global_log;

}
