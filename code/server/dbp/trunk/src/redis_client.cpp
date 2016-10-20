/**
 * @filedesc: 
 * redis_client.cpp, all redis operation define here
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/14 20:02:59
 * @modify:
 *
**/

#include "redis_client.h"
#include "timecounter.h"
#include "constants.h"

#define RDS_DB_SIZE   (16)
#define BASE_OFFSET   (0)
#define INDEX_START   (0)
#define INDEX_END     (-1)
#define KEY_LENGTH    (512)

using namespace common;

int RedisClient::SyncConnectRedis(uint32_t max_net_buf)
{
	int ret = 0;
    this->max_net_buf_ = max_net_buf;
    LOG(INFO)("connect redis, max buf size:%u", this->max_net_buf_);
	pRedisSendBuff = new char[this->max_net_buf_];
	aryMsgBuff = new char[this->max_net_buf_];

	ONCE_LOOP_ENTER
	TimeCounterAssistant tca("connect redis, SyncConnectRedis");
	string ip = serv_info_.server_ip;
	uint32_t port = serv_info_.server_port;
	// 单位是s
	uint32_t con_timeout = serv_info_.conn_time_out;
	// 读写redis的超时时间，单位是s
	uint32_t timeout = serv_info_.time_out;

	// check reset context
	if(NULL != pRdsContext)
	{
		redisFree(pRdsContext);
        pRdsContext = NULL;
	}

	bConnected_ = false;
	struct timeval tConnectTimeout;
	tConnectTimeout.tv_sec = con_timeout;
	tConnectTimeout.tv_usec = 0;

	struct timeval tTimeout;
	tTimeout.tv_sec = timeout;
	tTimeout.tv_usec = 0;

	// set pRdsContext flag REDIS_BLOCK in this function
	pRdsContext = redisConnectWithTimeout(ip.c_str(), port, tConnectTimeout);
	if (NULL == pRdsContext || REDIS_OK != pRdsContext->err)
	{
        ret = (NULL != pRdsContext) ? pRdsContext->err : -1;
		LOG(ERROR)("connect redis server failed, err code:%d, [%s]", ret, serv_info_.print());
		connect_fail();
		break;
	}
	if (0 != setRedisTimeOut(tTimeout))
	{
		connect_fail();
		break;
	}
	LOG(INFO)("connect redis server succeed. [%s]", serv_info_.print());
	bConnected_ = true;
	last_time_ = time(NULL);
	TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.heart_beat_interval), NULL);
	LOG(INFO)("set redis heart beat iterval:%us", serv_info_.heart_beat_interval);

	ONCE_LOOP_LEAVE
	return ret;
}

void RedisClient::connect_fail()
{
    LOG(ERROR)("redis lost connect, start to reconnect redis server. peer addr info %s:%u", serv_info_.server_ip.c_str(), serv_info_.server_port);    
    bConnected_ = false;
	if(NULL != pRdsContext)
	{
		redisFree(pRdsContext);
        pRdsContext = NULL;
	}
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
}

void RedisClient::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;
	LOG(DEBUG)("RedisClient::handle_time_out");
    time_t cur_time = time(NULL);
    if (!bConnected_ && ((cur_time - last_time_) >= serv_info_.reconnect_interval))
    {
        LOG(INFO)("reconnect redis server, addr:%s:%u", serv_info_.server_ip.c_str(), serv_info_.server_port);
        //重连redis ，重连失败则再次注册定时器

        if (0 != SyncConnectRedis())
        {
            LOG(ERROR)("reconnect redis server failed. reconnect in %us next time", serv_info_.reconnect_interval);
        }
        return ;
    }
    keepalive();

    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.heart_beat_interval), NULL);
}


// heart beat
void RedisClient::keepalive()
{
    redisReply* reply = NULL;
    if (NULL == pRdsContext || !bConnected_)
    {
        return;
    }
    reply = (redisReply*)redisCommand(pRdsContext, "PING");
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        connect_fail();
    }
    else
    {
        last_time_ = time(NULL);
        LOG(INFO)("[beat] redis keep alive success, peer addr info %s:%u", serv_info_.server_ip.c_str(), serv_info_.server_port);
    }
    if (NULL != reply)
        freeReplyObject(reply);
    reply = NULL;
}

int RedisClient::setRedisTimeOut(const struct timeval tv)
{
    int ret = REDIS_OK;
    if (NULL == pRdsContext)
    {
        LOG(ERROR)("redis server do not connect when set timeout.");
        ret = REDIS_ERR;
        return ret;
    }
    if (pRdsContext->flags & REDIS_BLOCK)
    {
        ret = redisSetTimeout(pRdsContext, tv);
        LOG(INFO)("set redis timeout ok, timeout:%lus", tv.tv_sec);
    }
    else
    {
        LOG(ERROR)("set redis timeout failed, invalid in redis nonblock mode");
        ret = REDIS_ERR;
    }
    return ret;
}


int RedisClient::GetDbByHashid(uint64_t hash_id, uint32_t region)
{
    int index = 0;
    index = (hash_id % RDS_DB_SIZE) + BASE_OFFSET;
    return index;
}

bool RedisClient::SelectRedisDb(uint64_t hash_id, uint32_t region)
{
    bool ret = true;
    redisReply* reply = NULL;
ONCE_LOOP_ENTER
    if(!bConnected_)
    {
        LOG(ERROR)("redis not connected.");
        ret = false;
        break;
    }

    int db_index = GetDbByHashid(hash_id, region);
    if (-1 == db_index)
    {
        ret = false;
        break;
    }
    LOG(DEBUG)("select db index:%u, hashid:%lu", db_index, hash_id);
    
    reply = (redisReply*)redisCommand(pRdsContext, "SELECT %d", db_index);
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        ret = false;
        LOG(ERROR)("SelectRedisDb redis error, db index:%d, err code:%d", db_index, re_code);
    }
    if(NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis occur fatal error, disconnect redis server.");
        connect_fail();
    }
ONCE_LOOP_LEAVE
    return ret;
}

void RedisClient::set_serv_info( const Server_Info &info )
{
    serv_info_ = info;
}

static char keystr[KEY_LENGTH] = {0};
const char *RedisClient::GetTaskIdsKey(uint32_t hashid)
{
    snprintf(keystr, KEY_LENGTH, "%s%d", UID_PREFIX, hashid);
    return keystr;
}

const char *RedisClient::GetTaskLimitKey(uint63_t taskid)
{
    snprintf(keystr, KEY_LENGTH, "%s%lu", TASK_LIMIT_PREFIX, taskid);
    return keystr;
}

