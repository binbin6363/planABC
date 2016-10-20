/*
 * some constant variable define
 *
*/
#ifndef CONSTANTS_H_
#define CONSTANTS_H_
#include <stdint.h>
#include <string>
#include <map>
#include <vector>

using namespace std;

namespace common
{

#define QUERY_DB 'Q'
#define CALL_PROCEDURE 'P'
#define RESET_STAT 'R'
#define STAT 'S'

typedef std::vector< string > StrVec;
typedef StrVec::const_iterator StrVCIter;
typedef StrVec::iterator StrVIter;

typedef std::map<string, string> StrMap;
typedef StrMap::const_iterator   StrMapCIter;
typedef StrMap::iterator         StrMapIter;



enum ENUM_OPRET
{
	RET_OK = 0x0, 
	RET_ERROR = 1001, 
	RET_NOTEXIST,
	RET_DBTIMEOUT,
	RET_STMTINITERROR,
	RET_STMTPREPAREERROR,
	RET_PARAMERROR,
	RET_MEMERROR,
	RET_STMTBINDERROR,
	RET_STMTEXECERROR,
	RET_STMTMEMEEROR,
	RET_STMTSTOREERROR,
	RET_INPUTPARAMERROR
};

    static const uint32_t BOY                                   = 0;
    static const uint32_t GIRL                                  = 1;
    static const uint32_t ONLINE                                = 1;

    static const uint32_t TASK_INVALID                          = 0;
    static const uint32_t TASK_VALID                            = 1;
    static const uint32_t REDIS_SENDBUFF_MAX                    = (1*1024*1024);
    static const uint32_t MIN_GLOBAL_USER_ID                    = 500000;
    static const uint32_t ACTIVATED_ACCOUNT                     = 1;
    static const uint32_t INACTIVATED_ACCOUNT                   = 0;

	
    const char *const DEFAULT_USER_NAME                       = "游客";
    const char *const DEFAULT_AVATAR_LINK                     = "www.baidu.com";
	const char *const DEFAULT_INCOME                          = "0.00";
	const char *const UID_PREFIX                              = "UID_TASK_LIST_";
	const char *const TASK_LIMIT_PREFIX                       = "TASK_LIMIT_";
	const char *const GLOBAL_USER_ID                          = "GLOBAL_USER_ID";

	
    const static string PASSWD_SALT                           = "dsp.";
    const static uint32_t VERSION_0                           = 0;
    const static uint32_t VERSION_1                           = 1;

	enum kSyncType
	{
		SYNC_FUTUTE_TASK = 1, // 同步新任务
		SYNC_HISTORY_TASK,    // 同步历史任务
		SYNC_INCOME,          // 同步收入
	};
    static const uint32_t SYNC_NUM_LIMIT                       = 10;
    static const uint32_t LEVELDB_VERSION                      = 1;

    // continue flag
    static const uint32_t NEED_CONTINUE                        = 1;
    static const uint32_t NOT_CONTINUE                         = 0;

	
// ========================================================================
// 常整型
// ========================================================================
    static const uint32_t CLIENT_VERSION_5000                  = 5000;
    static const uint32_t CLIENT_VERSION_5035                  = 5035;
    static const uint32_t CLIENT_VERSION_5100                  = 5100;
    static const uint32_t CLIENT_VERSION_5172                   = 5172;//禁用群会话使用该版本判断
    static const uint32_t CLIENT_VERSION_5225                   = 5225;//在故障模式下，取uc时低于该客户端版本的返回uc，否则返回-1
    static const uint32_t CLIENT_VERSION_6030                  = 6030;

