/**
 * @file HandlePushCmd.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月12日-上午10:30
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-12	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.cmd;

import com.imo.msg.BinaryHeader;
import com.imo.msg.BinaryPacket;
import com.imo.msg.PushRequest;
import com.imo.msg.PushResult;
import com.imo.pushclient.PushClient;
import com.imo.pushclient.PushClientMgr;
import org.apache.log4j.Logger;
import org.apache.mina.core.session.IoSession;

public class HandlePushCmd extends AbstractCommand{

    private static Logger logger = Logger.getLogger(HandlePushCmd.class);
    private PushRequest message;
    private IoSession session;

    public HandlePushCmd(BinaryPacket message, IoSession session){
        super(message, session);
        this.message = new PushRequest(message);
        this.session = session;
    }

    public boolean execute(){
        boolean ret = message.decode();
        if (!ret) {
            logger.error("decode push message failed.");
            return ret;
        }
        PushClient pushClient = PushClientMgr.getClient(message.getPushType());
        int retValue = 0;
        if (null != pushClient) {
            retValue = 0;
            pushClient.addRequest(message);
        } else {
            retValue = 100;
            logger.error("get hwclient failed, no connection.");
        }

        PushResult pushResult = new PushResult();
        pushResult.offsetHead(BinaryHeader.HEAD_LEN);

        BinaryHeader header = new BinaryHeader();
        header.cmd = message.getCmd();
        header.seq = message.getSeq();
        pushResult.setCid(message.getCid());
        pushResult.setUid(message.getUid());
        pushResult.setRetValue(retValue);
        pushResult.setTransId(message.getTransId());
        pushResult.setHead(header);
        /*
        if (!pushResult.encode()) {
            logger.error("encode response failed.");
            ret = false;
            return ret;
        }
        */
        logger.debug("response peer cmd:"+header.cmd+", transid:"+message.getTransId());
        //session.write(pushResult.toBytes());
        session.write(pushResult);
        return  ret;
    }
} // class HandlePushCmd end
