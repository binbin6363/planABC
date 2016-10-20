/**
 * @file ConstantValue.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月02日-下午7:38
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>15-12-2	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.util;


public class ConstantValue {

    // common tag
    public static final String PUSH_TAG_TITLE = "title";
    public static final String PUSH_TAG_MSG = "msg";
    public static final String PUSH_TAG_SOUND = "sound";
    public static final String PUSH_TAG_VIBRATE = "vibrate";
    public static final String PUSH_TAG_TIME = "srv_time";
    public static final String PUSH_TAG_CB = "cb";
    public static final String PUSH_TAG_SRV_ID = "srv_id";
    public static final String PUSH_TAG_EXTRAS = "extras";

    // 时间以毫秒计算
    public static final long SERVER_START_TIME = System.currentTimeMillis();

    // server config
    public static int SERVER_PORT = 8899;
    public static boolean OPEN_HTTP = true;
    public static int HTTP_SERVER_PORT = 80;
    public static int MAX_PUSH_QUEUE_NUM = 200000; // max push msg queue default is 20w

    public static String PUSH_CB_LINK = "http://count.imoffice.com/mobiledReport.php";
    public static int    PUSH_EXPIRE_TIME = 900; // unit in second, default is 15 min
    public static String PUSH_TITLE = "屌丝派";
    public static String PUSH_PACKAGE_NAME = "com.dsp";
    public static boolean UPLOAD_FLAG = false;
    public static int PUSH_ACCESS_TOKEN_EXPIRE_TIME = 604800; // unit in second, default is 7day

    public static final int PUSH_FLAG = 1;
    // 小米平台推送标志
    public static final int XMPlatform = 1 << 1;
    // 华为平台推送标志
    public static final int HWPlatform = 1 << 2;
    // 个推平台推送标志
    public static final int GTPlatform = 1 << 3;
    // 所有平台推送标志
    public static final int ALLPlatform = -1;

    public static final int SRV_CONNECTION_TIMEOUT = 8000;
    public static final int SRV_READ_TIMEOUT = 5000;

    // TRY STRATEGY
    public final static int NOT_RETRY = 0;
    public final static int TRY_ONCE = 1;
    public final static int TRY_TWICE = 2;

    // platform
    public final static String HUAWEI = "huawei";
    public final static String XIAOMI = "xiaomi";
    public final static String XINGE = "xinge";
    public final static String GEITUI = "getui";
    public static String PLATFORM = XIAOMI;

    public final static int XIAOMI_PLAT = 1;
    public final static int HUAWEI_PLAT = 2;
    public final static int GETUI_PLAT = 3;
    public final static int XINGE_PLAT = 4;
    public final static int MAX_PLAT = 5;

    // huawei push type
    public final static int SPECIFY_USER = 1;
    public final static int DEV_ANDROID = 1;
    public final static int OPEN_APP = 1;

    public static int PASSDEFAULT = 1;
    // passThrough type
    public static int MIPUSH_NOTIFICATION = 0; // 通知
    public static int MIPUSH_PASSTHROUGH = 1;  // 透传
    // notify type
    public static int MIPUSH_DEFAULT_ALL = -1;
    public static int MIPUSH_DEFAULT_SOUND  = 1;   // 使用默认提示音提示
    public static int MIPUSH_DEFAULT_VIBRATE = 2;   // 使用默认震动提示
    public static int MIPUSH_DEFAULT_LIGHTS = 4;    // 使用默认led灯光提示
    public static int LOOP_SEND_NUM = 1;
    public static int SendNum = 0;       // 已经发送的push次数，作为统计

    // xiao mi
    public final static String XIAOMI_APP_SECRET_KEY = "EHNGHhz7vbM5REYiLsz6Cw==";

    public static String ToRegId = null;
    public static String ToAlias = null;

    // hua wei
    // APP ID：1050766    |   应用包名：com.imo    |   APP SECRET：4nlhn1lwm78en8of43vomfd8pca0biw4
    public static String HUAWEI_APP_ID = "1050766";
    public static String HUAWEI_APP_SECRET_KEY = "4nlhn1lwm78en8of43vomfd8pca0biw4";
    public static String HUAWEI_KEY_FILE = "/mykeystorebj.jks";//resource
    public final static String HUAWEI_CREDENTIALS = "client_credentials";//resource
    public final static String HUAWEI_PASSWD = "123456";
    public final static Integer HUAWEI_NOTIFY_TYPE = 1;

    public final static String SINGLE_PUSH_SERVICE = "openpush.message.single_send";
    public final static String BATCH_PUSH_SERVICE = "openpush.message.batch_send";
    public final static String LBS_SERVICE = "openpush.openapi.lbs_send";
    public final static String NOTIFICATION_SERVICE = "openpush.openapi.notification_send";
    public final static String SET_USER_TAG_SERVICE = "openpush.openapi.set_user_tag";
    public final static String DELETE_USER_TAG_SERVICE = "openpush.openapi.delete_user_tag";
    public final static String QUERY_USER_TAG_SERVICE = "openpush.openapi.query_user_tag";
    public final static String QUERY_APP_TAGS_SERVICE = "openpush.openapi.query_app_tags";
    public final static String QUERY_MSG_RESULT_SERVICE = "openpush.openapi.query_msg_result";
    public final static String QUERY_TOKEN_BY_DATE_SERVICE = "openpush.openapi.get_token_by_date";

    // ge tui
    public static String GETUI_SERVICE_URL = "http://sdk.open.api.igexin.com/apiex.htm";
    public static String GETUI_APP_ID = "UlgeWBKTjx6os5mNPbvmb1";
    public static String GETUI_APP_KEY = "ey8UqeadrJ7KnRR25BQfm6";
    public static String GETUI_APP_SECRET = "0aXRVRjiXd7yDOlBFcB5v6";
    public static String GETUI_MASTER_SECRET = "3EAK7hk8F4ASGcvhCDVDOA";
    // MSG TYPE
    public final static String GETUI_TRANSMISSION_TYPE = "TransmissionMsg";
    public final static String GETUI_LINK_TYPE = "LinkMsg";
    public final static String GETUI_NOTIFY_TYPE = "NotifyMsg";

    // 给华为用户发送的列表
    public static String[] hwTokenList = null;
    // 连接数目
    public static int ConnectionNum = 5;
    public static boolean ThreadFlag = true;

    // redis使用的db索引
    public static int DB_INDEX = 0;
    // redis存储全局队列的key
    public static String MASTER_KEY = "task_global_list_key";

    public static String LOCAL_FILE_PATH = "/tmp/.push_consumer_task_key";
    public static String LOCAL_FILE_ENCODE = "utf-8";
    public static String TASK_KEY_PERFIX = "task";

    // error code
    public static int ERR_NO_TASK = 1000;
    public static int ERR_TASK_CELL_EMPTY = 1001;
    public static int ERR_DONE_TASK_OK = 1002;
    public static String REDIS_HOST = "localhost";
    public static int REDIS_PORT = 6379;

    // err code
    public static int SERVICE_NOT_INITED  = 500;
    public static int SERVICE_IS_ABNORMAL = 501;
    public static int PARAM_IS_ERROR      = 600;
    public static int TOKEN_INVALID       = 601;
    public static int UNKNOWN_ERR0R       = 700;



} // class ConstantValue end
