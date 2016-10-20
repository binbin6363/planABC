/**
 * @file BeatResult.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月13日-上午10:42
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

public class BeatResult extends AbstractResult{

    private static Logger logger = Logger.getLogger(BeatResult.class);
    private int transId = 0;
    private int retValue = 0;

    public BeatResult(){
        super(true);
    }

    public void setTransId(int transId) {
        this.transId = transId;
    }

    public void setRetValue(int retValue) {
        this.retValue = retValue;
    }

    @Override
    public boolean encode() {
        boolean ret = true;
        if (null == binaryPacket) {
            logger.error("PushResult encode failed.");
            ret = false;
        } else {
            binaryPacket.putInt(retValue);
            binaryPacket.putInt(transId);
        }
        super.encode();
        logger.debug("BeatResult encode, packet:"+binaryPacket.toString());
        return ret;
    }

} // class BeatResult end
