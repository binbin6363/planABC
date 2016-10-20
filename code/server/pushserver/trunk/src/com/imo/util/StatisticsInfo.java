/**
 * @file StatisticsInfo.java
 * @brief
 *
 *  Description here
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年02月01日-上午10:58
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-2-1	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.util;

import java.util.LinkedHashMap;
import java.util.Map;

// 還可以統計當前隊列有多少任務
public class StatisticsInfo {
    // 每个启动的push平台对应的推送统计
    public static final Map<String, PushStatistics> platform_push_statistics = new LinkedHashMap<String, PushStatistics>();

    private static final StatisticsInfo inst = new StatisticsInfo();

    private StatisticsInfo(){

    }

    public static StatisticsInfo getInst(){
        return inst;
    }

    public String statistics2HtmlTable(){

        StringBuilder stringBuilder = new StringBuilder();

        stringBuilder.append("<br/>");
        stringBuilder.append("<h2>推送信息统计</h2>");

        stringBuilder.append("<table border=\"1\">");
        stringBuilder.append("<tr/>");
        stringBuilder.append("<th>平台名称</th>");
        stringBuilder.append("<th>推送总耗时</th>");
        stringBuilder.append("<th>推送消息数</th>");
        stringBuilder.append("<th>成功消息数</th>");
        stringBuilder.append("<th>失败消息数</th>");
        stringBuilder.append("<th>队列消息数</th>");
        stringBuilder.append("<tr/>");

        for (Map.Entry<String, PushStatistics> entry : platform_push_statistics.entrySet()) {
            PushStatistics pushStatistic = entry.getValue();

            stringBuilder.append("<tr/>");
            stringBuilder.append("<td>");
            stringBuilder.append(String.valueOf(pushStatistic.platform_name));
            stringBuilder.append("</td>");

            stringBuilder.append("<td>");
            stringBuilder.append(DspStringUtils.normalizeDate(pushStatistic.push_total_cost_time));
            stringBuilder.append("</td>");

            stringBuilder.append("<td>");
            stringBuilder.append(String.valueOf(pushStatistic.push_msg_num));
            stringBuilder.append("</td>");

            stringBuilder.append("<td>");
            stringBuilder.append(String.valueOf(pushStatistic.push_success_msg_num));
            stringBuilder.append("</td>");

            stringBuilder.append("<td>");
            stringBuilder.append(String.valueOf(pushStatistic.push_failed_msg_num));
            stringBuilder.append("</td>");

            stringBuilder.append("<td>");

            stringBuilder.append(String.valueOf(pushStatistic.push_queue_msg_num));
            stringBuilder.append("</td>");
            stringBuilder.append("<tr/>");
        }
        stringBuilder.append("</table>");

        return stringBuilder.toString();
    }

    /**
     *
     * @param pushPlatform push平台，字符串表示
     * @param cost_time push所花总时间，毫秒為單位
     * @param result push结果，成功失败标志，0表示成功，繁殖失败
     */
    synchronized public void recordPush(String pushPlatform, long cost_time, int result, int queue_msg_num){
        if (platform_push_statistics.containsKey(pushPlatform)) {
            PushStatistics statistics = platform_push_statistics.get(pushPlatform);
            statistics.addCostTime(cost_time, result);
            statistics.setPush_queue_msg_num(queue_msg_num);
        } else {
            if (!platform_push_statistics.containsKey(pushPlatform)) {
                PushStatistics pushStatistics = new PushStatistics();
                pushStatistics.setPlatform_name(pushPlatform);
                pushStatistics.addCostTime(cost_time, result);
                pushStatistics.setPush_queue_msg_num(queue_msg_num);
                platform_push_statistics.put(pushPlatform, pushStatistics);
            }
        }
    }


    private class PushStatistics {
        // 推送的总耗时
        public long push_total_cost_time = 0;
        // 成功推送的消息数
        public long push_success_msg_num = 0;
        // 推送失败的消息数
        public long push_failed_msg_num = 0;
        // 推送的消息量统计
        public long push_msg_num = 0;

        public synchronized void setPush_queue_msg_num(long push_queue_msg_num) {
            this.push_queue_msg_num = push_queue_msg_num;
        }

        // 队列消息数
        public long push_queue_msg_num = 0;
        public String platform_name = "";

        public String getPlatform_name() {
            return platform_name;
        }

        public void setPlatform_name(String platform_name) {
            this.platform_name = platform_name;
        }

        public synchronized void addCostTime(long cost_time, int result) {
            if (cost_time == 0) {
                return;
            }
            ++push_msg_num;
            if (result == 0) {
                ++push_success_msg_num;
            } else {
                ++push_failed_msg_num;
            }
            push_total_cost_time += cost_time;
        }
    }
} // class StatisticsInfo end
