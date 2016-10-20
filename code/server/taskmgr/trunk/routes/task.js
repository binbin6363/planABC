var express = require('express');
var router = express.Router();
var Connectors = require("./client/connectors");
var logger = require('../etc/logger').logger;
var options = require('../etc/options').options;
var constants = require('./utils/constants');
var BinaryDecoder = require('./utils/utils').BinaryDecoder;
var BinaryEncoder = require('./utils/utils').BinaryEncoder;
var bufferToHex = require('./utils/utils').bufferToHex;
//var task_center_client_mgr = null;


var CMD_CREATE_TASK = 5000;
var CMD_QUERY_TASK_LIST = 5001;
var CMD_QUERY_TASK_INFO = 5002;

var task_center_handler = null;
var redis_init_flag = false;
var leveldb_init_flag = false;
var mysql_init_flag = false;

var seq = 0;

var autoid = Date.now() & 0xFFFF;

var task_center_beat = function (server_session, request_buf) {
    // body...
    var head = [0, 0, ++seq];
    var binaryEncoder = new BinaryEncoder(request_buf);
    //binaryEncoder.writeStreamUint32Array(head);
    // 发往任务中心，组包头 COHEADER
    binaryEncoder.writeStreamUint32(28); // head_len,固定长度
    binaryEncoder.writeStreamUint32(0);  // uid
    binaryEncoder.writeStreamUint32(0);  // sender_coid
    binaryEncoder.writeStreamUint32(0);  // receiver_coid
    // server id
    binaryEncoder.writeStreamUint32(1111);
    // write packet len
    binaryEncoder.writeStreamUint32Offset(binaryEncoder.get_offset(), 0);
    var send_buf = request_buf.slice(0, binaryEncoder.get_offset());
    logger.info("send beat request to task_center. " + bufferToHex(send_buf));
    server_session.sendRequest(head, send_buf, function (){});

}


var init = function(options){
    // init taskcenter
    logger.info(options);
    var task_center_opion = options.task_center_opion;
    logger.info(task_center_opion);
    task_center_handler = new Connectors();
    if (!task_center_handler.init(task_center_opion, recv_data_cb, task_center_beat)) {
        logger.error("init connectors failed.");
    }
    //task_center_client_mgr = createClient(task_center_opion);
}

if (task_center_handler == null) {
        init(options);
}


var recv_data_cb = function (result_buf) {
    logger.debug("recv data: " + bufferToHex(result_buf));
}


// taskid = (32)timestamp + (4)serverid + (16)autoid
// nodejs仅仅支持53位的整型，大于的都会出现精度误差，因为node里面都以float表示整型
function makeTaskId (serverid) {
        var now = Date.now(); // unix timestamp
        if (autoid == 0xFFFF) {
                autoid = 0;
        }
        ++autoid;
        return 'task_serial_number_'+serverid+'_'+now+'_'+autoid;
}


