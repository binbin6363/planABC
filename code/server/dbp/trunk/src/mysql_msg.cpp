/**
 * @filedesc: 
 * mysql_msg.h, handle mysql msg
 * @author: 
 *  bbwang
 * @date: 
 *  2015/11/4 21:52:59
 * @modify:
 *
**/
#include "mysql_msg.h"
#include "adv_protocol.pb.h"
#include "string_util.h"
#include "md5.h"
#include "constants.h"
#include "log.h"
#include <sstream>
#include <mysql/mysql.h>

using namespace com::adv::msg;
using namespace common;
using namespace utils;


string EscapeString(const string & s)
{
	if (s.empty())
		return s;

	string buf(s.size() * 2 + 1, 0);
	uint32_t len = mysql_escape_string(&*buf.begin(), s.c_str(), s.size());
	buf.resize(len);
	return buf;
}

// 地址比较，传入的也是常量
uint32_t getTableNum(const char *sTableName)
{
	if ((sTableName == TABLE_CASH) ||
		(sTableName == TABLE_ID) ||
        (sTableName == TABLE_TASK_LIST) ||
        (sTableName == TABLE_TASK_INFO))
	{
		return DB_TABLE_SIZE_SMALL;
	}

	if (sTableName == TABLE_USER)
	{
		return DB_TABLE_SIZE_BIG;
	}

	return 0;
}

string getDbTableName(const char *sDbName, const char *sTableName, uint32_t unId)
{
	return getDbTableName(sDbName, sTableName, utils::StringUtil::u32tostr(unId));
}


string getDbTableName(const char *sDbName, const char *sTableName, const string & sId)
{
	const uint32_t MD5_LEN = 33;
	char md5id[MD5_LEN] = { 0 };

	md5_string((const unsigned char*)sId.c_str(), md5id, sId.length());

	uint32_t unTableNum = getTableNum(sTableName);
	ostringstream ss;
	ss << "db_" << sDbName << "_" << md5id[0]
		<< "." << "t_" << sTableName << "_" << md5id[1];

	if (DB_TABLE_SIZE_BIG == unTableNum)
	{
		ss << md5id[2];
	}
	else if (DB_TABLE_SIZE_SMALL != unTableNum)
	{
	    LOG(FATAL)("get db table name failed, table size:%u get failed.", unTableNum);
		assert(false);
	}

	return ss.str();
}

string makePasswdAddSalt(const string &passwd)
{
    static char paswd_arr[32] = {0};
    string new_passwd = PASSWD_SALT + passwd;
    utils::md5_string((const unsigned char*)new_passwd.c_str(), paswd_arr, new_passwd.size());

    return string(paswd_arr, 32);
}

