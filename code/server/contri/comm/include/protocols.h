/*
 * some protocol number define
 *
*/
#ifndef PROTOCOLS_H_
#define PROTOCOLS_H_
#include <stdint.h>


namespace common
{
// ========================================================================
// new projrct common command number
// ========================================================================
    static const uint32_t CMD_KEEPALIVE                        = 0;
    static const uint32_t CMD_USER_KEEPALIVE                   = 100;
    static const uint32_t CMD_LOGIN                            = 1000;
    static const uint32_t CMD_SYNC_DATA                        = 2000;
    static const uint32_t CMD_SYNC_DATA_ACK                    = 2001;
    static const uint32_t CMD_SERVER_NOTIFY                    = 2002;
    static const uint32_t CMD_REPORT_DATA                      = 3001;
    static const uint32_t CMD_START_TASK                       = 4000;
    static const uint32_t CMD_PUBLISH_TASK                     = 5000;
    static const uint32_t CMD_QUERY_TASK_LIST                  = 5001;
    static const uint32_t CMD_QUERY_TASK_INFO                  = 5002;
    static const uint32_t CMD_THIRD_PARTY_LOGIN                = 6000;
    static const uint32_t CMD_WITHDRAW                         = 6666;

    static const uint32_t CMD_DB_OPERATE                       = 10000;

	// 服务器内部协议
    static const uint32_t CMD_INNER_GET_TASK                   = 11001;
    static const uint32_t CMD_INNER_GET_INCOME                 = 11002;
    static const uint32_t CMD_SAVE_TASK_INFO                   = 11010;
    static const uint32_t CMD_INNER_REPORT_TASK_INFO           = 11020;
	
    static const uint32_t CMD_INNER_CHECK_USER                 = 11101;
    static const uint32_t CMD_INNER_INSERT_USER                = 11102;
    static const uint32_t CMD_INNER_CHECK_TASK                 = 11103;
    static const uint32_t CMD_INNER_WRITE_TASK                 = 11104;
    static const uint32_t CMD_INNER_WRITE_USER_TASK_LIST       = 11105;
    static const uint32_t CMD_INNER_CLOSE_TASK                 = 11106;






// ========================================================================
// common command number
// ========================================================================
    static const uint32_t KEEPALIVE_CMD                        = 0;

    static const uint32_t SERVER_IS_DOWN_FOR_MAINTENANCE       = 1000;
    static const uint32_t CLIENT_ALIVE_CMD                     = 1001;
    static const uint32_t LOGIN_CMD                            = 1002;
    static const uint32_t LOGOUT_CMD                            = 1002;
    static const uint32_t OFFLINE_CMD                          = 1003;
    static const uint32_t KICK_CMD                             = 1004;
    static const uint32_t GET_ADDR_CMD                         = 1005;
    static const uint32_t GET_CID_UID_CMD                      = 1006;
    static const uint32_t NEW_LOGIN_OVER_CMD                   = 1007;
    static const uint32_t PREPARE_UPDATE_OVER_CMD              = 1008;
    static const uint32_t CLIENT_REPORT_SESSIONKEY_CMD         = 1009;
    static const uint32_t CLIENT_IMPLICIT_LOGIN_CMD            = 1010;
    static const uint32_t CLIENT_RELOGIN_CMD                   = 1011;
    static const uint32_t CLIENT_REPORT_MAC_CMD                = 1012;
    static const uint32_t CLIENT_REPORT_MOF_INFO_CMD           = 1013;
    static const uint32_t SERVER_MAINTAIN_TIPS_CMD             = 1014;

    static const uint32_t MOB_RELOGIN_CMD                      = 1015;
    static const uint32_t CLIENT_LOGIN_CMD_V2                  = 1016;
    static const uint32_t BATCH_REPORT_COND_USERDATA_TO_IMD    = 1017;
    static const uint32_t REMOVE_OVERDUE_COND_USER_CMD         = 1018;
    static const uint32_t CLIENT_ACK_CMD                       = 1100;

    static const uint32_t GET_CORP_INFO_CMD                    = 2001;
    static const uint32_t GET_ALL_DEPT_CMD                     = 2002;
    static const uint32_t GET_DEPT_USERID_CMD                  = 2003;
    static const uint32_t GET_USER_DEPTID_CMD                  = 2004;
    static const uint32_t GET_ROLE_INFO_CMD                    = 2005;
    static const uint32_t GET_DEPT_INFO_CMD                    = 2006;
    static const uint32_t MODIFY_USER_INFO_CMD                 = 2007;
    static const uint32_t MODIFY_PASSWD_CMD                    = 2008;
    static const uint32_t GET_ALL_DEPTUC_AND_DEPTUSERUC_CMD    = 2009;
    static const uint32_t GET_DEPT_INFO_UC_CMD                 = 2010;
    static const uint32_t GET_BATCH_DEPT_INFO_CMD              = 2011;
    static const uint32_t GET_ALL_UC                           = 2012;//取所有uc
    static const uint32_t NOTIFY_UPDATE_UC                     = 2013;//通知更新uc

    static const uint32_t CONDITION_FETCH_DEPT_UC = 2021;
    static const uint32_t GET_USER_POSITION_BYCID_CMD          = 2030;//通过cid获取全公司成员的职位信息

