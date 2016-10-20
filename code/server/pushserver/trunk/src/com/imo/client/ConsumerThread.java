/**
 * @file ConsumerThread.java
 * @brief
 *
 *  单独的线程，从redis队列中获取消息消费
 *   1. 先从全局队列获取任务
 *   2. 再获取任务下的目标推送用户
 *   3. 2完成之后再回到1
 *
 * @author bbwang
 * @version 1.0
 * @date 2016年02月08日-下午10:02
 *
 * @see
 *
 * @par 版本记录：
 * <table border=1>
 *  <tr> <th> 版本	<th>日期			<th>作者    	<th>备注 </tr>
 *  <tr> <td> 1.0	<td>16-2-8	    <td>bbwang      <td>创建 </tr>
 * </table>
 */
package com.imo.client;

import com.imo.msg.PushRequest;
import com.imo.pushclient.PushClient;
import com.imo.pushclient.PushClientMgr;
import com.imo.util.ConstantValue;
import com.imo.util.DspStringUtils;
import org.apache.commons.io.FileUtils;
import org.apache.commons.lang.StringUtils;
import org.apache.log4j.Logger;
import redis.clients.jedis.Jedis;
import redis.clients.jedis.Pipeline;
import redis.clients.jedis.Response;

import java.io.File;
import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;


public class ConsumerThread extends Thread{

    private final Logger logger = Logger.getLogger(ConsumerThread.class);
    private volatile boolean stop = false;
    private String host = "localhost";
    private int port = 6379;
    private Jedis jedis = null;
    private volatile long consume_task_num = 0;

    public void setHost(String host) {
        this.host = host;
    }

    public void setPort(int port) {
        this.port = port;
    }

    @Override
    public synchronized void start() {
        try {
            jedis = new Jedis(this.host, this.port);
            sleep(2000);
            jedis.configSet("timeout", "1");
            logger.info(jedis.ping());
        } catch (InterruptedException e) {
            logger.error("start consumer thread failed. interrupted exception.");
            logger.error(e.getMessage());
        }
        logger.info("start consumer thread.");
        super.start();
    }

    public synchronized void notifyStop(){
        stop = true;
        logger.info("notify stop consumer thread.");
    }

    @Override
    public void run() {
        logger.info("start consumer thread ok, thread id:"+getId());
        String localTaskKey = loadTaskKeyFromLocal();
        while (!stop) {
            if (jedis == null || !jedis.isConnected()){
                logger.error("redis connection is not established. exit consumer thread.");
                return;
            }
            String taskKey = getTask(localTaskKey);
            localTaskKey = null;
            consume_task_num = 0;
            int ret = consumeTask(taskKey);
            if (ConstantValue.ERR_NO_TASK == ret) {
                // 如果没有任务key，就sleep一下
                logger.debug("no task in queue.");
                try {
                    sleep(10);
                } catch (InterruptedException e) {
                    logger.error("sleep interrupted in consumer thread..");
                }
            } else if (ConstantValue.ERR_DONE_TASK_OK == ret) {
                logger.info("done consume task ok, task key:"+taskKey+", consume task num:"+ consume_task_num);
            }
        }
        try {
            jedis.getClient().getSocket().shutdownInput();
            jedis.getClient().getSocket().shutdownOutput();
        } catch (IOException e) {
            logger.error(e.getMessage());
        }
        logger.info("notify stop, exit consumer thread..");
    }

    /**
     * 循环消费任务中的元素
     * @param taskKey
     * @return int;任务为空，则直接返回ConstantValue.ERR_NO_TASK；任务循环处理完成，则返回ConstantValue.ERR_DONE_TASK_OK
     */
    private int consumeTask(String taskKey) {
        if (StringUtils.isEmpty(taskKey)) {
            return ConstantValue.ERR_NO_TASK;
        }

        // 1. 先将任务key存入本地文件
        saveTaskKeyToLocal(taskKey);

        // 2. 循环处理任务体
        return loopConsumeTaskTarget(taskKey);
    }

    private int loopConsumeTaskTarget(String taskKey){
        int ret;
        do {
            // 1. 获取任务目标
            String taskTarget = getTaskTarget(taskKey);
            if (StringUtils.isEmpty(taskTarget)) {
                ret = ConstantValue.ERR_DONE_TASK_OK;
                break;
            }
            // 2. 解析并压入push消费队列
            ret = addTaskToPushQueue(taskTarget);
            if (0 != ret) {
                break;
            }

            ++consume_task_num;
            if (consume_task_num % 100 == 0) {
                logger.info("done get task target, cnt:"+consume_task_num);
            }
        } while (true);
        return ret;
    }

