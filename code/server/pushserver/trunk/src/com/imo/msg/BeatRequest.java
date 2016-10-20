/**
 * @file BeatRequest.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月13日-上午10:30
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-13	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.msg;

import org.apache.log4j.Logger;

public class BeatRequest extends AbstractRequest{

    private static Logger logger = Logger.getLogger(BeatRequest.class);
    private int transId = 0;

    public int getTransId() {
        return transId;
    }

    public BeatRequest(BinaryPacket message){
        super(message);
    }


    public boolean decode() {
        // call base class decode head
        super.decode();
        transId = binaryPacket.getInt();
        return CheckMessage();
    }


} // class BeatRequest end
