/*
 * some error code define
 *
*/


#ifndef ERRCODES_H_
#define ERRCODES_H_
#include <stdint.h>

namespace common
{

	const static uint32_t ERR_SUCCESS              =  0;
    static const uint32_t ERR_MYSQL_ERROR          =  98;
	const static uint32_t ERR_SERVER_TIMEOUT       =  6000;
	const static uint32_t ERR_PROCOL_NOMATCH       =  6001;
	const static uint32_t ERR_SERVER_ENCODE_ERROR  =  6002;
	const static uint32_t ERR_SERVER_UNKNOWN_ERROR =  6003;
	const static uint32_t ERR_USER_NOT_EXIST       =  6100;
	const static uint32_t ERR_TASK_REPEATED        =  6200;
	const static uint32_t ERR_TASK_SN_EMPTY        =  6201;
	const static uint32_t ERR_SAVE_TASK_FAILED     =  6202;
	const static uint32_t ERR_TASK_TIME_INVALID    =  6203;
	const static uint32_t ERR_PARAM_INVALID        =  6210;

	// ������ش�����
	const static uint32_t ERR_TASK_FIRST_START     =  0;
	const static uint32_t ERR_TASK_ALREADY_START   =  401;
	const static uint32_t ERR_TASK_EXPIRE          =  402;
	const static uint32_t ERR_TASK_INVALID         =  403;
	const static uint32_t ERR_TASK_OVERFLOW        =  404;
	const static uint32_t ERR_TASK_DONE            =  405;
	const static uint32_t ERR_CODE_INVALID         =  406;

	// ���ִ�����
	const static uint32_t ERR_ALREADY_BIND         =  601;
	const static uint32_t ERR_AMOUNT_LIMIT         =  610;
	const static uint32_t ERR_FETCH_FAILED         =  611;
	const static uint32_t ERR_SYSTEM_BUSY          =  612;
	const static uint32_t ERR_NAME_MISMATCH        =  613;
	const static uint32_t ERR_FATAL_ERROR          =  614;
	const static uint32_t ERR_HUGE_MONEY           =  615;  // ���̫��
	const static uint32_t ERR_TINY_MONEY           =  616;  // ���̫С
	const static uint32_t ERR_UNKNOWN_ERROR        =  200;	

// mysql error
	const static uint32_t ERR_MYSQL_DUPLICATE_KEY  =  1062;
	const static uint32_t ERR_MYSQL_GOAWAY         =  2006;

// redis error
    const static uint32_t ERR_REDIS_ERROR		   =  8000;

// leveldb error
    const static uint32_t ERR_LEVELDB_ERROR 	   =  8001;


// ========================================================================
// common error code
// ========================================================================

	static const uint32_t OK                                      = 0;
	static const uint32_t UNKNOWN_ERROR                           = 200;


	static const uint32_t ERR_MYSQL_NO_VALID_VALUE                = 100;
    static const uint32_t SYSTEM_ERROR                            = -1;
    static const uint32_t INPKG_ERROR                             = 5001;
    static const uint32_t OUTPKG_ERROR                            = 5002;
    static const uint32_t DATA_FULL_ERROR                         = 5003;
    static const uint32_t UNSUPPORT_SYNC_TYPE_ERROR               = 5004;
    static const uint32_t NO_DELTA_DATA_ERROR                     = 5005;
    static const uint32_t PARAM_ERROR                             = 5006;
    static const uint32_t SERVICE_UNAVAILABLE_ERROR               = 6000;


// ========================================================================
// mobiled error code
// ========================================================================
    static const uint32_t MOB_SUCCESSED                           = 0;
    static const uint32_t MOB_USER_TOKEN_ERROR                    = 17;
    static const uint32_t MOB_DENY_LOGIN                          = 23;

// ========================================================================
// macd error code
// ========================================================================
    static const uint32_t MAC_DENY_LOGIN                         = 33;


// ========================================================================
// client error code
// ========================================================================
    static const uint32_t CLIENT_RET_FORCE_UPDATE_NOTICE         = 115;
    static const uint32_t CLIENT_RET_PROMPT_UPDATE_NOTICE        = 116;
    static const uint32_t CLIENT_RET_LOGIN_USER_FULLED           = 118;
    static const uint32_t CLIENT_RET_LOGIN_FAIL_CAN_NOT_KICK     = 119;
    //��Ĵ����붨���е��ң�dbp��imd�Ĵ���ֵ�õ�ͬһ����ֵ�����ҷ����˲�ͬ���ļ�
    static const uint32_t CLIENT_RET_LOGIN_USER_TOO_MUCH         = 130;
    static const uint32_t CLIENT_RET_STANDARD_VERSION_CANNOT_LOGIN = 131; //ADD FOR MOBILE,��׼�治�ܵ�¼
    static const uint32_t CLIENT_RET_LOGIN_ACCOUT_NOT_MATCH      = 201;//�˺Ų�ƥ��
    static const uint32_t CLIENT_RET_LOGIN_TOKEN_OVERDUE         = 202;//token����
    static const uint32_t CLIENT_RET_LOGIN_TOKEN_ERROR           = 203;//����token
    static const uint32_t CLIENT_RET_LOGIN_APPKEY_ERROR          = 204;//����appkey
    static const uint32_t EXCEED_MAX_ONLINE_USER_LIMIT           = 5;//add for private cloud2.2


// ========================================================================
// imd error code
// ========================================================================
    static const uint32_t IMD_RET_SUCCESS                       = 0;   // �ɹ�
    static const uint32_t IMD_RET_UNKOWN_ERROR                  = 99;  // δ֪ԭ�����
    static const uint32_t IMD_RET_USER_OFFLINE                  = 128; // �û�������
    static const uint32_t IMD_RET_CORP_OFFLINE                  = 129; // ��˾������
	static const uint32_t DB_SERVER_PASSWD_WRONG                = 125; //�������

    static const uint32_t CLIENT_RET_USER_NOT_ADMIN             = 1;
    static const uint32_t CLIENT_RET_ADMIN_NUM_IS_MAX           = 1;

// ========================================================================
// mysql error code
// ========================================================================

}

#endif // ERRCODES_H_