    static const uint32_t UID_MIN_VALUE                        = 500000;
    static const uint32_t GET_USER_INFO_KEY_MAX_NUM            = 20;
    static const uint32_t MAX_SEND_BUFF_LEN                    = (1*1024*1024);
    static const uint32_t CLIENT_PKG_ENCRYPTED_MAX_LEN         = 5*1024;
	static const uint32_t MSGCENTER_OTHER_NET_PKG_MAX_LEN      = 100000;
    static const uint32_t MAX_STACK_STR_LENGTH                 = 512;
    static const int64_t  MAX_INT64_VALUE                      = 0X7FFFFFFFFFFFFFFFLL;
    static const int64_t  MIN_INT64_VALUE                      = 0X8000000000000000LL;
    static const uint64_t MAX_UINT64_VALUE                     = 0XFFFFFFFFFFFFFFFFULL;
    static const uint64_t MIN_UINT64_VALUE                     = 0X00LL;
    static const uint32_t _1KB                                 = (1*1024);
    static const uint32_t _1MB                                 = (1024*_1KB);
    static const uint32_t _1GB                                 = (1024*_1MB);
    
    static const uint32_t MAX_REWARD_NUM                       = 10; // 最大奖励金额，10元

    // endflag
    static const uint32_t END                                  = 1;
    static const uint32_t NOT_END                              = 0;
    // limit num
    static const uint32_t MAX_LIMIT_NUM                        = 256; // 协议中num的最大取值
    static const uint32_t MAX_SYNC_MSG_NUM                     = 20; // 一次同步消息的最大条数

    // contactor data status 
    const uint32_t CONTACTOR_STATUA_VALID                     =  1; // 数据有效
    const uint32_t CONTACTOR_STATUA_INVALID                   =  0; // 数据无效

    // changestatus type
    static const uint32_t STATUS_CHANGE_TYPE_LOGIN            = 0;
    static const uint32_t STATUS_CHANGE_TYPE_ONLINE           = 1;

    //groupd const var
    static const uint32_t USER_MAX_ROOM_NUM                   = 500;
    static const uint32_t SHELL_COULD_NOT_BE_EXECUTED         = 127;
    // comm var
    static const uint32_t ONE_SEC_HAVE_USEC                   = 1000000;
    static const uint32_t FORCE_TIME_OUT_SEC                  = 20;
    static const uint32_t CONNECTED_FLAG                      = 2;
    static const uint32_t INVITED_DEFAULT_STATUS              = 0;
    static const uint32_t INVITED_DEFAULT_SESSION_ID          = 0;

    // contactor relationship
    static const uint32_t CONTACTOR_RELATION_WAIT            = 0X41;
    static const uint32_t CONTACTOR_RELATION_BE_REFUSED      = 0X42;
    static const uint32_t CONTACTOR_RELATION_DOUBLE          = 0X4D;
    static const uint32_t CONTACTOR_RELATION_BE_DELETED      = 0X4E;
    static const uint32_t CONTACTOR_RELATION_DELETE          = 0X50;

    static const uint32_t RET_CONTACTOR_ALREADY_EXIST        = 1;
     
    // login status
    static const uint32_t CLIENT_OFFLINE_STATUS              = 0;
    static const uint32_t CLIENT_LOGIN_ONLINE_STATUS         = 1;

