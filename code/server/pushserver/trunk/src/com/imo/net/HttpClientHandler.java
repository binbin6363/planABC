/**
 * @file HttpClientHandler.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月28日-上午9:37
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-28	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.net;

import org.apache.log4j.Logger;
import org.apache.mina.core.service.IoHandler;
import org.apache.mina.core.service.IoHandlerAdapter;
import org.apache.mina.core.session.IoSession;

public class HttpClientHandler  extends IoHandlerAdapter {

    private static final Logger logger = Logger.getLogger(HttpClientHandler.class);
    private static HttpClientHandler httpClientHandler = null;
    public static IoHandler getInstance() {
        if (httpClientHandler == null) {
            httpClientHandler = new HttpClientHandler();
        }
        return httpClientHandler;
    }

    @Override
    public void sessionOpened(IoSession session) throws Exception {
        super.sessionOpened(session);    //To change body of overridden methods use File | Settings | File Templates.
        logger.info("recv http open event.");
    }

    @Override
    public void sessionClosed(IoSession session) throws Exception {
        super.sessionClosed(session);    //To change body of overridden methods use File | Settings | File Templates.
        logger.info("recv http close event.");
    }

    @Override
    public void messageReceived(IoSession session, Object message) throws Exception {
        logger.info("recv http response.");
    }
} // class HttpClientHandler end
