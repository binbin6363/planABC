/**
 * @file PushServer.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月26日-上午11:45
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-26	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo;

import com.imo.client.ConsumerMgr;
import com.imo.client.ConsumerThread;
import com.imo.net.NetServer;
import com.imo.util.*;
import org.apache.commons.cli.*;
import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;

import java.io.*;
import java.util.Properties;

public class PushServer {

    private static Logger logger = Logger.getLogger(PushServer.class);
    private static Options options = null;
    private static CommandLine cmd = null;
    private static String configFilePath = "";
    private static boolean firstInitConfig = true;
    private static ConsumerThread consumerThread = null;


    public static String getConfigFilePath() {
        return configFilePath;
    }


    private static boolean parseArgs(String[] args){
        boolean ret = true;
        try {
            parseCmdOptions(args);
            if (cmd.hasOption('h')) {
                HelpFormatter formatter = new HelpFormatter();
                formatter.printHelp(args[3], options );
                return false;
            }
            if (cmd.hasOption('v')) {
                String versionInfo = JarUtils.getVersionInfo(null);
                System.out.println(versionInfo);
                return false;
            }
            if (cmd.hasOption('f')) {
                String configFile = cmd.getOptionValue('f');
                configFilePath = configFile;
                ret = loadConfig();
            } else {
                System.err.println("load config failed, not config");
                ret = false;
            }
        } catch (org.apache.commons.cli.ParseException e) {
            e.printStackTrace();
            System.err.println("start service failed, parse exception, exit.");
            ret = false;
        }
        logger.info("done load all config.");
        return ret;
    }


    private static boolean parseServerConfig(Properties properties) {
        System.out.println("show config:"+properties.toString());
        String value;
        // log config
        if (properties.containsKey("log_config_file")) {
            value = properties.getProperty("log_config_file");
            if (!new File(value).exists()) {
                System.err.println("log4j properties not exist. init log4j failed. properties file:" + value);
                return false;
            } else {
                Log4jCfg.initLog4jFromCfg(value);
                logger.info("start server, done init log4j.");
            }
        } else {
            Log4jCfg.initDefaultLog4j();
            logger.warn("start server, no log4j config, use default config.");
        }
        // common config
        if (properties.containsKey("port")){
            value = properties.getProperty("port");
            int server_port = Integer.valueOf(value);
            ConstantValue.SERVER_PORT = (server_port == 0) ? ConstantValue.SERVER_PORT : server_port;
            logger.info("server port:"+ConstantValue.SERVER_PORT);
        }
        if (properties.containsKey("http_port")) {
            value = properties.getProperty("http_port");
            int http_server_port = Integer.valueOf(value);
            ConstantValue.HTTP_SERVER_PORT = (http_server_port == 0) ? ConstantValue.HTTP_SERVER_PORT : http_server_port;
            logger.info("http maintain server port:"+ConstantValue.HTTP_SERVER_PORT);
        }
        if (properties.containsKey("push_title")) {
            value = properties.getProperty("push_title");
            ConstantValue.PUSH_TITLE = StringUtils.isEmpty(value) ? ConstantValue.PUSH_TITLE : value;
        }
        if (properties.containsKey("push_cb")) {
            value = properties.getProperty("push_cb");
            ConstantValue.PUSH_CB_LINK = StringUtils.isEmpty(value) ? ConstantValue.PUSH_CB_LINK : value;
        }
        if (properties.containsKey("push_expire_time")) {
            value = properties.getProperty("push_expire_time");
            int expire_time = Integer.valueOf(value);
            ConstantValue.PUSH_EXPIRE_TIME = (expire_time == 0) ? ConstantValue.PUSH_EXPIRE_TIME : expire_time;
        }
        if (properties.containsKey("push_queue_max_size")) {
            value = properties.getProperty("push_queue_max_size");
            int queue_max_size = Integer.valueOf(value);
            ConstantValue.MAX_PUSH_QUEUE_NUM = (queue_max_size == 0) ? ConstantValue.MAX_PUSH_QUEUE_NUM : queue_max_size;
        }
        if (properties.containsKey("package_name")) {
            value = properties.getProperty("package_name");
            ConstantValue.PUSH_PACKAGE_NAME = StringUtils.isEmpty(value) ? ConstantValue.PUSH_PACKAGE_NAME : value;
        }
        // huawei config
        if (properties.containsKey("hw_app_id")) {
            value = properties.getProperty("hw_app_id");
            ConstantValue.HUAWEI_APP_ID = StringUtils.isEmpty(value) ? ConstantValue.HUAWEI_APP_ID : value;
        }
        if (properties.containsKey("hw_app_secret_key")) {
            value = properties.getProperty("hw_app_secret_key");
            ConstantValue.HUAWEI_APP_SECRET_KEY = StringUtils.isEmpty(value) ? ConstantValue.HUAWEI_APP_SECRET_KEY : value;
        }
        if (properties.containsKey("hw_key_file")) {
            value = properties.getProperty("hw_key_file");
            ConstantValue.HUAWEI_KEY_FILE = StringUtils.isEmpty(value) ? ConstantValue.HUAWEI_KEY_FILE : value;
        }
        // ge tui config
        if (properties.containsKey("gt_service_url")) {
            value = properties.getProperty("gt_service_url");
            ConstantValue.GETUI_SERVICE_URL = StringUtils.isEmpty(value) ? ConstantValue.GETUI_SERVICE_URL : value;
        }
        if (properties.containsKey("gt_app_id")) {
            value = properties.getProperty("gt_app_id");
            ConstantValue.GETUI_APP_ID = StringUtils.isEmpty(value) ? ConstantValue.GETUI_APP_ID : value;
        }
        if (properties.containsKey("gt_app_key")) {
            value = properties.getProperty("gt_app_key");
            ConstantValue.GETUI_APP_KEY = StringUtils.isEmpty(value) ? ConstantValue.GETUI_APP_KEY : value;
        }
        if (properties.containsKey("gt_app_secret_key")) {
            value = properties.getProperty("gt_app_secret_key");
            ConstantValue.GETUI_APP_SECRET = StringUtils.isEmpty(value) ? ConstantValue.GETUI_APP_SECRET : value;
        }
        if (properties.containsKey("gt_master_secret_key")) {
            value = properties.getProperty("gt_master_secret_key");
            ConstantValue.GETUI_MASTER_SECRET = StringUtils.isEmpty(value) ? ConstantValue.GETUI_MASTER_SECRET : value;
        }
        if (properties.containsKey("upload_push_log")) {
            value = properties.getProperty("upload_push_log");
            ConstantValue.UPLOAD_FLAG = Boolean.valueOf(value);
        }
        if (properties.containsKey("access_token_expire_time")) {
            value = properties.getProperty("access_token_expire_time");
            ConstantValue.PUSH_ACCESS_TOKEN_EXPIRE_TIME = Integer.valueOf(value);
        }
        if (properties.containsKey("redis_host")) {
            value = properties.getProperty("redis_host");
            ConstantValue.REDIS_HOST = value;
        }
        if (properties.containsKey("redis_port")) {
            value = properties.getProperty("redis_port");
            ConstantValue.REDIS_PORT = Integer.valueOf(value);
        }
		if (properties.containsKey("push_thread_num")) {
            value = properties.getProperty("push_thread_num");
            ConstantValue.ConnectionNum = Integer.valueOf(value);
        }
        if (properties.containsKey("tmp_dir")) {
            value = properties.getProperty("tmp_dir");
            ConstantValue.LOCAL_FILE_PATH = value;
        }

        if (properties.containsKey("html_pages")) {
            value = properties.getProperty("html_pages");
            loopLoadPage(properties, value);
        }
        logger.info("show config:" + properties.toString());
        return true;
    }

    private static void loopLoadPage(Properties properties, String pages) {
        String[] pageNames = pages.split(";");
        HtmlPage.reset();

        String path;
        for (String pageName :  pageNames) {
            path = properties.getProperty(pageName);
            String content = readFile(path);
            if (!"".equals(content)) {
                logger.info("load page:"+pageName);
                HtmlPage.addPage(pageName, content);
            }
        }
    }

    public static String readFile(String filePath) {
        if (filePath == null) {
            logger.warn("page path is null.");
            return "";
        }
        File file = new File(filePath);
        if (!file.exists()) {
            logger.warn("page path is not exist. path:"+filePath);
            return "";
        }
        StringBuilder stringBuilder = new StringBuilder();
        FileInputStream inputStream = null;
        InputStreamReader inputStreamReader = null;
        try {
            inputStream = new FileInputStream(filePath);
            // 指定配置文件的编码格式，必须是UTF-8。否则中文有乱码
            inputStreamReader = new InputStreamReader(inputStream, "UTF-8");
            int oneChar;
            while ((oneChar = inputStreamReader.read()) != -1) {
                stringBuilder.append((char)oneChar);
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return stringBuilder.toString();
    }

    private static boolean makeCmdOptions() {
        // create Options object
        options = new Options();
        // add t option
        //options.addOption("t", true, "pushclient msg title");
        //options.addOption("c", true, "pushclient msg content string");
        //options.addOption("e", true, "pushclient msg extras, format: k:v;k;v:n;...");
        //options.addOption("p", true, "pushclient msg payload");
        options.addOption("v", false, "show version info");
        options.addOption("h", false, "show detail help");
        options.addOption("f", true, "server config file");
        //options.addOption("s", true, "pushclient server secret key");
        //options.addOption("l", true, "pushclient msg loop num, default 1");
        //options.addOption("P", true, "pushclient platform, 'xiaomi','huawei','xinge'. default 'xiaomi'");
        //options.addOption("alias", true, "for xiaomi, pushclient to device alias");
        //options.addOption("regid", true, "for xiaomi, pushclient to device regid");
        //options.addOption("token", true, "for huawei, user token");
        //options.addOption("clientid", true, "for getui, client id");
        return true;
    }

    private static boolean parseCmdOptions(String[] args) throws org.apache.commons.cli.ParseException{
        CommandLineParser parser = new DefaultParser();
        cmd = parser.parse( options, args);
        return true;
    }


    /**
     * 暴露给外部的接口，用于配置重新加载
     * @return boolean
     */
    public static boolean loadConfig(){
        boolean ret = true;
        FileInputStream inputStream = null;
        InputStreamReader inputStreamReader = null;
        logger.info("start load server config, config file:"+configFilePath);
        Properties properties = new Properties();
        try {
            inputStream = new FileInputStream(new File(configFilePath));
            // 指定配置文件的编码格式，必须是UTF-8。否则中文有乱码
            inputStreamReader = new InputStreamReader(inputStream, "UTF-8");
            properties.load(inputStreamReader);
            if (!parseServerConfig(properties) && firstInitConfig) {
                ret = false;
                System.err.println("start service failed, exit.");
            }
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            ret = !firstInitConfig;
            logger.error(e.getMessage());
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            ret = !firstInitConfig;
            logger.error(e.getMessage());
        } catch (IOException e) {
            e.printStackTrace();
            ret = !firstInitConfig;
            logger.error(e.getMessage());
        } finally {
            try {
                if (inputStream != null) {
                    inputStream.close();
                }
                if (inputStreamReader != null) {
                    inputStreamReader.close();
                }
            } catch (IOException e) {
                e.printStackTrace();
                logger.error(e.getMessage());
                ret = !firstInitConfig;
            }
        }
        return ret;
    }

    public static void main(String[] args) throws InterruptedException {
        makeCmdOptions();

        if (!parseArgs(args)) {
            return;
        }
        firstInitConfig = false;

        startHttpMaintainServer();

        consumerThread = ConsumerMgr.getConsumerThread();
        consumerThread.setHost(ConstantValue.REDIS_HOST);
        consumerThread.setPort(ConstantValue.REDIS_PORT);
        consumerThread.start();

        // init service
        logger.info("start init tcp service.");
        NetServer tcpServer = new NetServer(NetServer.SOCKET_TYPE.TCP);
        tcpServer.addServer(ConstantValue.SERVER_PORT);
        tcpServer.startServer();

        logger.info("end service.");
    }

    /**
     * 用于http进行维护监控的端口
     */
    private static void startHttpMaintainServer() {
        Thread httpServerThread = new Thread(new Runnable(){
            @Override
            public void run() {
                logger.info("start init http service.");
                NetServer httpServer = new NetServer(NetServer.SOCKET_TYPE.HTTP);
                httpServer.addServer(ConstantValue.HTTP_SERVER_PORT);
                httpServer.startServer();
            }
        });
        httpServerThread.start();
    }

} // class PushServer end