int RedisClient::GetTaskIds(uint32_t uid, vector<uint64_t> &ids)
{
    TimeCounterAssistant tca("visit redis, GetTaskIds");
    redisReply* reply = NULL;

    int ret = 0;
    if(!bConnected_)
    {
        return -1;
    }
    // select db
    if (!SelectRedisDb(uid, RDS_DB_SIZE))
    {
        return -1;
    }

    const char *key = GetTaskIdsKey(uid);
    LOG(DEBUG)("GetTaskIds, hash id:%lu, get key:%s", uid, key);
    reply = (redisReply*)redisCommand(pRdsContext, "LRANGE %s %d %d", key, INDEX_START, INDEX_END);
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        ret = re_code;
        if (reply == NULL)
        {
            LOG(ERROR)("visit redis occur fatal error, disconnect redis server.");
            connect_fail();
        }
        else
        {
            freeReplyObject(reply);
            reply = NULL;
        }
        LOG(ERROR)("GetTaskIds error, key:%s, err code:%d", key, re_code);
        return ret;
    }

    if (reply->type != REDIS_REPLY_ARRAY)
    {
        LOG(ERROR)("GetTaskIds error, reply type error, type:%u", reply->type);
        freeReplyObject(reply);
        reply = NULL;
        return -1;
    }

    uint64_t id = 0;
    for(uint32_t i = 0; i < reply->elements; i+=1)
    {
        id = atoll(reply->element[i]->str);
        ids.push_back(id);
    }

    return ret;

}



int RedisClient::CheckTaskUsedOut(uint32_t uid, const vector<uint64_t> &ids, vector<uint64_t> &invalid_ids)
{
    TimeCounterAssistant tca("visit redis, CheckTaskUsedOut");
    redisReply* reply = NULL;

    int ret = 0;
    if(!bConnected_)
    {
        return -1;
    }
    // select db
    if (!SelectRedisDb(uid, RDS_DB_SIZE))
    {
        return -1;
    }

    vector<uint64_t>::const_iterator iter = ids.begin();
    vector<uint64_t>::const_iterator iter_end = ids.end();
    for (; iter != iter_end; ++iter)
    {
        const char *key = GetTaskLimitKey(*iter);
        LOG(DEBUG)("CheckTaskUsedOut, task id:%lu, get key:%s", *iter, key);
        redisAppendCommand(pRdsContext, "DECR %s", key);
    }

    // fetch result
    int ret_value = 0;
    iter = ids.begin();
    int64_t cur_index = 0;
    for (; iter != iter_end; ++iter)
    {
        ret_value = redisGetReply(pRdsContext, (void **)(&reply));
        if (REDIS_OK == ret_value)
        {
            if (reply->str != NULL)
            {
                cur_index = atoll(reply->str);
                if (cur_index < 0)
                {
                    LOG(DEBUG)("task used out, task id:%lu, cur_index:%ld", *iter, cur_index);
                    invalid_ids.push_back(*iter);
                } else {
                    LOG(DEBUG)("task can used, task id:%lu, cur_index:%ld", *iter, cur_index);
                }
            }
            else
            {
                LOG(DEBUG)("check task used out error, task id:%lu, reply value is null.", *iter);
            }
        }
        else
        {
            LOG(ERROR)("CheckTaskUsedOut, redis error, break, ret:%d", ret_value);
            break;
        }
        if (reply != NULL) {
            freeReplyObject(reply);
            reply = NULL;
        }
    }

    return ret;

}


int RedisClient::RemoveTaskIds(uint32_t uid, vector<uint64_t> &ids)
{
    TimeCounterAssistant tca("visit redis, RemoveTaskIds");
    redisReply* reply = NULL;

    int ret_value = 0;
    if(!bConnected_)
    {
        return -1;
    }
    // select db
    if (!SelectRedisDb(uid, RDS_DB_SIZE))
    {
        return -1;
    }

    vector<uint64_t>::const_iterator iter = ids.begin();
    vector<uint64_t>::const_iterator iter_end = ids.end();
    for (; iter != iter_end; ++iter)
    {
        const char *key = GetTaskIdsKey(uid);
        LOG(DEBUG)("RemoveTaskIds, task id:%lu, get key:%s", *iter, key);
        redisAppendCommand(pRdsContext, "RPOP %s", key);
    }

    // fetch result
    iter = ids.begin();
    int64_t id = 0;
    for (; iter != iter_end; ++iter)
    {
        ret_value = redisGetReply(pRdsContext, (void **)(&reply));
        if (REDIS_OK == ret_value)
        {
            if (reply->str != NULL)
            {
                id = atoll(reply->element[i]->str);
                LOG(INFO)("RPOP task id:%lu", id);
            }
            else
            {
                LOG(DEBUG)("RPOP task id error, task id:%lu, reply value is null.", *iter);
            }
        }
        else
        {
            LOG(ERROR)("RemoveTaskIds, redis error, break, ret:%d", ret_value);
            break;
        }
        if (reply != NULL) {
            freeReplyObject(reply);
            reply = NULL;
        }
    }

    return ret;

}








string RedisClient::getQgroupMsgKey(uint32_t unQgroupId, string strBizPart)
{
    string strKey = strBizPart;
    char szKey[128] = { 0 };
    sprintf(szKey, "_MSG_QId:%u", unQgroupId);
    strKey.append(szKey);
    LOG(DEBUG)("getQgroupMsgKey:%s", strKey.c_str());
    return strKey;
}

string RedisClient::getUserMsgKey(uint32_t unUid, string strBizPart)
{
    string strKey = strBizPart;
    char szKey[128] = { 0 };
    sprintf(szKey, "_MSG_UId:%u", unUid);
    strKey.append(szKey);
    LOG(DEBUG)("getUserMsgKey:%s", strKey.c_str());
    return strKey;
}

string RedisClient::getUserMacMsgKey(uint32_t unUid, string strBizPart)
{
    string strKey = strBizPart;
    char szKey[128] = { 0 };
    sprintf(szKey, "_MSG_MAC_UId:%u", unUid);
    strKey.append(szKey);
    LOG(DEBUG)("getUserMacMsgKey:%s", strKey.c_str());
    return strKey;
}


// ngroup_ID_MSG_ID:qid/qgroup_ID_MSG_ID:qid
string RedisClient::getGroupIdMsgIdKey(uint32_t unGroupId, string strBizPart)
{
    string strKey = strBizPart;
    char szKey[128] = { 0 };
    sprintf(szKey, "_ID_MSG_ID:%u", unGroupId);
    strKey.append(szKey);
    LOG(DEBUG)("getGroupIdMsgIdKey:%s", strKey.c_str());
    return strKey;
}