string makeGetUserIdKey(const string &dev_id)
{
	ostringstream ossSql;
	ossSql << "SELECT Fuid FROM "
		<< TABLE_USER_REG_LIST
		<< " WHERE Fdeviceid='" << EscapeString(dev_id) << "'";
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

string makeGenerateUserIdKey(const string &dev_id)
{
	ostringstream ossSql;
	ossSql << "INSERT INTO "
		<< TABLE_USER_REG_LIST
		<< " VALUES(NULL, '" << EscapeString(dev_id) << "'" << ", NULL)";
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}


string makeSetUserKey(const string &dev_id, const DbUser &user)
{
	ostringstream ossSql;
	ossSql << "INSERT INTO "
		<< getDbTableName(DB_USER, TABLE_USER, user.uid)
        << " SET Fuid=" << user.uid
        << ",Fdeviceid='" << EscapeString(user.deviceid) << "'"
        << ",Faccount='" << EscapeString(user.account) << "'"
        << ",Faccounttype=" << user.accounttype
        << ",Fpassword='" << EscapeString(user.passwd) << "'"
        << ",Fnick_name='" << EscapeString(user.nick_name) << "'"
        << ",Fpre_cash='" << user.income << "'"
        << ",Fuseable_cash='" << user.income << "'"
        << ",Ffetched_cash='" << user.income << "'"
        << ",Fcreate_time=null,Fupdate_time=null";
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

string makeGenerateUidKey()
{
	ostringstream ossSql;

    // 让原有id加1并返回加1后的值?
	ossSql << "SELECT Fid FROM db_global_id_0.t_id_0";
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

// 从任务列表中取
// TODO:这个地方应该要从任务列表取任务的当前状态
string makeGetTaskListKey(uint32_t uid, uint32_t flag, const uint64_t &taskid, uint32_t limit)
{
	ostringstream ossSql;

	ossSql << "SELECT Ftaskid,Fflag,Ftask_progress,Faward_cash FROM "
	    << getDbTableName(DB_USER, TABLE_TASK_LIST, uid)
	    << " WHERE Fuid=" << uid;
    if (flag == SYNC_HISTORY_TASK) {
        ossSql << " AND Ftaskid > " << taskid;
    } else {
        ossSql << " AND Ftaskid < " << taskid;
    }
    ossSql << " LIMIT " << limit;

    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

// 从用户表中取
string makeGetIncomeKey(uint32_t uid)
{
	ostringstream ossSql;

	ossSql << "SELECT Fpre_cash,Fuseable_cash,Ffetched_cash FROM "
	    << getDbTableName(DB_USER, TABLE_USER, uid)
	    << " WHERE Fuid=" << uid;
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}


string makeSaveTaskKey(uint32_t uid, const com::adv::msg::SaveTaskRequest &pb_sync_req)
{
	ostringstream ossSql;
    string money = DEFAULT_INCOME;

	ossSql << "INSERT INTO "
	    << getDbTableName(DB_USER, TABLE_TASK_LIST, uid)
	    << " VALUSES ";
    int size = pb_sync_req.taskinfo_size();
    for (int i = 0; i < size; ++i)
    {
        const com::adv::msg::TaskInfo& taskinfo = pb_sync_req.taskinfo(i);
        ossSql << "('" << uid << "'" 
            << ",'" << taskinfo.taskid() << "'"
            << ",'" << taskinfo.tasktype() << "'"
            << ",'" << taskinfo.taskstatus() << "'"
            << ",'" << taskinfo.taskpcstep() << "'"
            << ",'" << taskinfo.taskptstep() << "'"
            << ",'" << taskinfo.taskstime() << "'"
            << ",'" << taskinfo.tasketime() << "'"
            << ",'" << taskinfo.taskprice() << "'"
            << ",'" << money << "'"
            << ",'" << money << "'"
            << ",'null'"
            << ",'null')";
        if (i < (size-1)) {
            ossSql << ", ";
        }
    }
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}


string makeReportTaskKey(uint32_t uid, uint64_t taskid, uint32_t cur_step)
{
	ostringstream ossSql;
	ossSql << "UPDATE "
	    << getDbTableName(DB_USER, TABLE_TASK_LIST, uid)
	    << " SET Ftask_progress = " << cur_step << " WHERE Fuid = "
	    << uid << " AND Ftaskid = " << taskid;
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

string makeGetTaskAlgorithmKey(uint64_t taskid)
{
	ostringstream ossSql;
	ossSql << "SELECT Ftask_algorithm FROM "
	    << getDbTableName(DB_TASK, TABLE_TASK_INFO, (taskid%0XFFFFFFFF))
	    << " WHERE Ftaskid = " << taskid;
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}


string makeRewardUserKey(uint32_t uid, uint64_t taskid, uint32_t reward)
{
	ostringstream ossSql;
	ossSql << "UPDATE "
	    << getDbTableName(DB_USER, TABLE_TASK_LIST, uid)
	    << " SET Faward_cash = (Faward_cash + "
	    << reward << ") WHERE Fuid = "
	    << uid << " AND Ftaskid = " << taskid;
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

string makeCheckLoginKey(uint32_t uid, uint32_t account_type)
{
	ostringstream ossSql;
	ossSql << "SELECT Fthird_id,Fnick_name FROM "
	    << getDbTableName(DB_THIRD, TABLE_THIRD_USER, uid)
	    << " WHERE Fuid = " << uid 
	    << " AND Fthird_type = " << account_type
	    << " AND Fstatus = " << ACTIVATED_ACCOUNT;
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

// 提现操作
string makeWithdrawKey(uint32_t uid, uint32_t withdraw, const string &password)
{
	ostringstream ossSql;
    //update db_user_a.t_user_d7 set Fpre_cash = (Fpre_cash - withdraw), Fuseable_cash = (Fuseable_cash - withdraw), Ffetched_cash = (Ffetched_cash + withdraw)
	ossSql << "UPDATE "
	    << getDbTableName(DB_USER, TABLE_USER, uid)
	    << "SET Fpre_cash = (Fpre_cash - " << withdraw << "), "
	    << "Fuseable_cash = (Fuseable_cash -" << withdraw << "), "
	    << "Ffetched_cash = (Ffetched_cash +" << withdraw << ") "
	    << " WHERE Fuid = " << uid 
	    << " AND Fpassword = " << password
	    << " LIMIT 1";
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

string makeThirdLoginKey(uint32_t uid, const DbUser &user)
{
	ostringstream ossSql;
	ossSql << "INSERT INTO "
	    << getDbTableName(DB_THIRD, TABLE_USER, uid)
	    << " VALUES(" << uid << ", '" 
	    << EscapeString(user.account) << "', "
	    << user.accounttype << ", '"
	    << EscapeString(user.nick_name) << "', "
	    << user.gender << ", '"
	    << EscapeString(user.avatar) << "', '"
	    << EscapeString(user.info) << "', '"
	    << EscapeString(user.location) << "', '"
	    << EscapeString(user.city) << "', "
	    << user.status << ", "
	    << "null, null";
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}


string makeUserInfoKey(uint32_t uid, const DbUser &user)
{
	ostringstream ossSql;
	ossSql << "INSERT INTO "
	    << getDbTableName(DB_USER, TABLE_USER, uid)
	    << " VALUES(" << uid << ", '" 
	    << EscapeString(user.deviceid) << "', "
	    << VERSION_0 << ", '"
	    << EscapeString(user.account) << "', '"
	    << EscapeString(user.mobile) << "', '"
	    << EscapeString(user.mail) << "', '"
	    << EscapeString(user.sign) << "', "
	    << user.accounttype << ", '"
	    << EscapeString(user.passwd) << "', '"
	    << EscapeString(user.nick_name) << "', "
	    << DEFAULT_INCOME << ", "
	    << DEFAULT_INCOME << ", "
	    << DEFAULT_INCOME << ", "
	    << "null, null";
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}


// 时间过期，超额，取消，无效任务
// 从任务信息表和用户的任务列表做查询
string makeCheckTaskKey(uint32_t uid, uint64_t taskid)
{
	ostringstream ossSql;
    string task_info_table = getDbTableName(DB_TASK, TABLE_TASK_INFO, (taskid%0XFFFFFFFF));
    string user_task_table = getDbTableName(DB_USER, TABLE_TASK_LIST, uid);
	ossSql << "SELECT " 
        << task_info_table << ".Ftask_status, " 
        << task_info_table << ".Flimit_num, "
        << task_info_table << ".Fstart_num, "
        << "UNIX_TIMESTAMP(" << task_info_table << ".Fend_time), "
        << task_info_table << ".Ftask_algorithm, "
        << user_task_table << ".Fflag, "
        << user_task_table << ".Ftask_progress "
        << " FROM " 
        << task_info_table 
        << " INNER JOIN " 
        << user_task_table
        << " ON " 
        << task_info_table << ".Ftaskid = "
        << user_task_table << ".Ftaskid AND "
	    << user_task_table << ".Fuid = " << uid 
	    << " AND " 
	    << user_task_table << ".Ftaskid = " << taskid;
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

string makeWriteTaskKey(uint32_t uid, uint64_t taskid)
{
	ostringstream ossSql;
    string user_task_table = getDbTableName(DB_USER, TABLE_TASK_LIST, uid);
	ossSql << "INSERT INTO " 
        << user_task_table 
        << " VALUES ("
        << uid << ", "
        << taskid << ", "
        << 1 << ", " // 表示任务进行中
        << 0 << ", " // 表示当前任务进度为0
        << 0         // 表示当前已奖励了0元
        << ", null, null);"; // 时间
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

string makeCloseTaskKey(uint64_t taskid)
{
	ostringstream ossSql;
    string task_info_table = getDbTableName(DB_TASK, TABLE_TASK_INFO, (taskid%0XFFFFFFFF));
	ossSql << "UPDATE " 
        << task_info_table 
        << " SET Ftask_status = 0;";
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}

string makeQueryTaskInfoKey(uint64_t taskid)
{
	ostringstream ossSql;
    string task_info_table = getDbTableName(DB_TASK, TABLE_TASK_INFO, (taskid%0XFFFFFFFF));
	ossSql << "SELECT Ftask_status,Ftask_algorithm,Fpre_cash,Faward_cash,Flimit_num,Fstart_num,Fstart_time,Fend_time"
        << " FROM " 
        << task_info_table 
        << " WHERE Ftaskid = " << taskid;
    LOG(INFO)("[sql] %s", ossSql.str().c_str());
    return ossSql.str();
}


#if 0
DbRequest::DbRequest(BinInputPacket<> &inpkg)
    : Msg(inpkg)
    , inpkg_(inpkg)
{
}

DbRequest::~DbRequest()
{
}


DbResult::DbResult(BinOutputPacket<> &outpkg)
    : outpkg_(outpkg)
{
}


DbResult::~DbResult()
{
}

char *DbResult::getData()
{
    if (!outpkg_.good())
    {
        LOG(ERROR)("get data, output pkg is not good!");
    }
    return outpkg_.getData();
}

uint32_t DbResult::length()
{
    if (!outpkg_.good())
    {
        LOG(ERROR)("get length, output pkg is not good!");
    }
    return outpkg_.length();
}


QueryRequest::QueryRequest(BinInputPacket<> &inpkg)
    : DbRequest(inpkg)
    , iSqlID(0)
    , sql("")
    , IsRequestField(0)
    , timeout(0)
{
}

QueryRequest::~QueryRequest()
{
}

int QueryRequest::decode()
{
    int ret = 0;
    MysqlRequest mysql_request;
    if (!mysql_request.ParsePartialFromArray(inpkg_.getCur(), inpkg_.remainLength()))
    {
        ret = 2;
        LOG(ERROR)("FrontLoginMsg ParsePartialFromArray failed.");
    }
    PrintPbData(mysql_request, "query request decode");

    iSqlID = mysql_request.sqlid();
    sql = mysql_request.sql();
    IsRequestField = mysql_request.isrequestfield();
    timeout = mysql_request.timeout();
    return ret;
}

QueryResult::QueryResult(BinOutputPacket<> &outpkg)
    : DbResult(outpkg)
    , ret_code(0)
    , ret_msg("")
    , insertid(0)
    , affectedrows(0)
{
}

QueryResult::~QueryResult()
{
}

int QueryResult::encode()
{
    int head_len = sizeof(COHEADER);
    outpkg_.offset_head(head_len);

    MysqlResult front_result;

    int body_len = front_result.ByteSize();
    int length = head_len + body_len;
    header_.len = length;
    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("BackLoginMsg encode msg failed.");
        return -1;
    }

    PrintPbData(front_result, header_.print());
    front_result.SerializePartialToArray(outpkg_.getCur(), outpkg_.remainLength());
    return 0;
}
#endif


