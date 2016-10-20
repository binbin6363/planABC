#include "mysqloperator.h"
#include <string>
#include <iostream>
#include <stdio.h>
#include "mysql/mysql.h"
#include "constants.h"
#include "log.h"
#include "string_util.h"
#include "timecounter.h"
#include "constants.h"
#include "adv_base.pb.h"
#include "mysql_msg.h"

using namespace std;
using namespace utils;
using namespace common;
using namespace com::adv::msg;


//void mempool_cleanup(void* p)
//{
//    Mempool  *pool = (Mempool*)p;
//    delete pool;
//}

MysqlDatabaseImpl::MysqlDatabaseImpl()
//:oTss_mempool(mempool_cleanup)
{
	
}

void MysqlDatabaseImpl::Init(MysqlConnPool * pool, size_t mempoolsize, size_t recordTime)
{
	pPool = pool;
	iMempoolsize = mempoolsize;
    _recordTime = recordTime;

}


#define  _PROCESSERROR do{if(result != NULL) mysql_free_result(result); \
			if(con) \
			{ \
				if(res.get_ret() == RET_MEMERROR || res.get_ret() == RET_NOTEXIST \
					|| res.get_ret() == RET_INPUTPARAMERROR || res.get_ret() == RET_STMTSTOREERROR \
					|| res.get_ret() == RET_PARAMERROR || res.get_ret() == RET_STMTBINDERROR \
					|| res.get_ret() == RET_STMTMEMEEROR) pPool->ReleaseConnection(con); \
				else pPool->TerminationConnection(con); \
			} \
			return -1; } while(0)
			
int MysqlDatabaseImpl::Process(const QueryRequest &req, QueryResult &res)
{
    LOG(DEBUG)("process, sql:[%s]", req.sql.c_str());
    MYSQL_RES *result = NULL;   
	unsigned int column = 0;
	res.set_ret(0);
	res.set_insertid(0);
	res.set_affected_rows(0);
	MYSQL * con = pPool->GetConnection();
	if ( !con ) 
    {
		LOG(ERROR)("[MYSQLCONNECT_ERROR], GetConnection null, sql:[%s]", req.sql.c_str());
		res.set_ret(RET_DBTIMEOUT);
		res.set_msg("RET_DBTIMEOUT");
		_PROCESSERROR;
	}
    LOG(DEBUG)("get connection succ, sql:[%s]", req.sql.c_str());

	static char printer[1024] = {0};
	int n = snprintf(printer
		, 1024
		, "record:mysql exec sql:'%s'"
		, req.sql.c_str());
	printer[n] = 0;
	TimeCounterAssistant tca(printer);
	
	if ( mysql_real_query(con, req.sql.c_str(),req.sql.size()) ){
		LOG(ERROR)("exec error:%s",mysql_error(con));
		res.set_ret(mysql_errno(con));
		res.set_msg(mysql_error(con));
		_PROCESSERROR;
	}
	
	result = mysql_store_result(con);

	if(NULL == result){
    	res.set_insertid(mysql_insert_id(con));
    	res.set_affected_rows(mysql_affected_rows(con));
		goto ok;
	}
	
	column = mysql_num_fields(result);

	if( ReadResult(result, &res, column) !=0 ){
		LOG(ERROR)("fetch result error:%s, errno:%d", mysql_error(con), mysql_errno(con));
		res.set_ret(-1);
		res.set_msg("fetch result error");
		_PROCESSERROR;
	}else{
		goto ok;
	}

ok:
	if(result != NULL){
		mysql_free_result(result);
		LOG(DEBUG)("free result");
	}

	if(con){
		pPool->ReleaseConnection(con);
		LOG(DEBUG)("free con");
	}
	res.set_ret(0);
	return 0;

}

