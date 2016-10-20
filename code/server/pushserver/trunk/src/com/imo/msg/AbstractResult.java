/**
 * @file AbstractResult.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月13日-下午1:52
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

public class AbstractResult implements Result {
    private static Logger logger = Logger.getLogger(AbstractResult.class);
    protected BinaryPacket binaryPacket = null;

    // hide default construction
    private AbstractResult(){}

    // use this construction to limit sub class call super(true)
    // make sure 'new BinaryPacket()'.
    public AbstractResult(boolean flag){
        binaryPacket = new BinaryPacket();
    }

    // this encode just write packet len
    @Override
    public boolean encode() {
        BinaryHeader head = binaryPacket.getHeader();
        binaryPacket.encodeHead(head);
        //logger.debug("encode head "+head.toString());
        return false;
    }

    @Override
    public byte[] toBytes() {
        return binaryPacket.toBytes();
    }


    public boolean offsetHead(int headlen){
        binaryPacket.offsetHead(headlen);
        return binaryPacket.isGood();
    }


    public boolean setHead(BinaryHeader header){
        binaryPacket.encodeHead(header);
        return binaryPacket.isGood();
    }


    public int getLen(){
        return binaryPacket.len();
    }

    public int getCmd() {
        return binaryPacket.cmd();
    }

    public int getSeq(){
        return binaryPacket.seq();
    }


} // class AbstractResult end
