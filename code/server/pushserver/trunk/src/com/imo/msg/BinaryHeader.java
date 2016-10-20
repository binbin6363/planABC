/**
 * @file BinaryHeader.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月14日-下午7:28
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>15-12-14	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.msg;

import java.nio.ByteBuffer;

public class BinaryHeader {

    public final static int HEAD_LEN = 12;
    public int len;
    public int cmd;
    public int seq;
    private ByteBuffer tmp = ByteBuffer.allocate(HEAD_LEN);

    public String toString(){
        return "[headinfo, len:"+len+", cmd:"+cmd+", seq:"+seq+"]";
    }

    public byte[] toBytes() {
        tmp.position(0);
        tmp.putInt(len);
        tmp.putInt(cmd);
        tmp.putInt(seq);
        return tmp.array();
    }
} // class BinaryHeader end
