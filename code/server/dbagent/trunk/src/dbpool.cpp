#include <errno.h>
#include <string>
#include <assert.h>
#include "log.h"
#include "dbpool.h"


MysqlConnPool::MysqlConnPool()
	:iCurrentConnectionNum(0),
	timeOut(0)
{
}

MysqlConnPool::~MysqlConnPool()
{
	Uninit();
}

int MysqlConnPool::Init(const std::string &host,const std::string &user
    , const std::string &pass,const int port,const std::string& characterset
    , const int size, const int timeout)
{
	iConnectionNum_ = size;
	sHost = host;
	sPassword = pass;
	sUser = user;
	iPort = port;
    sCharacterSet = characterset;
	dequeConnections.clear();
	iCurrentConnectionNum = 0;
	timeOut = timeout;
	for(int i=0;i<size;i++)
	{
		MYSQL * con = CreateConnection();
		if(!con){
			return -1;
		}
		dequeConnections.push_back(con);
	}
	return 0;
}

MYSQL * MysqlConnPool::CreateConnection()
{
	MYSQL * con = mysql_init(NULL);
	if(!con)
    {
		return NULL;
	}
	if(timeOut>0)
    {
		mysql_options(con,MYSQL_OPT_CONNECT_TIMEOUT,(char*)&timeOut);
	}
    mysql_options(con, MYSQL_OPT_RECONNECT, (char *)&timeOut);
    bool reportTruncate = true;
    mysql_options(con, MYSQL_REPORT_DATA_TRUNCATION, (my_bool*)&reportTruncate);

	if(!mysql_real_connect(con,sHost.c_str(),sUser.c_str(),sPassword.c_str(),NULL,iPort,
        NULL,CLIENT_MULTI_STATEMENTS))
    {
		LOG(ERROR)("mysql_real_connect(%s):%s",sHost.c_str(),mysql_error(con));
		mysql_close(con);
		LOG(INFO)("real connect error (%s):%s\n",sHost.c_str(),mysql_error(con));
		return NULL;
	}

    if (0 != mysql_set_character_set(con, sCharacterSet.c_str()))
    {
        LOG(ERROR)("[CONFIG_ERROR], mysql set character error, characterset:[%s]", sCharacterSet.c_str());
        mysql_close(con);
        return NULL;
    }

	return con;
}

MYSQL* MysqlConnPool::GetConnection()
{
	oMutex.acquire();
	MYSQL * conn;
	if(!dequeConnections.empty()){
		conn = dequeConnections.front();
		dequeConnections.pop_front();
		oMutex.release();
		return conn;
	}else{
		if(iCurrentConnectionNum< iConnectionNum_){
			oMutex.release();
			conn = CreateConnection();
			if(conn){
				oMutex.acquire();
				iCurrentConnectionNum++;
				oMutex.release();
			}
			return conn;
		}
		else{
			oMutex.release();
			LOG(WARN)("db connection empty");
			return NULL;
		}
	}
}

void MysqlConnPool::ReleaseConnection(MYSQL * conn)
{
	oMutex.acquire();
	dequeConnections.push_back(conn);
	oMutex.release();
}

void MysqlConnPool::TerminationConnection(MYSQL * conn)
{
	mysql_close(conn);   
	oMutex.acquire();
	iCurrentConnectionNum--;
	oMutex.release();
}

void MysqlConnPool::Uninit()
{
	for(std::deque<MYSQL*>::const_iterator pos = dequeConnections.begin();pos!=dequeConnections.end();pos++)
    {
		if(*pos != NULL)
        {
			mysql_close(*pos);
		}
	}
	dequeConnections.clear();
}