// ngroup_ID_MSG_ID:qid/qgroup_ID_MSG_ID:qid
string RedisClient::getGroupIdMsgIdKeyList(const GroupidList &groupid_list, string strBizPart)
{
    string strKey = "";
    if (groupid_list.empty())
    {
        LOG(ERROR)("getGroupIdMsgIdKeyList, group list is empty. bizpart:%s", strBizPart.c_str());
        return strKey;
    }
    GroupidList::const_iterator groupid_iter = groupid_list.begin();
    GroupidList::const_iterator end_iter = groupid_list.end();
    for (; groupid_iter != end_iter; ++groupid_iter)
    {
        char szKey[128] = { 0 };
        sprintf(szKey, "%s_ID_MSG_ID:%u ", strBizPart.c_str(), *groupid_iter);
        strKey.append(szKey);
    }
    LOG(DEBUG)("getGroupIdMsgIdKeyList:%s", strKey.c_str());
    return strKey;
}

// redis num * 256(uint32)
int RedisClient::SyncGetSingleChatMsgId(uint32_t& unMsgId, uint64_t hash_id)
{
    TimeCounterAssistant tca("visit redis, SyncGetSingleChatMsgId");
    redisReply* reply = NULL;

    int ret = 0;
    if(!bConnected_)
    {
        return 1;
    }
    // select db
    if (!SelectRedisDb(hash_id, ID_REGION))
    {
        return 1;
    }

    const char *key = GetChatMsgIdKey(hash_id);
    LOG(DEBUG)("SyncGetSingleChatMsgId, hash id:%lu, get key:%s", hash_id, key);
    reply = (redisReply*)redisCommand(pRdsContext, "INCR %s", key);
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        ret = re_code;
        LOG(ERROR)("SyncGetSingleChatMsgId error, key:%s, err code:%d", key, re_code);
    }

    if(NULL != reply)
    {
        unMsgId = reply->integer;
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis occur fatal error, disconnect redis server.");
        connect_fail();
    }

    return ret;
}


// redis num * 256(uint32)
int RedisClient::SyncGetUserUc(uint32_t& uc, uint32_t uid)
{
    TimeCounterAssistant tca("visit redis, SyncGetUserUc");
    redisReply* reply = NULL;

    int ret = 0;
    if(!bConnected_)
    {
        return 1;
    }
    // select db
    if (!SelectRedisDb(uid, ID_REGION))
    {
        return 1;
    }

    if (NULL == pRdsContext )
    {
        LOG(ERROR)("visit redis occur fatal error, context is null, disconnect redis server.");
        connect_fail();
        return 1;
    }

    const char *key = GetUserUcKey(uid);
    LOG(DEBUG)("SyncGetUserUc, uid:%u, get key:%s", uid, key);
    reply = (redisReply*)redisCommand(pRdsContext, "INCR %s", key);
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        ret = re_code;
        LOG(ERROR)("SyncGetUserUc error, key:%s, err code:%d", key, re_code);
    }

    if(NULL != reply)
    {
        uc = reply->integer;
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis occur fatal error, disconnect redis server.");
        connect_fail();
    }

    return ret;
}


// 每次操作就检测一次消息数目，如果超标就删除老的消息
void RedisClient::CheckRemoveMsg(const string &strMsgKey, uint32_t uid, uint32_t group_id)
{
    int re_code = 0;
    int msg_num = 0;
    redisReply* reply = NULL;
    if(!bConnected_)
    {
        return ;
    }

    // select db
    if (!SelectRedisDb(group_id, MSG_REGION))
    {
        LOG(ERROR)("CheckRemoveMsg, select redis db failed.");
        return ;
    }

    // 获取群消息条数
    reply = (redisReply*)redisCommand(pRdsContext, "ZCARD %s",strMsgKey.c_str());
    re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        LOG(ERROR)("CheckRemoveMsg error, get group msg num failed, don't remove msg, err code:%d", re_code);
        return;
    }
    if(NULL != reply)
    {
        msg_num = reply->integer;
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis get msg num occur fatal error, disconnect redis server.");
        connect_fail();
        return;
    }

    int remove_num = msg_num - ServerConfig::Instance()->rds_max_msg_num_;
    if (remove_num < (int)ServerConfig::Instance()->rds_remove_num_)
    {
        return ;
    }
    LOG(DEBUG)("[%u] check remove msg. groupid:%u, redis stored msg num:%u, need to remove msg num:%u."
        , uid, group_id, msg_num, remove_num);
    // 删除老的消息
    reply = (redisReply*)redisCommand(pRdsContext, "ZREMRANGEBYRANK %s 0 %d"
        ,strMsgKey.c_str(), remove_num);
    re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        LOG(ERROR)("CheckRemoveMsg error, err code:%d", re_code);
    }
    if(NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis remove expired msg occur fatal error, disconnect redis server.");
        connect_fail();
    }
    return ;
}

bool RedisClient::AddQgroupMsg(uint32_t unQgroupId, uint32_t unFromCid, uint32_t unFromUid,
                                uint32_t unTime, uint64_t unCliMsgId, uint64_t unMsgId, const string& msg, string strBizPart)
{
    TimeCounterAssistant tca("visit redis, AddQgroupMsg");
    bool ret = true;
    string strMsgKey = getQgroupMsgKey(unQgroupId, strBizPart);

    redisReply* reply = NULL;
    if(!bConnected_)
    {
        return false;
    }

    // select db
    if (!SelectRedisDb(unQgroupId, MSG_REGION))
    {
        return false;
    }

    QgroupPublicMsgInfo msgInfo;
    msgInfo.unQgroupId = unQgroupId;
    msgInfo.unFromCid = unFromCid;
    msgInfo.unFromUid = unFromUid;
    msgInfo.unMsgId = unMsgId;
    msgInfo.unCliMsgId = unCliMsgId;
    msgInfo.unTime = unTime;
    msgInfo.strMsg = msg;

    // 检测超出数目的msg, 并删除
    CheckRemoveMsg(strMsgKey, unFromUid, unQgroupId);
    
    BinOutputPacket<> outpkg(getMsgBuff(), max_net_buf_);
    uint32_t unMsgLen = msgInfo.Serialize(outpkg.getData(), outpkg.remainLength());
    if(0 == unMsgLen)
    {
        return false;
    }
    outpkg.offset(unMsgLen);

    reply = (redisReply*)redisCommand(pRdsContext, "ZADD %s %llu %b",
                                      strMsgKey.c_str(), unMsgId, outpkg.getData(), outpkg.length());
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        LOG(ERROR)("AddQgroupMsg error, err code:%d", re_code);
        ret = false;
    }
    if(NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis add msg occur fatal error, disconnect redis server.");
        connect_fail();
    }

    return ret;
}


