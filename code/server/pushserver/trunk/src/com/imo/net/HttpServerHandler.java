/**
 * @file HttpServerHandler.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月26日-下午4:23
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-26	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.net;

import com.imo.PushServer;
import com.imo.util.*;
import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import org.apache.mina.core.buffer.IoBuffer;
import org.apache.mina.core.service.IoHandlerAdapter;
import org.apache.mina.core.session.IoSession;
import org.apache.mina.http.HttpRequestImpl;

import java.lang.management.ManagementFactory;
import java.lang.management.RuntimeMXBean;
import java.net.InetAddress;
import java.nio.charset.Charset;
import java.util.LinkedHashMap;
import java.util.Map;

public class HttpServerHandler  extends IoHandlerAdapter {
    private static Logger logger = Logger.getLogger(HttpServerHandler.class);
    private static HttpServerHandler httpServerHandler = null;

    private HttpServerHandler(){}

    public static HttpServerHandler getInstances(){
        if(httpServerHandler == null){
            httpServerHandler = new HttpServerHandler();
        }
        return httpServerHandler;
    }

    @Override
    public void exceptionCaught(IoSession session, Throwable cause)
            throws Exception {
        // TODO Auto-generated method stub
        cause.printStackTrace();
        logger.error("http exception in session:"+session.getId()+". err message:"+cause.getMessage());
        //cause.printStackTrace();
    }

    @Override
    public void sessionOpened(IoSession session) throws Exception {
        super.sessionOpened(session);
        logger.debug("one http client connect to http server, session id:" + session.getId());
    }

    @Override
    public void sessionClosed(IoSession session) throws Exception {
        super.sessionClosed(session);
        logger.debug("one http client close from http server, session id:"+session.getId());
    }

    @Override
    public void messageReceived(IoSession session, Object message) throws Exception {
        if (message instanceof HttpRequestImpl) {
            HttpRequestImpl httpRequest = (HttpRequestImpl)message;
            logger.info("recv one http msg. "+httpRequest.toString());
            String result_buf = HtmlPage.getPage(HtmlPage.TAG_RESULT_PAGE);
            String operateResult = null;
            String request_path = httpRequest.getRequestPath();
            if ("/".equals(request_path)) {
                IoBuffer buf = IoBuffer.allocate(1024).setAutoExpand(true);
                buf.putString(HtmlPage.getPage(HtmlPage.TAG_INDEX_PAGE), Charset.forName("UTF-8").newEncoder());
                buf.flip();
                session.write(buf);
                session.close(false);
                return;
            } else if ("/reload_config".equals(request_path)){
                if (!PushServer.loadConfig()) {
                    operateResult = "<h2>reload config failed.</h2>";
                    logger.warn("something error, reload config failed.");
                } else {
                    operateResult = "<h2>reload config succeed.</h2>";
                    logger.info("reload config ok.");
                }
            } else if ("/show_config".equals(request_path)){
                operateResult = PushServer.readFile(PushServer.getConfigFilePath());
                operateResult = operateResult.replaceAll("\n", "<br/>");
            } else if ("/favicon.ico".equals(request_path)){
                // do nothing
                session.close(false);
                return;
            } else if ("/run_info".equals(request_path)) {
                String workSpacePath = JarUtils.getJarDir();
                String jarFileName = JarUtils.getJarName();
                String localIp = InetAddress.getLocalHost().getHostAddress();
                String serverPort = String.valueOf(ConstantValue.SERVER_PORT);
                long startTime = ConstantValue.SERVER_START_TIME;
                long now = System.currentTimeMillis();
                Runtime runTime = Runtime.getRuntime();
                // the number of processors available to the Java virtual machine
                int processorsNum = runTime.availableProcessors();
                // total amount of memory in the Java virtual machine
                long totalMem = runTime.totalMemory();
                // the amount of free memory in the Java Virtual Machine
                long freeMem = runTime.freeMemory();
                // the amount of memory used
                long usedMem = totalMem - freeMem;

                RuntimeMXBean runtimeMXBean = ManagementFactory.getRuntimeMXBean();
                String name = runtimeMXBean.getName();
                int pid = 0;
                int index = name.indexOf("@");
                if (index != -1) {
                    pid = Integer.parseInt(name.substring(0, index));
                    logger.info("当前进程的PID为："+pid);
                }
                Map<String, String> runInfo = new LinkedHashMap<String, String>();
                runInfo.put("pid", String.valueOf(pid));
                runInfo.put("server_ip", localIp);
                runInfo.put("server_port", serverPort);
                runInfo.put("workspace", workSpacePath);
                runInfo.put("jar_file", jarFileName);
                runInfo.put("run_time", DspStringUtils.normalizeDate(now - startTime));
                runInfo.put("processor_num", String.valueOf(processorsNum));
                runInfo.put("total_memory", DspStringUtils.normalizeByte(totalMem));
                runInfo.put("free_memory", DspStringUtils.normalizeByte(freeMem));
                runInfo.put("used_memory", DspStringUtils.normalizeByte(usedMem));
                String systemInfo = DspStringUtils.map2HtmlTable(runInfo);
                String pushStac = StatisticsInfo.getInst().statistics2HtmlTable();
                operateResult = systemInfo + pushStac;
            } else {
                // 存在路径对应的资源，就返回静态资源
                if (HtmlPage.existPage(request_path.substring(1))) {
                    result_buf = HtmlPage.getPage(request_path.substring(1));
                    IoBuffer buf = IoBuffer.allocate(1024).setAutoExpand(true);
                    buf.putString(result_buf, Charset.forName("UTF-8").newEncoder());
                    buf.flip();
                    session.write(buf);
                    session.close(false);
                    return;
                }

                logger.warn("unsupported request path:" + request_path);
                result_buf = "";
            }

            if (StringUtils.isEmpty(result_buf)) {
                result_buf = HtmlPage.getPage(HtmlPage.TAG_404_PAGE);
            } else {
                result_buf = result_buf.replace("{result}", operateResult==null?"empty result":operateResult);
            }
            IoBuffer buf = IoBuffer.allocate(1024).setAutoExpand(true);
            buf.putString(result_buf, Charset.forName("UTF-8").newEncoder());
            buf.flip();
            session.write(buf);
        }
        session.close(false);
    }

    @Override
    public void messageSent(IoSession session, Object message) throws Exception {
        super.messageSent(session, message);    //To change body of overridden methods use File | Settings | File Templates.
        logger.debug("response http client ok, sesion id:"+session.getId());
    }
} // class HttpServerHandler end