    // pkg_padding = xtea_max_padding(16) + cond_client_head(8=28-20)
    // ensure max_pkg_padding > pkg_padding
    // max_pkg_padding is encrypt_algorithm dependent and pkg_head dependent
    static const uint32_t MAX_PKG_PADDING                    = 100;
    static const uint32_t MAX_NOTICE_PKG_PADDING             = 1024;
    static const uint32_t MIN_MSGPKG_LEN                     = 4;    //消息包最小长度
    // netpkg max size
    static const uint32_t MSG_NET_PKG_MAX_LEN                = 4400;
    static const uint32_t ID_NET_PKG_MAX_LEN                 = 4400;
    static const uint32_t DB_NET_PKG_MAX_LEN                 = 1000000;  //之前是500k，而dbp发包包长最大1m, 如果超过500k就会断连接。故调大其数值。
    //ps:实际可以调成1024*1024。
    static const uint32_t IMD_COND_NET_PKG_MAX_LEN           = 4400;
    static const uint32_t IMD_IMD_NET_PKG_MAX_LEN            = 4400;
    static const uint32_t IMD_DB_NET_PKG_MAX_LEN             = 4400;
    static const uint32_t COND_DB_NET_PKG_MAX_LEN            = 4400;
    static const uint32_t GROUPD_COND_NET_PKG_MAX_LEN        = 4400;
    static const uint32_t QGROUPD_COND_NET_PKG_MAX_LEN       = 4400;
    static const uint32_t IMD_GROUPD_NET_PKG_MAX_LEN         = 4400;
    static const uint32_t IMD_NGROUPD_NET_PKG_MAX_LEN        = 4400;
    static const uint32_t IMD_QGROUPD_NET_PKG_MAX_LEN        = 20*1024;//一个群消息加上群的所有用户列表 
    static const uint32_t IMD_WEBAGENTD_NET_PKG_MAX_LEN      = 1024*10;    // 4400
    static const uint32_t WEBAGENTD_WEB_NET_PKG_MAX_LEN      = 1024*10; //4400
    static const uint32_t WEBAGENTD_IMD_NET_PKG_MAX_LEN      = 4400;
    // todo: this value should be 1024*4, change it to 1024 for test!!!
    static const uint32_t MOBILED_CLIENT_PKG_MAX_LEN         = 65536; // SYNC, reply packet max length
    static const uint32_t MOBILED_CLIENT_PKG_ENCRYPTED_MAX_LEN = MOBILED_CLIENT_PKG_MAX_LEN+MAX_PKG_PADDING;

    static const uint32_t MOBILED_NOTICE_PKG_MAX_LEN         = 31744;
    static const uint32_t MOBILED_NOTICE_PKG_ENCRYPTED_MAX_LEN = MOBILED_NOTICE_PKG_MAX_LEN+MAX_NOTICE_PKG_PADDING;

    static const uint32_t MSGCENTER_COND_NET_PKG_MAX_LEN     = 4500;
    static const uint32_t DB_MOBILED_PKG_MAX_LEN             = 1*1024*1024;

    static const uint32_t MOBILED_COND_NET_PKG_MAX_LEN       = 1024 * 4;

    static const uint32_t COND_CLIENT_NET_ENCRYPT_PKG_MAX_LEN = IMD_COND_NET_PKG_MAX_LEN + MAX_PKG_PADDING;
    static const uint32_t COND_CLIENT_UDP_NET_PKG_MAX_LEN    = 1400;

    // Data length
    static const uint32_t CLIENT_PASSWD_LENGTH               = 16;
    static const uint32_t CLIENT_TOKEN_LENGTH                = 256;//token长度
    static const uint32_t CLIENT_SESSIONID_LENGTH            = 128;//token长度

    // user status change type
    static const uint32_t NGROUP_USER_STATUS_IS_OFFLINE        = 0;
    static const uint32_t NGROUP_USER_STATUS_IS_ONLINE         = 1;
    static const uint32_t NGROUP_USER_STATUS_IS_BUSY           = 3;
    static const uint32_t NGROUP_USER_STATUS_IS_RIGHTBACK      = 4;
    static const uint32_t NGROUP_USER_STATUS_IS_LEAVE          = 5;
    //ngroup user type
    static const uint32_t NGROUP_DELETED_USER_TYPE             = 0;
    static const uint32_t NGROUP_INVITED_USER_TYPE             = 1;
    static const uint32_t NGROUP_USER_TYPE                     = 2;
    static const uint32_t NGROUP_ADMIN_TYPE                    = 3;
    // ngroupd
    static const uint32_t MAX_NGROUP_INVITE_ONCE_NUM           = 100;
    static const uint32_t MAX_GET_NGROUP_OFFLINE_NUM           = 50;
    static const uint32_t MAX_GET_NGROUP_OFFLINE_CHATMSG_NUM   = 50; //for test old 50
    // ngroup flag
    static const uint32_t NGROUP_FLAG_NORMAL                   = 1;
    static const uint32_t NGROUP_FLAG_DELETE                   = 0;
    //ngroup init type
    static const uint32_t NGROUP_INIT_OK                       = 1;
    static const uint32_t NGROUP_NO_INIT                       = 0;
    static const uint32_t NGROUP_INITING                       = 2;
    // ngroup offline num
    static const uint32_t MAX_DELETE_NGROUP_OFFLINE_NUM        = 50;
    // ngroup chat msg type
    static const uint32_t NGROUP_GENERAL_MSG                   = 0;
    static const uint32_t NGROUP_REVERSE_MSG                   = 1;

