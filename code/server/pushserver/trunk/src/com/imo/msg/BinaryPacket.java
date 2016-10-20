/**
 * @file BinaryPacket.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月14日-下午7:35
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>15-12-14	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.msg;

import com.imo.util.DspStringUtils;
import org.apache.log4j.Logger;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;

public class BinaryPacket {
    private static Logger logger = Logger.getLogger(BinaryPacket.class);
    private final static int intLen = 4;

    public final static int MAX_SEND_BUF = 4 * 1024;
    public final static String defaultCharSet = "UTF-8";
    private BinaryHeader header = null;      // 包头
    private ByteBuffer packetBuffer = null;  // 整个包的数据，包括包头
    private byte[] bytesData = null;
    private boolean isGood = true;
    private boolean isEof = false;

    public boolean isGood() {
        return isGood;
    }

    public boolean isEof() {
        return isEof;
    }

    /**
     * 默认生成4k的空间
     */
    public BinaryPacket(){
        this(MAX_SEND_BUF);
    }

    public BinaryPacket(int packetLen) {
        header = new BinaryHeader();
        if (packetLen > 0) {
            packetBuffer = ByteBuffer.allocate(packetLen);
        } else {
            logger.error("input packet error! len:" + packetLen);
        }
    }

    //=========================================================
    // 读取操作
    //=========================================================
    public int readDataFromPkg(byte[] data){
        if (data.length < BinaryHeader.HEAD_LEN) {
            logger.error("input data is error, data len:"+data.length);
            return -1;
        }
        if (data.length > packetBuffer.capacity()) {
            logger.error("decode packet body failed. data len:"+data.length+", capacity:" + packetBuffer.capacity());
            return -1;
        }
        packetBuffer.mark();
        packetBuffer.put(data);
        // 解出包头
        packetBuffer.reset();
        header.len = packetBuffer.getInt();
        header.cmd = packetBuffer.getInt();
        header.seq = packetBuffer.getInt();
        logger.debug("recv data, "+header.toString());
        //showHexData();
        return 0;
    }

    private void showHexData(){
        logger.debug(DspStringUtils.Bin2Hex(packetBuffer.array(), packetBuffer.capacity()));
    }

    public int free(){
        showHexData();
        packetBuffer.clear();
        packetBuffer = null;
        return 0;
    }

    public int len(){
        return header.len;
    }

    public int cmd(){
        return header.cmd;
    }

    public int seq(){
        return header.seq;
    }

    public BinaryHeader getHeader(){
        /*
        packetBuffer.mark();
        packetBuffer.position(0);
        header.len = packetBuffer.getInt();
        header.cmd = packetBuffer.getInt();
        header.seq = packetBuffer.getInt();
        packetBuffer.reset();
        */
        return header;
    }

    public int getInt(){
        if (!isGood) {
            logger.warn("getInt error, not good.");
            return 0;
        }
        if (isEof) {
            logger.warn("getInt error, eof already.");
            return 0;
        }
        if (packetBuffer.remaining() < intLen) {
            isGood = false;
            logger.error("getInt failed, packetBuffer remaining:"+ packetBuffer.remaining()+", need len:"+ intLen);
            return 0;
        }
        int retVal = packetBuffer.getInt();
        if (0 == packetBuffer.remaining()) {
            isEof = true;
        }
        return retVal;
    }

    public String getString(String charSet){
        if (!isGood) {
            logger.warn("getString error, not good.");
            return null;
        }
        if (isEof) {
            logger.warn("getString error, eof already.");
            return null;
        }
        if (packetBuffer.remaining() < intLen) {
            isGood = false;
            logger.error("getString failed, packetBuffer remaining:"+ packetBuffer.remaining()+", need len:"+ intLen);
            return null;
        }
        String myCharSet = defaultCharSet;
        if (charSet != null) {
            myCharSet = charSet;
        }
        int strLen = getInt();
        if (packetBuffer.remaining() < strLen) {
            isGood = false;
            logger.error("getString failed, packetBuffer remaining:"+ packetBuffer.remaining()+", need len:"+strLen);
            return null;
        }
        byte[] dst = new byte[strLen];
        packetBuffer.get(dst, 0, strLen);
        if (0 == packetBuffer.remaining()) {
            isEof = true;
        }
        try {
            return new String(dst, myCharSet);
        } catch (UnsupportedEncodingException e) {
            logger.error(e.getMessage());
        }
        return null;
    }

    //=========================================================
    // 写操作
    //=========================================================
    private boolean checkOk(){
        boolean ret = true;
        if (!isGood)
            ret = false;
        if (isEof)
            ret = false;
        return ret;
    }

    private boolean checkEnough(int len){
        if (packetBuffer.arrayOffset() + len > packetBuffer.capacity()) {
            isGood = false;
            logger.error("buf overflow.");
            return false;
        }
        return true;
    }

    public BinaryPacket offsetHead(int headlen){
        //logger.debug("BinaryPacket offset head, ori position:"+packetBuffer.position()+", head len:"+headlen);
        packetBuffer.position(headlen);
        return this;
    }

    /**
     * call this at last
     * @param header
     * @return BinaryPacket
     */
    public BinaryPacket encodeHead(BinaryHeader header){
        if (!checkOk()) {
            logger.warn("buffer is error, setHead failed.");
            return this;
        }
        header.len = packetBuffer.position();
        this.header = header;
        packetBuffer.putInt(0, this.header.len);
        packetBuffer.putInt(4, this.header.cmd);
        packetBuffer.putInt(8, this.header.seq);
        logger.debug("encodeHead "+this.header.toString()+", current position:"+packetBuffer.position());
        return this;
    }

    public BinaryPacket putInt(int ele){
        if (!checkOk()) {
            logger.warn("buffer is error, putInt failed.");
            return this;
        }
        if (!checkEnough(intLen)) {
            logger.error("putInt failed, buf overflow.");
            return this;
        }
        packetBuffer.putInt(ele);
        return this;
    }

    public BinaryPacket put(byte[] data, int len) {
        if (!checkOk()) {
            logger.warn("buffer is error, put bytes failed.");
            return this;
        }
        if (!checkEnough(len)) {
            logger.error("put bytes failed, buf overflow.");
            return this;
        }
        packetBuffer.put(data, packetBuffer.position(), len);
        return this;
    }

    public BinaryPacket putString(String str){
        if (!checkOk()) {
            logger.warn("buffer is error, putString failed.");
            return this;
        }
        byte [] data = str.getBytes();
        if (!checkEnough(intLen)) {
            logger.error("putString failed, buf overflow.");
            return this;
        }
        putInt(data.length);
        if (!checkEnough(data.length)) {
            logger.error("putString failed, buf overflow.");
            return this;
        }
        packetBuffer.put(data);
        return this;
    }

    /**
     * 调用此接口，每次会new一段空间，返回当前数据的副本
     * @return
     */
    public byte[] toBytes() {
        if (!isGood()){
            logger.warn("toBytes, but buf is not good!");
        }
        int position = packetBuffer.position();
        //logger.info("==test== toBytes show position:"+position + ", "+header.toString());
        bytesData = new byte[position];
        packetBuffer.position(0);
        packetBuffer.get(bytesData, 0, position);
        packetBuffer.position(position);
        return bytesData;
    }

    public int getDataLen(){
        return packetBuffer.position();
    }
} // class BinaryPacket end
