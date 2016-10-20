/**
 * @file TcpServerHandler.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月14日-下午5:47
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

import com.imo.cmd.BeatCmd;
import com.imo.cmd.HandlePushCmd;
import com.imo.msg.BinaryPacket;
import org.apache.log4j.Logger;
import org.apache.mina.core.service.IoHandlerAdapter;
import org.apache.mina.core.session.IdleStatus;
import org.apache.mina.core.session.IoSession;


public class TcpServerHandler extends IoHandlerAdapter{

    private static Logger logger = Logger.getLogger(TcpServerHandler.class);
    private static TcpServerHandler tcpServerHandler = null;

    public static TcpServerHandler getInstances(){
        if(tcpServerHandler == null){
            tcpServerHandler = new TcpServerHandler();
        }
        return tcpServerHandler;
    }

    private TcpServerHandler(){}

    @Override
    public void exceptionCaught(IoSession session, Throwable cause)
            throws Exception {
        // TODO Auto-generated method stub
        cause.printStackTrace();
        logger.error("tcp exception in session:"+session.getId()+". err message:"+cause.getMessage());
        //cause.printStackTrace();
    }

    /**
     * 服务端成功收到消息后触发事件
     */
    @Override
    public void messageReceived(IoSession session, Object message) throws Exception {
        // TODO Auto-generated method stub
        //logger.info("messageReceived, session id:"+session.getId());
        if (message instanceof BinaryPacket) {
            // 处理业务
            BinaryPacket recvMsg = (BinaryPacket)message;
            logger.info("recv msg, cmd:" + recvMsg.cmd() + ", packet len:" + recvMsg.len()+", session id:" + session.getId());
            switch (recvMsg.cmd()) {
                case CmdConstant.CMD_KEEP_ALIVE:
                {
                    BeatCmd cmd = new BeatCmd(recvMsg, session);
                    cmd.execute();
                }
                break;
                case CmdConstant.CMD_PUSH_MSG: {
                // (12)head + (uint32)cid + (uint32)uid + (uint32)TransId + (uint32)PlatForm + (uint32)PushType
                // (string)token + (uint32)kv_num + [(string)key + (string)value + ... ]
                    HandlePushCmd cmd = new HandlePushCmd(recvMsg, session);
                    cmd.execute();
                }
                break;
                default: {
                    logger.warn("unknown cmd:"+recvMsg.cmd());
                }
                break;
            }

        } else {
            logger.error("receive a error message.");
            session.close(false);
        }
    }

    /**
     * 服务端响应发送消息成功后触发
     */
    @Override
    public void messageSent(IoSession session, Object message) throws Exception {
        // TODO Auto-generated method stub
        logger.debug("tcp send message ok");
        //session.close(true);//发送成功后主动断开与客户端的连接
    }

    @Override
    public void inputClosed(IoSession ioSession) throws Exception {
        logger.error("tcp input close, session id:"+ioSession.getId() + ", create time:" + ioSession.getCreationTime());
        ioSession.close(false);
    }

    @Override
    public void sessionClosed(IoSession session) throws Exception {
        // TODO Auto-generated method stub

    }

    /**
     * 当一个新客户端连接后触发此方法
     */
    @Override
    public void sessionCreated(IoSession session) throws Exception {
        // TODO Auto-generated method stub
        super.sessionCreated(session);
    }

    /**
     * 当连接空闲时触发此方法
     */
    @Override
    public void sessionIdle(IoSession session, IdleStatus arg1) throws Exception {
        // TODO Auto-generated method stub

    }

    /**
     * 当连接后打开时触发此方法，一般此方法与 sessionCreated 会被同时触发
     */
    @Override
    public void sessionOpened(IoSession session) throws Exception {
        // TODO Auto-generated method stub

        logger.info("tcp session open, session id:"+session.getId());
    }


} // class TcpServerHandler end