    // msg type
    static const uint32_t NGROUP_INVITE_USER_JOIN_MSG_TYPE     = 1;
    static const uint32_t NGROUP_USER_REJECT_INVITE_MSG_TYPE   = 2;
    static const uint32_t NGROUP_USER_AGREE_INVITE_MSG_TYPE    = 3;
    static const uint32_t NGROUP_DESTROY_MSG_TYPE              = 7;
    static const uint32_t INVITE_USER_JOIN_MSG_TYPE            = 1;
    static const uint32_t USER_REJECT_INVITE_MSG_TYPE          = 2;
    static const uint32_t USER_AGREE_INVITE_MSG_TYPE           = 3;
    static const uint32_t QGROUP_DESTROY_MSG_TYPE              = 7;
    static const uint32_t QGROUP_USER_EXIT_NOTICE_QHOST_MSG_TYPE = 8;
    static const uint32_t JOIN_QGROUP_MSG_TYPR = 9;
    static const uint32_t QGROUP_OWNER_AGREE_JOIN_MSG_TYPE    = 10;
    static const uint32_t QGROUP_OWNER_REJECT_JOIN_MSG_TYPE   = 11;
    static const uint32_t QGROUP_OWNER_SET_MANAGER_MSG_TYPE   = 12;
    static const uint32_t QGROUP_OWNER_CANCEL_MANAGER_MSG_TYPE= 13;
    static const uint32_t QGROUP_OWNER_TRANSFER_QGROUP_MSG_TYPE= 14;
    static const uint32_t QGROUP_ADMIN_KICK_USER_NOTICE_ADMIN_MSG_TYPE = 15;
    static const uint32_t LIGHT_INVITE_USER_JOIN_MSG_TYPE     = 17;
    static const uint32_t QGROUP_ADMIN_KICK_USER_NOTICE       = 18;

    // user status change type
    static const uint32_t QGROUP_USER_STATUS_IS_OFFLINE        = 0;
    static const uint32_t QGROUP_USER_STATUS_IS_ONLINE         = 1;
    static const uint32_t QGROUP_USER_STATUS_IS_BUSY           = 3;
    static const uint32_t QGROUP_USER_STATUS_IS_RIGHTBACK      = 4;
    static const uint32_t QGROUP_USER_STATUS_IS_LEAVE          = 5;
    //qgroup user type
    static const uint32_t QGROUP_DELETED_USER_TYPE             = 0;
    static const uint32_t QGROUP_INVITED_USER_TYPE             = 1;
    static const uint32_t QGROUP_USER_TYPE                     = 2;
    static const uint32_t QGROUP_ADMIN_TYPE                    = 3;
    // qgroupd
    static const uint32_t MAX_QGROUP_INVITE_ONCE_NUM           = 100;
    static const uint32_t MAX_GET_QGROUP_OFFLINE_NUM           = 50;
    static const uint32_t MAX_GET_QGROUP_OFFLINE_CHATMSG_NUM   = 50; //for test old 50
    // qgroup flag
    static const uint32_t QGROUP_FLAG_NORMAL                   = 1;
    static const uint32_t QGROUP_FLAG_DELETE                   = 0;
    //qgroup init type
    static const uint32_t QGROUP_INIT_OK                       = 1;
    static const uint32_t QGROUP_NO_INIT                       = 0;
    static const uint32_t QGROUP_INITING                       = 2;
    // qgroup offline num
    static const uint32_t MAX_DELETE_QGROUP_OFFLINE_NUM        = 50;
    // qgroup chat msg type
    static const uint32_t QGROUP_GENERAL_MSG                   = 0;
    static const uint32_t QGROUP_REVERSE_MSG                   = 1;