bool RedisClient::GetQgroupMsgByScoreWith(uint32_t unQgroupId, vector<QgroupPublicMsgInfo>& vtMsgInfo,
        					string strBizPart, uint64_t unMinVal, uint64_t unMaxVal)
{
    TimeCounterAssistant tca("visit redis, GetQgroupMsgByScoreWith");
    string strMsgKey = getQgroupMsgKey(unQgroupId, strBizPart);
    bool ret = true;

    redisReply* reply = NULL;
    if(!bConnected_)
    {
        return false;
    }

    // select db
    if (!SelectRedisDb(unQgroupId, MSG_REGION))
    {
        LOG(ERROR)("select redis db failed.");
        return false;
    }

    if(0 == unMinVal)
    {
        if(0 == unMaxVal)
        {
            reply = (redisReply*)redisCommand(pRdsContext, "ZRANGEBYSCORE %s -inf +inf WITHSCORES", strMsgKey.c_str());
        }
        else
        {
            reply = (redisReply*)redisCommand(pRdsContext, "ZRANGEBYSCORE %s -inf %llu WITHSCORES", strMsgKey.c_str(), unMaxVal);
        }
    }
    else
    {
        if(0 == unMaxVal)
        {
            reply = (redisReply*)redisCommand(pRdsContext, "ZRANGEBYSCORE %s (%llu +inf WITHSCORES", strMsgKey.c_str(), unMinVal);
        }
        else
        {
            reply = (redisReply*)redisCommand(pRdsContext, "ZRANGEBYSCORE %s (%llu %llu WITHSCORES", strMsgKey.c_str(), unMinVal, unMaxVal);
        }
    }
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        LOG(ERROR)("GetQgroupMsgByScoreWith error, err code:%d", re_code);
    }
	if (NULL == reply)
	{
        ret = false;
        LOG(ERROR)("visit redis get group msg occur fatal error, disconnect redis server.");
        connect_fail();
	}

    if (reply->type != REDIS_REPLY_ARRAY)
    {
        freeReplyObject(reply);
        reply = NULL;
        return false;
    }

    uint64_t unMsgScore = 0;
    char *pContent = NULL;
    uint32_t unMsgSize = 0;
    for(uint32_t i = 0; i < reply->elements; i+=2)
    {
        unMsgScore = atoll(reply->element[i+1]->str);

        QgroupPublicMsgInfo singleMsgInfo;
        pContent = reply->element[i]->str;
        unMsgSize = singleMsgInfo.Unserialize(pContent, reply->element[i]->len);
        vtMsgInfo.push_back(singleMsgInfo);
        LOG(DEBUG)("GetQgroupMsgByScoreWith info: score:%lu, qid:%u, msgId:%lu",
                  unMsgScore, singleMsgInfo.unQgroupId, singleMsgInfo.unMsgId);
    }
    if (NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        ret = false;
        LOG(ERROR)("visit redis get group msg occur fatal error, disconnect redis server.");
        connect_fail();
    }

    return ret;
}

bool RedisClient::GetOneQgroupMsgHasMaxIdWith(uint32_t unQgroupId, QgroupPublicMsgInfo& userMsgInfo, string strBizPart)
{
    TimeCounterAssistant tca("visit redis, GetOneQgroupMsgHasMaxIdWith");
    string strMsgKey = getQgroupMsgKey(unQgroupId, strBizPart);

    redisReply* reply = NULL;
    if(!bConnected_)
    {
        return false;
    }

    // select db
    if (!SelectRedisDb(unQgroupId, MSG_REGION))
    {
        return false;
    }

    reply = (redisReply*)redisCommand(pRdsContext, "ZREVRANGE %s 0 0 WITHSCORES", strMsgKey.c_str());

    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        LOG(ERROR)("GetOneQgroupMsgHasMaxIdWith error, err code:%d", re_code);
    }
    if (NULL != reply)
    {
        if (reply->type == REDIS_REPLY_ARRAY)
        {
            if(0 != reply->elements)
            {
                LOG(DEBUG)("fetch result, the first reply length:%u", reply->element[0]->len);
                char * pContent = reply->element[0]->str;
                userMsgInfo.Unserialize(pContent, reply->element[0]->len);
            }
            else
            {
                LOG(DEBUG)("fetch no result, qgroupid:%u", unQgroupId);
            }
        }
        else
        {
            LOG(ERROR)("redis return error reply type:%u, str:%s", reply->type, reply->str);
        }
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis get one group msg occur fatal error, disconnect redis server.");
        connect_fail();
    }

    return true;
}

bool RedisClient::UpdateUserUcMsgInfo(uint32_t unUid, UserQgroupMsgInfo& userMsgInfo, const string &strKey, 
                                       string strBizPart/* = BIZ_QGROUP*/)
{
    if( strKey.empty() )
    {
        LOG(ERROR)("UpdateUserUcMsgInfo param_key is null. uid:%u, user_uc:%lu, bizpart:%s",
            unUid, userMsgInfo.unUserUc, strBizPart.c_str());

        return false;
    }

    TimeCounterAssistant tca("visit redis, UpdateUserUcMsgInfo");
    redisReply* reply = NULL;
    bool ret = true;
    if(!bConnected_)
    {
        return false;
    }
    
    // select db
    if (!SelectRedisDb(unUid, MSG_REGION))
    {
        return false;
    }

    for(uint32_t i = 0; i < userMsgInfo.vtMsgInfo.size(); i++)
    {
        LOG(DEBUG)("UpdateUserUcMsgInfo info: uid:%u, qid:%u, msgId:%lu",
                  unUid, userMsgInfo.vtMsgInfo[i].unQgroupId, userMsgInfo.vtMsgInfo[i].unMaxId);
    }

    BinOutputPacket<> outpkg(getMsgBuff(), max_net_buf_);
    uint32_t unMsgLen = userMsgInfo.Serialize(outpkg.getData(), outpkg.remainLength());
    outpkg.offset(unMsgLen);

    reply = (redisReply*)redisCommand(pRdsContext, "HSET %s %lu %b",
                                      strKey.c_str(), userMsgInfo.unUserUc, outpkg.getData(), outpkg.length());
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        LOG(ERROR)("UpdateUserUcMsgInfo error, err code:%d", re_code);
        ret = false;
    }

    if (NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis uodate uc occur fatal error, disconnect redis server.");
        connect_fail();
    }
    return ret;
}

