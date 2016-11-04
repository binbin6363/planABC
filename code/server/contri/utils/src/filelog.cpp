#include "filelog.h"
#include <sstream>
#include "linux_util.h"
#include <unistd.h>


namespace utils
{


    namespace FILELOG
    {
		FLOG mlog;
	}
  
	FLOG::FLOG():m_fileOpen(false),m_rank(INFO),m_initRank(INFO),m_isBuf(false)
	{
    
#ifdef _USE_THREAD
		::pthread_mutex_init(&m_mutex, NULL);
#endif  
    
	}
  
	FLOG::~FLOG()
	{
		if(m_fileOpen){
			//cout<<"[close file]"<<m_cur_logname<<endl;
			m_logfile.close();
			m_fileOpen = false;
		}
    
#ifdef _USE_THREAD
		::pthread_mutex_destroy (&m_mutex);
#endif
    
	}
  
	void FLOG::init(string filename,LOGRANK rank,bool isBuf)
	{     
		//cout<<"into FLOG::init"<<endl;
		m_initRank = rank;
		m_isBuf = isBuf;
		if(filename.size()==0) return;
		m_logname = filename;

		ostringstream oss;
		oss << m_logname << "_" << getpid() << "_" + LinuxUtil::getTime();
		m_cur_logname = oss.str();
		//cout<<"[log file name]"<<m_cur_logname<<endl;
		m_logfile.open(m_cur_logname.c_str(),ios::out|ios::app);
		if(!m_logfile){
			m_fileOpen = false;
			return;
		}
		//cout<<m_cur_logname<<"open sucess."<<endl;
		m_fileOpen = true;
	}
  
	void FLOG:: init(map<string,string> cfg)
	{
		string fname = "";
		LOGRANK rank = INFO;
		bool isBuf = false;
		map<string,string>::iterator it = cfg.find("FILE");
		if(it != cfg.end()) fname = it->second;
		it = cfg.find("RANK");
		if(it != cfg.end()){
			if("ERROR" == it->second)      rank = LOG::LOG_ERROR;
			else if("WARN" == it->second)  rank = LOG::LOG_WARN;
			else if("INFO" == it->second)  rank = LOG::LOG_INFO;
			else if("DEBUG" == it->second) rank = LOG::LOG_DEBUG;
		}
		it = cfg.find("ISBUF"); 
		if(it != cfg.end()){
			if("TRUE" == it->second)       isBuf = true;
			else if("FALSE" == it->second) isBuf = false;
		}
		init(fname,rank,isBuf);
	}
  
	void FLOG::openlog()
	{     
		if(!m_fileOpen) return;
		if(1792*1024*1024 < m_logfile.tellp()){ // FLOG文件不超过1.8GB      
			m_logfile.close();

            ostringstream oss;
            oss << m_logname << "_" << getpid() << "_" << LinuxUtil::getTime();
			m_cur_logname = oss.str();
			m_logfile.open(m_cur_logname.c_str(),ios::out|ios::app);
			if(m_logfile.fail()) m_fileOpen = false;
			else m_fileOpen = true;
		}
	}

	FLOG& FLOG::getTime(LOGRANK rank)
	{
		m_rank = rank;

		if( m_rank <= m_initRank ){
#ifdef _USE_THREAD
			m_os<<"[TID:"<<::pthread_self()<<"]";
#endif

			m_os << LinuxUtil::getTime1();
			switch(rank){
			case ERROR:{
				m_os<<"[ERROR]";
			}break;
			case WARN:{
				m_os<<"[WARN]";
			}break;
			case INFO:{
				m_os<<"[INFO]";
			}break;
			case DEBUG:{
				m_os<<"[DEBUG]";
			}break;
			default:{
			}break;
			}
		}
		return *this;
	}

    FLOG& FLOG::WriteLog(const string& sFileName, uint32_t unLineNum,
                         const string& sFuncName, LOGRANK rank)
    {
#ifdef _USE_THREAD
        ::pthread_mutex_lock(&m_mutex);
#endif

        getTime(rank);
        if( rank <= m_initRank )
	{
	   m_os << sFileName << " " << unLineNum << " " << sFuncName << " ";
        }
       return *this;
    }

// 	FLOG& operator<<(FLOG& out,FLOG::LOGRANK rank)  //设置日志级别
// 	{   
    
// #ifdef _USE_THREAD
// 		::pthread_mutex_lock (&out.m_mutex);
// #endif
    
// 		out.m_rank = rank;
    
// #ifdef _USE_THREAD
// 		if( out.m_rank <= out.m_initRank )
// 			out.m_os<<"[TID:"<<::pthread_self()<<"]";      
// #endif
    
// 		return out;
// 	}       

	FLOG& operator<<(FLOG& out,FLOG::HSTR hstr)
	{
		if( out.m_rank <= out.m_initRank )
		{
			char tdata[3]={0};
			for (uint32_t i=0; i<hstr.len; i++)
			{
				sprintf(tdata, "%02x", static_cast<unsigned char>(hstr.data[i]));
				out.m_os<<tdata;
			}
		}
		return out;
	}

	FLOG& operator<<(FLOG& out,ostream& (*fendl)(ostream&))
	{     
		if( out.m_rank <= out.m_initRank ){
			out.openlog();
			if(out.m_fileOpen){
				out.m_os<<"\n";
				out.m_logfile<<out.m_os.str();
				if(!out.m_isBuf) out.m_logfile<<flush;
			}else cout<<out.m_os.str()<<endl;                         
			out.m_os.str(""); //清空缓冲区
		}
		out.m_rank = FLOG::LOG_INFO; //恢复日志默认级别
        
#ifdef _USE_THREAD
		::pthread_mutex_unlock (&out.m_mutex);
#endif
    
		return out;
	}
} // namespace utils

/*
  int main(int argc,char * argv[])
  {
  mlog.init("/tmp/log.log",LOG::LOG_ERROR);
  mlog.getTime(LOG::LOG_ERROR)<<"test LOG_ERROR"<<endl;
  mlog<<LOG::LOG_ERROR<<T::getTime1()<<"test LOG_ERROR"<<endl;
  mlog<<LOG::LOG_WARN<<T::getTime1()<<"test LOG_WARN"<<endl;
  mlog<<LOG::LOG_INFO<<T::getTime1()<<"test LOG_INFO"<<endl;
  mlog<<LOG::LOG_DEBUG<<T::getTime1()<<"test LOG_DEBUG"<<endl;        
  mlog<<LOG::LOG_ERROR<<T::getTime1()<<"test end---------"<<endl; 
  return 0;
  }
*/
