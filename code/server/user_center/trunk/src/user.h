/**
 * @filedesc: 
 * user.h, user struct
 * @author: 
 *  bbwang
 * @date: 
 *  2015/4/14 14:02:59
 * @modify:
 *
**/
#ifndef USER_H_
#define USER_H_
#include <list>
#include <vector>
#include <map>
#include "list_hash.h"
#include "constants.h"
#include "timeoutmanager.h"

using namespace std;
using namespace common;
using namespace object_pool;


// 用户
class User
{
public:

	// 超时检测间隔,单位秒
	static uint32_t check_interval;
	// 超时时间,单位秒
	static uint32_t user_timeout;
	// 清理时间,单位天
	static uint32_t user_clean;
	
	list_head uid_item;	     // 按照uid排序
    list_head timeout_item;  // 检测超时的项
    
    uint32_t uid;
	uint8_t  device_type;
	uint8_t  status; // 前台在线，后台在线，不在线
	uint16_t cond_id;
	uint32_t client_ver;
	string   device_id;
	uint32_t alive_time;
	

	static void set_check_interval(uint32_t interval);
	static void set_user_timeout(uint32_t timeout);
	static void set_user_clean(uint32_t clean_time);
    const char *print();
    const char *print() const;
	void update_status(uint32_t sta);

	friend class UserMgr;
private:
// method
    User();
    // noncopy
    User(const User &);
    ~User();
    User &operator=(const User &);

	void update_time();
	uint32_t last_time();
  
};


// 单例
class UserMgr : public TimeoutEvent
{
public:
	~UserMgr();

	static UserMgr &Instance();
	User *new_user();
	void del_user(User *user);

	int add_user(User *user);
	void update_time(User *user);
	User *get_user_by_uid(uint32_t uid);

    virtual void handle_timeout(int id,void *userData);

private:
	int move_to_backend_queue(User *user);
	int move_to_front_queue(User *user);
    int check_front_user_timeout();
    int check_backend_user_timeout();


private:
	UserMgr();
    // noncopy
    UserMgr(const UserMgr &);
    UserMgr &operator=(const UserMgr &);

	// 按照uid排序的列表
	list_head user_uid_list[DEF_MAX_HASH_NUM];
	// 前台在线超时检测的列表
	list_head front_user_timeout_list;
	// 后台在线超时检测的列表
	list_head backend_user_timeout_list;
};



#endif //USER_H_

