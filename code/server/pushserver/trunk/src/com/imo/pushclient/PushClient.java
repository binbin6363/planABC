/**
 * @file PushClient.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月02日-下午9:00
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

import com.imo.msg.PushRequest;

import java.util.Date;
import java.util.Map;
import java.util.Queue;

public interface PushClient {

    /**
     * @param properties
     * @return boolean
     */
    public boolean initService(Map<String, Object> properties);

    /**
     * 公共的push接口，制定用户推送
     * @param title      push的标题
     * @param context    push的内容
     * @param payload    payload
     * @param timeToSend 定时发送
     * @param extras     push额外附带的信息
     * @return 成功返回true，反之false
     */
    //public boolean sendPushMsg(String title, String context, String payload, Date timeToSend, Map<String, String> extras);


    /**
     * 发送push消息到第三方服务
     * @param pushRequest pushRequest
     * @return PushRet
     */
    public PushRet sendPushMsg(PushRequest pushRequest);

    /**
     * 获取请求队列
     * @return PushRequest
     */
    public Queue<PushRequest> getPushQueue();

    /**
     * 公共的push接口,广播所有用户
     * @param title      push的标题
     * @param context    push的内容
     * @param payload    payload
     * @param timeToSend 定时发送
     * @param extras     push额外附带的信息
     * @return 成功返回true，反之false
     */
    //public boolean broadcastPushMsg(String title, String context, String payload, Date timeToSend, Map<String, String> extras);


    /**
     * 向队列中添加一个push请求
     * @param pushRequest
     * @return ret
     */
    public int addRequest(PushRequest pushRequest);

} // class PushClient end