    static const uint32_t LOGIN_TYPE_DEFAULT                   = 0;
    static const uint32_t LOGIN_TYPE_FORCE                     = 1;
    // leveldb constant
    const static uint32_t ONLY_ONE_MASTER                      = 1;


    // db table size
    static const uint32_t DB_TABLE_SIZE_SMALL                 = 16;
    static const uint32_t DB_TABLE_SIZE_BIG                   = 256;
    static const uint32_t MAX_COLLEAGUE_BASE_INFO_NUM         = 20;
    static const uint32_t MAX_EXTEND_MAIL_NUM                 = 20;

    // mask.dept
    const uint32_t MASK_DEPT_DEPT_ID        = 1 << 0;
    const uint32_t MASK_DEPT_PARENT_DEPT_ID = 1 << 1;
    const uint32_t MASK_DEPT_NAME           = 1 << 2;
    const uint32_t MASK_DEPT_DESP           = 1 << 3;
    const uint32_t MASK_DEPT_DEPT_UC        = 1 << 4;
    const uint32_t MASK_DEPT_FAX            = 1 << 5;
    const uint32_t MASK_DEPT_HIDE_DEPT_LIST = 1 << 6;
    const uint32_t MASK_DEPT_FIRST_CHILD    = 1 << 7;
    const uint32_t MASK_DEPT_NEXT_SIBLING   = 1 << 8;
    const uint32_t MASK_DEPT_FADDR          = 1 << 9;
    const uint32_t MASK_DEPT_FTEL           = 1 << 10;
    const uint32_t MASK_DEPT_FWEBSITE       = 1 << 11;
    // FOR DEPT USER MEM SORT ZHANGHONGQIANG 20110322
    const uint32_t MASK_DEPT_FIRST_USER     = 1 << 12;

    // mask.user
    const uint32_t MASK_USER_USER_ACCOUNT   = 1 << 0;
    const uint32_t MASK_USER_CORP_ACCOUNT   = 1 << 1;
    const uint32_t MASK_USER_NAME           = 1 << 2;
    const uint32_t MASK_USER_GENDER         = 1 << 3;
    const uint32_t MASK_USER_SIGN           = 1 << 4;
    const uint32_t MASK_USER_MOBILE         = 1 << 5;
    const uint32_t MASK_USER_EMAIL          = 1 << 6;
    const uint32_t MASK_USER_ROLE_ID        = 1 << 7;
    const uint32_t MASK_USER_HEAD_PIC       = 1 << 8;
    const uint32_t MASK_USER_PRIVACY_FLAG   = 1 << 9;
    const uint32_t MASK_USER_BIRTH          = 1 << 10;
    const uint32_t MASK_USER_POS            = 1 << 11;
    const uint32_t MASK_USER_TEL            = 1 << 12;
    const uint32_t MASK_USER_DESP           = 1 << 13;
    const uint32_t MASK_USER_HIDE_DEPT_LIST = 1 << 14;

    // mask.fieldgroup
    const uint32_t MASK_USER_INFO_CHANGE_NOTICE_FIELD = MASK_USER_NAME    |
        MASK_USER_GENDER  |
        MASK_USER_SIGN;

    // mask.user get contactor info
    const uint32_t MASK_GET_CONTACTOR_INFO_FIELD = MASK_USER_USER_ACCOUNT   |
        MASK_USER_CORP_ACCOUNT   |
        MASK_USER_NAME           |
        MASK_USER_GENDER         |
        MASK_USER_SIGN           |
        MASK_USER_MOBILE         |
        MASK_USER_EMAIL          |
        MASK_USER_ROLE_ID        |
        MASK_USER_HEAD_PIC       |
        MASK_USER_PRIVACY_FLAG   |
        MASK_USER_BIRTH          |
        MASK_USER_POS            |
        MASK_USER_TEL            |
        MASK_USER_DESP           |
        MASK_USER_HIDE_DEPT_LIST;

