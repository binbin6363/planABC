/**
 * @filedesc: 
 * user.cpp, user struct
 * @author: 
 *  bbwang
 * @date: 
 *  2015/4/14 14:02:59
 * @modify:
 *
**/
#include "user.h"
#include <algorithm>
#include "constants.h"
#include "log.h"
#include "xml_parser.h"

using namespace std;
using namespace utils;
using namespace common;
using namespace object_pool;

uint32_t userNum = 0;
// ================================
// User struct
// ================================

uint32_t User::check_interval;
uint32_t User::user_timeout;
uint32_t User::user_clean;

void User::set_check_interval(uint32_t interval)
{
    LOG(INFO)("set check user time out interval is %us", interval);
    check_interval = interval;
}

void User::set_user_timeout(uint32_t timeout)
{
    LOG(INFO)("set user time out is %us", timeout);
    user_timeout = timeout;
}

void User::set_user_clean(uint32_t clean_time)
{
    LOG(INFO)("set user clean time is %us", clean_time);
    user_clean = clean_time;
}

User::User()
    : uid(0)
    , device_type(DEV_TYPE_INVALID)
    , status(FRONT_ONLINE_STATUS)
	, cond_id(0)
	, client_ver(0)
	, device_id("")
{
    ++userNum;
    INIT_LIST_HEAD(&uid_item);
    INIT_LIST_HEAD(&timeout_item);
    LOG(DEBUG)("new User, num:%u", userNum);
}

User::~User()
{
    LOG(INFO)("[status change] delete user, [%s]", print());
    list_del(&uid_item);
    list_del(&timeout_item);
    --userNum;
    LOG(DEBUG)("del User, num:%u", userNum);
}

const char *User::print()
{
    static char info[MAX_STACK_STR_LENGTH];
    snprintf(info, MAX_STACK_STR_LENGTH
        , "uid:%u, dev type:%u, status:%u, cond_id:%u, version:%u, device id:%s"
        , uid, device_type, status, cond_id, client_ver, device_id.c_str());
    
    return info;
}

const char *User::print() const
{
    static char info[MAX_STACK_STR_LENGTH];
    snprintf(info, MAX_STACK_STR_LENGTH
        , "uid:%u, dev type:%u, status:%u, cond_id:%u, version:%u, device id:%s"
        , uid, device_type, status, cond_id, client_ver, device_id.c_str());
    
    return info;
}

void User::update_status(uint32_t sta)
{
    LOG(INFO)("[status change] [%s]", print());
    status = sta;
}


void User::update_time()
{
    alive_time = time(NULL);
}

uint32_t User::last_time()
{
    return alive_time; 
}

UserMgr::UserMgr()
{
    LOG(INFO)("init user manager.");
    for (int i = 0; i < DEF_MAX_HASH_NUM; ++i)
    {
        INIT_LIST_HEAD(&(user_uid_list[i]));
    }
    INIT_LIST_HEAD(&front_user_timeout_list);
    INIT_LIST_HEAD(&backend_user_timeout_list);
    
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(10), NULL);
}

UserMgr::~UserMgr()
{
}

UserMgr &UserMgr::Instance()
{
    static UserMgr inst;
    return inst;
}

User *UserMgr::new_user()
{
    return new User;
}

void UserMgr::del_user(User *user)
{
	//list_del(&(user->timeout_item));
    SAFE_DELETE(user);
}

int UserMgr::add_user(User *user)
{
    // add to uid_bucket_
    uint32_t unIndex = hashfn(user->uid, DEF_MAX_HASH_NUM);
	list_add_tail(&(user->uid_item), &(user_uid_list[unIndex]));
    LOG(DEBUG)("insert uid bucket, [%s]", user->print());
    
    update_time(user);
    return 0;
}


// 移动到后台在线队列
int UserMgr::move_to_backend_queue(User *user)
{
    if (NULL == user)
    {
        LOG(ERROR)("move_to_backend_queue, user is null.");
        return -1;
    }
	list_del(&(user->timeout_item));
	list_add_tail(&(user->timeout_item), &backend_user_timeout_list);
    return 0;
}