    static const uint32_t ADMIN_GET_CONTACTOR_LIST_CMD         = 2900;
    static const uint32_t GET_ALL_USER_UC                      = 2901 ; //add for login optimization
    static const uint32_t GET_CORP_UC                          = 2902 ; //add for login optimization
    static const uint32_t GET_CORP_UC_AND_ALL_USER_UC_CMD      = 2903 ; //add for login optimization
    static const uint32_t GET_COLLEAGUE_GROUP_CMD              = 3001;
    static const uint32_t GET_COLLEAGUE_LIST_CMD               = 3002;
    static const uint32_t ADD_COLLEAGUE_GROUP_CMD              = 3003;
    static const uint32_t DELETE_COLLEAGUE_GROUP_CMD           = 3004;
    static const uint32_t ADD_COLLEAGUE_CMD                    = 3005;
    static const uint32_t DELETE_COLLEAGUE_CMD                 = 3006;
    static const uint32_t MODIFY_COLLEAGUE_GROUP_CMD           = 3007;
    static const uint32_t MODIFY_COLLEAGUE_GROUP_NAME_CMD      = 3008;
    static const uint32_t GET_COLLEAGUE_BASEINFO_CMD           = 3009;
    static const uint32_t IMD_GET_COLLEAGUE_LIST_CMD           = 3010;
    static const uint32_t GET_COLIEAGUE_GROUP_LIST_UC_CMD      = 3101;
    static const uint32_t GET_COLIEAGUE_LIST_UC_CMD            = 3102;
    static const uint32_t GET_COLLEAGUE_BASEINFO_POS_CMD       = 3019;

    static const uint32_t GET_STATUS_CMD                       = 3900;
    static const uint32_t USER_STATUS_CHANGE_CMD               = 3901;
    static const uint32_t CONTACTOR_INFO_CHANGE_CMD            = 3902;
    static const uint32_t MOB_USER_STATUS_CHANGE_ACK_CMD       = 3903;
    static const uint32_t GET_CONTACTOR_STATUS                 = 3904;
    static const uint32_t BATCH_GET_USER_ALL_STATUS            = 3905;
    static const uint32_t BATCH_USER_STATUS_CHANGE_CMD         = 3906;
	static const uint32_t BATCH_USERS_STATUS_CHANGE_TO_ONEUSER_CMD = 3907;

    static const uint32_t GET_CONTACTOR_GROUP_CMD              = 4001;
    static const uint32_t GET_CONTACTOR_LIST_CMD               = 4002;
    static const uint32_t ADD_CONTACTOR_GROUP_CMD              = 4003;
    static const uint32_t DELETE_CONTACTOR_GROUP_CMD           = 4004;
    static const uint32_t ADD_CONTACTOR_CMD                    = 4005;
    static const uint32_t DELETE_CONTACTOR_CMD                 = 4006;
    static const uint32_t MODIFY_CONTACTOR_GROUP_CMD           = 4007;
    static const uint32_t MODIFY_CONTACTOR_GROUP_NAME_CMD      = 4008;
    static const uint32_t GET_CONTACTOR_BASEINFO_CMD           = 4009;
    static const uint32_t GET_CONTACTOR_INFO_CMD               = 4010;
    static const uint32_t AGREE_CONTACTOR_CMD                  = 4011;
    static const uint32_t REFUSE_CONTACTOR_CMD                 = 4012;
    static const uint32_t RECV_ADD_CONTACTOR_INVITE_CMD        = 4013;
    static const uint32_t RECV_ADD_CONTACTOR_INVITE_ACK_CMD    = 4014;
    static const uint32_t GET_OFFLINE_ADD_CONTACTOR_MSG_CMD    = 4015;
    static const uint32_t DELETE_OFFLINE_ADD_CONTACTOR_MSG_CMD = 4016;
    static const uint32_t RELATION_CHANGE_CMD                  = 4017;
    static const uint32_t IMD_GET_CONTACTOR_LIST_CMD           = 4018;
    static const uint32_t GET_BATCH_CONTACTOR_BASEINFO_CMD     = 4019;
    static const uint32_t GET_CONTACTOR_GROUP_LIST_UC_CMD      = 4101;
    static const uint32_t GET_CONTACTOR_LIST_UC_CMD            = 4102;
    static const uint32_t GET_CONTACTOR_BASEINFO_POS_CMD       = 4029;
    static const uint32_t GET_BATCH_CONTACTOR_BASEINFO_POS_CMD = 4039;

