/**
 * @file TaskInfo.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年02月11日-上午8:37
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-2-11	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.client;

import java.util.Map;

public class TaskInfo {
    private int cid;
    private int uid;
    private int transid;
    private int platForm;
    private int pushType;
    private String token;
    private Map<String, String> msgMap;

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

    public int getTransid() {
        return transid;
    }

    public void setTransid(int transid) {
        this.transid = transid;
    }

    public int getPlatForm() {
        return platForm;
    }

    public void setPlatForm(int platForm) {
        this.platForm = platForm;
    }

    public int getPushType() {
        return pushType;
    }

    public void setPushType(int pushType) {
        this.pushType = pushType;
    }

    public String getToken() {
        return token;
    }

    public void setToken(String token) {
        this.token = token;
    }

    public Map<String, String> getMsgMap() {
        return msgMap;
    }

    public void setMsgMap(Map<String, String> msgMap) {
        this.msgMap = msgMap;
    }
} // class TaskInfo end
