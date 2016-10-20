/**
 * @file PushClientMgr.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月02日-下午9:09
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

import com.imo.util.ConstantValue;
import com.imo.util.DspStringUtils;
import org.apache.log4j.Logger;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class PushClientMgr {

    private static Logger logger = Logger.getLogger(PushClientMgr.class);
    //private static PushClient client = null;
    // 2,hwpush;3,gtpush,...
    private static int[] serviceHashCnt = new int[ConstantValue.MAX_PLAT];

    private static boolean hwInitFlag = false;
    private static List<PushClient> hwClientList = new ArrayList<PushClient>();
    private static final Object hwInitObj = new Object();

    private static boolean gtInitFlag = false;
    private static List<PushClient> gtClientList = new ArrayList<PushClient>();
    private static final Object gtInitObj = new Object();


    static {
        for (int i = 0; i < serviceHashCnt.length; ++i) {
            serviceHashCnt[i] = 0;
        }
    }

    public PushClientMgr(){

    }

    public static PushClient getClient(String paltform) {
        logger.info("get client, platform:" + paltform);
        PushClient client = null;
        if (DspStringUtils.IsEqual(ConstantValue.HUAWEI, paltform)) {
            client = new HWPushClient();
        } else if (DspStringUtils.IsEqual(ConstantValue.GEITUI, paltform)) {
            client = new GTPushClient();
        }
        return client;
    }

    public static PushClient getClient(int pushTpye) {
        logger.info("get client, pushTpye:" + pushTpye);
        PushClient client = null;
        switch (pushTpye) {
            case ConstantValue.HUAWEI_PLAT: {
                lazyInitHWPush();
                client = getHWPushClientFromPool();
            }
            break;
            case ConstantValue.GETUI_PLAT: {
                lazyInitGTPush();
                client = getGTPushClientFromPool();
            }
            break;
            default: {
                logger.error("unknown pushTpye:"+pushTpye);
            }
            break;
        }
        return client;
    }

    private static PushClient getGTPushClientFromPool() {
        int platform = ConstantValue.GETUI_PLAT;
        if (platform >= serviceHashCnt.length) {
            logger.error("get gtclient failed.");
            return null;
        }
        int hash = serviceHashCnt[platform] % ConstantValue.ConnectionNum;
        if (hash >= gtClientList.size()) {
            hash = 0;
            logger.warn("get hash id failed, select the first gtclient.");
        }
        ++serviceHashCnt[platform];
        return gtClientList.get(hash);
    }

    private static int lazyInitGTPush() {
        int ret = 0;
        // lazy init gtclient
        if (!gtInitFlag) {
            synchronized (gtInitObj) {
                logger.debug("enter lazy init gtpush.");
                if (!gtInitFlag) {
                    Map<String, Object> pushProperties = new HashMap<String, Object>();
                    pushProperties.put("name", "getui");
                    gtInitFlag = true;
                    PushClient client = null;
                    for (int i = 0; i < ConstantValue.ConnectionNum; ++i) {
                        client = new GTPushClient(ConstantValue.ThreadFlag);

                        if (client.initService(pushProperties)) {
                            gtClientList.add(client);
                        } else {
                            logger.warn("init gtclient failed.");
                        }
                    }
                    logger.info("done init gtclient.");
                } else {
                    logger.debug("gtpush init already.");
                }
            }
        }
        return ret;
    }

    /**
     * 此函数处理比较轻量，直接使用synchronized
     * @return
     */
    private static synchronized PushClient getHWPushClientFromPool() {
        int platform = ConstantValue.HUAWEI_PLAT;
        if (platform >= serviceHashCnt.length) {
            logger.error("get hwclient failed.");
            return null;
        }
        int hash = serviceHashCnt[platform] % ConstantValue.ConnectionNum;
        if (hash >= hwClientList.size()) {
            hash = 0;
            logger.warn("get hash id failed, select the first hwclient.");
        }
        ++serviceHashCnt[platform];
        return hwClientList.get(hash);
    }

    private static int lazyInitHWPush() {
        int ret = 0;
        // lazy init hwclient
        if (!hwInitFlag) {
            synchronized (hwInitObj) {
                logger.debug("enter lazy init hwpush.");
                if (!hwInitFlag) {
                    Map<String, Object> pushProperties = new HashMap<String, Object>();
                    pushProperties.put("name", "huawei");
                    hwInitFlag = true;
                    PushClient client = null;
                    for (int i = 0; i < ConstantValue.ConnectionNum; ++i) {
                        client = new HWPushClient(ConstantValue.ThreadFlag);
                        if (client.initService(pushProperties)) {
                            hwClientList.add(client);
                        } else {
                            logger.warn("init hwclient failed.");
                        }
                    }
                    logger.info("done init hwclient.");
                } else {
                    logger.debug("hwpush init already.");
                }
            }
        }
        return ret;
    }

} // class PushClientMgr end