    // mask.user modify user info
    const uint32_t MASK_MODIFY_USER_INFO_FIELD = MASK_USER_NAME         |
        MASK_USER_GENDER       |
        MASK_USER_SIGN         |
        MASK_USER_MOBILE       |
        MASK_USER_EMAIL        |
        MASK_USER_HEAD_PIC     |
        MASK_USER_PRIVACY_FLAG |
        MASK_USER_BIRTH        |
        MASK_USER_POS          |
        MASK_USER_TEL          |
        MASK_USER_DESP;


// ========================================================================
// 常量字符串
// ========================================================================

    // db name
    const char *const DB_USER                            = "user";
    const char *const DB_THIRD                           = "third";
    const char *const DB_TASK                            = "task";
    const char *const DB_GLOBAL_ID                       = "global_id";

    const char *const TABLE_USER                         = "user";
    const char *const TABLE_THIRD_USER                   = "user "; // 此处故意躲一个空格区分上面的地址
    const char *const TABLE_USER_REG_LIST                = "db_user.t_reg_list";
    const char *const TABLE_CASH                         = "cash";
    const char *const TABLE_ID                           = "id";
    const char *const TABLE_TASK_LIST                    = "task_list";
    const char *const TABLE_TASK_INFO                    = "task_info";


// ========================================================================
// 常用宏定义
// ========================================================================
    // define single msg key store format, sgle_msglist_uid_syncpoint
    #ifndef SINGLE_MSG_PERFIX
    #define SINGLE_MSG_PERFIX "sglemsglist"
    #endif

    // define group msg key store format, grpmsglist_uid_syncpoint
    #ifndef GROUP_MSG_PERFIX
    #define GROUP_MSG_PERFIX "grpmsglist"
    #endif

    // define system msg key store format, sys_msglist_uid_syncpoint
    #ifndef SYSTEM_MSG_PERFIX
    #define SYSTEM_MSG_PERFIX "sysmsglist"
    #endif

    // define contact key store format, contact_uid_syncpoint
    #ifndef CONTACT_PERFIX
    #define CONTACT_PERFIX "contact"
    #endif

    // define contactgroup key store format, contactgrp_uid_syncpoint
    #ifndef CONTACTGROUP_PERFIX
    #define CONTACTGROUP_PERFIX "contactgrp"
    #endif

    // define group key store format, grp_uid_syncpoint
    #ifndef GROUP_PERFIX
    #define GROUP_PERFIX "grp"
    #endif

// some marco
    #ifndef SAFE_DELETE
    #define SAFE_DELETE(p)       if (p) {delete p; p = NULL;}
    #define SAFE_DELETE_ARRAY(p) if (p) {delete[] p; p = NULL;}
    #endif

	#ifndef ONCE_LOOP_ENTER
	#define ONCE_LOOP_ENTER do{
	#define ONCE_LOOP_LEAVE }while(0);
	#endif

	#ifndef CHECK_ERROR_BREAK
    #define CHECK_ERROR_BREAK(EXPRESSION, RET, MSG) if ((EXPRESSION))\
    {\
        ret = RET;\
        LOG(ERROR)("%s", MSG);\
        break;\
    }
	#endif

	#ifndef CHECK_WARN_BREAK
    #define CHECK_WARN_BREAK(EXPRESSION, RET, MSG) if ((EXPRESSION))\
    {\
        ret = RET;\
        LOG(WARN)("%s", MSG);\
        break;\
    }
	#endif

	#ifndef CHECK_ERROR_RETURN
    #define CHECK_ERROR_RETURN(EXPRESSION, RET, MSG) \
    if ((EXPRESSION))\
    {\
    LOG(ERROR)("%s", MSG);\
    return RET;\
    }
	#endif

	#ifndef CHECK_WARN_RETURN
    #define CHECK_WARN_RETURN(EXPRESSION, RET, MSG)\
    if ((EXPRESSION))\
    {\
    LOG(WARN)("%s", MSG);\
    return RET;\
    }
	#endif

