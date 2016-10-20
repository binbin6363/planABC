/**
 * @file HWPushClient.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月02日-下午8:53
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>15-12-2	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.pushclient;

import com.gexin.rp.sdk.base.uitls.LangUtil;
import com.imo.msg.PushRequest;
import com.imo.util.ConstantValue;
import com.imo.util.DspStringUtils;
import nsp.NSPClient;
import nsp.OAuth2Client;
import nsp.support.common.AccessToken;
import nsp.support.common.NSPException;
import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import org.json.simple.parser.JSONParser;

import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.*;
import java.util.concurrent.ConcurrentLinkedQueue;

/**
 * 华为的push接口服务
 */
public class HWPushClient extends AbstractPushClient {

    private static Logger logger = Logger.getLogger(HWPushClient.class);
    private static final String TIMESTAMP_NORMAL = "yyyy-MM-dd HH:mm:ss";
    private static final SimpleDateFormat dataFormat = new SimpleDateFormat(TIMESTAMP_NORMAL);
    private static final Queue<PushRequest> pushQueue = new ConcurrentLinkedQueue<PushRequest>();

    private boolean initFlag = false;
    private NSPClient nspClient = null;
    private JSONParser jsonParser = null; // 每个实例一个jsonParser，不可公用
    private HashMap<String, Object> msgHashMap = null;

    /**
     * 不在线程中处理，手动调用sendPushMsg接口
     */
    public HWPushClient() {
        this(false);
    }

    /**
     * 启用消息队列，在线程中处理，外层仅仅需要将消息加入到消息队列中即可，框架自动调用sendPushMsg接口
     */
    public HWPushClient(boolean thread) {
        jsonParser = new JSONParser();
        msgHashMap = new HashMap<String, Object>();
        if (thread) {
            logger.info("start hwpush in thread, no need call sendPushMsg by hand.");
            new Thread(this).start();
        } else {
            logger.info("start hwpush not in thread, need call sendPushMsg by hand.");
        }
    }



    /**
     * 如果消息发送成功，服务器返回消息的ID；如果发送失败，返回null
     * 发送消息返回的错误码，如果返回ErrorCode.Success表示发送成功，其他表示发送失败
     * 成功返回true，反之返回false
     * @param pushRet
     * @param result
     * @return boolean
     */
    private boolean resultHandle(PushRet pushRet, String result){
        if (result == null) {
            logger.error("actually hwpush server abnormal.");
            pushRet.setResultCode(ConstantValue.SERVICE_IS_ABNORMAL);
            pushRet.setStatus("actually hwpush server abnormal.");
            return false;
        }
        int err_code = 0;
        Map<String, Object> responseMap = DspStringUtils.jsonStr2ObjMap(result);
        if (responseMap != null) {
            Long err_code2 = (Long)responseMap.get("resultcode");
            err_code = err_code2.intValue();
            pushRet.setResultID(LangUtil.parseString(responseMap.get("requestID")));
            pushRet.setStatus(LangUtil.parseString(responseMap.get("message")));
        } else {
            logger.warn("get err code failed, response map is null. response str:"+result);
            err_code = ConstantValue.SERVICE_IS_ABNORMAL;
            pushRet.setStatus("hwpush server abnormal.");
        }
        pushRet.setResultCode(err_code);
        logger.debug("actually done push hwpush, result:" + result);
        return true;
    }

    public boolean initService(Map<String, Object> properties) {
        super.initService(properties);
        try {

            OAuth2Client oauth2Client = new OAuth2Client();
            logger.info("init key store stream. key:" + ConstantValue.HUAWEI_KEY_FILE);
            // debug
            //oauth2Client.initKeyStoreStream(new FileInputStream(new File("mykeystorebj.jks")), ConstantValue.HUAWEI_PASSWD);
            InputStream keyStoreStream = HWPushClient.class.getResource(ConstantValue.HUAWEI_KEY_FILE).openStream();
            oauth2Client.initKeyStoreStream(keyStoreStream, ConstantValue.HUAWEI_PASSWD);
            keyStoreStream.close();
            AccessToken accessToken = oauth2Client.getAccessToken(ConstantValue.HUAWEI_CREDENTIALS, ConstantValue.HUAWEI_APP_ID, ConstantValue.HUAWEI_APP_SECRET_KEY);
            accessToken.setExpires_in(ConstantValue.PUSH_ACCESS_TOKEN_EXPIRE_TIME);
            logger.info("access token :" + accessToken.getAccess_token() + ",expires time[access token 过期时间]:"
                    + accessToken.getExpires_in());

            nspClient = new NSPClient(accessToken.getAccess_token());
            nspClient.initHttpConnections(30, 50);//设置每个路由的连接数和最大连接数
            // debug
            //nspClient.initKeyStoreStream(new FileInputStream(new File("mykeystorebj.jks")), ConstantValue.HUAWEI_PASSWD);//如果访问https必须导入证书流和密码
            InputStream keyStoreStream2 = HWPushClient.class.getResource(ConstantValue.HUAWEI_KEY_FILE).openStream();
            nspClient.initKeyStoreStream(keyStoreStream2, ConstantValue.HUAWEI_PASSWD);//如果访问https必须导入证书流和密码
            //设置http超时时间
            nspClient.setTimeout(5000, 5000);
            keyStoreStream2.close();
            initFlag = true;
        } catch (NSPException e) {
            logger.error(e.getMessage());
        } catch (IOException e) {
            logger.error(e.getMessage());
        }
        logger.info("hwpush service init, init flag:"+initFlag);
        if (!initFlag) {
            logger.error("init hwpush service failed, stop thread.");
            stop();
        }
        return initFlag;
    }

