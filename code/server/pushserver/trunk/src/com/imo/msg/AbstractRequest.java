/**
 * @file AbstractRequest.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月13日-下午1:41
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

import com.imo.net.CmdConstant;
import org.apache.log4j.Logger;

public class AbstractRequest implements Request {
    private static Logger logger = Logger.getLogger(AbstractRequest.class);
    protected BinaryPacket binaryPacket = null;

    // sub class must call base class construction by hand
    // hide default construction
    private AbstractRequest(){

    }

    public AbstractRequest(BinaryPacket message){
        this.binaryPacket = message;
    }

    @Override
    public boolean decode() {
        if (binaryPacket == null) {
            logger.warn("binaryPacket is null.");
            return false;
        }
        BinaryHeader head = binaryPacket.getHeader();
        //logger.debug("call AbstractRequest class decode head, "+head.toString());
        return true;
    }


    protected boolean CheckMessage(){
        if (binaryPacket == null) {
            logger.error("packet is null.");
            return false;
        }
        boolean ret = true;
        if (!binaryPacket.isGood()) {
            logger.error("check packet, packet is not good.");
            ret = false;
        }
        return ret;
    }

    public int getLen(){
        if (binaryPacket == null) {
            logger.warn("binaryPacket is null.");
            return 0;
        }
        return binaryPacket.len();
    }

    public int getCmd() {
        if (binaryPacket == null) {
            logger.warn("binaryPacket is null.");
            return CmdConstant.CMD_PUSH_MSG;
        }
        return binaryPacket.cmd();
    }

    public int getSeq(){
        if (binaryPacket == null) {
            logger.warn("binaryPacket is null.");
            return 0;
        }
        return binaryPacket.seq();
    }


} // class AbstractRequest end