function publishTask (req, res) {
	// 转发请求到taskcenter
    header = [0, CMD_CREATE_TASK, 0];

    var body = new Buffer(constants.SEND_BUFFER_LEN);
    var binaryEncoder = new BinaryEncoder(body);
    var publish_transid = ++seq;

    // 发往任务中心，组包头 COHEADER
    binaryEncoder.writeStreamUint32(28); // head_len,固定长度
    binaryEncoder.writeStreamUint32(0);  // uid
    binaryEncoder.writeStreamUint32(0);  // sender_coid
    binaryEncoder.writeStreamUint32(0);  // receiver_coid
    binaryEncoder.writeStreamUint32(publish_transid); // transid

    // kv_num
    var kv_num = 0;
    var kv_pos = binaryEncoder.get_offset();
    binaryEncoder.writeStreamUint32(kv_num);
    binaryEncoder.writeStreamString("task_sn");
    binaryEncoder.writeStreamString(makeTaskId(1));
    ++kv_num;
    binaryEncoder.writeStreamString("desc");
    binaryEncoder.writeStreamString(req.body.task_desc);
    ++kv_num;
    binaryEncoder.writeStreamString("signature");
    binaryEncoder.writeStreamString(req.body.task_signature);
    ++kv_num;
    binaryEncoder.writeStreamString("platform");
    binaryEncoder.writeStreamString(req.body.platform);
    ++kv_num;
    binaryEncoder.writeStreamString("type");
    binaryEncoder.writeStreamString(req.body.task_type);
    ++kv_num;
    binaryEncoder.writeStreamString("name");
    binaryEncoder.writeStreamString(req.body.task_name);
    ++kv_num;
    binaryEncoder.writeStreamString("stime");
    binaryEncoder.writeStreamString(req.body.task_stime);
    ++kv_num;
    binaryEncoder.writeStreamString("etime");
    binaryEncoder.writeStreamString(req.body.task_etime);
    ++kv_num;
    binaryEncoder.writeStreamString("rule");
    binaryEncoder.writeStreamString(req.body.task_rule);
    ++kv_num;
    binaryEncoder.writeStreamString("pay");
    binaryEncoder.writeStreamString(req.body.task_pay);
    ++kv_num;
    binaryEncoder.writeStreamString("icon");
    binaryEncoder.writeStreamString(req.body.task_icon);
    ++kv_num;
    binaryEncoder.writeStreamString("link");
    binaryEncoder.writeStreamString(req.body.task_file);
    ++kv_num;
    binaryEncoder.writeStreamString("size");
    binaryEncoder.writeStreamString("-1");
    ++kv_num;
    binaryEncoder.writeStreamString("status");
    binaryEncoder.writeStreamString("1");
    ++kv_num;
    binaryEncoder.writeStreamString("used_num");
    binaryEncoder.writeStreamString("0");
    ++kv_num;
    binaryEncoder.writeStreamString("total_num");
    binaryEncoder.writeStreamString(req.body.task_total_num);
    ++kv_num;
    binaryEncoder.writeStreamString("reward_num");
    binaryEncoder.writeStreamString(req.body.task_reward_num);
    ++kv_num;
    binaryEncoder.writeStreamString("cstep");
    binaryEncoder.writeStreamString("0");
    ++kv_num;
    binaryEncoder.writeStreamString("tstep");
    binaryEncoder.writeStreamString(req.body.task_total_step);
    ++kv_num;
    binaryEncoder.writeStreamString("publisher_name");
    binaryEncoder.writeStreamString(req.body.publisher_name);
    ++kv_num;
    binaryEncoder.writeStreamString("publisher_email");
    binaryEncoder.writeStreamString(req.body.publisher_email);
    ++kv_num;
    binaryEncoder.writeStreamString("publisher_phone");
    binaryEncoder.writeStreamString(req.body.publisher_phone);
    ++kv_num;
    binaryEncoder.writeStreamString("uid_list");
    binaryEncoder.writeStreamString(req.body.uid_list);
    ++kv_num;

    // write kv_num
    binaryEncoder.writeStreamUint32Offset(kv_num, kv_pos);
    logger.info("show publish request:");
    logger.info(header);
    logger.info(binaryEncoder.get_offset());
    logger.info(bufferToHex(body.slice(0, binaryEncoder.get_offset())));

    task_center_handler.sendRequest(""+publish_transid, header, body.slice(0, binaryEncoder.get_offset()), function (err,result) {
        var resObj = {}
        resObj.transid = publish_transid;
        resObj.ret_status = '成功';
        resObj.ret_code = 0;
        resObj.ret_msg = '';
        resObj.ret_data = []


        if (err.errNum == constants.ERROR_NUM.BACKEND_TIMEOUT) {
            logger.error("backend timeout.");
            resObj.ret_status = '失败'
            resObj.ret_code = err.errNum;
            resObj.ret_msg = '后端服务超时';
            res.render('task_publish_result_page', { title: '任务发布' , result: resObj});
            res.end();
            return;
        }
        // body...
		var binaryDecoder = new BinaryDecoder(result);
		var len = binaryDecoder.readStreamUint32();
		var cmd = binaryDecoder.readStreamUint32();
		var seq = binaryDecoder.readStreamUint32();
		var head_len = binaryDecoder.readStreamUint32();
		var uid = binaryDecoder.readStreamUint32();
		var sndid = binaryDecoder.readStreamUint32();
		var rcvid = binaryDecoder.readStreamUint32();
		var transid = binaryDecoder.readStreamUint32();
		var ret = binaryDecoder.readStreamUint32();
		var ret_msg = binaryDecoder.readStreamString();
		var taskid = binaryDecoder.readStreamString();

        resObj.ret_code = ret;
        resObj.ret_msg = ret_msg;
		if (!binaryDecoder.is_good)
		{
			logger.error("decode response failed.");
            resObj.ret_status = '任务发布失败'
            resObj.ret_msg = '解析回包错误';
            res.render('task_publish_result_page', { title: '任务发布' , result: resObj});
			res.end();
			return;
		}

        logger.info(err);
        if (err.errNum == 0 && ret == 0) {
            console.log("publish task ok. task id:"+taskid);
            logger.info("publish task ok. task id:"+taskid);
            var err_msg = '任务发布成功，任务id:'+taskid;
            resObj.ret_msg = err_msg;
            res.render('task_publish_result_page', { title: '任务发布' , result: resObj});
        } else {
            //var err_msg = '任务发布失败，任务id:'+taskid + ', ret:'+ret+', msg:'+err.errMsg;
            console.log("publish task failed. "+ret_msg);
            logger.error("publish task failed. "+ret_msg);
            resObj.ret_status = '失败';
            //resObj.ret_msg = err_msg;
            if (err.errNum != 0) {
                resObj.ret_code = err.errNum;
                //resObj.ret_msg = err.errMsg;
            }
            res.render('task_publish_result_page', { title: '任务发布' , result: resObj});
        }
        res.end();
    });
    
}


