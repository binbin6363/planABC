/**
 * @file Log4jCfg.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月02日-下午8:47
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

import org.apache.log4j.PropertyConfigurator;

import java.util.Properties;

public class Log4jCfg {

    public static boolean initDefaultLog4j(){
        System.out.println("init log config use default config, log file name: pushclient.log");
        Properties pro = new Properties();
        pro.put("log4j.rootLogger", "DEBUG,C,R");

        pro.put("log4j.appender.C", "org.apache.log4j.ConsoleAppender");
        pro.put("log4j.appender.C.Threshold", "INFO");
        pro.put("log4j.appender.C.layout", "org.apache.log4j.PatternLayout");
        pro.put("log4j.appender.C.layout.ConversionPattern", "%d{yyyy-MM-dd HH:mm:ss} [%-5p] %m%n");

        pro.put("log4j.appender.R", "org.apache.log4j.RollingFileAppender");
        pro.put("log4j.appender.R.File", "pushclient.log");
        pro.put("log4j.appender.R.MaxFileSize", "500000KB");
        pro.put("log4j.appender.R.MaxBackupIndex", "20");
        pro.put("log4j.appender.R.Threshold", "INFO");
        pro.put("log4j.appender.R.layout", "org.apache.log4j.PatternLayout");
        pro.put("log4j.appender.R.layout.ConversionPattern", "%d{yyyy-MM-dd HH:mm:ss.SSS} [%-5p] %m%n");
/*
        pro.put("log4j.appender.A", "org.apache.log4j.RollingFileAppender");
        pro.put("log4j.appender.A.layout", "org.apache.log4j.PatternLayout");
        pro.put("log4j.appender.A.File", "push_debug.log");
        pro.put("log4j.appender.A.MaxFileSize", "10000KB");
        pro.put("log4j.appender.A.MaxBackupIndex", "20");
        pro.put("log4j.appender.A.Threshold", "DEBUG");
        pro.put("log4j.appender.A.layout.ConversionPattern", "%d{yyyy-MM-dd HH:mm:ss.SSS} [%p] %m%n");
*/
        PropertyConfigurator.configure(pro);
        return true;
    }

    public static boolean initLog4jFromCfg(String propFile){
        System.out.println("init log config use custom log4j properties file:"+propFile);
        PropertyConfigurator.configure(propFile);
        return true;
    }
} // class Log4jCfg end
