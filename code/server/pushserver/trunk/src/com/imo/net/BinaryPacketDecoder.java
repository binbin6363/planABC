/**
 * @file BinaryPacketDecoder.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月14日-下午6:59
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

import com.imo.msg.BinaryHeader;
import com.imo.msg.BinaryPacket;
import org.apache.log4j.Logger;
import org.apache.mina.core.buffer.IoBuffer;
import org.apache.mina.core.session.IoSession;
import org.apache.mina.filter.codec.CumulativeProtocolDecoder;
import org.apache.mina.filter.codec.ProtocolDecoderOutput;


public class BinaryPacketDecoder extends CumulativeProtocolDecoder {

    private Logger logger = Logger.getLogger(BinaryPacketDecoder.class);

    @Override
    protected boolean doDecode(IoSession ioSession, IoBuffer ioBuffer, ProtocolDecoderOutput protocolDecoderOutput) throws Exception {
        if(ioBuffer.remaining() >= BinaryHeader.HEAD_LEN){// 包头长度
            //标记当前position的快照标记mark，以便后继的reset操作能恢复position位置
            ioBuffer.mark();

            // 整个包数据长度
            int pkgLen = ioBuffer.getInt();
            if (pkgLen > 1 * 1024 * 1024) {
                ioSession.close(true);
                logger.error("recv error packet. close session, session id:"+ ioSession.getId() + ", packet len:"+pkgLen);
                return false;
            }
            ioBuffer.reset();// 重置恢复position位置到操作前
            //注意上面的get操作会导致下面的remaining()值发生变化
            if(ioBuffer.remaining() < pkgLen){
                // 如果整个包内容不够，则进入下一轮, 接收新数据，以拼凑成完整数据
                logger.debug("input packetBuffer len:"+ioBuffer.remaining()+" < pkglen:"+pkgLen);
                return false;
            }else{
                // 整个包内容足够
                byte[] packArr = new byte[pkgLen];
                ioBuffer.get(packArr, 0, pkgLen);
                BinaryPacket binaryPacket = new BinaryPacket(pkgLen);
                binaryPacket.readDataFromPkg(packArr);
                packArr = null;
                protocolDecoderOutput.write(binaryPacket);
                //logger.info("recv data, len:" + binaryPacket.len() + ", cmd:"+ binaryPacket.cmd());

                if(ioBuffer.remaining() > 0){//如果读取一个完整包内容后还粘了包，就让父类再调用一次，进行下一次解析
                    logger.debug("input packetBuffer len bigger than one packet, split packet continue. remaining:"+ioBuffer.remaining());
                    return true;
                }
            }
        }
        return false;
    }

} // class BinaryPacketDecoder end
