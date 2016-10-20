const net = require('net');
var logger = require('../etc/logger').logger;
var ErrRet = require('./err_ret');
var events = require('events');
var BinaryDecoder = require('.utils').BinaryDecoder;
var BinaryEncoder = require('.utils').BinaryEncoder;
var BinarySpliter = require('./utils').BinarySpliter;
var encodeMessage = require('./utils').encodeMessage;
var bufferToHex = require('./utils').bufferToHex;
var getHeader = require('./utils').getHeader;
var utils = require('./utils');

var PUBLISH_TASK = 4000;
var seq = 0;
var connection_id = 0;
var SEND_BUFFER_LEN = 4 * 1024 * 1024;
var buf = new Buffer(SEND_BUFFER_LEN);

function getSeq () {
    if (seq == 0xffffffff) {
        seq = 0;
    }
    return ++seq;
}

function makeEventName(seq){
    return "event_seq:" + String(seq);
}


/**
 * @param  {json}
 * @return {[type]}
 */
function taskCenterClient () {
    this.option = null;
    this.client = null;
    this.connection_id = ++connection_id;
    this.connected = false;
    this.emitter =  new events.EventEmitter();
    this.emitter.setMaxListeners(0);
    this.binarySpliter = new BinarySpliter();
}

/**
 * 连接server
 * @param  {object} {ip:'xx', port:xx}
 * @return {[type]}
 */
taskCenterClient.prototype.connectServer = function (option){
    this.option = option;
    this.client = net.connect(option, function(){
        console.log('connect server ok.');
        this.connected = true;
        this.ready = true;
    });
    var self = this;

    this.client.on("data", function(data) {
        logger.info("recv raw data:", bufferToHex(data));
        var packets = self.binarySpliter.getPackets(data);
            if(!utils.binarySpliter.is_good){
                logger.error("binarySpliter spliter packet error data:", bufferToHex(data));
                self.binarySpliter.reset();
                self.client.end();
                self.connected = false;
                return ;
            } else {
                for(var i = 0;i < packets.length;i++){
                    //client.recv_msg_cb(packets[i]);
                    //decode header
                    var header = getHeader(packets[i]);
                    if(!header){
                        logger.error("get packet header error ");
                        return ;
                    }
                    var recv_cmd = header[1];
                    if(self.request_cmd_list.indexOf(recv_cmd) != -1){
                        //todo  
                        //means this is a response
                        self.emitter.emit(makeEventName(header[constants.HEADER_OFFSET.SEQ]),
                            packets[i]);
                    }
                    else{
                        //means this is a notice msg
                        self.recv_msg_cb(packets[i]);
                    }
                }
            }
    });
}

/**
 * 发送字节数据到服务端
 * @param  {buffer} req
 * @return {[type]}
 */
taskCenterClient.prototype.sendRequest = function (req) {
    if (this.connected == false) {
        return;
    }
    logger.debug('send data to server, id:'+this.connection_id);
    client.write(req);
}

/**
 * @param  {number} taskid
 * @param  {object} request
 * @param  {Function} callback
 * @return {[type]}
 * 
 * len cmd seq transid kv_num [k v]
 */