function queryTaskList (req, res) {
    // 转发请求到taskcenter
    header = [0, CMD_QUERY_TASK_LIST, 0];

    var body = new Buffer(constants.SEND_BUFFER_LEN);
    var binaryEncoder = new BinaryEncoder(body);
    var query_transid = ++seq;

    // 发往任务中心，组包头 COHEADER
    binaryEncoder.writeStreamUint32(28); // head_len,固定长度
    binaryEncoder.writeStreamUint32(0);  // uid
    binaryEncoder.writeStreamUint32(0);  // sender_coid
    binaryEncoder.writeStreamUint32(0);  // receiver_coid
    binaryEncoder.writeStreamUint32(query_transid); // transid

    // kv_num
    var kv_num = 0;
    var kv_pos = binaryEncoder.get_offset();
    binaryEncoder.writeStreamUint32(kv_num);
    binaryEncoder.writeStreamString("task_stime");
    binaryEncoder.writeStreamString(req.body.task_stime);
    ++kv_num;
    binaryEncoder.writeStreamString("task_etime");
    binaryEncoder.writeStreamString(req.body.task_etime);
    ++kv_num;

    // write kv_num
    binaryEncoder.writeStreamUint32Offset(kv_num, kv_pos);
    logger.info("show query task list request:");
    logger.info(header);
    logger.info(req.body);
    logger.info(binaryEncoder.get_offset());
    logger.info(bufferToHex(body.slice(0, binaryEncoder.get_offset())));

    task_center_handler.sendRequest(""+seq, header, body.slice(0, binaryEncoder.get_offset()), function (err,result) {
        var resObj = {}
        resObj.transid = query_transid;
        resObj.ret_status = '成功';
        resObj.ret_code = 0;
        resObj.ret_msg = '';


        if (err.errNum == constants.ERROR_NUM.BACKEND_TIMEOUT) {
            logger.error("backend timeout.");
            resObj.ret_status = '失败'
            resObj.ret_code = err.errNum;
            resObj.ret_msg = '后端服务超时';
            res.render('task_publish_result_page', { title: '任务列表查询' , result: resObj});
            res.end();
            return;
        }
        // body...
        var binaryDecoder = new BinaryDecoder(result);
        var len = binaryDecoder.readStreamUint32();
        var cmd = binaryDecoder.readStreamUint32();
        var seq = binaryDecoder.readStreamUint32();
        var head_len = binaryDecoder.readStreamUint32();
        var uid = binaryDecoder.readStreamUint32();
        var sndid = binaryDecoder.readStreamUint32();
        var rcvid = binaryDecoder.readStreamUint32();
        var transid = binaryDecoder.readStreamUint32();
        var ret = binaryDecoder.readStreamUint32();
        var ret_msg = binaryDecoder.readStreamString();

        var task_list = [];
        var num = binaryDecoder.readStreamUint32();
        for (var i = 0; i < num; ++i) {
            var task_info = binaryDecoder.readStreamString();
            task_list.push(JSON.parse(task_info));
        }

        resObj.ret_code = ret;
        resObj.ret_msg = ret_msg;
        resObj.ret_data = task_list;
        logger.info("get task list");
        logger.info(task_list);

        // 解任务列表
    

        if (!binaryDecoder.is_good)
        {
            resObj.ret_status = '失败'
            resObj.ret_msg = '解析回包错误';
            logger.error("decode response failed.");
            res.render('task_publish_result_page', { title: '任务详情查询' , result: resObj});
            res.end();
            return;
        }

        logger.info(err);
        if (err.errNum == 0) {
            logger.info("query task list ok.");
            var err_msg = '任务列表查询成功';
            resObj.ret_msg = err_msg;
            res.render('task_publish_result_page', { title: '任务列表查询' , result: resObj});
        } else {
            var err_msg = '任务列表查询失败' + ', ret:'+err.errNum+', msg:'+err.errMsg;
            logger.error(err_msg);
            resObj.ret_status = '失败';
            resObj.ret_msg = err_msg;
            if (err.errNum != 0) {
                resObj.ret_code = err.errNum;
                resObj.ret_msg = err.errMsg;
            }
            res.render('query_task_list_result_page', { title: '任务列表查询' , result: resObj});
        }
        res.end();
    });
}


