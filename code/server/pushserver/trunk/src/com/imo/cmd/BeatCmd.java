/**
 * @file BeatCmd.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年01月13日-上午10:28
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-1-13	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.cmd;

import com.imo.msg.BeatRequest;
import com.imo.msg.BeatResult;
import com.imo.msg.BinaryHeader;
import com.imo.msg.BinaryPacket;
import org.apache.log4j.Logger;
import org.apache.mina.core.session.IoSession;

public class BeatCmd extends AbstractCommand {
    private static Logger logger = Logger.getLogger(BeatCmd.class);
    private BeatRequest message;
    private IoSession session;

    public BeatCmd(BinaryPacket message, IoSession session) {
        super(message, session);
        this.message = new BeatRequest(message);
        this.session = session;
    }

    @Override
    public boolean execute() {
        boolean ret = message.decode();
        if (!ret) {
            logger.error("decode beat message failed.");
            return ret;
        }
        logger.debug("receive client keep alive request.");

        BeatResult beatResult = new BeatResult();
        beatResult.offsetHead(BinaryHeader.HEAD_LEN);
        beatResult.setRetValue(0);
        beatResult.setTransId(message.getTransId());
        BinaryHeader header = new BinaryHeader();
        header.cmd = message.getCmd();
        header.seq = message.getSeq();
        beatResult.setHead(header);
        session.write(beatResult);
        logger.debug("done response keep alive request.");
        return true;
    }
} // class BeatCmd end