int MysqlDatabaseImpl::ReadResult(MYSQL_RES * result, QueryResult * res, int column)
{
    CHECK_ERROR_RETURN((result == NULL), 1, "mysql result is null.");
    CHECK_ERROR_RETURN((res == NULL), 1, "query result is null.");
	int rownum = mysql_num_rows(result);
    LOG(DEBUG)("ReadResult, rownum:%d", rownum);
	if ( rownum <= 0) 
    {
        res->set_affected_rows(0);
        res->set_ret(0);
        res->set_msg("query mysql no result.");
		return 0;
	}
    else 
    {
        res->set_affected_rows(rownum);
		//res->db_result_rows.reserve(rownum);
	}

    // get field name
    MYSQL_FIELD *field;
    vector<string> field_name;
    field_name.reserve(column);
    for(int i = 0; i < column; ++i)
    {
        field = mysql_fetch_field(result);
        if (NULL != field) {
            field_name.push_back(field->name);
        }
    }

	MYSQL_ROW row;
    for (int current=0; current<rownum; ++current )
    {
    	row = mysql_fetch_row(result);
		if(NULL == row)
		{
            LOG(ERROR)("mysql_fetch_row error, row is null");
            return -1;
		}
		unsigned long *pFieldLen = mysql_fetch_lengths(result);
		if(NULL == pFieldLen)
		{
            LOG(ERROR)("mysql_fetch_lengths error");
            return -1;		
		}
        PRow proto_row;
        //res->db_result_rows[current].reserve(column);
        for(int i=0;i<column;i++)
        {
			string stringOut = string((char*)row[i], pFieldLen[i]);
            PField *proto_field = proto_row.add_field();
            proto_field->set_name(field_name[i]);
            proto_field->set_value(stringOut);
			//res->db_result_rows[current][i] = stringOut;
			LOG(DEBUG)(" Value= %s,column=%d, row=%d",stringOut.c_str(),i, current);
        }
        res->add_row(proto_row);
    }
	
    res->set_ret(0);
    res->set_msg("query mysql succeed.");
	return 0;
}



#if 0
int MysqlDatabaseImpl::GetParam(MYSQL_BIND & var , const ac::Object& param , Mempool * pData)
{
    

	switch(param.type){
		case CHAR:
            {
			var.buffer_type = MYSQL_TYPE_STRING;
			var.buffer = (char*)&param.charValue;
			var.buffer_length = sizeof(char);
			var.length = 0;
			//LOG(DEBUG)("charParam=%c",param.charValue);
			break;
            }
		case SHORT: 
            {
			var.buffer_type = MYSQL_TYPE_SHORT;
			var.buffer = (char*)&param.shortValue;
			var.buffer_length = sizeof(short);
			var.length = 0;
			//LOG(DEBUG)("shortParam=%d",param.shortValue);
			break;
            }
		case INT:
			var.buffer_type = MYSQL_TYPE_LONG;
			var.buffer = (char*)&param.intValue;
			var.buffer_length = sizeof(int);
			var.length = 0;
			//LOG(DEBUG)("intParam=%d",param.intValue);
			break;
		case LONG:
            {
			var.buffer_type = MYSQL_TYPE_LONGLONG;
			var.buffer = (char*)&param.longValue;
			var.buffer_length = sizeof(int64_t);
			var.length = 0;
			//LOG(DEBUG)("longParam=%d",param.longValue);
			break;
            }
		case FLOAT:
            {
			var.buffer_type = MYSQL_TYPE_DOUBLE;
			var.buffer = (char*)&param.floatValue;
			var.buffer_length = sizeof(float);
			var.length = 0;
			break;
            }
		case STRING:
            {
			var.buffer_type = MYSQL_TYPE_VAR_STRING;
			var.buffer = (char *)param.stringValue.c_str();
			var.buffer_length = param.stringValue.size();
			var.length = (unsigned long *) pData->GetBuf(sizeof(unsigned long));
			*(var.length) = param.stringValue.size();
			//LOG(DEBUG)("stringParam=%s, length =%p",ac::Escape(param.stringValue).c_str(),var.length);
			break;
            }
		case NIL:
            {
			var.buffer = NULL;
			var.buffer_length = 0;
			var.length = 0;
            //LOG(DEBUG)("null value");
			break;
            }
		default:
            assert(false && "wrong value");
			return -1;
	}

	var.is_null = pData->GetBuf(sizeof(my_bool));

	return 0;
}