function queryTaskInfo (req, res) {
    // 转发请求到taskcenter
    header = [0, CMD_QUERY_TASK_INFO, 0];

    var body = new Buffer(constants.SEND_BUFFER_LEN);
    var binaryEncoder = new BinaryEncoder(body);
    var query_transid = ++seq;

    // 发往任务中心，组包头 COHEADER
    binaryEncoder.writeStreamUint32(28); // head_len,固定长度
    binaryEncoder.writeStreamUint32(0);  // uid
    binaryEncoder.writeStreamUint32(0);  // sender_coid
    binaryEncoder.writeStreamUint32(0);  // receiver_coid
    binaryEncoder.writeStreamUint32(query_transid); // transid

    // kv_num
    var kv_num = 0;
    var kv_pos = binaryEncoder.get_offset();
    binaryEncoder.writeStreamUint32(kv_num);
    binaryEncoder.writeStreamString("id");
    binaryEncoder.writeStreamString(req.body.task_id);
    ++kv_num;

    // write kv_num
    binaryEncoder.writeStreamUint32Offset(kv_num, kv_pos);
    logger.info("show query task info request:");
    logger.info(header);
    logger.info(binaryEncoder.get_offset());
    logger.info(bufferToHex(body.slice(0, binaryEncoder.get_offset())));

    task_center_handler.sendRequest(""+query_transid, header, body.slice(0, binaryEncoder.get_offset()), function (err,result) {
        var resObj = {}
        resObj.transid = query_transid;
        resObj.ret_status = '成功';
        resObj.ret_code = 0;
        resObj.ret_msg = '';


        if (err.errNum == constants.ERROR_NUM.BACKEND_TIMEOUT) {
            logger.error("backend timeout.");
            resObj.ret_status = '失败'
            resObj.ret_code = err.errNum;
            resObj.ret_msg = '后端服务超时';
            res.render('task_publish_result_page', { title: '任务详情查询' , result: resObj});
            res.end();
            return;
        }

        // body...
        var binaryDecoder = new BinaryDecoder(result);
        var len = binaryDecoder.readStreamUint32();
        var cmd = binaryDecoder.readStreamUint32();
        var seq = binaryDecoder.readStreamUint32();
        var head_len = binaryDecoder.readStreamUint32();
        var uid = binaryDecoder.readStreamUint32();
        var sndid = binaryDecoder.readStreamUint32();
        var rcvid = binaryDecoder.readStreamUint32();
        var transid = binaryDecoder.readStreamUint32();
        var ret = binaryDecoder.readStreamUint32();
        var ret_msg = binaryDecoder.readStreamString();

        resObj.ret_code = ret;
        resObj.ret_msg = ret_msg;
        // 解任务信息

        if (!binaryDecoder.is_good)
        {
            resObj.ret_status = '失败'
            resObj.ret_msg = '解析回包错误';
            logger.error("decode response failed.");
            res.render('task_publish_result_page', { title: '任务详情查询' , result: resObj});
            res.end();
            return;
        }

        logger.info(err);
        if (err.errNum == 0) {
            var task_info = binaryDecoder.readStreamString();
            var taskInfoList = [];
            taskInfoList.push(JSON.parse(task_info)); 
            resObj.ret_data = taskInfoList;
            logger.info("query task info ok.");
            logger.info(taskInfoList);
            var err_msg = '任务信息查询成功';
            res.render('task_info_result_page', { title: '任务详情查询' , result: resObj});
        } else {
            var err_msg = '任务信息查询失败' + ', ret:'+err.errNum+', msg:'+err.errMsg;
            logger.error(err_msg);
            resObj.ret_status = '失败';
            resObj.ret_msg = err_msg;
            if (err.errNum != 0) {
                resObj.ret_code = err.errNum;
                resObj.ret_msg = err.errMsg;
            }
            res.render('query_task_info_result_page', { title: '任务详情查询' , result: resObj});
        }
        res.end();
    });
}