    static const uint32_t SYSTEM_MSG_CMD                       = 5001;
    static const uint32_t ADD_NOTICE_CMD                       = 5002;
    static const uint32_t RECV_NOTICE_CMD                      = 5003;
    static const uint32_t RECV_NOTICE_ACK_CMD                  = 5004;
    static const uint32_t GET_OFFLINE_NOTICE_CMD               = 5005;
    static const uint32_t DELETE_OFFLINE_NOTICE_CMD            = 5006;
    static const uint32_t CHAT_MSG_CMD                         = 5007;
    static const uint32_t CHAT_MSG_ACK_CMD                     = 5008;
    static const uint32_t GET_OFFLINE_CHAT_MSG_CMD             = 5009;
    static const uint32_t DELETE_OFFLINE_CHAT_MSG_CMD          = 5010;
    static const uint32_t TRANSPARENT_CMD                      = 5011;
    static const uint32_t GET_OFFLINE_SYSTEM_MSG_CMD           = 5012;
    static const uint32_t DELETE_OFFLINE_SYSTEM_MSG_CMD        = 5013;
    static const uint32_t INSTANT_NOTICE_CMD                   = 5014;
    static const uint32_t CHAT_MSG_REACK_CMD                   = 5015;
    static const uint32_t GET_OFFLINE_SHORT_MSG_CMD            = 5016;
    static const uint32_t SHORT_MSG_CHAT_ACK_CMD               = 5017;
    static const uint32_t DELETE_OFFLINE_SHORT_MSG_CMD         = 5018;
    static const uint32_t OFFLINE_FILE_PRELOAD_CMD             = 5020;
    static const uint32_t OFFLINE_FILE_UPLOAD_OVER_CMD         = 5021;
    static const uint32_t OFFLINE_FILE_NOTICE_CMD              = 5022;
    static const uint32_t OFFLINE_FILE_NOTICE_OPPOSITE_CMD     = 5023;
    static const uint32_t OFFLINE_FILE_DOWNLOAD_ACK_CMD        = 5024;
    static const uint32_t GET_OFFLINE_FILE_CMD                 = 5025;
    static const uint32_t GET_OFFLINE_CHAT_SUMMARY_CMD         = 5026; 
    static const uint32_t GET_OFFLINE_CHAT_FROM_ONE_USER_CMD   = 5027;
    static const uint32_t USER_CONTROL_CMD                     = 5028;
    static const uint32_t NOTITY_USER_CONTROL_CMD              = 5029;
    static const uint32_t CHAT_WITH_ONESELF_CMD                = 5030;
    static const uint32_t BATCH_GET_SINGLE_CHAT_MSG_STATUS     = 5500;
    static const uint32_t SET_SINGLE_CHAT_MSG_STATUS           = 5501;
    static const uint32_t SINGLE_CHAT_MSG_STATUS_NOTICE        = 5502;

    static const uint32_t CREATE_ROOM_CMD                      = 6001;
    static const uint32_t INVITE_PERSON_JOIN_ROOM_CMD           = 6002;
    static const uint32_t JOIN_ROOM_CMD                        = 6003;
    static const uint32_t NOTICE_USER_JOIN_ROOM_CMD            = 6004;
    static const uint32_t GET_ROOM_INFO_CMD                    = 6005;
    static const uint32_t GET_USER_ROOM_LIST_CMD               = 6006;
    static const uint32_t GET_ROOM_USER_LIST_CMD               = 6007;
    static const uint32_t GET_ROOM_USERS_STATUS_CMD            = 6008;
    static const uint32_t PEOPLE_CHAT_CMD                      = 6009;
    static const uint32_t NEW_MSG_REACH_NOTICE_USER_CMD        = 6010;
    static const uint32_t PEOPLE_CHAT_USER_CHANGE_STATUS_CMD   = 6011;
    static const uint32_t EXIT_ROOM_CMD                        = 6012;
    static const uint32_t MODIFY_THE_ROOM_TITLE_CMD            = 6013;
    static const uint32_t RECOVER_ROOM_NOTICE_USERS_CMD        = 6014;
    static const uint32_t EXIT_ROOM_NOTICE_USERS_CMD           = 6015;
    static const uint32_t MODIFY_TITLE_NOTICE_USERS_CMD        = 6016;
    static const uint32_t GET_GROUP_OFFLINE_CHAT_MSG_CMD       = 6017;
    static const uint32_t APP_MSG_NOTICE_CMD                   = 6022;
    static const uint32_t APP_MSG_NOTICE_OPPOSITE_CMD          = 6023;
    static const uint32_t APP_MSG_NOTICE_DOWNLOAD_ACK_CMD      = 6024;
    static const uint32_t APP_MSG_NOTICE_GET_CMD               = 6025;

    static const uint32_t SYSTEM_MSG_SEND_ONE_USER_CMD         = 7001;
    static const uint32_t SYSTEM_MSG_SEND_CORP_USERS_CMD       = 7002;