taskCenterClient.prototype.publishTask = function (taskid, req, cb) {
    // make request 
    var seq = getSeq();
    var transid = getSeq();
    var binaryEncoder = new BinaryEncoder(buf);
    binaryEncoder.writeStreamUint32Array([0, PUBLISH_TASK, seq]);
    // cid, uid
    binaryEncoder.writeStreamUint32(0);
    binaryEncoder.writeStreamUint32(0);
    binaryEncoder.writeStreamUint32(transid);

    // kv_num
    var kv_num = 0;
    var kv_pos = binaryEncoder.get_offset();
    binaryEncoder.writeStreamUint32(kv_num);
    binaryEncoder.writeStreamString("taskid");
    binaryEncoder.writeStreamString(taskid+"");
    ++kv_num;
    binaryEncoder.writeStreamString("task_desc");
    binaryEncoder.writeStreamString(req.body.task_desc);
    ++kv_num;
    binaryEncoder.writeStreamString("platform");
    binaryEncoder.writeStreamString(req.body.platform);
    ++kv_num;
    binaryEncoder.writeStreamString("task_type");
    binaryEncoder.writeStreamString(req.body.task_type);
    ++kv_num;
    binaryEncoder.writeStreamString("task_name");
    binaryEncoder.writeStreamString(req.body.task_name);
    ++kv_num;
    binaryEncoder.writeStreamString("task_stime");
    binaryEncoder.writeStreamString(req.body.task_stime);
    ++kv_num;
    binaryEncoder.writeStreamString("task_etime");
    binaryEncoder.writeStreamString(req.body.task_etime);
    ++kv_num;
    binaryEncoder.writeStreamString("task_rule");
    binaryEncoder.writeStreamString(req.body.task_rule);
    ++kv_num;
    binaryEncoder.writeStreamString("task_pay");
    binaryEncoder.writeStreamString(req.body.task_pay);
    ++kv_num;
    binaryEncoder.writeStreamString("task_total_num");
    binaryEncoder.writeStreamString(req.body.task_total_num);
    ++kv_num;
    binaryEncoder.writeStreamString("task_reward_num");
    binaryEncoder.writeStreamString(req.body.task_reward_num);
    ++kv_num;
    binaryEncoder.writeStreamString("task_total_step");
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

    // write kv_num
    binaryEncoder.writeStreamUint32Offset(kv_num, kv_pos);

    // write len
    binaryEncoder.writeStreamUint32Offset(binaryEncoder.get_offset(), 0);

    this.sendRequest(buf.slice(0, binaryEncoder.get_offset());
    var err = new ErrRet(0, 'ok');
    var event_name = makeEventName(seq);
    var self = this;
    //set timeout
    var eventid = setTimeout(function(){
        //once timeout , remove onseq event
        self.emitter.removeAllListeners(event_name);
        clearInterval(eventid);
        result = new ErrRet(5000, 'backend timeout');
        return  cb(result);
    }, 5000)
    //regist callback   
    self.emitter.once(event_name, function(packet){
        self.emitter.removeAllListeners(event_name);
        clearTimeout(eventid);
        cb(packet);
    });     
}




/**
 * taskCenterClient wrap
 * @return {[type]}
 */
function taskCenterClientMgr () {
    this.task_center_clients = [];
    this.task_center_clients_num = 0;
}

/**
 * @param  {number} hashid
 * @return {taskCenterClient} client/null
 */
taskCenterClientMgr.prototype.selectClient = function (hashid) {
    var client_num = this.task_center_clients.length;
    if (client_num == 0) {
        return null;
    }
    var index = hashid % client_num;
    return this.task_center_clients[index];
}

/**
 * @param  {number} taskid
 * @param  {object} req
 * @param  {Function} response callback
 * @return {[type]}
 */
taskCenterClientMgr.prototype.publishTask = function (taskid, req, cb) {
    var client = this.selectClient(taskid);
    client.publishTask(taskid, req, cb);
}

var task_center_clients_mgr = null;

/**
 * @param  {array} options {
 * [
 *      ip:'xx', port:xx
 * ]
 * }
 * @return {taskCenterClientMgr} task_center_clients_mgr
 */
var createClient = function (options) {
    // init taskcenter
    if (task_center_clients_mgr == null) {
        task_center_clients_mgr = new taskCenterClientMgr();
    }
    logger.debug("createClient, option:"+options);
    //var task_center_opion = options;
    for (var i = 0; i < options.length; i++) {
        var client = new taskCenterClient();
        client.connectServer(options[i]);
        task_center_clients_mgr.task_center_clients.push(client);
        ++task_center_clients_mgr.task_center_clients_num;
    };
    logger.debug("create client, num:"+task_center_clients_mgr.task_center_clients_num+", option:"+options);
    return task_center_clients_mgr;
}

// 导出类型
exports.taskCenterClientMgr = taskCenterClientMgr;
// 导出实例操作接口
exports.createClient = createClient;