/* get task publish page. */
router.get('/publish', function(req, res, next) {
  res.render('task_publish_page', { title: '任务发布' });
});


/**
 * post data to page. handle
 * 1. 请求转发到任务中心处理
 */
router.post('/publish', function(req, res, next) {
	// param: task_desc,platform,task_type,task_name,task_icon,task_file,task_stime,task_etime,task_rule,task_pay,
	// task_total_num,task_reward_num,task_total_step,publisher_name,publisher_email,publisher_phone
	console.log("show /task/publish post param:");
	console.log(req.body);
	// 直接把请求转发到taskcenter处理
	publishTask(req, res);
});




/* query task list page. */
router.get('/query_task_list', function(req, res, next) {
  res.render('query_task_list_page', { title: '任务列表查询' });
});


/**
 * query task list
 * 1. 请求转发到任务中心处理
 */
router.post('/query_task_list', function(req, res, next) {
    // param: task_desc,platform,task_type,task_name,task_icon,task_file,task_stime,task_etime,task_rule,task_pay,
    // task_total_num,task_reward_num,task_total_step,publisher_name,publisher_email,publisher_phone
    console.log("show /task/query_task_list post param:");
    console.log(req.body);
    // 直接把请求转发到taskcenter处理
    queryTaskList(req, res);
});



/* query task list page. */
router.get('/query_task_info', function(req, res, next) {
  res.render('query_task_info_page', { title: '任务详情查询' });
});


/**
 * query task info
 * 1. 请求转发到任务中心处理
 */
router.post('/query_task_info', function(req, res, next) {
    // param: task_desc,platform,task_type,task_name,task_icon,task_file,task_stime,task_etime,task_rule,task_pay,
    // task_total_num,task_reward_num,task_total_step,publisher_name,publisher_email,publisher_phone
    console.log("show /task/query_task_info post param:");
    console.log(req.body);
    // 直接把请求转发到taskcenter处理
    queryTaskInfo(req, res);
});


module.exports = router;
exports.init = init;
