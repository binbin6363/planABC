#include "comm_operate.h"
#include "constants.h"
#include "md5.h"
#include <assert.h>

#if 0
using namespace common;

namespace utils {

uint32_t GetTableNum(const string& sTableName)
{
    if ((TABLE_CORP == sTableName) ||
        (TABLE_CORP_ACCOUNT_REVERSE_CID == sTableName) ||
        (TABLE_CORP_ROLE == sTableName) ||
        (TABLE_CORP_DOMAIN_REVERSE_CID == sTableName) ||
        (TABLE_CORP_DEPT == sTableName) ||
        (TABLE_CORP_INDUSTRY == sTableName) ||
        (TABLE_CORP_FAX == sTableName) ||
        (TABLE_NGROUP_USER == sTableName) ||
        (TABLE_QGROUP_USER == sTableName))
    {
        return DB_TABLE_SIZE_SMALL;
    }
    else if ((TABLE_USER_DEPT == sTableName) ||
             (TABLE_USER == sTableName) ||
             (TABLE_USER_REVERSE_UID == sTableName) ||
             (TABLE_CONTACTOR == sTableName) ||
             (TABLE_CONTACTOR_GROUP == sTableName) ||
             (TABLE_COLLEAGUE == sTableName) ||
             (TABLE_COLLEAGUE_GROUP == sTableName) ||
             (TABLE_CHATMSG == sTableName) ||
             (TABLE_NOTICE == sTableName) ||
             (TABLE_SYSTEM_MSG == sTableName) ||
             (TABLE_ADD_CONTACTOR == sTableName) ||
             (TABLE_USER_NGROUP == sTableName) ||
             (TABLE_NGROUP_OFFLINE == sTableName) ||
             (TABLE_NGROUP_MSGLOG == sTableName) ||
             (TABLE_USER_QGROUP == sTableName) ||
             (TABLE_QGROUP_OFFLINE == sTableName) ||
             (TABLE_EXTEND_MAIL == sTableName) ||
             (TABLE_QGROUP_MSGLOG == sTableName))
    {
        return DB_TABLE_SIZE_BIG;
    }
    else
    {
        assert(false);
        // shouldn't get here
        return 0;
    }
}

void GetDbTableName(const string& sDbName, const string& sTableName,
                    const string& sId, string& sFullDbName,
                    string& sFullTableName)
{
    const uint32_t MD5_LEN = 33;
    char md5id[MD5_LEN] = {0};
    md5_string(reinterpret_cast<unsigned char*>(const_cast<char*>(sId.c_str())),
               md5id, sId.length());

    // int nDbNum = GetDbNum(sDb);
    uint32_t unTableNum = GetTableNum(sTableName);

    sFullDbName = "db_" + sDbName + "_" + string(md5id, 1);
    sFullTableName = "t_" + sTableName + "_";
    if (DB_TABLE_SIZE_SMALL == unTableNum)
    {
        sFullTableName += string(md5id, 1, 1);
    }
    else if (DB_TABLE_SIZE_BIG == unTableNum)
    {
        sFullTableName += string(md5id, 1, 2);
    }
    else
    {
        assert(false);
    }
}

int32_t GetImdID(uint32_t unCid, uint32_t unImdNum)
{
    if (0 == unImdNum)
    {
        assert(false);
        return -1;
    }

    return unCid%unImdNum + 1;
}

int32_t GetDBID(uint32_t unCid, uint32_t unDBNum)
{
    if (0 == unDBNum)
    {
        assert(false);
        return -1;
    }

    return unCid%unDBNum + 1;
}
} // namespace utils

#endif