    static const uint32_t QGROUP_NOTICE_CMD                    = 8000;
    static const uint32_t CREATE_QGROUP_CMD                    = 8001;
    static const uint32_t MOBILED_CREATE_QGROUP_CMD            = 8200;
    static const uint32_t INVITE_PERSON_JOIN_QGROUP_CMD        = 8002;
    static const uint32_t AGREE_JOIN_QGROUP_CMD                = 8003;
    static const uint32_t REJECT_JOIN_QGROUP_CMD               = 8004;
    static const uint32_t NOTICE_USER_JOIN_QGROUP_CMD          = 8005;
    static const uint32_t UPDATE_QGROUP_ANNOUNCEMENT_CMD       = 8006;
    static const uint32_t QGROUP_MODIFY_ANNOUNCEMENT_NOTICE_CMD= 8007;
    static const uint32_t GET_QGROUP_INFO_CMD                  = 8008;
    static const uint32_t GET_USER_QGROUP_LIST_CMD             = 8009;
    static const uint32_t GET_QGROUP_USER_LIST_CMD             = 8010;
    static const uint32_t GET_QGROUP_USERS_STATUS_CMD          = 8011;
    static const uint32_t QGROUP_CHAT_CMD                      = 8012;
    static const uint32_t NEW_MSG_NOTICE_QGROUP_CMD            = 8013;
    static const uint32_t ADMIN_AGREE_USER_JOIN_QGROUP_CMD     = 8014;
    static const uint32_t QGROUP_USER_CHANGE_STATUS_NOTICE_CMD = 8031;
    static const uint32_t EXIT_QGROUP_CMD                      = 8015;
    static const uint32_t EXIT_QGROUP_NOTICE_USERS_CMD         = 8016;
    static const uint32_t MODIFY_QGROUP_NAME_CMD               = 8017;
    static const uint32_t MODIFY_QGROUP_NAME_NOTICE_CMD        = 8018;
    static const uint32_t DESTROY_THE_QGROUP_CMD               = 8019;
    static const uint32_t QGROUP_DESTROY_NOTICE_USER_CMD       = 8020;
    static const uint32_t QGROUP_OWNER_KICK_USER_CMD           = 8021;
    static const uint32_t QGROUP_KICK_USER_NOTICE_CMD          = 8022;
    static const uint32_t JOIN_QGROUP_CMD                      = 8023;
    static const uint32_t NOTICE_QGROUP_APPROVAL_CMD           = 8024;
    static const uint32_t QGROUP_OWNER_AGREE_JOIN_CMD          = 8025;
    static const uint32_t QGROUP_OWNER_REJECT_JOIN_CMD         = 8026;
    static const uint32_t NOTICE_USER_REJECTED_CMD             = 8027;
    static const uint32_t NOTICE_USER_HAVE_JOIN_QGROUP_CMD     = 8028;
    static const uint32_t GET_QGROUP_OFFLINE_CMD               = 8029;
    static const uint32_t GET_OFFLINE_QGROUP_CHAT_MSG_CMD      = 8030;
    static const uint32_t DELETE_QGROUPD_OFFLINE_CMD           = 8032;
    static const uint32_t LOAD_QGROUP_BASE_INFO_CMD            = 8033;
    static const uint32_t LOAD_QGROUP_USER_LIST_CMD            = 8034;
    static const uint32_t CLIENT_AUTOLOGIN_QGROUPD_CMD         = 8035;
    static const uint32_t CLIENT_LOGIN_QGROUPD_CMD             = 8036;
    static const uint32_t DELETE_USER_QGROUP_CMD               = 8037;
    static const uint32_t UPDATE_QGROUP_HOST_LAST_TIME_CMD     = 8038;
    // for manager del uesr
    static const uint32_t QGROUP_USER_DELETED_NOTICE_CMD       = 8039;
    static const uint32_t QGROUP_DEL_USER_NOTICE_DB_CMD        = 8040;
    static const uint32_t QGROUP_GET_OFFLINE_MSG_ACK_CMD       = 8041;
    //for client login qgroup
    static const uint32_t LOAD_QGROUP_INFO_AND_USER_LIST_CMD   = 8042;    // 已废弃,由8045消息替换
    //for cond rewrite chat msg to qgroup
    static const uint32_t WRITE_CHATMSG_BACK_TO_QGROUP_CMD     = 8043;
    static const uint32_t    LOAD_QGROUP_USER_LIST_CMD_V2      = 8044;
    static const uint32_t NEW_LOAD_QGROUP_INFO_AND_USER_LIST_CMD = 8045;
    // for qgroup file share list
    static const uint32_t GET_QGROUP_FILE_SHARE_LIST           = 8050;
    static const uint32_t GET_QGROUP_SPACE_INFO                = 8051;
    static const uint32_t DELETE_QGROUP_SHARE_FILE_LIST        = 8052;
    static const uint32_t QGROUP_SHARE_FILE_NOTIFY             = 8053;
    static const uint32_t QGROUP_RENAME_THE_SHARE_FILE         = 8054;
    static const uint32_t QGROUP_RENAME_SHARE_FILE_NOTIFY      = 8055;
    static const uint32_t QGROUP_GET_OFFLINE_SHARE_FILE_LIST   = 8056;
    static const uint32_t QGROUP_SHARE_FILE_NOTIFY_CLIENT      = 8057;
    static const uint32_t CLIENT_PREPARE_UPLOAD_NOTICE_SERVER  = 8059;
    static const uint32_t QGROUP_DELETE_SHARE_FILE_NOTIFY      = 8058;
    static const uint32_t QGROUP_SHARE_NOTICE_CMD              = 8060;
    static const uint32_t MSG_NOTICE_QGROUP_COMMON_CMD         = 8061;
    static const uint32_t QGROUP_DESTORY_NOTICE_DBP_UPUC_CMD   = 8062;
    static const uint32_t QGROUP_SHARE_PIC_NOTIFY              = 8063;
    static const uint32_t GET_QGROUP_SPACE_INFO_64             = 8064;
    static const uint32_t CLIENT_PREPARE_UPLOAD_NOTICE_SERVER_64=8065;
    static const uint32_t GET_QGROUP_FILE_SHARE_LIST_PWD       = 8066;
    static const uint32_t UPDATE_SINGLE_QGROUP_FILE_SHARE_PWD  = 8067;
    static const uint32_t FETCH_QGROUP_FILE_SHARE_LIST_PWD     = 8068;
    static const uint32_t CLIENT_PREPARE_UPLOAD_NOTICE_SERVER_64_PWD  = 8069;
    // end region
    static const uint32_t BATCH_GET_USER_QGROUP_LIST_CMD       = 8108;
    static const uint32_t GET_USER_GROUP_LIST_UC_CMD           = 8109;
    static const uint32_t BATCH_GET_USER_QGROUP_LIST_MULRET_CMD= 8110;
    static const uint32_t BATCH_GET_QGROUP_USER_LIST_MULRET_CMD= 8111;
    static const uint32_t BATCH_GET_QGROUP_USERS_STATUS_MULRET_CMD = 8112;   
    static const uint32_t QGROUP_GET_PROPERTY_CMD              = 8113;
    static const uint32_t QGROUP_UPDATE_PROPERTY_CMD           = 8114;
    static const uint32_t QGROUP_SEARCH_CMD                    = 8115;
    static const uint32_t QGROUP_SET_MANAGER_CMD               = 8116;
    static const uint32_t QGROUP_CANCEL_MANAGER_CMD            = 8117;
    static const uint32_t QGROUP_TRANSFER_CMD                  = 8118;
    static const uint32_t QGROUP_SEARCH_BY_QGROUPID_CMD        = 8119;
    static const uint32_t NOTICE_USER_SET_MANAGER_CMD          = 8120;
    static const uint32_t NOTICE_USER_CANCEL_MANAGER_CMD       = 8121;
    static const uint32_t NOTICE_USER_TRANSFER_CMD             = 8122;
    static const uint32_t NEW_AGREE_JOIN_QGROUP_CMD            = 8123;
    static const uint32_t NEW_REJECT_JOIN_QGROUP_CMD           = 8124;
    static const uint32_t QGROUP_GET_USER_STATUS_CMD           = 8125;
    static const uint32_t MP_QGROUP_CHAT_SELF                  = 8126;
    static const uint32_t MP_QGROUP_NOTICE_SELF                = 8127;
    static const uint32_t QGROUP_BATCH_GET_UESR_STATUS_CMD     = 8128;
    static const uint32_t MP_QGROUP_USER_KICK                  = 8129;
    static const uint32_t NEW_CREATE_QGROUP_CMD                = 8200;
    //向imd发送群消息，以便于后面的push
    static const uint32_t QGROUP_SEND_MSG_TO_IMD               = 8130;
    static const uint32_t LIGHT_INVITE_PERSON_JOIN_QGROUP_CMD  = 8302;
    static const uint32_t BATCH_GET_QGROUP_CHAT_MSG_STATUS     = 8500;
    static const uint32_t SET_QGROUP_CHAT_MSG_STATUS           = 8501;
    static const uint32_t QGROUP_CHAT_MSG_STATUS_NOTICE           = 8502;

