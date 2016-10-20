/**
 * @file PushClientTool.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月02日-下午3:02
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>15-12-2	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.pushtool;


import com.imo.pushclient.PushClient;
import com.imo.util.ConstantValue;
import org.apache.commons.cli.*;
import org.apache.log4j.Logger;

import java.io.*;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;

public class PushClientTool {

    private static Logger logger = Logger.getLogger(PushClientTool.class);
    private static Options options = null;
    private static CommandLine cmd = null;

    // pushclient args
    private static String title = null;
    private static String context = null;
    private static String payload = null;
    private static Date timeToSend = null;
    private static Map<String, String> extras = null;
    private static PushClient pushApp = null;
    // multi platform
    private PushClient hwClient = null;
    private PushClient xmClient = null;
    private PushClient gtClient = null;

    //private static String platform = null;
    private static SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss SSS");

    private static boolean loadPushInfoFromArgs() {
        extras = new HashMap<String, String>(10);
        // parse cmd
        if (cmd.hasOption('t')) {
            title = cmd.getOptionValue('t');
            logger.debug("title:" + title);
        }
        if (cmd.hasOption('c')) {
            context = cmd.getOptionValue('c');
            logger.debug("context:" + context);
        }
        if (cmd.hasOption('p')) {
            payload = cmd.getOptionValue('p');
            logger.debug("payload:" + payload);
        }
        if (cmd.hasOption('e')) {
            String extStr = cmd.getOptionValue('e');
            logger.debug("extras:"+extStr);
            String[] kv = extStr.split(";");
            for (String aKv : kv) {
                String k , v;
                String[] subKv = aKv.split(":");
                if (2 != subKv.length) {
                    logger.warn("extras format is error, str:"+aKv);
                    continue;
                }
                k = subKv[0];
                v = subKv[1];
                extras.put(k, v);
                logger.debug("extras, k:" + k + ", v:" + v);
            }
        }
        if (cmd.hasOption('l')) {
            ConstantValue.LOOP_SEND_NUM = Integer.valueOf(cmd.getOptionValue('l'));
            logger.debug("loop send pushclient num:" + ConstantValue.LOOP_SEND_NUM);
        }
        if (cmd.hasOption('P')) {
            ConstantValue.PLATFORM = cmd.getOptionValue('P');
            logger.debug("use pushclient platform:" + ConstantValue.PLATFORM);
        }
        if (cmd.hasOption("alias")) {
            ConstantValue.ToAlias = cmd.getOptionValue("alias");
            logger.debug("pushclient to alias:" + ConstantValue.ToAlias);
        }
        if (cmd.hasOption("regid")) {
            ConstantValue.ToRegId = cmd.getOptionValue("regid");
            logger.debug("pushclient to alias:" + ConstantValue.ToRegId);
        }
        return true;
    }

    public int initService(){
        //hwClient = new HWPushClient();
        if (null != hwClient && !hwClient.initService(null)) {
            logger.error("init hwClient failed.");
            return -1;
        }
        /*
        xmClient = new MiPushClient();
        if (!xmClient.initService(null)) {
            logger.error("init xmClient failed.");
            return -1;
        }
        */
        //gtClient = new GTPushClient();
        if (null != gtClient&&!gtClient.initService(null)) {
            logger.error("init gtClient failed.");
            return -1;
        }
        logger.info("init all platform pushclient service succeed.");
        return 0;
    }

    public int sendPushMessage(int platform, int sendNum, String title, String content, Map<String, String> tokeMap){
        boolean ret = true;
        if (sendNum > 10000) {
            logger.error("send num is too bigger!!! not send pushclient! send num:"+sendNum);
            return -1;
        }
        if (platform == ConstantValue.ALLPlatform) {
            ret = sendAllPlatform(sendNum, title, content, tokeMap);
            logger.info("done send all platform pushclient, ret:" + ret);
            return 0;
        }
        if (0 != (platform & ConstantValue.XMPlatform)) {
            for (int i = 0; i < sendNum; ++i) {
                ret &= send(xmClient, title, content, tokeMap.get("xiaomi"));
            }
            logger.info("done send xiaomi pushclient, ret:" + ret);
        }
        if (0 != (platform & ConstantValue.HWPlatform)) {
            for (int i = 0; i < sendNum; ++i) {
                ret &= send(hwClient, title, content, tokeMap.get("huawei"));
            }
            logger.info("done send huawei pushclient, ret:" + ret);
        }
        if (0 != (platform & ConstantValue.GTPlatform)) {
            for (int i = 0; i < sendNum; ++i) {
                ret &= send(gtClient, title, content, tokeMap.get("getui"));
            }
            logger.info("done send getui pushclient, ret:" + ret);
        }
        return 0;
    }

    @Deprecated
    private boolean send(PushClient client, String title, String content, String token){
        long cur_time = System.currentTimeMillis();
        ++ConstantValue.SendNum;
        if (ConstantValue.SendNum % 100 == 0) {
            logger.info("send pushclient msg num:"+ConstantValue.SendNum);
        }
        String msg = cur_time + "#" + ConstantValue.SendNum + " # " + content + "#" + sdf.format(cur_time);
        // 作为测试，向所有用户广播
        //return client.broadcastPushMsg(title, msg, null, null, null);
        Map<String, String> extras = new HashMap<String, String>();
        extras.put("token", token);
        logger.error("call no using");
        return false;//client.sendPushMsg(title, msg, null, null, extras);
    }

    private boolean sendAllPlatform(int sendNum, String title, String content, Map<String, String> tokeMap){
        boolean ret = true;
        for (int i = 0; i < sendNum; ++i) {
            ret &= send(xmClient, title, content, tokeMap.get("xiaomi"));//xmClient.sendPushMsg(title, content, null, null, null);
            ret &= send(hwClient, title, content, tokeMap.get("huawei"));//hwClient.sendPushMsg(title, content, null, null, null);
            ret &= send(gtClient, title, content, tokeMap.get("getui"));//gtClient.sendPushMsg(title, content, null, null, null);
        }
        return ret;
    }

    public static void main(String[] args) throws InterruptedException, FileNotFoundException {
/*
        PushClientTool pushServer = new PushClientTool();

        int retValue = 0;
        makeCmdOptions();
        try {
            parseCmdOptions(args);
            if (cmd.hasOption('h')) {
                HelpFormatter formatter = new HelpFormatter();
                formatter.printHelp( "pushDemo", options );
                return;
            }
            if (!cmd.hasOption('f')) {
                Log4jCfg.initDefaultLog4j();
            } else if (cmd.hasOption('f')) {
                String propertiesFile = cmd.getOptionValue('f');
                Log4jCfg.initLog4jFromCfg(propertiesFile);
            }
        } catch (org.apache.commons.cli.ParseException e) {
            e.printStackTrace();
            return;
        }

        boolean ret = loadPushInfoFromArgs();
        if (!ret) {
            logger.error("load pushclient info failed.");
            return ;
        }

        pushApp = PushClientMgr.getClient(ConstantValue.PLATFORM);
        if (!pushApp.initService(null)) {
            logger.error("init service failed.");
            return;
        }

        pushServer.initService();

        logger.info("start send pushclient msg, title:" + title + ", content:" + context);
        for (int i = 0; i < ConstantValue.LOOP_SEND_NUM; ++i) {
            long cur_time = System.currentTimeMillis();
            extras.put("send_time", String.valueOf(cur_time));
            ++ConstantValue.SendNum;
            String msg = cur_time + "#" + ConstantValue.SendNum + " # " + context + "#" + sdf.format(cur_time);
            if (ConstantValue.SendNum % 100 == 0) {
                logger.info("send pushclient msg num:"+ConstantValue.SendNum);
            }
            //ret = pushApp.sendPushMsg(title, msg, payload, timeToSend, extras);
// int sendPushMessage(int platform, int sendNum, String title, String content, Map<String, String> tokeMap){
            retValue = pushServer.sendPushMessage((1<<1), 1, "mipush", msg, extras);
            long cur_time2 = System.currentTimeMillis();
            logger.info("start time:" + cur_time + ", end time:" + cur_time2 + ", cost:" + (cur_time2 - cur_time));
            //Thread.sleep(1);
        }
        logger.info("end send pushclient msg, ret:" + retValue);
        */
    }

    private static boolean makeCmdOptions() {
        // create Options object
        options = new Options();
        // add t option
        options.addOption("t", true, "pushclient msg title");
        options.addOption("c", true, "pushclient msg content string");
        options.addOption("e", true, "pushclient msg extras, format: k:v;k;v:n;...");
        options.addOption("p", true, "pushclient msg payload");
        options.addOption("h", false, "show detail help");
        options.addOption("f", true, "log properties file");
        options.addOption("s", true, "pushclient server secret key");
        options.addOption("l", true, "pushclient msg loop num, default 1");
        options.addOption("P", true, "pushclient platform, 'xiaomi','huawei','xinge'. default 'xiaomi'");
        options.addOption("alias", true, "for xiaomi, pushclient to device alias");
        options.addOption("regid", true, "for xiaomi, pushclient to device regid");
        options.addOption("token", true, "for huawei, user token");
        options.addOption("clientid", true, "for getui, client id");
        return true;
    }

    private static boolean parseCmdOptions(String[] args) throws org.apache.commons.cli.ParseException{
        CommandLineParser parser = new DefaultParser();
        cmd = parser.parse( options, args);
        return true;
    }

} // class PushClientTool end