uint32_t RedisClient::UpdateUserMsgDataForJoinGroup(uint32_t unUid, const QgroupMsgInfo &rInfo, const string &strKey,
                                                 string strBizPart /* = BIZ_QGROUP */)
{
    if( strKey.empty() )
    {
        LOG(ERROR)("UpdateUserMsgDataForJoinGroup param_key is null. uid:%u, groupId:%u, bizpart:%s",
            unUid, rInfo.unQgroupId, strBizPart.c_str());

        return false;
    }

    set<UserQgroupMsgInfo> stUserMsgInfo;
    bool rev = GetAllUserQgroupMsgInfo(unUid, stUserMsgInfo, strKey, strBizPart);
    if(!rev)
    {
        LOG(ERROR)("join group read rds failed. uid:%u, groupid:%u, key:%s", unUid, rInfo.unQgroupId, strKey.c_str());

        return (uint32_t)JOINGROUP_RDS_ERROR;
    }
    
    LOG(DEBUG)("start to insert group to user group list, uid:%u, groupid:%u, max msg id:%lu, key:%s, bizpart:%s"
        , unUid, rInfo.unQgroupId, rInfo.unMaxId, strKey.c_str(), strBizPart.c_str());

    set<UserQgroupMsgInfo>::iterator itor = stUserMsgInfo.begin();
    while(itor != stUserMsgInfo.end())
    {
        UserQgroupMsgInfo &msgUcInfo = const_cast<UserQgroupMsgInfo&>(*itor);

        // 如果已经添加，则不再加入
        if (msgUcInfo.vtMsgInfo.end() == std::find(msgUcInfo.vtMsgInfo.begin(), msgUcInfo.vtMsgInfo.end(), rInfo))
            
            
        {
            msgUcInfo.vtMsgInfo.push_back(rInfo);
        } 
        else
        {
            LOG(WARN)("UpdateUserMsgDataForJoinGroup, group is already exist in user. uid:%u, groupid:%u, key:%s, bizpart:%s"
                , unUid, rInfo.unQgroupId, strKey.c_str(), strBizPart.c_str());
        }
        itor++;
    }

    bool ret = UpdateAllUserQgroupMsgInfo(unUid, stUserMsgInfo, strKey, strBizPart);
    if (!ret)
    {
        LOG(ERROR)("UpdateUserMsgDataForJoinGroup, user join group write rds failed. uid:%u, groupid:%u, key:%s, bizpart:%s", 
            unUid, rInfo.unQgroupId, strKey.c_str(), strBizPart.c_str());

        return (uint32_t)JOINGROUP_RDS_ERROR;
    }

    return (uint32_t)JOINGROUP_SUCCESS;
}

uint32_t RedisClient::UpdateUserMsgDataForQuitGroup(uint32_t unUid, uint32_t unGroupId, const string &strKey, 
                                                     string strBizPart /* = BIZ_QGROUP */)
{
    if( strKey.empty() )
    {
        LOG(ERROR)("UpdateUserMsgDataForQuitGroup param_key is null. uid:%u, groupId:%u, bizpart:%s",
            unUid, unGroupId, strBizPart.c_str());

        return false;
    }

    set<UserQgroupMsgInfo> stUserMsgInfo;
    bool rev = GetAllUserQgroupMsgInfo(unUid, stUserMsgInfo, strKey, strBizPart);
    if(!rev)
    {
        LOG(WARN)("UpdateUserMsgDataForQuitGroup, GetAllUserQgroupMsgInfo ret error. uid:%u, groupId:%u, key:%s, bizpart:%s", 
           unUid, unGroupId, strKey.c_str(), strBizPart.c_str());

        return (uint32_t)EXITGROUP_RDS_ERROR;
    }

    LOG(DEBUG)("start to exit group from user group list, uid:%u, groupId:%u, key:%s, bizpart:%s"
        , unUid, unGroupId, strKey.c_str(), strBizPart.c_str());
    
    set<UserQgroupMsgInfo>::iterator itor = stUserMsgInfo.begin();
    while(stUserMsgInfo.end() != itor)
    {
        UserQgroupMsgInfo& userMsgUcInfo= const_cast<UserQgroupMsgInfo&>(*itor);

        QgroupMsgInfoVec::iterator groupItor = userMsgUcInfo.vtMsgInfo.begin();
        while(groupItor != userMsgUcInfo.vtMsgInfo.end())
        {
            if(groupItor->unQgroupId == unGroupId)
            {
                userMsgUcInfo.vtMsgInfo.erase(groupItor);
                break;
            }
            groupItor++;
        }
        itor++;
    }

    rev = UpdateAllUserQgroupMsgInfo(unUid, stUserMsgInfo, strKey, strBizPart);
    if(!rev)
    {
        LOG(ERROR)("UpdateUserMsgDataForQuitGroup, exit group write rds failed. uid:%u, groupId:%u, key:%s, bizpart:%s", 
            unUid, unGroupId, strKey.c_str(), strBizPart.c_str());

        return (uint32_t)EXITGROUP_RDS_ERROR;
    }

    return (uint32_t)EXITGROUP_SUCCESS;
}

