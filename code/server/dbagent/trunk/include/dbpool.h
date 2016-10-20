#ifndef DB_CONNPOOL_H_
#define DB_CONNPOOL_H_

#include "mysql/mysql.h"
#include <deque>
#include "thread_mutex.h"

using namespace utils;

class MysqlConnPool 
{
	public:
		MysqlConnPool();
		~MysqlConnPool();
		MYSQL * GetConnection();
		MYSQL * CreateConnection();
		void ReleaseConnection(MYSQL *conn);
		void TerminationConnection(MYSQL *conn);
        int Init(const std::string &host,const std::string &user
		    , const std::string &pass,const int port,const std::string& characterset
		    , const int size, const int timeout);
		void Uninit();
		inline void SetConnectionNum(int iConnectionNum){iConnectionNum_ = iConnectionNum;}
		inline int GetConnectionNum() const {return iConnectionNum_;}
		inline int GetCurrentConnectionNUm () const {return iCurrentConnectionNum;}
	private:
		std::deque<MYSQL*> dequeConnections; 
		std::string sHost;
		std::string sPassword;
		std::string sUser;
        std::string sCharacterSet;
		int iConnectionNum_;
		int iCurrentConnectionNum;	
		int iPort;
		Thread_Mutex oMutex;
		int timeOut;
		
};

#endif // MYSQL_POOL_H_