    static const uint32_t QGROUP_DEL_FILE_NOTICE               = 1;
    static const uint32_t QGROUP_NEW_SHARE_FILE_NOTICE         = 2;
    static const uint32_t QGROUP_SHARE_FILE_RENAME_NOTICE      = 3;
    static const uint32_t QGROUP_NEW_SHARE_PIC_NOTICE          = 4;

    //ngroup cmd info
    static const uint32_t NGROUP_NOTICE_CMD                    = 9000;
    static const uint32_t CREATE_NGROUP_CMD                    = 9001;
    static const uint32_t INVITE_PERSON_JOIN_NGROUP_CMD        = 9002;
    static const uint32_t AGREE_JOIN_NGROUP_CMD                = 9003;
    static const uint32_t REJECT_JOIN_NGROUP_CMD               = 9004;
    static const uint32_t NOTICE_USER_JOIN_NGROUP_CMD          = 9005;
    static const uint32_t UPDATE_NGROUP_ANNOUNCEMENT_CMD       = 9006;
    static const uint32_t NGROUP_MODIFY_ANNOUNCEMENT_NOTICE_CMD= 9007;
    static const uint32_t GET_NGROUP_INFO_CMD                  = 9008;
    static const uint32_t GET_USER_NGROUP_LIST_CMD             = 9009;
    static const uint32_t GET_NGROUP_USER_LIST_CMD             = 9010;
    static const uint32_t GET_NGROUP_USERS_STATUS_CMD          = 9011;
    static const uint32_t NGROUP_CHAT_CMD                      = 9012;// change to 20210
    static const uint32_t NEW_MSG_NOTICE_NGROUP_CMD            = 9013;
    static const uint32_t HOST_EXIT_NGROUP_CMD                 = 9014;
    static const uint32_t EXIT_NGROUP_CMD                      = 9015;
    static const uint32_t EXIT_NGROUP_NOTICE_USERS_CMD         = 9016;
    static const uint32_t MODIFY_NGROUP_NAME_CMD               = 9017;
    static const uint32_t MODIFY_NGROUP_NAME_NOTICE_CMD        = 9018;
    static const uint32_t DESTROY_THE_NGROUP_CMD               = 9019;
    static const uint32_t NGROUP_DESTROY_NOTICE_USER_CMD       = 9020;
    static const uint32_t NGROUP_OWNER_KICK_USER_CMD           = 9021;
    static const uint32_t NGROUP_KICK_USER_NOTICE_CMD          = 9022;
    static const uint32_t JOIN_NGROUP_CMD                      = 9023;
    static const uint32_t NOTICE_NGROUP_APPROVAL_CMD           = 9024;
    static const uint32_t NGROUP_OWNER_AGREE_JOIN_CMD          = 9025;
    static const uint32_t NGROUP_OWNER_REJECT_JOIN_CMD         = 9026;
    static const uint32_t NOTICE_USER_REJECTED_NGROUP_CMD      = 9027;
    static const uint32_t NOTICE_USER_HAVE_JOIN_NGROUP_CMD     = 9028;
    static const uint32_t GET_NGROUP_OFFLINE_CMD               = 9029;
    static const uint32_t GET_OFFLINE_NGROUP_CHAT_MSG_CMD      = 9030;
    static const uint32_t NGROUP_USER_CHANGE_STATUS_NOTICE_CMD = 9031;
    static const uint32_t DELETE_NGROUPD_OFFLINE_CMD           = 9032;
    static const uint32_t LOAD_NGROUP_BASE_INFO_CMD            = 9033;
    static const uint32_t LOAD_NGROUP_USER_LIST_CMD            = 9034;
    static const uint32_t CLIENT_LOGIN_NGROUPD_CMD             = 9036;
    static const uint32_t DELETE_USER_NGROUP_CMD               = 9037;
    static const uint32_t UPDATE_NGROUP_HOST_LAST_TIME_CMD     = 9038;
    //for manger del user
    static const uint32_t NGROUP_USER_DELETED_NOTICE_CMD       = 9039;
    static const uint32_t NGROUP_DEL_USER_NOTICE_DB_CMD        = 9040;
    static const uint32_t NGROUP_GET_OFFLINE_MSG_ACK_CMD       = 9041;
    static const uint32_t MSG_NOTICE_NGROUP_COMMON_CMD         = 9042;
    static const uint32_t NGROUP_DESTORY_NOTICE_DBP_UPUC_CMD   = 9043;
    //for cond rewrite chat msg to ngroup
    static const uint32_t WRITE_CHATMSG_BACK_TO_NGROUP_CMD     = 9044;
    static const uint32_t NGROUP_CHAT_WITH_ONESELF_CMD         = 9045;
    static const uint32_t NEW_CREATE_NGROUP_CMD                = 9046;
    static const uint32_t INITIATIVE_JOIN_NGROUP_CMD           = 9047;
    static const uint32_t BATCH_GET_NGROUP_MSG_CMD             = 9108;
    static const uint32_t GET_USER_NGROUP_LIST_UC_CMD          = 9109;
    static const uint32_t BATCH_GET_NGROUP_MSG_MULRET_CMD      = 9110;
    static const uint32_t MOBILED_NOTICE_USER_JOIN_NGROUP_CMD  = 9050;
    static const uint32_t BATCH_GET_NGROUP_USERS_STATUS_MULRET_CMD = 9112;  
    static const uint32_t SET_NGROUP_RECYCLE_FLAG_CMD          = 9115;
    //向imd发送群消息，以便于后面的push
    static const uint32_t NGROUP_SEND_MSG_TO_IMD               = 9130;
    static const uint32_t MP_NGROUP_CHAT_SELF                  = 9126;
    static const uint32_t BATCH_GET_NGROUP_CHAT_MSG_STATUS     = 9500;
    static const uint32_t SET_NGROUP_CHAT_MSG_STATUS           = 9501;
    static const uint32_t NGROUP_CHAT_MSG_STATUS_NOTICE           = 9502;