uint32_t RedisClient::UpdateUserMsgDataForMsgSetting(uint32_t unUid, uint32_t unGroupId, uint64_t unTargetMsgId, 
                                                      const string &strKey, 
                                                      string strBizPart /* = BIZ_QGROUP */)
{
    if( strKey.empty() )
    {
        LOG(ERROR)("UpdateUserMsgDataForMsgSetting param_key is null. uid:%u, groupId:%u, targetId:%lu, bizpart:%s",
            unUid, unGroupId, unTargetMsgId, strBizPart.c_str());

        return false;
    }

    set<UserQgroupMsgInfo> stUserMsgInfo;
    bool rev = GetAllUserQgroupMsgInfo(unUid, stUserMsgInfo, strKey, strBizPart);
    if(!rev)
    {
        LOG(ERROR)("UpdateUserMsgDataForMsgSetting, get all user group msg info error. uid:%u, groupId:%u, targetId:%lu, key:%s, bizpart:%s",
           unUid, unGroupId, unTargetMsgId, strKey.c_str(), strBizPart.c_str());
       
        return 1;
    }

    LOG(DEBUG)("start to set user group max msg id when msg setting msg receive. uid:%u, groupId:%u, targetId:%lu, key:%s, bizpart:%s"
        , unUid, unGroupId, unTargetMsgId, strKey.c_str(), strBizPart.c_str());

    set<UserQgroupMsgInfo> updatedUserMsgInfo;
    set<UserQgroupMsgInfo>::const_iterator iter = stUserMsgInfo.begin();
    // 对所有uc下的unQgroupId，设置消息id
    for (; stUserMsgInfo.end() != iter; ++iter)
    {
        UserQgroupMsgInfo uqmInfo = *iter;
        // 按照群id查找QgroupMsgInfo
        QgroupMsgInfoVec &groupMsgInfoSet = uqmInfo.vtMsgInfo;
        QgroupMsgInfoVec::iterator infoIter = groupMsgInfoSet.begin();
        for (; groupMsgInfoSet.end() != infoIter; ++infoIter)
        {
            if (unGroupId == infoIter->unQgroupId)
            {
                LOG(INFO)("UpdateUserMsgDataForMsgSetting, uid:%u, key:%s, bizpart:%s, find uc:%lu, groupid:%u, set msgid:%lu to msgid:%lu"
                    , unUid, strKey.c_str(), strBizPart.c_str(), uqmInfo.unUserUc, infoIter->unQgroupId, infoIter->unMaxId, unTargetMsgId);

                infoIter->unMaxId = unTargetMsgId;
                break;
            }
        }
        updatedUserMsgInfo.insert( uqmInfo );
    }

    rev = UpdateAllUserQgroupMsgInfo(unUid, updatedUserMsgInfo, strBizPart);
    if(!rev)
    {
        LOG(ERROR)("UpdateUserMsgDataForMsgSetting, set group msg setting write rds failed. uid:%u, groupId:%u, targetId:%lu, key:%s, bizpart:%s", 
            unUid, unGroupId, unTargetMsgId, strKey.c_str(), strBizPart.c_str());

        return 1;
    }

    return 0;
}

bool RedisClient::GetAllUserQgroupMsgInfo(uint32_t unUid, set<UserQgroupMsgInfo>& stUserMsgInfo, const string &strKey,
                                           string strBizPart/* = BIZ_QGROUP*/)
{
    if( strKey.empty() )
    {
        LOG(ERROR)("GetAllUserQgroupMsgInfo param_key is null. uid:%u, bizpart:%s",
            unUid, strBizPart.c_str());

        return false;
    }

    TimeCounterAssistant tca("visit redis, GetAllUserQgroupMsgInfo");
    redisReply* reply = NULL;
    if(!bConnected_)
    {
        return false;
    }
    // select db
    if (!SelectRedisDb(unUid, MSG_REGION))
    {
        return false;
    }

    reply = (redisReply*)redisCommand(pRdsContext, "HGETALL %s", strKey.c_str());
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        LOG(ERROR)("GetAllUserQgroupMsgInfo error, err code:%d", re_code);
        return false;
    }
    if (reply->type != REDIS_REPLY_ARRAY)
    {
        freeReplyObject(reply);
        reply = NULL;
        return false;
    }

    uint64_t userUc = 0;
    char *pContent = NULL;
    for(uint32_t i = 0; i < reply->elements; i+=2)
    {
        userUc = atoll(reply->element[i]->str);
        LOG(DEBUG)("GetAllUserQgroupMsgInfo, uid:%u, userUc:%lu", unUid, userUc);
        UserQgroupMsgInfo userMsgInfo;
        userMsgInfo.unUserUc = userUc;

        pContent = reply->element[i+1]->str;
        userMsgInfo.Unserialize(pContent, max_net_buf_);
        stUserMsgInfo.insert(userMsgInfo);
    }

    if (NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis get user msg info occur fatal error, disconnect redis server.");
        connect_fail();
        return false;
    }
    return true;
}

bool RedisClient::UpdateAllUserQgroupMsgInfo(uint32_t unUid, set<UserQgroupMsgInfo>& userMsgInfo, const string &strKey,
                                              string strBizPart/* = BIZ_QGROUP*/)
{
    if( strKey.empty() )
    {
        LOG(ERROR)("UpdateAllUserQgroupMsgInfo param_key is null. uid:%u, bizpart:%s",
            unUid, strBizPart.c_str());

        return false;
    }

    TimeCounterAssistant tca("visit redis, UpdateAllUserQgroupMsgInfo");
    int re_code = 0;
    bool ret = true;
    redisReply* reply = NULL;
    if(!bConnected_)
    {
        LOG(ERROR)("UpdateAllUserQgroupMsgInfo, redis did not connect.");
        return false;
    }
    // select db
    if (!SelectRedisDb(unUid, MSG_REGION))
    {
        return false;
    }

    BinOutputPacket<> outpkg(getMsgBuff(), max_net_buf_);
    set<UserQgroupMsgInfo>::iterator itor = userMsgInfo.begin();
    uint32_t unMsgLen = 0;
    while(userMsgInfo.end() != itor)
    {
        outpkg.clear();
        unMsgLen = const_cast<UserQgroupMsgInfo&>(*itor).Serialize(outpkg.getData(), outpkg.remainLength());
        outpkg.offset(unMsgLen);

        reply = (redisReply*)redisCommand(pRdsContext, "HSET %s %lu %b",
                                          strKey.c_str(), itor->unUserUc, outpkg.getData(), outpkg.length());
        re_code = pRdsContext->err;
        if (REDIS_OK != re_code)
        {
            if (REDIS_ERR_IO == re_code)
            {
                re_code = errno;
            }
            ret = false;
            LOG(ERROR)("UpdateAllUserQgroupMsgInfo error, err code:%d", re_code);
        }

        if(NULL != reply)
        {
            freeReplyObject(reply);
            reply = NULL;
        }
        else // 表示出现重大问题，需要断连接
        {
            connect_fail();
            ret = false;
            LOG(ERROR)("UpdateAllUserQgroupMsgInfo, write redis error. disconnect redis server.");
            break;
        }

        itor++;
    }

    return ret;
}

bool RedisClient::RemoveUserUcExpired(uint32_t unUid, uint64_t unUserUc, const string &strKey, 
                                       string strBizPart/* = BIZ_QGROUP*/)
{
    if( strKey.empty() )
    {
        LOG(ERROR)("RemoveUserUcExpired param_key is null. uid:%u, user_uc:%lu, bizpart:%s",
            unUid, unUserUc, strBizPart.c_str());

        return false;
    }

    bool ret = true;
    TimeCounterAssistant tca("visit redis, RemoveUserUcExpired");
    redisReply* reply = NULL;
    if(!bConnected_)
    {
        return false;
    }
    // select db
    if (!SelectRedisDb(unUid, MSG_REGION))
    {
        return false;
    }

    reply = (redisReply*)redisCommand(pRdsContext, "HDEL %s %lu", strKey.c_str(), unUserUc);
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        LOG(ERROR)("RemoveUserUcExpired error, err code:%d", re_code);
    }

    if(NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    else // 表示出现重大问题，需要断连接
    {
        connect_fail();
        ret = false;
        LOG(ERROR)("UpdateAllUserQgroupMsgInfo, write redis error. disconnect redis server.");
    }
    return ret;
}

