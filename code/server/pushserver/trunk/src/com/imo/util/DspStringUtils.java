/**
 * @file DspStringUtils.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月02日-下午3:19
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>15-12-2	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.util;

import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import org.json.simple.JSONObject;
import org.json.simple.parser.ContainerFactory;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;

import java.text.DecimalFormat;
import java.util.*;

public class DspStringUtils {

    private static String hexStr =  "0123456789ABCDEF";
    private static String[] binaryArray =
            {"0000","0001","0010","0011",
            "0100","0101","0110","0111",
            "1000","1001","1010","1011",
            "1100","1101","1110","1111"};
    private static Logger logger = Logger.getLogger(DspStringUtils.class);

    private static final long _1Byte = 1;
    private static final long _1KB = (1024*_1Byte);
    private static final long _1MB = (1024*_1KB);
    private static final long _1GB = (1024*_1MB);

    // 以毫秒为最基础的时间单位
    private static final long _1S = 1000;
    private static final long _1HOUR = 3600*_1S;
    private static final long _1DAY = (24*_1HOUR);
    private static final long _1MONTH = (30 * _1DAY);
    private static final long _1YEAR = (365*_1DAY);
    private static final DecimalFormat decimalFormat = new DecimalFormat("#.00");
    private static final JSONParser defaultJsonParser = new JSONParser();
/*
    public static boolean isEmpty(String str) {
        if (null == str) {
            return true;
        } else if (str.isEmpty()){
            return true;
        }
        return false;
    }
*/

    public static boolean IsEqual(String arg1, String arg2){
        if (null == arg1 && null == arg2) {
            return true;
        } else if (null == arg1 || null == arg2) {
            return false;
        } else {
            return arg1.equals(arg2);
        }
    }

    /**
     *
     * @param str
     * @param len
     * @return 转换为二进制字符串
     */
    public static String bytes2Bin(byte[] str, int len){
        StringBuffer stringBuffer = new StringBuffer();
        int pos = 0;
        for(int i = 0; i < len; ++i){
            byte b = str[i];
            //高四位
            pos = (b & 0xF0) >> 4;
            stringBuffer.append(binaryArray[pos]);
            //低四位
            pos = b & 0x0F;
            stringBuffer.append(binaryArray[pos]);
        }
        return stringBuffer.toString();

    }

    /**
     * @param bytes
     * @param len
     * @return 将二进制转换为十六进制字符输出
     */
    public static String Bin2Hex(byte[] bytes, int len){
        StringBuffer stringBuffer = new StringBuffer();
        String hex = "";
        for(int i = 0; i < len; i++){
            if (i % 16 == 0) {
                stringBuffer.append("\n");
            }
            byte b = bytes[i];
            // 字节高4位
            hex = String.valueOf(hexStr.charAt((b & 0xF0) >> 4));
            // 字节低4位
            hex += String.valueOf(hexStr.charAt(b & 0x0F));
            stringBuffer.append(hex);
            stringBuffer.append(" ");
        }
        return stringBuffer.toString();
    }

    /**
     * @param hexString
     * @return 将十六进制转换为字节数组
     */
    public static byte[] Hex2Bin(String hexString){
        //hexString的长度对2取整，作为bytes的长度
        int len = hexString.length()/2;
        if (len > 10 * 1024 * 1024) {
            return null;
        }
        byte[] bytes = new byte[len];
        byte high = 0;//字节高四位
        byte low = 0;//字节低四位

        for(int i=0;i<len;i++){
            //右移四位得到高位
            high = (byte)((hexStr.indexOf(hexString.charAt(2*i))) << 4 );
            low = (byte)hexStr.indexOf(hexString.charAt(2*i+1));
            bytes[i] = (byte) (high|low);//高地位做或运算
        }
        return bytes;
    }


    /**
     * 生成透传消息，json格式
     * {
     * sound : "android.resource://com.imo/raw/work_ring",
     * extras : {
     *     "stype": "0",
     *     "sid": "2345595",
     *     "rid": "56a836a837a002f6338f0a7e",
     *     "rname": "shen2",
     *     "rtm": "1453864616",
     *     "etm": "60",
     *     "rtype": "0"
     * };
     * vibrate : "true",
     * msg : "shen2正在呼叫你，请打开imo班聊接听...[11: 16 ]",
     * title : ;
     * }
     * 转换为=======>
     * {
     *     "title": "imo班聊",
     *     "msg": "sk:x ",
     *     "extras": {
     *         "sound": "default",
     *         "vibrate": "true",
     *         "srv_time": "1453866977",
     *         "cb": "http://222.73.33.230/count/mobileReport.php",
     *         "srv_id": "1102"
     *     }
     * }
     * @param messageId int, msg id
     * @param msgMap Map, contains all msg info
     * @param jsonParser JSONParser, for parse map
     * @return String
     */
    @SuppressWarnings("unchecked")
    public static String makeJsonMessage(int messageId, Map<String, String> msgMap, JSONParser jsonParser){
        if (msgMap == null) {
            logger.error("makeJsonMessage failed, msgMap is null.");
            return "";
        }
        String msgContent = msgMap.get(ConstantValue.PUSH_TAG_MSG);
        if (StringUtils.isEmpty(msgContent)) {
            logger.error("makeJsonMessage failed, push msgcontent is empty.");
            return "";
        }
        LinkedHashMap<String, Object> jsonMap = new LinkedHashMap<String, Object>();
        jsonMap.put(ConstantValue.PUSH_TAG_TITLE, ConstantValue.PUSH_TITLE);   // 设置消息标题
        jsonMap.put(ConstantValue.PUSH_TAG_MSG, msgContent);                      // 设置消息体
        long now = System.currentTimeMillis()/1000;
        String extraStr = msgMap.get(ConstantValue.PUSH_TAG_EXTRAS);
        if (!StringUtils.isEmpty(extraStr)) {
            logger.debug("makeJsonMessage, contains extras field, show extras:"+extraStr);
            try {
                jsonParser.reset();
                Object obj = jsonParser.parse(extraStr, InnerStringFactory.Instance());
                LinkedHashMap<String, String> extraMap = null;
                if (obj instanceof Map) {
                    extraMap = new LinkedHashMap<String, String>((LinkedHashMap<String, String>)obj);
                    if (!msgMap.containsKey(ConstantValue.PUSH_TAG_SOUND)) {
                        extraMap.put(ConstantValue.PUSH_TAG_SOUND, "default");
                    } else {
                        extraMap.put(ConstantValue.PUSH_TAG_SOUND, msgMap.get(ConstantValue.PUSH_TAG_SOUND));
                    }
                    if (!msgMap.containsKey(ConstantValue.PUSH_TAG_VIBRATE)) {
                        extraMap.put(ConstantValue.PUSH_TAG_VIBRATE, "true");
                    } else {
                        extraMap.put(ConstantValue.PUSH_TAG_VIBRATE, msgMap.get(ConstantValue.PUSH_TAG_VIBRATE));
                    }
                    extraMap.put(ConstantValue.PUSH_TAG_TIME, String.valueOf(now));
                    //extraMap.put(ConstantValue.PUSH_TAG_CB, ConstantValue.PUSH_CB_LINK);
                    extraMap.put(ConstantValue.PUSH_TAG_SRV_ID, String.valueOf(messageId));
                } else if (obj instanceof List){
                    logger.warn("makeJsonMessage, no supported list type.");
                    return "";
                }
                jsonMap.put(ConstantValue.PUSH_TAG_EXTRAS, extraMap);
            } catch (ParseException e) {
                logger.error("makeJsonMessage, parse extras failed. "+e.getMessage());
            }
        } else {
            LinkedHashMap<String, String> extraMap = new LinkedHashMap<String, String>();
            if (!msgMap.containsKey(ConstantValue.PUSH_TAG_SOUND)) {
                extraMap.put(ConstantValue.PUSH_TAG_SOUND, "default");
            } else {
                extraMap.put(ConstantValue.PUSH_TAG_SOUND, msgMap.get(ConstantValue.PUSH_TAG_SOUND));
            }
            if (!msgMap.containsKey(ConstantValue.PUSH_TAG_VIBRATE)) {
                extraMap.put(ConstantValue.PUSH_TAG_VIBRATE, "true");
            } else {
                extraMap.put(ConstantValue.PUSH_TAG_VIBRATE, msgMap.get(ConstantValue.PUSH_TAG_VIBRATE));
            }
            extraMap.put(ConstantValue.PUSH_TAG_TIME, String.valueOf(now));
            //extraMap.put(ConstantValue.PUSH_TAG_CB, ConstantValue.PUSH_CB_LINK);
            extraMap.put(ConstantValue.PUSH_TAG_SRV_ID, String.valueOf(messageId));
            jsonMap.put(ConstantValue.PUSH_TAG_EXTRAS, extraMap);
        }

        return JSONObject.toJSONString(jsonMap);
    }

    @SuppressWarnings("unchecked")
    public static Map<String, String> JsonStr2StrMap(String jsonStr) {
        Object obj = null;
        synchronized (defaultJsonParser) {
            defaultJsonParser.reset();
            try {
                obj = defaultJsonParser.parse(jsonStr, InnerStringFactory.Instance());
            } catch (ParseException e) {
                logger.error("parse json string failed.");
            }
        }
        Map<String, String> strMap = null;
        if (obj instanceof Map) {
            strMap = (LinkedHashMap<String, String>)obj;
        } else {
            strMap = new LinkedHashMap<String, String>();
        }
        return strMap;
    }


    @SuppressWarnings("unchecked")
    public static Map<String, Object> jsonStr2ObjMap(String jsonStr){
        Object obj = null;
        synchronized (defaultJsonParser) {
            defaultJsonParser.reset();
            try {
                obj = defaultJsonParser.parse(jsonStr, InnerObjectFactory.Instance());
            } catch (ParseException e) {
                logger.error("parse json string failed.");
            }
        }
        Map<String, Object> strMap = null;
        if (obj instanceof Map) {
            strMap = (LinkedHashMap<String, Object>)obj;
        } else {
            strMap = new LinkedHashMap<String, Object>();
        }
        return strMap;
    }

    private static class InnerStringFactory implements ContainerFactory {
        private static InnerStringFactory inst;
        public static InnerStringFactory Instance(){
            if (inst == null) {
                inst = new InnerStringFactory();
            }
            return inst;
        }

        @Override
        public Map createObjectContainer() {
            return new LinkedHashMap<String, String>();
        }

        @Override
        public List creatArrayContainer() {
            return new ArrayList<String>();
        }
    }


    private static class InnerObjectFactory implements ContainerFactory {
        private static InnerObjectFactory inst;
        public static InnerObjectFactory Instance(){
            if (inst == null) {
                inst = new InnerObjectFactory();
            }
            return inst;
        }

        @Override
        public Map createObjectContainer() {
            return new LinkedHashMap<String, Object>();
        }

        @Override
        public List creatArrayContainer() {
            return new ArrayList<Object>();
        }
    }

    public static String map2HtmlTable(Map<String, String> stringMap){
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append("<br/>");
        stringBuilder.append("<h2>详细信息：</h2>");
        stringBuilder.append("<table border=\"1\">");

        for (Map.Entry<String, String> field : stringMap.entrySet()) {
            stringBuilder.append("<tr>");
            stringBuilder.append("<td>");
            stringBuilder.append(field.getKey());
            stringBuilder.append("</td>");
            stringBuilder.append("<td>");
            stringBuilder.append(field.getValue());
            stringBuilder.append("</td>");
            stringBuilder.append("</tr>");
        }

        stringBuilder.append("</table>");
        return stringBuilder.toString();
    }

    /**
     * 将字节转化为更大单位
     * @param byteNum
     * @return
     */
    public static String normalizeByte(long byteNum){
        if (byteNum > _1GB) {
            return decimalFormat.format(1.0*byteNum/_1GB) + " GB";
        } else if (byteNum > _1MB) {
            return decimalFormat.format(1.0*byteNum/_1MB) + " MB";
        } else if (byteNum > _1KB) {
            return decimalFormat.format(1.0*byteNum/_1KB) + " KB";
        }
        return String.valueOf(byteNum) + " Byte";
    }

    /**
     * 将秒转化为更大单位
     * @param s 单位是秒
     * @return 返回字符串
     */
    public static String normalizeDate(long s){
        if (s > _1YEAR) {
            return decimalFormat.format(1.0*s/_1YEAR) + " 年";
        } else if (s > _1MONTH) {
            return decimalFormat.format(1.0*s/_1MONTH) + " 月";
        } else if (s > _1DAY) {
            return decimalFormat.format(1.0*s/_1DAY) + " 天";
        } else if (s > _1HOUR) {
            return decimalFormat.format(1.0*s/_1HOUR) + " 小时";
        } else if (s > _1S) {
            return decimalFormat.format(1.0*s/_1S) + " 秒";
        }
        return String.valueOf(s) + " 毫秒";
    }
} // class DspStringUtils end