int MysqlDatabaseImpl::BindResult(MYSQL_BIND * bindResult, MYSQL_RES * result, Mempool * pData)
{
    

	MYSQL_FIELD * Field = mysql_fetch_fields(result);

	for(size_t i =0;i<mysql_num_fields(result);i++){
		my_bool *is_null = (my_bool*)(pData->GetBuf(sizeof(my_bool)));
            	unsigned long *length = (unsigned long*)(pData->GetBuf(sizeof(unsigned long)));

			switch(Field[i].type){
				// define in mysql_com.h
				case FIELD_TYPE_LONG:{
					//LOG(DEBUG)("bind int");
					int *intData = (int*)(pData->GetBuf(sizeof(int)));
					*intData = 0;
					if(is_null && length && intData){
						bindResult[i].buffer = (char*)intData;
						bindResult[i].buffer_type = MYSQL_TYPE_LONG;
						bindResult[i].length = length;
						bindResult[i].is_null = is_null;
                        bindResult[i].is_unsigned  = Field[i].flags&UNSIGNED_FLAG ? true : false;
						break;
					}else{
						//LOG(ERROR)("mempool is not enough to allocate int");
						return -1;
					}
				}
				case MYSQL_TYPE_INT24:
				case FIELD_TYPE_LONGLONG:{
					//LOG(DEBUG)("bind long");
					int64_t *longData = (int64_t *)(pData->GetBuf(sizeof(int64_t)));
					*longData = 0;
					if(longData && is_null && length ){
						bindResult[i].buffer = (char*)longData;
						bindResult[i].buffer_type = MYSQL_TYPE_LONGLONG;
						bindResult[i].length = length;
						bindResult[i].is_null = is_null;
                        bindResult[i].is_unsigned  = Field[i].flags&UNSIGNED_FLAG ? true : false;
						break;
					}else{
						//LOG(ERROR)("mempool is not enough for alloccate column=%d long",i);
						return -1;
					}
				}

				case FIELD_TYPE_DOUBLE:
				case FIELD_TYPE_DECIMAL:
                case FIELD_TYPE_NEWDECIMAL:{
					//LOG(DEBUG)("bind double");
					double * doubleData = (double *)(pData->GetBuf(sizeof(double)));
					*doubleData = 0;
					if( is_null && length && doubleData){
						bindResult[i].buffer = (char*)doubleData;
						bindResult[i].buffer_type = MYSQL_TYPE_DOUBLE;
						bindResult[i].length = length;
						bindResult[i].is_null = is_null;
						break;
					}else{
						LOG(ERROR)("mempool is not enough for alloccate column=%d decimal",i);
						return -1;
					}	
				}

				
				case MYSQL_TYPE_TINY:	
				case FIELD_TYPE_SHORT:{
					//LOG(DEBUG)("bind short");
					short *shortData = (short*)(pData->GetBuf(sizeof(short)));
					*shortData = 0;
					if(shortData && is_null && length ) {
						bindResult[i].buffer = (char*)shortData;
						bindResult[i].buffer_type = Field[i].type;
						bindResult[i].length = length;
						bindResult[i].is_null = is_null;
                        bindResult[i].is_unsigned  = Field[i].flags&UNSIGNED_FLAG ? true : false;
						break;
					} else {
						LOG(ERROR)("mempool is not enough for alloccate column=%d short",i);
						return -1;
					}
				}

				case FIELD_TYPE_FLOAT:{
					//LOG(DEBUG)("bind float");
					double *floatData = (double*)(pData->GetBuf(sizeof(double)));
					*floatData = 0;
					if(floatData && is_null && length ) {
						bindResult[i].buffer = (char*)floatData;
						bindResult[i].buffer_type = MYSQL_TYPE_FLOAT;
						bindResult[i].length = length;
						bindResult[i].is_null = is_null;
						break;
					} else {
						//LOG(ERROR)("mempool is not enough for alloccate column=%d float",i);
						return -1;
					}
				}
			
				case FIELD_TYPE_VAR_STRING:		
				case FIELD_TYPE_STRING:		
				case FIELD_TYPE_NULL:{
					//LOG(DEBUG)("bind string");
					char * stringData = (char*)(pData->GetBuf(Field[i].length));
                    if( is_null && length && stringData ) {
                        bindResult[i].buffer = stringData;
                        bindResult[i].buffer_type = MYSQL_TYPE_STRING;
                        bindResult[i].buffer_length = Field[i].length*sizeof(char);
                        bindResult[i].length = length;
                        bindResult[i].is_null = is_null;
                        break;
                    }else{
                        //LOG(ERROR)("mempool is not enough for alloccate column=%d newdate",i);
                        return -1;
                    }
				}
				case FIELD_TYPE_BLOB:                                                                             
                case FIELD_TYPE_TINY_BLOB:                                                                        
                case FIELD_TYPE_MEDIUM_BLOB:                                                                      
                case FIELD_TYPE_LONG_BLOB: {
					//LOG(DEBUG)("bind blob");
					char * stringData = (char*)(pData->GetBuf(Field[i].length*sizeof(char)));                     
                    if( is_null && length && stringData ) {                                                       
                        bindResult[i].buffer = stringData;                                                        
                        bindResult[i].buffer_type = MYSQL_TYPE_BLOB;
                        bindResult[i].buffer_length = Field[i].length*sizeof(char);                               
                        bindResult[i].length = length;                                                            
                        bindResult[i].is_null = is_null;                                                          
                        break;                                                                                    
                    }else{                                                                                        
                        //LOG(ERROR)("mempool is not enough for alloccate column=%d newdate",i);                      
                        return -1;                                                                                
                    } 
				}
					
				case FIELD_TYPE_TIMESTAMP:
				case FIELD_TYPE_TIME:{
					//LOG(DEBUG)("bind time");                                                                        
                    char * stringData = (char*)(pData->GetBuf(Field[i].length*sizeof(char)));                     
                    if( is_null && length && stringData ) {                                                       
                        bindResult[i].buffer = stringData;                                                        
                        bindResult[i].buffer_type = MYSQL_TYPE_STRING ;
                        bindResult[i].buffer_length = Field[i].length*sizeof(char);                               
                        bindResult[i].length = length;                                                            
                        bindResult[i].is_null = is_null;                                                          
                        break;                                                                                    
                    }else{                                                                                        
                        //LOG(ERROR)("mempool is not enough for alloccate column=%d newdate",i);                      
                        return -1;                                                                                
                    }
				}		
			
				case FIELD_TYPE_DATE:
				case FIELD_TYPE_YEAR:
				case FIELD_TYPE_NEWDATE:{
					//LOG(DEBUG)("bind date");
                    char * stringData = (char*)(pData->GetBuf(Field[i].length*sizeof(char)));
                    if( is_null && length && stringData ) {
                        bindResult[i].buffer = stringData;
                        bindResult[i].buffer_type = MYSQL_TYPE_STRING ;
                        bindResult[i].buffer_length = Field[i].length*sizeof(char);
                        bindResult[i].length = length;
                        bindResult[i].is_null = is_null;
                        break;
                    }else{
                        //LOG(ERROR)("mempool is not enough for alloccate column=%d newdate",i);
                        return -1;
                    }
				}		
				case FIELD_TYPE_DATETIME:{		
					//LOG(DEBUG)("bind datetime");		
					char * stringData = (char*)(pData->GetBuf(Field[i].length*sizeof(char)));
					if( is_null && length && stringData ) {
						bindResult[i].buffer = stringData;
						bindResult[i].buffer_type = MYSQL_TYPE_STRING ;
						bindResult[i].buffer_length = Field[i].length*sizeof(char);
						bindResult[i].length = length;
						bindResult[i].is_null = is_null;
						break;
					}else{
						//LOG(ERROR)("mempool is not enough for alloccate column=%d newdate",i);
						return -1;
					}
				}
			
				default:
					LOG(ERROR)("not support the type[%x], geometry type;[%x]", Field[i].type, MYSQL_TYPE_GEOMETRY);
					return -1;
			}		
		}
	return 0;
}