const char *RedisClient::GetChatMsgIdKey(uint64_t hashid)
{
    static char keystr[TMP_STR_LENGTH] = {0};
    memset(keystr, 0, TMP_STR_LENGTH);
    int hit = 0;
    hit = (hashid % ID_BUCKET)+1; // 1~256
    sprintf(keystr, "%s:%d", CHAT_MSG_ID, hit);
    return keystr;
}

const char *RedisClient::GetUserUcKey(uint64_t id)
{
    static char keystr[TMP_STR_LENGTH] = {0};
    memset(keystr, 0, TMP_STR_LENGTH);
    sprintf(keystr, "%s", USER_UC);
    return keystr;
}


// TODO: NEED TO 

/*   R E D I S   S E S S I O N .   G E T   C H A T   M S G   K E Y   */
/*-------------------------------------------------------------------------
    获取key，多线程不安全，不可重入，仅限单线程
-------------------------------------------------------------------------*/
const char *RedisClient::GetChatMsgKey(uint32_t uid, uint64_t msg_id, uint8_t flag)
{
    static char keystr[TMP_STR_LENGTH] = {0};
    memset(keystr, 0, TMP_STR_LENGTH);
    switch (flag)
        {
        // 
        case SINGLE_CHAT: // 1
            sprintf(keystr, "UID_MSGID:%u_%lu", uid, msg_id);
            break;
        // 群
        case QGROUP_CHAT: // 2
            sprintf(keystr, "QGROUP_MSGID:%u_%lu", uid, msg_id);
            break;
        // 多人
        case NGROUP_CHAT: // 3
            sprintf(keystr, "NGROUP_MSGID:%u_%lu", uid, msg_id);
            break;
        default: // UNDEFINED_CHAT = 0
            sprintf(keystr, "ERROR_UID:%u", uid);
            break;
        }
    return keystr;
}


/*   R E D I S   S E S S I O N .   A D D   U I D   C L I   M S G   I D   */
/*-------------------------------------------------------------------------
 *向redis写入消息id，用于消息去重
 * 超时时间
 * format:
 *     key    ->     value
 * UID_MSGID:%u_%lu   uid
-------------------------------------------------------------------------*/
int RedisClient::AddUidCliMsgId(uint32_t uid, uint64_t msg_id, uint8_t flag)
{
    int ret = 0;
    // pc上来的消息不做去重处理
    if (0 == msg_id)
    {
        return ret;
    }
    // select db, 没错，去重的消息id是放在MSG_REGION中的
    if (!SelectRedisDb(uid, MSG_REGION))
    {
        LOG(ERROR)("select redis db failed.");
        return -1;
    }
    TimeCounterAssistant tca("visit redis, AddUidCliMsgId");
    redisReply* reply = NULL;
    if(!bConnected_)
    {
        ret = -1;
        return ret;
    }

    uint32_t expire_time = ServerConfig::Instance()->rds_expire_time_;

    const char *keystr = GetChatMsgKey(uid, msg_id, flag);
    LOG(DEBUG)("insert client msg id, [%s]", keystr);

    // 不能直接传一个string，那样会报错的
    // 添加，设置过期时间
    reply = (redisReply*)redisCommand(pRdsContext, "SET %s %u EX %u", keystr, uid, expire_time);
    ret = pRdsContext->err;
    if (REDIS_OK != ret)
    {
        if (REDIS_ERR_IO == ret)
        {
            ret = errno;
        }
        LOG(ERROR)("RemoveUserUcExpired error, err code:%d", ret);
    }

    if(NULL == reply)
    {
        connect_fail();
        LOG(ERROR)("AddUidCliMsgId, failed");
        ret = -1;
        return ret;
    }
    else
    {
        if(reply->type == REDIS_REPLY_STATUS)
        {
            LOG(DEBUG)("AddUidCliMsgId, succeed, (string) %s", reply->str);
        }
        else
        {
            if( reply->str != NULL )
				LOG(WARN)("unexpetded reply. reply type:%d , (err msg) %s", reply->type , reply->str);
			else
				LOG(WARN)("unexpetded reply. reply type:%d", reply->type);
        }
    }
    freeReplyObject(reply);
    reply = NULL;
    return ret;
}

/*   R E D I S   S E S S I O N .   C H E C K   U I D   C L I   M S G   I D   E X I S T   */
/*-------------------------------------------------------------------------
    0表示无重复，大于0表示重复数目，负数表示出错
-------------------------------------------------------------------------*/
int RedisClient::CheckUserMsgExist(uint32_t uid, uint64_t msg_id, uint8_t flag)
{
    int ret = 0;
    // pc上来的消息不做检测
    if (0 == msg_id)
    {
        return ret;
    }
    TimeCounterAssistant tca("visit redis, CheckUserMsgExist");
    redisReply* reply = NULL;
    if(!bConnected_)
    {
        ret = -1;
        return ret;
    }
    // select db, 没错，去重的消息id是放在MSG_REGION中的
    if (!SelectRedisDb(uid, MSG_REGION))
    {
        return -1;
    }

    const char *keystr = GetChatMsgKey(uid, msg_id, flag);
    reply = (redisReply*)redisCommand(pRdsContext, "EXISTS %s", keystr);
    ret = pRdsContext->err;
    if (REDIS_OK != ret)
    {
        if (REDIS_ERR_IO == ret)
        {
            ret = errno;
        }
        LOG(ERROR)("CheckUserMsgExist error, err code:%d", ret);
        if (NULL != reply)
        {
            freeReplyObject(reply);
            reply = NULL;
        }
        return ret;
    }
    if(NULL == reply)
    {
        connect_fail();
        LOG(ERROR)("CheckUserMsgExist, failed");
        ret = -1;
        return ret;
    }
    else
    {
        if(reply->type == REDIS_REPLY_INTEGER)
        {
            ret = reply->integer;
            if (ret > 0)
            {
                LOG(WARN)("CheckUserMsgExist, msg is repeated, fuid:%u, msg_id:%lu, check key:%s, num:%llu"
                    , uid, msg_id, keystr, reply->integer);
            }
        }
        else
        {
            LOG(WARN)("unexpetded reply. reply type:%d, fuid:%u, msg_id:%lu"
                , reply->type, uid, msg_id);
        }
    }
    freeReplyObject(reply);
    reply = NULL;

    return ret;
}

