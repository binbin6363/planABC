/**
 * @file BinaryPacketEncoder.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月14日-下午7:00
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>15-12-14	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.net;


import com.imo.msg.Result;
import com.imo.util.DspStringUtils;
import org.apache.log4j.Logger;
import org.apache.mina.core.buffer.IoBuffer;
import org.apache.mina.core.session.IoSession;
import org.apache.mina.filter.codec.ProtocolEncoderAdapter;
import org.apache.mina.filter.codec.ProtocolEncoderOutput;

public class BinaryPacketEncoder extends ProtocolEncoderAdapter {
    private static Logger logger = Logger.getLogger(BinaryPacketEncoder.class);
    @Override
    public void encode(IoSession ioSession, Object o, ProtocolEncoderOutput protocolEncoderOutput) throws Exception {
        Result result = (Result)o;
        if (!result.encode()) {
            logger.error("encode result failed, no response peer. cmd:"+result.toString());
            return;
        }
        byte[] b = result.toBytes();
        logger.info("show send data:"+ DspStringUtils.Bin2Hex(b, b.length));
        IoBuffer buffer = IoBuffer.allocate(b.length);
        buffer.put(b);
        buffer.flip();
        protocolEncoderOutput.write(buffer);
        //protocolEncoderOutput.flush();
    }
} // class BinaryPacketEncoder end