// 移动到前台在线队列
int UserMgr::move_to_front_queue(User *user)
{
    if (NULL == user)
    {
        LOG(ERROR)("move_to_front_queue, user is null.");
        return -1;
    }
	list_del(&(user->timeout_item));
	list_add_tail(&(user->timeout_item), &front_user_timeout_list);
    return 0;
}

void UserMgr::update_time(User *user)
{
    if (NULL == user)
    {
        LOG(ERROR)("update_time, user is null.");
        return ;
    }
    user->update_time();
    user->update_status(FRONT_ONLINE_STATUS);
    int ret = move_to_front_queue(user);
    LOG(DEBUG)("update user active time, ret:%d", ret);
}


User *UserMgr::get_user_by_uid(uint32_t uid)
{
    User *user = NULL;
	list_head *pPos = NULL;
	uint32_t unIndex = hashfn(uid, DEF_MAX_HASH_NUM);
	list_head* pHead = &user_uid_list[unIndex];
	list_for_each(pPos, pHead)
	{
	    if (pPos == NULL)
        {
            LOG(ERROR)("uid list is empty.");
            break;
        }   
		User* p = list_entry(pPos, User, uid_item);
		if (p->uid == uid)
		{
			user = p;
		}
	}
    LOG(DEBUG)("try find user by uid, uid:%u, bucket index:%u", uid, unIndex);  
    
    return user;
}

void UserMgr::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;
    LOG(DEBUG)("enter UserMgr handle_timeout");

    check_front_user_timeout();

    check_backend_user_timeout();
    
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(User::check_interval), NULL);

}


// 检测前台在线的用户超时
int UserMgr::check_front_user_timeout()
{
    uint32_t frontend_timeout = User::user_timeout;
    uint32_t curTime = time(NULL);
    uint32_t iCount = 0;
    list_head* pPos;
    list_head* pHead = &front_user_timeout_list;
    for( pPos = pHead->next; pPos != pHead; )
    {
        if (NULL == pPos) {
            break;
        }
        list_head *pCurPos = pPos;
        pPos = pPos->next;

        User* pUser = list_entry(pCurPos, User, timeout_item);
        if(pUser)
        {
            if (curTime > frontend_timeout + pUser->last_time()) {
                pUser->update_status(BACKEND_ONLINE_STATUS);
                move_to_backend_queue(pUser);
                LOG(INFO)("[update] user frontkend online timeout, frontkend online => backend online, cur_time:%u, user info:%s"
                    , curTime, pUser->print());
                ++iCount;
            }
            else
            {
                break;
            }
        }
    }
    if( iCount > 0 )
    {
        LOG(INFO)("[sum] sum timeout user num:%u.", iCount);
    }
    return 0;
}


// 检测后台在线的用户超时
int UserMgr::check_backend_user_timeout()
{
    uint32_t backend_timeout = User::user_clean * (60 * 60 * 24);
    uint32_t curTime = time(NULL);
    uint32_t iCount = 0;
    list_head* pPos;
    list_head* pHead = &backend_user_timeout_list;
    for( pPos = pHead->next; pPos != pHead; )
    {
        if (NULL == pPos) {
            break;
        }
        list_head *pCurPos = pPos;
        pPos = pPos->next;

        User* pUser = list_entry(pCurPos, User, timeout_item);
        if(pUser)
        {
            if(curTime > backend_timeout + pUser->last_time()) {
                LOG(INFO)("[clean] user backend online timeout and do auto clean, backend online => offline, cur_time:%u, user info:%s"
                    , curTime, pUser->print());
                del_user(pUser);
                iCount ++;
            }
            else
            {
                break;
            }
        }
    }
    if( iCount > 0 )
    {
        LOG(INFO)("[sum] sum clean user num:%u.", iCount);
    }
    return 0;
}