    private int getErrcode(String response){
        int err_code = 0;
        if (StringUtils.isEmpty(response)) {
            logger.warn("get err code failed, response is empty");
            err_code = ConstantValue.SERVICE_IS_ABNORMAL;
            return err_code;
        }
        Map<String, Object> responseMap = DspStringUtils.jsonStr2ObjMap(response);
        if (responseMap != null) {
            Long err_code2 = (Long)responseMap.get("resultcode");
            err_code = err_code2.intValue();
        } else {
            logger.warn("get err code failed, response map is null. response str:"+response);
            err_code = ConstantValue.SERVICE_IS_ABNORMAL;
        }
        return err_code;
    }

    public PushRet sendPushMsg(PushRequest pushRequest){
        PushRet pushRet = new PushRet();
        if (pushRequest == null) {
            logger.error("push request is null, push to hwpush failed.");
            pushRet.setStatus("push request is null, push to hwpush failed.");
            pushRet.setResultCode(ConstantValue.SERVICE_IS_ABNORMAL);
            return pushRet;
        }
        if (!initFlag) {
            logger.error("hwpush service not init.");
            pushRet.setStatus("hwpush service is not inited, push hwpush failed.");
            pushRet.setResultCode(ConstantValue.SERVICE_NOT_INITED);
            return pushRet;
        }
        if (pushRequest.getMsgMap() == null) {
            logger.error("hwpush, push msg is null.");
            pushRet.setStatus("hwpush, push msg is null.");
            pushRet.setResultCode(ConstantValue.PARAM_IS_ERROR);
            return pushRet;
        }

        //目标用户，必选。
        //由客户端获取， 32 字节长度。手机上安装了push应用后，会到push服务器申请token，申请到的token会上报给应用服务器
        String token = pushRequest.getToken();
        if (StringUtils.isEmpty(token)) {
            logger.error("hwpush, token is null.");
            pushRet.setStatus("hwpush, token is null.");
            pushRet.setResultCode(ConstantValue.TOKEN_INVALID);
            return pushRet;
        }

        //unix时间戳，可选
        //格式：2013-08-29 19:55
        // 消息过期删除时间
        //如果不填写，默认超时时间为当前时间后48小时
        // 3小时之后就超时
        long currentTime = System.currentTimeMillis();
        String expire_time = dataFormat.format(currentTime + ConstantValue.PUSH_EXPIRE_TIME * 1000);
        logger.debug("show hwpush expire_time:"+expire_time);

        //构造请求
        msgHashMap.clear();
        msgHashMap.put("deviceToken", token);
        String jsonMsgStr = DspStringUtils.makeJsonMessage(pushRequest.getTransId(), pushRequest.getMsgMap(), jsonParser);
        msgHashMap.put("message", jsonMsgStr);
        msgHashMap.put("priority", 0);
        msgHashMap.put("cacheMode", 0); // 不缓存消息
        msgHashMap.put("msgType", 1);
        msgHashMap.put("expire_time", expire_time);

        //接口调用
        String resp = null;
        try {
            // 绕过华为sdk的bug，首次setTimeout会失败，后续才会设置成功
            nspClient.setTimeout(5000, 5000);
            logger.debug("hwpush push msg to hw server. "+pushRequest.toString()+", json msg:"+jsonMsgStr);
            resp = nspClient.call(ConstantValue.SINGLE_PUSH_SERVICE, msgHashMap, String.class);
            resultHandle(pushRet, resp);
        } catch (NSPException e) {
            int ret_code = getErrcode(resp);
            if (NSPException.ACCESSTOKEN_SESSION_TIMEOUT == e.getCode() ||  102 == ret_code) {
                logger.info("access token expire, try get token and init service again.");
                initService(null);
                try {
                    // 绕过华为sdk的bug，首次setTimeout会失败，后续才会设置成功
                    nspClient.setTimeout(5000, 5000);
                    resp = nspClient.call(ConstantValue.SINGLE_PUSH_SERVICE, msgHashMap, String.class);
                    resultHandle(pushRet, resp);
                } catch (NSPException e1) {
                    pushRet.setResultCode(ConstantValue.SERVICE_IS_ABNORMAL);
                    pushRet.setResultID("");
                    pushRet.setStatus("hwpush sendPushMsg exception.");
                    logger.error(e1.getMessage());
                }
            } else {
                logger.error("hwpush sendPushMsg exception.");
                pushRet.setResultCode(ConstantValue.SERVICE_IS_ABNORMAL);
                pushRet.setResultID("");
                pushRet.setStatus("hwpush sendPushMsg exception.");
                logger.error(e.getMessage());
            }
        }

        return pushRet;
    }

    @Override
    public Queue<PushRequest> getPushQueue() {
        return pushQueue;
    }

} // class HWPushClient end