/*   R E D I S   S E S S I O N .   D I S M I S S   G R O U P   */
/*-------------------------------------------------------------------------
    解散群或多人
-------------------------------------------------------------------------*/
bool RedisClient::DismissGroup(uint32_t group_id, const string &strBizPart)
{
    bool ret = true;
    TimeCounterAssistant tca("visit redis, DismissGroup");
    // select db
    if (!SelectRedisDb(group_id, MSG_REGION))
    {
        LOG(ERROR)("DismissGroup, select redis db failed.");
        return false;
    }
    string strMsgKey = getQgroupMsgKey(group_id, strBizPart);
    int ret_value = DelByKey(strMsgKey.c_str());
    ret = (0 == ret_value) ? true : false;
    if (ret)
    {
        LOG(INFO)("DismissGroup, succeed. delete group id:%u", group_id);
    }
    else
    {
        LOG(ERROR)("DismissGroup, failed. delete group id:%u", group_id);
    }
    return ret;
}

/*   R E D I S   S E S S I O N .   D E L   B Y   K E Y   */
/*-------------------------------------------------------------------------
    删除by key
-------------------------------------------------------------------------*/
int RedisClient::DelByKey(const char *key)
{
    int ret = 0;
    redisReply* reply = NULL;
    if(!bConnected_)
    {
        ret = -1;
        return ret;
    }

    reply = (redisReply*)redisCommand(pRdsContext, "DEL %s", key);
    ret = pRdsContext->err;
    if (REDIS_OK != ret)
    {
        if (REDIS_ERR_IO == ret)
        {
            ret = errno;
        }
        LOG(ERROR)("DelByKey error, err code:%d", ret);
        if (NULL != reply)
        {
            freeReplyObject(reply);
            reply = NULL;
        }
        return ret;
    }
    if(reply->type == REDIS_REPLY_INTEGER)
    {
        LOG(INFO)("DelByKey, del key:%s, del num:%lld"
            , key, reply->integer);
    }
    else
    {
        LOG(WARN)("unexpetded reply. reply type:%d, msg:%s"
            , reply->type, reply->str);
    }
    freeReplyObject(reply);
    reply = NULL;

    return ret;
}


bool RedisClient::UpdateGroupIdCurMsgId(uint32_t group_id, uint64_t msg_id, const string &bizpart)
{
    bool ret = true;
    ONCE_LOOP_ENTER
    redisReply* reply = NULL;
    if(!bConnected_)
    {
        ret = false;
        break;
    }

    TimeCounterAssistant tca("visit redis, UpdateGroupIdCurMsgId");
    // select 0 db
    reply = (redisReply*)redisCommand(pRdsContext, "SELECT 0");
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        ret = false;
        LOG(ERROR)("SelectRedisDb redis error, db index:0, err code:%d", re_code);
    }
    if(NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis occur fatal error, disconnect redis server.");
        connect_fail();
        break;
    }

    string key = getGroupIdMsgIdKey(group_id, bizpart);
    reply = (redisReply*)redisCommand(pRdsContext, "SET %s %lu", key.c_str(), msg_id);
    ret = pRdsContext->err;
    if (REDIS_OK != ret)
    {
        if (REDIS_ERR_IO == ret)
        {
            ret = errno;
        }
        LOG(ERROR)("UpdateGroupIdCurMsgId error, err code:%d", ret);
        if (NULL != reply)
        {
            freeReplyObject(reply);
            reply = NULL;
        }
        break;
    }
    if(reply->type == REDIS_REPLY_STATUS)
    {
        LOG(INFO)("UpdateGroupIdCurMsgId, key:%s, msgid:%lu, msg:%s"
            , key.c_str(), msg_id, reply->str);
    }
    else
    {
        LOG(WARN)("unexpetded reply. reply type:%d, msg:%s"
            , reply->type, reply->str);
    }
    freeReplyObject(reply);
    reply = NULL;
    ONCE_LOOP_LEAVE
    return ret;

}

bool RedisClient::BatchGetGroupIdCurMsgId(const GroupidList &groupid_list, GroupidMsgid &groupid_msgid, const string &bizpart)
{
    bool ret = true;
    ONCE_LOOP_ENTER
    redisReply* reply = NULL;
    if(!bConnected_)
    {
        ret = false;
        break;
    }

    TimeCounterAssistant tca("visit redis, BatchGetGroupIdCurMsgId");
    // select 0 db
    reply = (redisReply*)redisCommand(pRdsContext, "SELECT 0");
    int re_code = pRdsContext->err;
    if (REDIS_OK != re_code)
    {
        if (REDIS_ERR_IO == re_code)
        {
            re_code = errno;
        }
        ret = false;
        LOG(ERROR)("SelectRedisDb redis error, db index:0, err code:%d", re_code);
    }
    if(NULL != reply)
    {
        freeReplyObject(reply);
        reply = NULL;
    }
    // reply为NULL表示出现重大问题，与redis断连接
    else
    {
        LOG(ERROR)("visit redis occur fatal error, disconnect redis server.");
        connect_fail();
        break;
    }

    GroupidList::const_iterator iter = groupid_list.begin();
    GroupidList::const_iterator iter_end = groupid_list.end();
    for (; iter != iter_end; ++iter)
    {
        string key = getGroupIdMsgIdKey(*iter, bizpart);
        LOG(DEBUG)("key: %s\n", key.c_str());
        redisAppendCommand(pRdsContext, "GET %s", key.c_str());
    }

    // fetch result
    int ret_value = 0;
    groupid_msgid.clear();
    iter = groupid_list.begin();
    for (; iter != iter_end; ++iter)
    {
        ret_value = redisGetReply(pRdsContext, (void **)(&reply));
        //ret_value = redisGetReply(pRdsContext, (void **)&reply));
        if (REDIS_OK == ret_value)
        {
            if (reply->str != NULL)
            {
                groupid_msgid[*iter] = atoll(reply->str);
                LOG(DEBUG)("get group cur msg id, qid:%u, msgid:%lu", *iter, groupid_msgid[*iter]);
            }
            else
            {
                LOG(DEBUG)("get group cur msg id, qid:%u, reply value is null.", *iter);
            }
        }
        else
        {
            LOG(ERROR)("BatchGetGroupIdCurMsgId, redis error, break, ret:%d", ret_value);
            break;
        }
        freeReplyObject(reply);
        reply = NULL;
    }
    ONCE_LOOP_LEAVE
    return ret;
}


