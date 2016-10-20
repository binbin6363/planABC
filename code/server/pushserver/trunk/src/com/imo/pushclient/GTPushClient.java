/**
 * @file GTPushClient.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月14日-上午10:04
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>15-12-14	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.pushclient;


import com.gexin.rp.sdk.base.IPushResult;
import com.gexin.rp.sdk.base.impl.SingleMessage;
import com.gexin.rp.sdk.base.impl.Target;
import com.gexin.rp.sdk.base.uitls.LangUtil;
import com.gexin.rp.sdk.exceptions.RequestException;
import com.gexin.rp.sdk.http.IGtPush;
import com.gexin.rp.sdk.template.TransmissionTemplate;
import com.imo.msg.PushRequest;
import com.imo.util.ConstantValue;
import com.imo.util.DspStringUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import org.json.simple.parser.JSONParser;

import java.io.IOException;
import java.security.NoSuchAlgorithmException;
import java.util.*;
import java.util.concurrent.ConcurrentLinkedQueue;

public class GTPushClient extends AbstractPushClient{

    private static Logger logger = Logger.getLogger(GTPushClient.class);
    private static final Queue<PushRequest> pushQueue = new ConcurrentLinkedQueue<PushRequest>();

    private boolean initFlag = false;
    private IGtPush push = null;
    private JSONParser jsonParser = null;
    private Target target = null;
    //private LinkedHashMap<String, Object> jsonMap = null;
    TransmissionTemplate template = null;
    SingleMessage message = null;

    /**
     * 不在线程中处理，手动调用sendPushMsg接口
     */
    public GTPushClient() {
        this(false);
    }

    /**
     * 启用消息队列，在线程中处理，外层仅仅需要将消息加入到消息队列中即可，框架自动调用sendPushMsg接口
     */
    public GTPushClient(boolean thread){
        jsonParser = new JSONParser();
        target = new Target();
        target.setAppId(ConstantValue.GETUI_APP_ID);

        template = new TransmissionTemplate();
        template.setAppId(ConstantValue.GETUI_APP_ID);
        template.setAppkey(ConstantValue.GETUI_APP_KEY);
        // 透传消息设置，1为强制启动应用，客户端接收到消息后就会立即启动应用；2为等待应用启动。
        template.setTransmissionType(2);

        message = new SingleMessage();
        // 设置离线是否存储，默认为true
        message.setOffline(true);
        // 设置离线存储有效时间，单位是毫秒
        message.setOfflineExpireTime(ConstantValue.PUSH_EXPIRE_TIME * 1000);

        if (thread) {
            logger.info("start gtpush in thread, no need call sendPushMsg by hand.");
            new Thread(this).start();
        } else {
            logger.info("start gtpush not in thread, need call sendPushMsg by hand.");
        }
    }

    public Queue<PushRequest> getPushQueue() {
        return pushQueue;
    }

    @Override
    public boolean initService(Map<String, Object> properties) {
        super.initService(properties);
        push = new IGtPush(ConstantValue.GETUI_APP_KEY, ConstantValue.GETUI_MASTER_SECRET, true); // https
        try {
            initFlag = push.connect();
            logger.info("gtpush service init, init flag:"+initFlag);
        } catch (IOException e) {
            logger.info("gtpush service init failed.");
            logger.error(e.getMessage());
        }
        return initFlag;
    }

    @Override
    public PushRet sendPushMsg(PushRequest pushRequest) {
        PushRet pushRet = new PushRet();
        if (pushRequest == null) {
            logger.error("push request is null, push to gtpush failed.");
            pushRet.setStatus("push request is null, push to gtpush failed.");
            pushRet.setResultCode(ConstantValue.PARAM_IS_ERROR);
            return pushRet;
        }
        if (!initFlag){
            logger.error("gtpush service is not inited, push gtpush failed. " + pushRequest.toString());
            pushRet.setStatus("gtpush service is not inited, push gtpush failed.");
            pushRet.setResultCode(ConstantValue.SERVICE_NOT_INITED);
            return pushRet;
        }
        if (StringUtils.isEmpty(pushRequest.getToken())) {
            logger.error("gtpush tokn is empty, push gtpush failed. " + pushRequest.toString());
            pushRet.setStatus("gtpush tokn is empty,, push gtpush failed.");
            pushRet.setResultCode(ConstantValue.TOKEN_INVALID);
            return pushRet;
        }
        if (!setMessageData(pushRequest)) {
            logger.error("set message data failed. gtpush lost one push message. "+pushRequest.toString());
            pushRet.setStatus("set message data failed. gtpush lost one push message.");
            pushRet.setResultCode(ConstantValue.PARAM_IS_ERROR);
            return pushRet;
        }

        target.setAlias(pushRequest.getToken());
        IPushResult result = null;
        try {
            result = push.pushMessageToSingle(message, target);
        } catch (RequestException e) {
            result = null;
            e.printStackTrace();
            logger.error(e.getMessage());
        } finally {
            resultHandle(pushRet, result);
        }
        // 立刻进行gc
        result = null;
        pushRequest = null;
        return pushRet;
    }

    private boolean setMessageData(PushRequest pushRequest){
        String messageStr = pushRequest.getMsgMap().get("msgcontent");
        if (StringUtils.isEmpty(messageStr)) {
            logger.error("gtpush, push msg content is empty.");
            return false;
        }
        String passThroughMsg = DspStringUtils.makeJsonMessage(pushRequest.getTransId(), pushRequest.getMsgMap(), jsonParser);
        logger.debug("gtpush push msg to gt server, json msg:"+passThroughMsg);
        if (StringUtils.isEmpty(passThroughMsg)) {
            logger.error("make push jdon message failed");
            return false;
        }

        template.setTransmissionContent(passThroughMsg);

        // 设置推送消息消息内容
        message.setData(template);

        return true;
    }

    public TransmissionTemplate TransmissionTemplateDemo(String content) {
        TransmissionTemplate template = new TransmissionTemplate();
        template.setAppId(ConstantValue.GETUI_APP_ID);
        template.setAppkey(ConstantValue.GETUI_APP_KEY);
        // 透传消息设置，1为强制启动应用，客户端接收到消息后就会立即启动应用；2为等待应用启动。
        template.setTransmissionType(2);
        template.setTransmissionContent(content);
        // template.setDuration("2015-01-16 11:40:00", "2015-01-16 12:24:00");
        // template.setPushInfo("", 1, "", "", "", "", "", "");
        // template.getPushInfo().toString().toBytes().length;
        // template.getPushInfo()
        return template;
    }

    /**
     * 生成Sign方法
     */
    public String makeSign(String masterSecret, Map<String, Object> params) throws IllegalArgumentException {
        if (masterSecret == null || params == null) {
            throw new IllegalArgumentException("masterSecret and params can not be null.");
        }

        if (!(params instanceof SortedMap)) {
            params = new TreeMap<String, Object>(params);
        }

        StringBuilder input = new StringBuilder(masterSecret);
        for (Map.Entry<String, Object> entry : params.entrySet()) {
            Object value = entry.getValue();
            if (value instanceof String || value instanceof Integer || value instanceof Long) {
                input.append(entry.getKey());
                input.append(entry.getValue());
            }
        }

        return getMD5Str(input.toString());
    }

    /**
     * MD5加密
     */
    public String getMD5Str(String sourceStr) {
        byte[] source = sourceStr.getBytes();
        // 用来将字节转换成 16 进制表示的字符
        char hexDigits[] = new char[] {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        java.security.MessageDigest md = null;

        try {
            md = java.security.MessageDigest.getInstance("MD5");
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        }

        if (md == null) {
            return null;
        }

        md.update(source);
        byte tmp[] = md.digest(); // MD5 的计算结果是一个 128 位的长整数，
        // 用字节表示就是 16 个字节
        char str[] = new char[16 * 2]; // 每个字节用 16 进制表示的话，使用两个字符，
        // 所以表示成 16 进制需要 32 个字符
        int k = 0; // 表示转换结果中对应的字符位置
        for (int i = 0; i < 16; i++) {
            // 从第一个字节开始，对 MD5 的每一个字节
            // 转换成 16 进制字符的转换
            byte byte0 = tmp[i]; // 取第 i 个字节
            str[k++] = hexDigits[byte0 >>> 4 & 0xf]; // 取字节中高 4 位的数字转换,
            // >>> 为逻辑右移，将符号位一起右移
            str[k++] = hexDigits[byte0 & 0xf]; // 取字节中低 4 位的数字转换
        }
        return new String(str); // 换后的结果转换为字符串
    }

    /**
     * @param pushRet pushRet
     * @param result result
     * @return boolean
     */
    private boolean resultHandle(PushRet pushRet, IPushResult result){
        if (result == null) {
            logger.error("actually gtpush server abnormal.");
            pushRet.setResultCode(ConstantValue.SERVICE_IS_ABNORMAL);
            pushRet.setStatus("actually gtpush server abnormal.");
            return false;
        }
        Map<String, Object> resultObj = result.getResponse();
        if (resultObj != null) {
            logger.debug("done push gtpush msg, result:" + LangUtil.parseString(resultObj));
        } else {
            logger.error("gtpush, result is null, maybe failed.");
            pushRet.setResultCode(ConstantValue.SERVICE_IS_ABNORMAL);
            pushRet.setStatus("actually gtpush server abnormal.");
            return false;
        }

        pushRet.setResultCode("ok".equals(LangUtil.parseString(resultObj.get("result"))) ? 0 : ConstantValue.UNKNOWN_ERR0R);
        pushRet.setStatus(LangUtil.parseString(resultObj.get("status")));
        pushRet.setResultID(LangUtil.parseString(resultObj.get("taskId")));
        return true;
    }

} // class GTPushClient end