    /**
     * taskTarget json format:
     * {\"uid\":500001,\"pushid\":\"push_890098121\",\"platform\":1,\"push_type\":3,\"token\":\"08602750202301622000000525000001\",\"title\":\"dsp\",\"msgcontent\":\"你收到一条新任务\"}
     * @param taskTarget json string
     * @return int
     */
    private int addTaskToPushQueue(String taskTarget) {

        Map<String, String> taskMap = DspStringUtils.JsonStr2StrMap(taskTarget);

        TaskInfo taskInfo = new TaskInfo();
        taskInfo.setCid(0);
        String uidStr = taskMap.get("uid");
        if (!StringUtils.isEmpty(uidStr)) {
            taskInfo.setUid(Integer.valueOf(uidStr));
        }
        String platform = taskMap.get("platform");
        if (!StringUtils.isEmpty(platform)) {
            taskInfo.setPlatForm(Integer.valueOf(platform));
        }
        String pushType = taskMap.get("push_type");
        if (!StringUtils.isEmpty(pushType)) {
            taskInfo.setPushType(Integer.valueOf(pushType));
        }
        String token = taskMap.get("token");
        if (!StringUtils.isEmpty(token)) {
            taskInfo.setToken(token);
        } else {
            logger.error("task token is empty, push task failed. lost one push msg.");
            return 2;
        }
        Map<String, String> msgMap = new LinkedHashMap<String, String>();
        String msgStr = taskMap.get(ConstantValue.PUSH_TAG_MSG);
        if (StringUtils.isEmpty(msgStr)) {
            logger.error("task msg is empty, push task failed. lost one push msg.");
            return 2;
        }
        msgMap.put(ConstantValue.PUSH_TAG_MSG, msgStr);
        String extrasStr = taskMap.get(ConstantValue.PUSH_TAG_EXTRAS);
        if (!StringUtils.isEmpty(extrasStr)) {
            msgMap.put(ConstantValue.PUSH_TAG_EXTRAS, extrasStr);
        }
        taskInfo.setMsgMap(msgMap);

        PushRequest pushRequest = new PushRequest(taskInfo);
        PushClient pushClient = PushClientMgr.getClient(pushRequest.getPushType());
        if (pushClient == null) {
            logger.error("get push client failed. push type:"+pushRequest.getPushType());
            return -1;
        }
        logger.info("consume thread add one push request to queue. "+pushRequest.toString());
        return pushClient.addRequest(pushRequest);

    }

    private String popOneItem(String key, int dbIndex){
        Pipeline p = jedis.pipelined();
        Response<String> select_response  = p.select(dbIndex);
        Response<String> lpop_response = p.lpop(key);
        p.sync();

        String response_str = select_response.get();
        if (!StringUtils.equalsIgnoreCase("ok", response_str)) {
            logger.error("select db index failed. response:"+response_str);
            return null;
        }
        response_str = lpop_response.get();
        if (StringUtils.equalsIgnoreCase("(nil)", response_str)) {
            logger.debug("lpop msg from db ok. response:"+response_str);
            return null;
        }
        logger.info("pop one key:"+response_str);

        return response_str;
    }

    /**
     * 从主队列获取任务key
     * @return string
     */
    private String getTask(String specifiedTaskKey) {
        if (!StringUtils.isEmpty(specifiedTaskKey)) {
            logger.info("get task key from local data. key:"+specifiedTaskKey);
            return specifiedTaskKey;
        }
        String taskKey = popOneItem(ConstantValue.MASTER_KEY, ConstantValue.DB_INDEX);
        if (!StringUtils.isEmpty(taskKey) && !taskKey.startsWith(ConstantValue.TASK_KEY_PERFIX)) {
            logger.warn("done get task, key format is no good. key:"+taskKey);
        }
        return taskKey;
    }

    /**
     * 从各任务队列获取任务目标
     * @return task key string
     */
    private String getTaskTarget(String taskTargetKey){
        if (!StringUtils.isEmpty(taskTargetKey)) {
            logger.info("get task target key from local data. key:"+taskTargetKey);
            return taskTargetKey;
        }
        return popOneItem(taskTargetKey, ConstantValue.DB_INDEX);
    }

    /**
     * 从本地磁盘获取任务key
     * @return string
     */
    private String loadTaskKeyFromLocal() {
        String taskKey = null;
        logger.info("load task key from local file:"+ConstantValue.LOCAL_FILE_PATH);
        File taskKeyFile = new File(ConstantValue.LOCAL_FILE_PATH);
        if (!taskKeyFile.exists()) {
            logger.info("the first time load task key from local file, file is not exist.");
            return null;
        }
        try {
            taskKey = FileUtils.readFileToString(taskKeyFile);
            if (StringUtils.isEmpty(taskKey)) {
                logger.info("load task key from local file, task key is empty.");
                return null;
            } else if (!taskKey.startsWith(ConstantValue.TASK_KEY_PERFIX)){
                logger.warn("load task key from local file, but content is no match. ignore this key:"+taskKey);
                return null;
            } else {
                logger.info("load task key from local file, task key:"+taskKey);
                return taskKey;
            }
        } catch (IOException e) {
            logger.error(e.getMessage());
        }
        return null;
    }

    private int saveTaskKeyToLocal(String taskKey) {
        File taskKeyFile = new File(ConstantValue.LOCAL_FILE_PATH);
        try {
            FileUtils.writeStringToFile(taskKeyFile, taskKey, ConstantValue.LOCAL_FILE_ENCODE, false);
        } catch (IOException e) {
            logger.error("write task key to local file failed. file path:"+ConstantValue.LOCAL_FILE_PATH);
        }
        return 0;
    }

    public static void main(String[] args) {

        ConstantValue.LOCAL_FILE_PATH = "./tmpfile";
        ConstantValue.REDIS_HOST = "139.196.42.222";
        ConstantValue.REDIS_PORT = 6380;
        ConsumerThread consumerThread = ConsumerMgr.getConsumerThread();
        consumerThread.setHost(ConstantValue.REDIS_HOST);
        consumerThread.setPort(ConstantValue.REDIS_PORT);
        consumerThread.start();

    }
} // class ConsumerThread end
