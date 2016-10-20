#if !defined _FILE_FLOG_H
#define _FILE_FLOG_H
/*
 *日志写入文件，
 *
 *
 */

#include <iostream>
#include <ios>
//#include <time.h>
#include <fstream>
#include <stdio.h>
#include <stdarg.h>
#include <string>
#include <sstream>
#include <errno.h>
//#include <cutil.h>
#include <map>
#include <binpacket.h>

#ifdef _USE_THREAD
#include<pthread.h>
#endif

using namespace std;
//using namespace CPPSocket;

#ifndef LOG
#define LOG(rank) mlog.WriteLog(__FILE__, __LINE__, __FUNCTION__, rank)
#endif

#define COUT_UID(uid) "[" << uid << "]"

namespace utils
{
    class FLOG{
    public:
        typedef struct LOG_HEX_STR
        {
            LOG_HEX_STR(char *data_,uint32_t len_):data(data_),len(len_){}
            char * data;
            uint32_t len;
        }HSTR;
        typedef enum RANK
        {
            ERROR = 0,
            WARN  = 1,
            INFO  = 2,
            DEBUG = 3
        }LOGRANK;

    public:
        FLOG();
        ~FLOG();

        void init(map<string,string> cfg);
        void init(string filename,LOGRANK rank=INFO,bool m_isBuf=false);
        FLOG& getTime(LOGRANK rank);

        FLOG& WriteLog(const string& sFileName, uint32_t unLineNum,
                       const string& sFuncName, LOGRANK rank);

        template<class T> friend FLOG& operator<<(FLOG& out,T i)
        {
            if( out.m_rank <= out.m_initRank )
                out.m_os<<i;
            return out;
        }

        friend FLOG& operator<<(FLOG& out,HSTR hstr);

        template<bool D>
        friend FLOG& operator<<(FLOG& out, BinOutputPacket<D> outpkg)
		{
			if( out.m_rank <= out.m_initRank )
			{
				char tdata[3]={0};
				for (uint32_t i=0; i<outpkg.length(); i++)
				{
					sprintf(tdata, "%02x", static_cast<unsigned char>(outpkg.getData()[i]));
					out.m_os<<tdata;
				}
			}
            return out;
		}

//         friend FLOG& operator<<(FLOG& out,LOGRANK rank);  //设置日志级别

        friend FLOG& operator<<(FLOG& out,FLOG& lout)
        {
            return out;
        }

        friend FLOG& operator<<(FLOG& out,ostream& (*fendl)(ostream&));

        void setInitRank(LOGRANK rank=INFO)
        {
            m_initRank = rank;
        }

    private:
        FLOG& operator=(FLOG& log);
        FLOG(FLOG& log);
        void openlog();
    private:
        string m_logname;
        string m_cur_logname;
        ofstream m_logfile;
        stringstream m_os;
        bool m_fileOpen;
        LOGRANK m_rank;
        LOGRANK m_initRank;
        bool m_isBuf;

#ifdef _USE_THREAD
        pthread_mutex_t m_mutex;
#endif

    public:
        static const LOGRANK LOG_ERROR = ERROR;
        static const LOGRANK LOG_WARN  = WARN;
        static const LOGRANK LOG_INFO  = INFO;
        static const LOGRANK LOG_DEBUG = DEBUG;
    };

    namespace FILELOG
    {
        typedef  FLOG LOG;
        extern FLOG mlog;
    }

} // namespace utils

using namespace utils::FILELOG;

#endif
