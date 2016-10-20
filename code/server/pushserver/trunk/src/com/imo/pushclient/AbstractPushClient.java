/**
 * @file AbstractPushClient.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月12日-下午4:06
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-12	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.pushclient;

import com.gexin.rp.sdk.base.uitls.LangUtil;
import com.imo.msg.PushRequest;
import com.imo.net.NetClient;
import com.imo.util.ConstantValue;
import com.imo.util.StatisticsInfo;
import org.apache.log4j.Logger;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Queue;

public class AbstractPushClient implements PushClient, Runnable {
    private static Logger logger = Logger.getLogger(AbstractPushClient.class);
    private final Object obj = new Object();
    private boolean stop = false;
    private static final PushRet defaultRet = new PushRet();
    private Map<String, Object> properties = new HashMap<String, Object>();

    public void stop(){
        stop = true;
    }

    @Override
    public boolean initService(Map<String, Object> properties) {
        if (properties != null) {
            this.properties = properties;
        }
        //logger.error("push client not implement initService, call base method!");
        return false;
    }


    @Override
    public PushRet sendPushMsg(PushRequest pushRequest) {
        logger.error("push client not implement sendPushMsg, call base method!");
        defaultRet.setResultCode(100);
        defaultRet.setResultID("0");
        defaultRet.setStatus("call base method sendPushMsg");
        return defaultRet;
    }

    @Override
    public Queue<PushRequest> getPushQueue() {
        logger.error("push client not implement getPushQueue, call base method!");
        return null;
    }

    @Override
    public int addRequest(PushRequest pushRequest) {
        String platform_name = LangUtil.parseString(properties.get("name"));
        Queue<PushRequest> queue = getPushQueue();
        if (null == queue) {
            logger.error("queue is not inited.");
            return -1;
        }
        if (queue.size() > ConstantValue.MAX_PUSH_QUEUE_NUM) {
            logger.error("push msg queue is too large, platform:"+platform_name+", current size:"+queue.size()+", lost one push. "+pushRequest.toString());
        } else {
            logger.info("add one push request. platform:"+platform_name+", transid:"+pushRequest.getTransId()+", queue current size:"+queue.size());
            queue.add(pushRequest);
            StatisticsInfo.getInst().recordPush(platform_name, 0, 0, queue.size());
        }

        synchronized (obj) {
            obj.notify();
        }
        return 0;
    }

    @Override
    public void run() {
        logger.info("run in thread, init push service.");
        String platform_name = LangUtil.parseString(properties.get("name"));
        while (true) {
            Queue<PushRequest> pushRequestQueue = getPushQueue();
            if (null == pushRequestQueue) {
                logger.error("push request queue is null, exit thread...");
                return;
            }
            if (pushRequestQueue.isEmpty()) {
                try {
                    synchronized (obj) {
                        obj.wait();
                    }
                } catch (InterruptedException e) {
                    logger.error(e.getMessage());
                    continue;
                }
            }
            if (stop) {
                logger.info("exit thread...");
                return;
            }
            PushRequest pushRequest = pushRequestQueue.poll();
            if (null != pushRequest) {
                logger.info("pop one push request. transid:"+pushRequest.getTransId()+", platform name:"+platform_name);
                long timeStart = System.currentTimeMillis();
                PushRet pushRet = null;
                try {
                    pushRet = sendPushMsg(pushRequest);
                } catch (Exception e) {
                    long timeEnd = System.currentTimeMillis();
                    timeStart = (timeEnd - timeStart);
                    logger.error("actually pop one push message failed. catch exception: "+e.getMessage());
                    StatisticsInfo.getInst().recordPush(platform_name, timeStart, -1, pushRequestQueue.size());
                    continue;
                }
                long timeEnd = System.currentTimeMillis();
                timeStart = (timeEnd - timeStart);
                if (pushRet == null || pushRet.getResultCode() != 0) {
                    StatisticsInfo.getInst().recordPush(platform_name, timeStart, -1, pushRequestQueue.size());
                    logger.error("actually pop one push message failed. request info:" + pushRequest.toString());
                    if (pushRet != null) {
                        logger.error("actually pop one push message failed. result info:"+pushRet.toString());
                    }
                } else {
                    StatisticsInfo.getInst().recordPush(platform_name, timeStart, 0, pushRequestQueue.size());
                    logger.info("actually pop one push message ok, cid:"+pushRequest.getCid()+", uid:"+pushRequest.getUid()
                            + ", transid:" + pushRequest.getTransId() + ". " + pushRet.toString());
                    pushRequest.setRequestId(pushRet.getResultID());
                    uploadPushRecord(pushRequest);
                }
                pushRequest = null;
                pushRet = null;
            }
        }
    }

    private void uploadPushRecord(PushRequest pushRequest) {
        if (ConstantValue.UPLOAD_FLAG) {
            logger.info("push to server ok, record it to url:"+ ConstantValue.PUSH_CB_LINK);
            NetClient.uploadPushInfo(ConstantValue.PUSH_CB_LINK, pushRequest);
        }
    }
} // class AbstractPushClient end