	#ifndef LOCAL_UNREFERENCED_PARAMETER
    #define LOCAL_UNREFERENCED_PARAMETER(P)\
    {\
        (P) = (P);\
    }
	#endif

	
    // 定义与xml节点对应的服务名称
    #define SYSTEM_NODE_NAME    "system"
    #define COND_NODE_NAME      "cond"
    #define CLIENT_NODE_NAME    "client"
    #define PARTNER_NODE_NAME   "partner_client"
    #define LOGIN_SERVER_NAME   "logind"
    #define MSG_SERVER_NAME     "msg_center"
    #define DATA_SERVER_NAME     "data"
    #define USER_CENTER_SERVER_NAME    "user_center"
    #define TASK_CENTER_SERVER_NAME    "task_center"
    #define STATUS_SERVER_NAME  "statusd"
    #define DBP_SERVER_NAME     "dbproxyd"
    #define RDS_SERVER_NAME     "redis"
    #define DBAGENT_SERVER_NAME "dbagent"
    #define PUSH_SERVER_NAME    "pushserver"
    #define REDIS_PROXY_SERVER_NAME "redis_proxy"
    #define LDB_SERVER_NAME     "leveldb"
    #define UDP_CMD_NAME        "udpcmd"
    #define UDP_CLIENT_NAME     "udpclient"
    #define PARTNER_SERVER_NAME "partner_server"
    #define MASTER_SERVER_NAME  "master"
    #define SLAVE_SERVER_NAME   "slave"




// ========================================================================
// 常用枚举定义
// ========================================================================
enum kUserStatus
{
    OFFLINE_STATUS               = 0, // 离线
    FRONT_ONLINE_STATUS          = 1, // 前台在线
    BACKEND_ONLINE_STATUS        = 2, // 后台在线
    INVALID_STATUS               = 3, // 无线的状态
};


enum
{
	NO_UPDATE   = 0,
	HIT_UPDATE  = 1,
	LESS_UPDATE = 2,
	DIFF_UPDATE = 3,
	MAX_UPDATE  = 4,
};

enum kMsgType
{
    UNKNOWN_MSG_TYPE        = 0,
    P2P_MSG_TYPE,
    GROUP_MSG_TYPE,
    APP_MSG_TYPE,
    MAX_MSG_TYPE
};

enum kUserDevice
{
    DEV_TYPE_INVALID      = 0, // 无效值
    DEV_TYPE_ANDROID      = 1, // Android版
    DEV_TYPE_IPHONE       = 2, // iPhone/iTouch版
    DEV_TYPE_IPAD         = 3, // iPad版
    DEV_TYPE_WEB          = 4, // web版
    DEV_TYPE_MAX          // 最大设备，超过此值就是无效设备
};

enum GROUP_TYPE{
    Group_UnDef          = 0,
    Group_Q              = 1,
    Group_N              = 2,
    Group_Single         = 3,
    Group_Max
};

// 用户屏蔽群消息或多人消息类型枚举
enum MsgAttrSettingType
{
    MsgAttrSetting_ModReject             = 1 << 0,		// 手机屏蔽
    MsgAttrSetting_ModHideReceive        = 1 << 1,	// 手机接收但不提示

    MsgAttrSetting_PCReject              = 1 << 8,		// PC端屏蔽
    MsgAttrSetting_PCHideReceive         = 1 << 9,	// PC端接收但不提示
};


// 数据同步状态
enum kSyncStatus
{
    NOT_START = 0,   // 没有开始
    ON_COPY = 1,     // 在COPY中
    ON_SYNC = 2,     // 在SYNC中
    OUT_OF_SYNC = 4, // 数据有偏差，可能要重新来过COPY了
};

enum kConnectFlag
{
    DISCONNECTED = 0,   // 连接断开
    CONNECTING = 1,     // 在连接中
    CONNECTED = 2,      // 已连接
};
}

#endif // CONSTANTS_H_
