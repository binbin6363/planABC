/**
 * @file PushRequest.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2015年12月14日-下午8:44
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

import com.imo.client.TaskInfo;
import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;

public class PushRequest extends AbstractRequest{

    private static final Logger logger = Logger.getLogger(PushRequest.class);
    private int cid = 0;
    private int uid = 0;
    private int transId = 0;
    private int platForm = 0;
    private int pushType = 0;
    private String token = null;
    private Map<String, String> msgMap = null;
    private String requestId = null;

    public String getRequestId() {
        return requestId;
    }

    public void setRequestId(String requestId) {
        this.requestId = requestId;
    }

    public PushRequest(TaskInfo taskInfo){
        super(null);
        cid = taskInfo.getCid();
        uid = taskInfo.getUid();
        transId = taskInfo.getTransid();
        platForm = taskInfo.getPlatForm();
        pushType = taskInfo.getPushType();
        token = taskInfo.getToken();
        msgMap = taskInfo.getMsgMap();
    }

    public PushRequest(BinaryPacket binaryPacket) {
        super(binaryPacket);
        msgMap = new HashMap<String, String>();
    }

    public int getCid() {
        return cid;
    }

    public int getUid() {
        return uid;
    }

    public int getTransId() {
        return transId;
    }

    public int getPlatForm() {
        return platForm;
    }

    public int getPushType() {
        return pushType;
    }

    public String getToken() {
        return token;
    }

    public Map<String, String> getMsgMap() {
        return msgMap;
    }

    public boolean decode() {
        cid = binaryPacket.getInt();
        if (!CheckMessage()) {
            return false;
        }
        uid = binaryPacket.getInt();
        if (!CheckMessage()) {
            return false;
        }
        transId = binaryPacket.getInt();
        if (!CheckMessage()) {
            return false;
        }
        platForm = binaryPacket.getInt();
        if (!CheckMessage()) {
            return false;
        }
        pushType = binaryPacket.getInt();
        if (!CheckMessage()) {
            return false;
        }
        token = binaryPacket.getString(BinaryPacket.defaultCharSet);
        if (!CheckMessage()) {
            return false;
        }
        int kvNum = binaryPacket.getInt();
        if (!CheckMessage()) {
            return false;
        }
        String key = null;
        String value = null;
        for (int i = 0; i < kvNum; ++i) {
            if (!CheckMessage()) {
                return false;
            }
            key = binaryPacket.getString(BinaryPacket.defaultCharSet);
            value = binaryPacket.getString(BinaryPacket.defaultCharSet);
            if (StringUtils.isEmpty(key)) {
                logger.error("packet key is empty");
                break;
            }
            msgMap.put(key, value);
        }
        binaryPacket.free();
        logger.info("decode pushclient msg ok. cmd:"+getCmd()+", uid:"+uid + ", transid:"+transId);
        return true;
    }


    @Override
    public String toString() {
        StringBuilder stringBuilder = new StringBuilder();
        stringBuilder.append("[cid:");
        stringBuilder.append(cid);
        stringBuilder.append(", uid:");
        stringBuilder.append(uid);
        stringBuilder.append(", transid:");
        stringBuilder.append(transId);
        stringBuilder.append(", platform:");
        stringBuilder.append(platForm);
        stringBuilder.append(", pushtype:");
        stringBuilder.append(pushType);
        stringBuilder.append(", token:");
        stringBuilder.append(token);
        stringBuilder.append("; {");
        if (msgMap == null) {
            stringBuilder.append("null");
        } else {
            Set<Map.Entry<String, String>> entrySet = msgMap.entrySet();
            for (Map.Entry<String, String> entry : entrySet) {
                stringBuilder.append(entry.getKey());
                stringBuilder.append(" => ");
                stringBuilder.append(entry.getValue());
                stringBuilder.append("; ");
            }
        }
        stringBuilder.append("}]");
        return stringBuilder.toString();
    }

} // class PushRequest end