    static const uint32_t WEBAGENTD_UPDATE_CORP_CACHE_NOTICE_CMD = 10000;
    static const uint32_t WEBAGENTD_USER_LOGIN_STAT_CMD        = 10001;
    static const uint32_t WEBAGENTD_USER_CHAT_MSG_CMD          = 10002;
    static const uint32_t WEBAGENTD_REPORT_MAC_CMD             = 10003;
    static const uint32_t WEBAGENTD_REPORT_MNG_DEL_USER_CMD    = 10004;
    static const uint32_t WEB_NOTICE_CMD                       = 10005;
    static const uint32_t WEBAGENTD_SHORT_MSG_SERVICE_CMD      = 10006;
    static const uint32_t WEBAGENTD_MSG_PUSH_CMD               = 10007;
    static const uint32_t WEBAGENTD_CUSTOM_CMD                 = 10008;
    static const uint32_t WEBAGENTD_ONLY_PUSH_CMD              = 10020;
    static const uint32_t WEBAGENTD_ADD_NOTICE_CMD             = 10021;
    static const uint32_t WEBAGENTD_CREATE_QGROUP_COMMAND      = 10022;
    static const uint32_t WEBAGENTD_CREATE_NGROUP_COMMAND      = 10023;
    static const uint32_t WEBAGENTD_FORCE_PUSH_CMD             = 10024;
    static const uint32_t WEBAGENTD_SEND_QGROUP_CHAT_CMD       = 10012;
    static const uint32_t WEBAGENTD_REGISTER_CHAT_CMD          = 10013;
    static const uint32_t WEBAGENTD_SEND_CHAT_MSG_CMD          = 10100;
    static const uint32_t WEBAGENTD_APP_MSG_NOTICE_CMD         = 10107;
    static const uint32_t WEBAGENTD_BATCH_TRANFER_MSG_CMD      = 10200;
    static const uint32_t COLLECTION_USER_CHAT_MSG_CMD         = 10300;

    //for auto login                                
    static const uint32_t CLIENT_AUTOLOGIN_NGROUPD_CMD         = 9035;
    static const uint32_t NGROUP_GET_USER_STATUS_CMD           = 9125;
    //add for ngroup multi login
    static const uint32_t NGROUP_BATCH_GET_UESR_STATUS_CMD     = 9128;
    //for share pic                                
    static const uint32_t NGROUP_SHARE_PIC_NOTIFY              = 9060;
    static const uint32_t NGROUP_SHARE_NOTICE_CMD              = 9061;

