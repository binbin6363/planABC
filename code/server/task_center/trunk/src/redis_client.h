/**
 * @filedesc: 
 * redis_client.h, all redis operation define here
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/14 20:02:59
 * @modify:
 *
**/

#ifndef _REDIS_CLIENT_H_
#define _REDIS_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hiredis.h>
#include <string>
#include "timeoutmanager.h"
#include "constants.h"
#include "binpacket_wrap.h"

using namespace std;
using namespace common;
using namespace utils;


const char *MakeTaskLimitKey(uint64_t taskid);
const char *MakeTaskIdsKey(uint32_t hashid);



// 继承自TimeoutEvent， 超时做redis心跳
class RedisClient : public TimeoutEvent
{
public:
	RedisClient()
        : bConnected_(false)
        , last_time_(0)
        , pRdsContext(NULL)
        , pRedisSendBuff(NULL)
        , aryMsgBuff(NULL)
	{
		bConnected_ = false;
	}
	
	~RedisClient()
	{
        TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
		if(NULL != pRdsContext)
		{
			redisFree(pRdsContext);
		}
		
		if(NULL != pRedisSendBuff)
		{
			delete[] pRedisSendBuff;
            pRedisSendBuff = NULL;
		}
        
		if(NULL != aryMsgBuff)
		{
			delete[] aryMsgBuff;
            aryMsgBuff = NULL;
		}
	}

	virtual void handle_timeout(int id,void *userData);

	int SyncConnectRedis(uint32_t max_net_buf = REDIS_SENDBUFF_MAX);

	void set_serv_info( const Server_Info &info );
    int setRedisTimeOut(const struct timeval tv);

    // redis的心跳
    void keepalive();

    bool ExistKey(const uint64_t &id, const string &key);
    int SetKeyValue(const uint64_t &id, const string &key, const string &value, uint32_t expire = 0);

// redis operate
public:
	int GetTaskIds(uint32_t uid, vector<uint64_t> &ids);
	int CheckTaskUsedOut(uint32_t uid, const vector<uint64_t> &ids, vector<uint64_t> &invalid_ids);
	int DecrTaskNum(const uint64_t &hash_id, const uint64_t &task_id);
	int RemoveTaskIds(uint32_t uid, const uint64_t &id);
	int AddTaskToUser(uint32_t uid, const uint64_t &task_id, int expire = 0);


//	int appendFormatedCommand(const string &cmd);
//	int appendCommand(const char *format, ...);

private:

// bussiness func
public:
#if 0
	static string getQgroupMsgKey(uint32_t unQgroupId, string strBizPart = BIZ_QGROUP);
	static string getUserMsgKey(uint32_t unUid, string strBizPart = BIZ_QGROUP);
    static string getUserMacMsgKey(uint32_t unUid, string strBizPart = BIZ_QGROUP);
    static string getGroupIdMsgIdKey(uint32_t unQgroupId, string strBizPart = BIZ_QGROUP);
    static string getGroupIdMsgIdKeyList(const GroupidList &groupid_list, string strBizPart = BIZ_QGROUP);

	int SyncGetSingleChatMsgId(uint32_t& unMsgId, uint64_t hash_id);
    int SyncGetUserUc(uint32_t& uc, uint32_t uid);
    int AddUidCliMsgId(uint32_t uid, uint64_t msg_id, uint8_t flag);
    int CheckUserMsgExist(uint32_t uid, uint64_t msg_id, uint8_t flag);

///////////////////////////////////////////////////////////////////////////////
	bool UpdateUserUcMsgInfo(uint32_t unUid, UserQgroupMsgInfo& userMsgInfo, const string &strKey,
        string strBizPart = BIZ_QGROUP);

	bool GetAllUserQgroupMsgInfo(uint32_t unUid, set<UserQgroupMsgInfo>& userMsgInfo, const string &strKey, 
        string strBizPart = BIZ_QGROUP);

	bool UpdateAllUserQgroupMsgInfo(uint32_t unUid, set<UserQgroupMsgInfo>& userMsgInfo, const string &strKey, 
        string strBizPart = BIZ_QGROUP);

	bool RemoveUserUcExpired(uint32_t unUid, uint64_t unUserUc, const string &strKey, string strBizPart = BIZ_QGROUP);

    uint32_t UpdateUserMsgDataForJoinGroup(uint32_t unUid, const QgroupMsgInfo &rInfo, const string &strKey, 
        string strBizPart = BIZ_QGROUP);

    uint32_t UpdateUserMsgDataForQuitGroup(uint32_t unUid, uint32_t unGroupId, const string &strKey, 
        string strBizPart = BIZ_QGROUP);

    uint32_t UpdateUserMsgDataForMsgSetting(uint32_t unUid, uint32_t unGroupId, uint64_t unTargetMsgId, const string &strKey,
        string strBizPart = BIZ_QGROUP);

////////////////////////////////////////////////////////////////////////////////
	bool AddQgroupMsg(uint32_t unQgroupId, uint32_t unFromCid, uint32_t unFromUid, 
								uint32_t unTime, uint64_t unCliMsgId, uint64_t unMsgId, const string& msg, string strBizPart = BIZ_QGROUP);
	bool GetQgroupMsgByScoreWith(uint32_t unQgroupId, vector<QgroupPublicMsgInfo>& vtMsgInfo, 
								string strBizPart = BIZ_QGROUP, uint64_t unMinVal = 0, uint64_t unMaxVal = 0);
	bool GetOneQgroupMsgHasMaxIdWith(uint32_t unQgroupId, QgroupPublicMsgInfo& userMsgInfo, string strBizPart = BIZ_QGROUP);

    // 选择db
    bool SelectRedisDb(uint64_t hash_id, uint32_t region);
    bool DismissGroup(uint32_t group_id, const string &strBizPart);

    // 群及其当前消息id
    bool UpdateGroupIdCurMsgId(uint32_t group_id, uint64_t msg_id, const string &bizpart = BIZ_QGROUP);
    bool BatchGetGroupIdCurMsgId(const GroupidList &groupid_list, GroupidMsgid &groupid_msgid, const string &bizpart = BIZ_QGROUP);

#endif
private:

    // 选择db
    bool SelectRedisDb(uint64_t hash_id, uint32_t region);
    int GetDbByHashid(uint64_t hash_id, uint32_t region);
	inline char* getMsgBuff(){return aryMsgBuff;}
    void CheckRemoveMsg(const string &strMsgKey, uint32_t uid, uint32_t group_id);
    const char *GetChatMsgKey(uint32_t uid, uint64_t msg_id, uint8_t flag);
    const char *GetChatMsgIdKey(uint64_t hashid);
    const char *GetUserUcKey(uint64_t id);
    void connect_fail();
    int DelByKey(const char *key);

private:
	Server_Info serv_info_;
	bool   bConnected_;
    time_t last_time_;

	redisContext *pRdsContext;
	char *pRedisSendBuff;
	char *aryMsgBuff;
    uint32_t max_net_buf_;

};

#endif //_REDIS_CLIENT_H_

