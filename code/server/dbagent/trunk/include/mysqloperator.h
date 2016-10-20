#ifndef DB_MYSQL_OPERATOR_H_
#define DB_MYSQL_OPERATOR_H_
#include <stdint.h>
#include <string>
#include <vector>
#include "dbpool.h"
//#include "dboperator.h"

class QueryRequest;
class QueryResult;

class MysqlDatabaseImpl// : public Database
{
	public:
		MysqlDatabaseImpl();
		
		~MysqlDatabaseImpl(){}
		
		void Init(MysqlConnPool * pool, size_t mempoolsize, size_t recordTime);
		
		int Process(const QueryRequest &req, QueryResult &res);
		
//		int Process(const ProcedureRequest * req, ProcedureResult * res);

//		int Process(const QueryMutilRequest * req, QueryMutilResult * res);		

//		int GetParam(MYSQL_BIND & var , const ac::Object& param, Mempool * pData);
        
protected:

	private:
		MysqlConnPool *pPool;
//		ac::TSS oTss_mempool;
        size_t iMempoolsize;
        size_t _recordTime ; //unit:us

//		int BindResult(MYSQL_BIND * bindResult, MYSQL_RES * result, Mempool * pData);
		
		int ReadResult(MYSQL_RES * result, QueryResult * res, int column);

//		int ReadResult(MYSQL_STMT * stmt, MYSQL_BIND * bindResult , QueryResult * res, int column);

//		int ReadFieldNames(MYSQL_RES * result , QueryResult * res);
		
};


#endif

