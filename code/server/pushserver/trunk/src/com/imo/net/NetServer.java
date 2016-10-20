/**
 * @file NetServer.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月14日-下午5:39
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

import org.apache.log4j.Logger;
import org.apache.mina.core.filterchain.DefaultIoFilterChainBuilder;
import org.apache.mina.core.session.IdleStatus;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.apache.mina.http.HttpServerCodec;
import org.apache.mina.transport.socket.SocketAcceptor;
import org.apache.mina.transport.socket.nio.NioSocketAcceptor;

import java.io.*;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.util.ArrayList;
import java.util.List;

public class NetServer {

    public static enum SOCKET_TYPE {
        UNKNOWN_TYPE(0, "unknown socket type"),
        TCP(1, "tcp socket"),
        HTTP(2, "http socket"),
        UDP(3, "udp socket");

        private int type;
        private String desc;
        private SOCKET_TYPE(int type, String desc) {
            this.type = type;
            this.desc = desc;
        }

        public String getDesc(){
            return desc;
        }

        public int getType(){
            return type;
        }
    };

    private static Logger logger = Logger.getLogger(NetServer.class);
    private static NetServer defaultTcpServer = null;
    private SocketAcceptor acceptor = new NioSocketAcceptor();
    private DefaultIoFilterChainBuilder chainBuilder = acceptor.getFilterChain();
    List<SocketAddress> socketAddresses = new ArrayList<SocketAddress>();
    private SOCKET_TYPE serverType = SOCKET_TYPE.UNKNOWN_TYPE; // unknown type


    public static NetServer defaultTcpServer(){
        if(defaultTcpServer == null){
            defaultTcpServer = new NetServer(SOCKET_TYPE.TCP);
        }
        return defaultTcpServer;
    }

    /**
     * 添加一个端口监听
     * @param port 端口号
     * @return boolean
     */
    public boolean addServer(int port){
        InetSocketAddress inetSocketAddress = new InetSocketAddress(port);
        socketAddresses.add(inetSocketAddress);
        logger.info("add server, type:"+serverType+", listen in port:"+port);
        return true;
    }

    /**
     * startServer 调用之后会挂起在此处，如果需要启动两种不同类型的服务，就要另起线程
     * @return boolean
     */
    public boolean startServer(){
        boolean ret = true;
        try {
            acceptor.bind(socketAddresses);
        } catch (IOException e) {
            e.printStackTrace();
            logger.error(e.getMessage());
            ret = false;
        }
        return ret;
    }

    /**
    * @param serverType 服务类型，tcp/http/udp，当前仅仅支持tcp/http
    */
    public NetServer(SOCKET_TYPE serverType){
        this.serverType = serverType;
        acceptor.setReuseAddress(true);
        if (serverType == SOCKET_TYPE.TCP) {
            /**
             * 设置过滤器(自定义二进制数据编解码器)
             * 解码原理 headlen + body
             */
            chainBuilder.addLast("ChainTcp", new ProtocolCodecFilter(new BinaryPacketCodecFactory()));
            // 设置读取数据的缓冲区大小,如果事件处理线程没有来得及处理，导致数据堆积，可能超过这个值
            acceptor.getSessionConfig().setReadBufferSize(4 * 1024 * 1024);
            acceptor.getSessionConfig().setReceiveBufferSize(4 * 1024 * 1024);
            //读写通道10秒内无操作进入空闲状态
            acceptor.getSessionConfig().setIdleTime(IdleStatus.BOTH_IDLE, 10);
            acceptor.setHandler(TcpServerHandler.getInstances());
        } else if (serverType == SOCKET_TYPE.HTTP) {
            /**
             * 设置过滤器(自定义二进制数据编解码器)
             * 解码原理 headlen + body
             */
            chainBuilder.addLast("ChainHttp", new HttpServerCodec());
            // 设置读取数据的缓冲区大小,如果事件处理线程没有来得及处理，导致数据堆积，可能超过这个值
            acceptor.getSessionConfig().setReadBufferSize(4 * 1024 * 1024);
            acceptor.getSessionConfig().setReceiveBufferSize(4 * 1024 * 1024);
            //读写通道10秒内无操作进入空闲状态
            acceptor.getSessionConfig().setIdleTime(IdleStatus.BOTH_IDLE, 10);
            acceptor.setHandler(HttpServerHandler.getInstances());
        } else if (serverType == SOCKET_TYPE.UDP) {
            logger.error("unsupport udp server type yet.");
        } else {
            logger.error("unsupported server type.");
        }
        /*
        try {
            logger.info("service listen at port:"+bindPort);
            //setReuseAddress
            acceptor.setReuseAddress(true);
            //acceptor.bind(new InetSocketAddress(bindPort));//绑定端口
            // 添加tcp端口
            socketAddresses.add(new InetSocketAddress(bindPort));
            if (ConstantValue.OPEN_HTTP) {
                socketAddresses.add(new InetSocketAddress(ConstantValue.HTTP_SERVER_PORT));
                logger.info("start http server in port "+ConstantValue.HTTP_SERVER_PORT);
            }
            acceptor.bind(socketAddresses);
        } catch (IOException e) {
            e.printStackTrace();
        }
        */
    }

} // class NetServer end