    //idservice
    static const uint32_t GET_UNIQ_ID                          = 12000;

    
// ========================================================================
// msg_center command
// ========================================================================
    static const uint32_t MSG_CENTER_CHAT_MSG_CMD                = 13000;//下发消息
    static const uint32_t MSG_CENTER_WRITE_CHATMSG_BACK_CMD      = 13001;//消息回写
    static const uint32_t MSG_CENTER_MESSAGE_UNREACHABLE         = 13002;//消息不可达
    static const uint32_t MSG_CENTER_P2P_MSG_NOTICE              = 13020;
    static const uint32_t MSG_CENTER_QGROUP_MSG_NOTICE           = 13030;
    static const uint32_t MOBILE_GET_USER_QGROUP_LIST            = 13039;
    static const uint32_t MSG_CENTER_NGROUP_MSG_NOTICE           = 13040;
    static const uint32_t MOBILE_GET_USER_NGROUP_LIST            = 13049;
    static const uint32_t MSG_CENTER_USER_JOIN_QGROUP            = 13031;
    static const uint32_t MSG_CENTER_USER_QUIT_QGROUP            = 13032;
    static const uint32_t MSG_CENTER_USER_JOIN_NGROUP            = 13041;
    static const uint32_t MSG_CENTER_USER_QUIT_NGROUP            = 13042;


// ========================================================================
// mobiled command
// ========================================================================
    static const uint32_t MOBILE_UPDATE_CMD                      = 11000;
    static const uint32_t MOBILE_INFO_COLLECTION_CMD             = 11001;

    static const uint32_t SYNC_DEPT_INFO                         = 20001;
    static const uint32_t FETCH_DEPT_COUNT                       = 20002;
    static const uint32_t SYNC_DEPT_INFO_POSITION                = 20003;
    static const uint32_t SYNC_QGROUP_LIST                       = 20101;
    static const uint32_t SYNC_QGROUP_USER_LIST                  = 20102;
    static const uint32_t QGROUP_SET_ATTRIBUTE                   = 20103;
    static const uint32_t QGROUP_GET_CLOUD_MSG                   = 20105;
    static const uint32_t MOBILED_QGROUP_CHAT_CMD                = 20110;
    static const uint32_t MOBILED_QGROUP_NEW_MSG_NOTICE          = 20111;
    static const uint32_t SYNC_QGROUP_MSG                        = 20112;
    static const uint32_t SYNC_NGROUP_LIST                       = 20201;
    static const uint32_t SYNC_NGROUP_USER_LIST                  = 20202;
    static const uint32_t NGROUP_SET_ATTRIBUTE                   = 20203;
    static const uint32_t NGROUP_GET_CLOUD_MSG                   = 20205;
    static const uint32_t MOBILED_NGROUP_CHAT_CMD                = 20210;
    static const uint32_t MOBILED_NGROUP_NEW_MSG_NOTICE          = 20211;
    static const uint32_t SYNC_NGROUP_MSG                        = 20212;
    static const uint32_t USER_GET_CLOUD_MSG                     = 20305;
    static const uint32_t MOBILE_P2P_CHAT_MSG_SENDER             = 20310;
    static const uint32_t MOBILE_P2P_CHAT_MSG                    = 20311;
    static const uint32_t MOBILE_P2P_DEL_OFFLINE_MSG             = 20320;
    static const uint32_t NEW_ADD_OFFLINE_CHAT_MSG_CMD           = 20390; 
    static const uint32_t NEW_GET_OFFLINE_CHAT_MSG_CMD           = 20391;
    static const uint32_t PC_GET_OFFLINE_CHAT_MSG_CMD            = 20392;
    static const uint32_t SEND_CHATMSG_TO_MYSELF_DEV             = 20410;
    static const uint32_t SEND_CHATMSG_TO_MYSELF_DEV_NOTICE      = 20411;
    static const uint32_t NEW_DEL_OFFLINE_CHAT_MSG_CMD           = 5040;
    static const uint32_t SYNC_CONTACTS_LIST                     = 3012;
    static const uint32_t SYNC_CONTACTS_USER_LIST                = 3011;
    static const uint32_t FETCH_USER_INFO                        = 3111;
    static const uint32_t SEARCH_USER_INFO                       = 2020;
    static const uint32_t NEW_GET_QGROUP_INFO_CMD                = 8080;
    static const uint32_t NEW_GET_NGROUP_INFO_CMD                = 9080;


// ========================================================================
// status command
// ========================================================================
    static const uint32_t PARTNER_KEEPALIVE_CMD                  = 1;
    static const uint32_t BATCH_GET_STATUS                       = 40000;
    static const uint32_t CHANGE_STATUS                          = 40001;
    static const uint32_t BATCH_GET_COMMON_STATUS                = 40002;
    static const uint32_t SYNC_DATA_CMD_REQ                      = (1 << 15) - 1;
    static const uint32_t SYNC_DATA_CMD_RES                      = (1 << 15);


// ========================================================================
// leveldb command
// ========================================================================
    static const uint32_t LEVELDB_GET                          = 30000;
    static const uint32_t LEVELDB_PUT                          = 30001;
    static const uint32_t LEVELDB_DEL                          = 30002;
    static const uint32_t LEVELDB_RANGE_GET                    = 30003;
    static const uint32_t LEVELDB_RANGE_DEL                    = 30004;
    static const uint32_t LEVELDB_PUT_BY_SAME_VALUE            = 30005;
    static const uint32_t LEVELDB_REVERSE_RANGE_GET            = 30006;
    static const uint32_t LEVELDB_DETECT_MASTER                = 30100; // 探测master
    static const uint32_t LEVELDB_SYNC                         = 30101; // 同步数据
    static const uint32_t LEVELDB_COPY                         = 30102; // 拷贝数据


// ========================================================================
// upload service command
// ========================================================================
    static const uint32_t UPLOAD_VOICE_FILE_CMD              = 31000;
    static const uint32_t DOWNLOAD_VOICE_FILE_CMD            = 31001;


// ========================================================================
// group store command
// ========================================================================
    static const uint32_t MSGSTORE_ARRIVED_CMD              = 15000;                  
    static const uint32_t MSGSTORE_CARRY_USER_CMD           = 15001;         
    static const uint32_t MSGSTORE_REWRITE_CMD              = 15002;
    static const uint32_t MSGSTORE_GET_OFFLINE_MSG_CMD      = 15003;
    static const uint32_t MSGSTORE_GET_OFFLINE_MSG_ACK      = 15004;
    static const uint32_t MSGSTORE_DEL_TOTAL_MSG_CMD        = 15005;
    static const uint32_t MSGSTORE_DEL_OFFLINE_MSG_CMD      = 15006;


// ========================================================================
// push_server command
// ========================================================================
    static const uint32_t CMD_PUSHMESSAGE                   = 16000;
    static const uint32_t CMD_PUTTOKEN                      = 16001;
    static const uint32_t CMD_DELETETOKEN                   = 16002;


// ========================================================================
// dbp command
// ========================================================================
    static const uint32_t DBP_GET_CONTACT_LIST                = 200; // 服务器内部，获取联系人列表
    static const uint32_t DBP_GET_UID_BY_ACCOUNT              = 201; // 服务器内部，获取联系人
    static const uint32_t DBP_AGREE_ADD_CONTACT               = 2101;
    static const uint32_t DBP_REFUSE_ADD_CONTACT              = 2102;
    static const uint32_t DBP_GET_GROUP_ID                    = 3100;
    static const uint32_t DBP_GET_GROUP_INFO                  = 3101;
    static const uint32_t DBP_GET_GROUP_USER_LIST             = 3102;
    static const uint32_t DBP_GET_GROUP_USER_DETAIL_LIST      = 3103; 


// ========================================================================
// status and status proxy command
// ========================================================================
    static const uint32_t STATUS_GET_STATUS_INFO              = 40000;
    static const uint32_t STATUS_UPDATE_STATUS_INFO           = 40001;
    static const uint32_t STATUS_WRITE_STATUS_INFO            = 40002;


// ========================================================================
// inner command
// ========================================================================
    static const uint32_t USER_TIMEOUT_CMD                     = 501;
    static const uint32_t ADD_OFFLINE_CHAT_MSG_CMD             = 502;
    static const uint32_t ADD_OFFLINE_ADD_CONTACTOR_MSG_CMD    = 503;
    static const uint32_t ADD_OFFLINE_NOTICE_CMD               = 504;
    static const uint32_t AUTHEN_ACCOUNT_CMD                   = 505;
    static const uint32_t RECOVER_ROOM_NOTICE_DB_CMD           = 506;
    static const uint32_t USER_JOIN_ROOM_ADD_TO_DB_CMD         = 507;
    static const uint32_t APPLY_ROOM_AAD_TO_DB_CMD             = 508;
    static const uint32_t GET_THE_NUM_SESSION_ID_CMD           = 509;
    static const uint32_t ADD_OFFLINE_SMS_MSG_CMD              = 510;
    static const uint32_t AUTHEN_ACCOUNT_FAUTHORITY_CMD        = 511;