int MysqlDatabaseImpl::ReadFieldNames(MYSQL_RES * result , QueryResult * res)
{
    

	MYSQL_FIELD * Field = mysql_fetch_fields(result);
	for(size_t i=0;i<mysql_num_fields(result);i++){	
		res->fieldNames.push_back(std::string(Field[i].name,(size_t)Field[i].name_length) );
	}
	return 0;
}


int MysqlDatabaseImpl::ReadResult(MYSQL_STMT * stmt, MYSQL_BIND * bindResult , QueryResult * res, int column)
{
	int rownum = mysql_stmt_num_rows(stmt);
    LOG(DEBUG)("rownum:%d", rownum);
	if ( rownum <= 0) 
    {
		return 0;
	}
    else 
    {
		res->db_result_rows.reserve(rownum);
	}
    for (int current=0; current<rownum; ++current )
    {
        int ret = mysql_stmt_fetch(stmt);

        if (ret == MYSQL_DATA_TRUNCATED)
        {
           LOG(ERROR)("trucate columns");
        }
        else if(ret != 0 )
        {
            LOG(ERROR)("mysql_stmt_fetch error,ret:[%d], errno:[%d], errmsg:[%s]", ret, mysql_stmt_errno(stmt), mysql_stmt_error(stmt));
            return -1;
        }
        res->db_result_rows[current].reserve(column);
        for(int i=0;i<column;i++)
        {
            if(true  ==  *(bool *)bindResult[i].is_null)
            {
                struct Nil nullOut = Nil();
                nullOut.type = NIL;
                res->db_result_rows[current][i] = nullOut;
            }
            else
            {
                switch (bindResult[i].buffer_type)
                {
                case MYSQL_TYPE_LONG:{
                    //struct Integer intOut = Integer(*(int*)bindResult[i].buffer);
                    LOG(DEBUG)("data type unsigned:%d", bindResult[i].is_unsigned ? 1 : 0);
                    res->db_result_rows[current][i] = bindResult[i].is_unsigned ? (ac::Object)ac::Long(*(unsigned int*)bindResult[i].buffer) : (ac::Object)ac::Integer(*(int*)bindResult[i].buffer);
                    //LOG(DEBUG)("int Vaule =%d,column=%d, row=%d",intOut.intValue, i, current);
                    break;
                   }
                case MYSQL_TYPE_TINY:
                case MYSQL_TYPE_SHORT:{
                    struct Short shortOut = Short(*(short *)bindResult[i].buffer);
                    res->db_result_rows[current][i] = shortOut;
                    res->db_result_rows[current][i] = bindResult[i].is_unsigned ? (ac::Object)ac::Integer(*(unsigned short*)bindResult[i].buffer) : (ac::Object)ac::Short(*(short*)bindResult[i].buffer);
                    LOG(DEBUG)("short Value = %d,column=%d, row=%d",shortOut.shortValue,i, current);
                    break;
                                      }
                case MYSQL_TYPE_INT24:
                case MYSQL_TYPE_LONGLONG:{
                    struct Long longOut = Long(*(int64_t *)bindResult[i].buffer);
                    res->db_result_rows[current][i] = longOut;
                    LOG(DEBUG)("long Value = %lld,column=%d, row=%d",longOut.longValue,i, current);
                    break;
                                         }
                case MYSQL_TYPE_BLOB:
                case MYSQL_TYPE_TINY_BLOB:
                case MYSQL_TYPE_MEDIUM_BLOB:
                case MYSQL_TYPE_LONG_BLOB:
                case MYSQL_TYPE_STRING:
                case MYSQL_TYPE_VAR_STRING:
                case MYSQL_TYPE_NULL:
                case MYSQL_TYPE_TIME:
                case MYSQL_TYPE_DATE:
                case MYSQL_TYPE_TIMESTAMP:
                case MYSQL_TYPE_YEAR:
                case MYSQL_TYPE_NEWDATE: 
                case MYSQL_TYPE_DATETIME:
                    {
                        string stringOut = string((char*)bindResult[i].buffer,*(bindResult[i].length));
                        res->db_result_rows[current][i] = stringOut;
                        LOG(DEBUG)(" Value= %s,column=%d, row=%d",stringOut.c_str(),i, current);
                        break;
                    }
                case MYSQL_TYPE_DECIMAL:
                case MYSQL_TYPE_FLOAT:
                case MYSQL_TYPE_DOUBLE:
                    {
                        struct Float floatOut = Float((*(double*)bindResult[i].buffer));
                        res->db_result_rows[current][i] = floatOut;
                        LOG(DEBUG)("float value=%f,column=%d, row=%d",floatOut.floatValue,i, current);
                        break;
                    }
                default:
                    {
                        LOG(ERROR)("not support type =%d, column=%d, row=%d",bindResult[i].buffer_type, i, current);
                        break;
                    } 
                }
            }
        }
    }
	
	return 0;
}

int MysqlDatabaseImpl::Process(const QueryMutilRequest * req, QueryMutilResult * res)
{
	
	MYSQL * con = NULL;
	int retnum = 0;
	std::string retmsg=" ";
	res = NULL;
	if( (con = pPool->GetConnection()) == NULL){
		LOG(ERROR)("get connection null");
		retnum = -1;
		retmsg = "get connection null";
        	goto error;
	}
	
	if ( mysql_real_query(con, req->sql.c_str(),req->sql.size()) ){
		LOG(ERROR)("exec error:%s",mysql_error(con));
		retnum = mysql_errno(con);
		retmsg = mysql_error(con);
		goto error;
	}

error:
	return -1;

	/*
		todo:put the multiresult to the res
	*/

	return 0;	
	
}

int MysqlDatabaseImpl::Process(const ProcedureRequest * req, ProcedureResult * res)
{
    
	req = NULL;
	res = NULL;
	return 0;
}
#endif



