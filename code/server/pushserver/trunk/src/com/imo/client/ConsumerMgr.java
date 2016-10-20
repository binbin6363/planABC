/**
 * @file ConsumerMgr.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年02月09日-上午12:03
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-2-9	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.client;

public class ConsumerMgr {

    private static ConsumerThread consumerThread = new ConsumerThread();

    public static ConsumerThread getConsumerThread() {
        return consumerThread;
    }


} // class ConsumerMgr end