    static const uint32_t ADD_MONITOR_CHAT_MSG_CMD             = 520;//add for private_cloud_2.0
    static const uint32_t ADD_MONITOR_QGROUP_CHAT_MSG_CMD      = 521;//add for private_cloud_2.0
    static const uint32_t ADD_MONITOR_NGROUP_CHAT_MSG_CMD      = 522;//add for private_cloud_2.0


    static const uint32_t USER_AT_SAME_WM_REPEAT_LOGIN         = 600;

    static const uint32_t RELOGIN_AUTHEN_ACCOUNT_CMD           = 601;
    static const uint32_t AUTHEN_ACCOUNT_CMD_V2                = 602; //add for 5.1
    static const uint32_t AUTHEN_ACCOUNT_CMD_V3                = 603 ; //add for mobile refactor
    static const uint32_t GET_TOKEN_LOGIN_INFO                 = 604;  //add for mobile auth login


// ========================================================================
// private cloud command
// ========================================================================
    static const uint32_t WEBAGENTD_GET_CUR_ONLINE_USER        = 10009;
    static const uint32_t WEBAGENTD_CREATE_QGROUP_CMD          = 10010;
    static const uint32_t WEBAGENTD_ADD_USER_TO_QGROUP_CMD     = 10011;
    static const uint32_t WEBAGENTD_REMOVE_QGROUP_USER_CMD     = 10113;
    static const uint32_t WEBAGENTD_MODIFY_QGROUP_INFO_CMD     = 10014;
    static const uint32_t WEBAGENTD_DELETE_QGROUP_CMD          = 10015;
    static const uint32_t WEBAGENTD_GET_QGROUP_USER_LIST_CMD   = 10016;
    static const uint32_t WEBAGENTD_SET_QGROUP_MANAGER_CMD     = 10017;
    static const uint32_t WEBAGENTD_RESET_QGROUP_CREATER_CMD   = 10018;
    static const uint32_t WEBAGENTD_CANCEL_QGROUP_MANAGER_CMD  = 10019;
//private cloud---end---

}

#endif // PROTOCOLS_H_


