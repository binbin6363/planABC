/**
 * @file PushResult.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月12日-上午11:18
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-12	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.msg;

import org.apache.log4j.Logger;

public class PushResult extends AbstractResult{

    private static Logger logger = Logger.getLogger(PushResult.class);
    private int cid = 0;
    private int uid = 0;
    private int retValue = 0;
    private int transId = 0;

    public PushResult(){
        super(true);
    }

    public int getCid() {
        return cid;
    }

    public void setCid(int cid) {
        this.cid = cid;
    }

    public int getUid() {
        return uid;
    }

    public void setUid(int uid) {
        this.uid = uid;
    }

    public int getRetValue() {
        return retValue;
    }

    public void setRetValue(int retValue) {
        this.retValue = retValue;
    }

    public int getTransId() {
        return transId;
    }

    public void setTransId(int transId) {
        this.transId = transId;
    }

    public boolean encode(){
        boolean ret = true;
        if (null == binaryPacket) {
            logger.error("PushResult encode failed.");
            ret = false;
        } else {
            binaryPacket.putInt(cid);
            binaryPacket.putInt(uid);
            binaryPacket.putInt(retValue);
            binaryPacket.putInt(transId);
        }
        super.encode();
        logger.debug("PushResult encode, packet:"+binaryPacket.toString());
        return ret;
    }

} // class PushResult end
