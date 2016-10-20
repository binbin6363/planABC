var net = require('net');
var events = require('events');
var BinaryDecoder = require('../utils/utils').BinaryDecoder;
var BinaryEncoder = require('../utils/utils').BinaryEncoder;
var BinarySpliter = require('../utils/utils').BinarySpliter;
var encodeMessage = require('../utils/utils').encodeMessage;
var bufferToHex = require('../utils/utils').bufferToHex;
var getHeader = require('../utils/utils').getHeader;
var utils = require('../utils/utils');
var constants = require('../utils/constants');
var proto = require('../utils/proto');
var logger = require('../../etc/logger').logger;

function makeEventName(seq){
	return "seq:" + String(seq);
}
function Connector(){
	this.client = {};
	this.config = {}; 
	this.seq = 0;
	this.binarySpliter = new BinarySpliter();
	this.recv_msg_cb = function(){};
    this.beat_request_func = function(){};
	this.emitter =  new events.EventEmitter();
	this.emitter.setMaxListeners(0);
	this.connected = false;
	this.request_cmd_list = [];
	this.intervalId = {};
	this.deleted = false; //Connector will be detele
    this.bead_buf = new Buffer(1024);
}

Connector.prototype.getUniqId = function(){
	return this.config.address.ip + ":" + this.config.address.port; //"192.168.161.34:34"
}

Connector.prototype.sendkeepalive = function(){
    this.beat_request_func(this, this.bead_buf);
}

Connector.prototype.isConnected = function(){
	return this.connected;
}

Connector.prototype.setConnectedFlag = function(){
	return this.connected = true;
}

Connector.prototype.setDisconnectedFlag = function(){
	return this.connected = false;
}

Connector.prototype.getRemoteAddress = function(){
	return this.client.remoteAddress;
}
Connector.prototype.getRemotePort = function(){
	return this.client.remotePort;
}

Connector.prototype.disconnect = function(){
	return this.client.end();
}
/*
* delete this connector
*
*/
Connector.prototype.destroy = function(){
	if(this.intervalId){
		clearInterval(this.intervalId);
	}
	this.client.end();
	this.deleted = true;
}

Connector.prototype.send = function(msg){
	return this.client.write(msg);
}


/*
* @param {object} config
	  	{       
		  "address" : {"ip":'192.168.16.216',"port":2702},
		  "timeout_interval" : 10000 , // support reload
		  "keepalive_interval" : 5000, // support reload
		  "serverName" : "webagent",
		  "serverId" : "192.168.16.216:1234",
		  "serverType" : "web",
		  "timeout" : 2000             // support reload
	  	}
*
*
*/
Connector.prototype.reload = function(new_config){
	var self = this;
	if(new_config.timeout_interval != self.config.timeout_interval){
		logger.info("reload new timeout_interval:", new_config.timeout_interval);
		self.client.setTimeout(self.config.timeout_interval);
	}
	if(new_config.keepalive_interval != self.config.keepalive_interval){
		logger.info("reload new keepalive_interval:", new_config.keepalive_interval);
		if(self.intervalId){
			clearInterval(this.intervalId);
			self.intervalId = setInterval(function(){
				if(self.client.destroyed){
					logger.warn("interval check connect destroyed remoteAddress:",self.config.address.ip," port:", self.config.address.port);
					self.setDisconnectedFlag();
					self.client.connect(self.config.address.port, self.config.address.ip);
				}else{
					self.sendkeepalive();
				}
			},self.config.keepalive_interval);
		}
	}
	if(new_config.timeout != this.config.timeout){
		logger.info("reload new timeout:", new_config.timeout);
		//do nothing , just copy new_config to config
	}
	self.config = new_config;
	return 
}

Connector.prototype.sendRequestByStream = function(header, body, response_cb){
	var err = {
		errNum:constants.ERROR_NUM.SUCCESS,
		errMsg:constants.ERROR_MSG.SUCCESS
	}
    //replace seq
    var self = this;
    header[constants.HEADER_OFFSET.SEQ] = this.seq++;
    if(this.request_cmd_list.indexOf(header[constants.HEADER_OFFSET.CMD]) == -1)
    {
        this.request_cmd_list.push(header[constants.HEADER_OFFSET.CMD]);
    }
    //encode msg
    //notice send buffer must be applied once again
    var send_buf = new Buffer(constants.SEND_BUFFER_LEN);
    var ret = encodeMessage(send_buf, header, body);
    if(!ret.is_good){
    	utils.setResult(err,constants.ERROR_NUM.SERVER_INTERNAL_ENCODE_ERROR, constants.ERROR_MSG.SERVER_INTERNAL_ENCODE_ERROR);
    	return 	response_cb(err, {});
    }
    //send msg
    if(self.connected){
    	self.send(ret.msg);
        var event_name = makeEventName(header[constants.HEADER_OFFSET.SEQ]);
        //set timeout
        var eventid = setTimeout(function(){
        	//once timeout , remove onseq event
        	self.emitter.removeAllListeners(event_name);
        	clearInterval(eventid);
			utils.setResult(err, constants.ERROR_NUM.BACKEND_TIMEOUT, constants.ERROR_MSG.BACKEND_TIMEOUT);
        	return 	response_cb(err, {});
		}, self.config.timeout)
		//regist callback	
        self.emitter.once(event_name, function(packet){
        	self.emitter.removeAllListeners(event_name);
        	clearTimeout(eventid);
        	response_cb(err, packet);
		});
    }else{
    	utils.setResult(err,constants.ERROR_NUM.SERVER_INTERNAL_CONNECTION_ERROR, constants.ERROR_MSG.SERVER_INTERNAL_CONNECTION_ERROR);
    	return 	response_cb(err, {});
    }
}

Connector.prototype.sendRequest = function(header, body, response_cb){
	var err = {
		errNum:constants.ERROR_NUM.SUCCESS,
		errMsg:constants.ERROR_MSG.SUCCESS
	}
    //replace seq
    var self = this;
    header[constants.HEADER_OFFSET.SEQ] = this.seq++;
    if(this.request_cmd_list.indexOf(header[constants.HEADER_OFFSET.CMD]) == -1)
    {
        this.request_cmd_list.push(header[constants.HEADER_OFFSET.CMD]);
    }
    
    //encode msg
    //notice send buffer must be applied once again
    var send_buf = new Buffer(constants.SEND_BUFFER_LEN);
    var ret = encodeMessage(send_buf, header, body);
    if(!ret.is_good){
    	utils.setResult(err,constants.ERROR_NUM.SERVER_INTERNAL_ENCODE_ERROR, constants.ERROR_MSG.SERVER_INTERNAL_ENCODE_ERROR);
    	return 	response_cb(err, {});
    }
    //send msg
    if(self.connected){
        logger.debug("send data: "+bufferToHex(ret.msg));
    	self.send(ret.msg);
        var event_name = makeEventName(header[constants.HEADER_OFFSET.SEQ]);
        //set timeout
        var eventid = setTimeout(function(){
        	//once timeout , remove onseq event
        	self.emitter.removeAllListeners(event_name);
        	clearInterval(eventid);
			utils.setResult(err, constants.ERROR_NUM.BACKEND_TIMEOUT, constants.ERROR_MSG.BACKEND_TIMEOUT);
        	return 	response_cb(err, {});
		}, self.config.timeout)
		//regist callback	
        self.emitter.once(event_name, function(packet){
        	self.emitter.removeAllListeners(event_name);
        	clearTimeout(eventid);
        	response_cb(err, packet);
		});
    }else{
    	utils.setResult(err,constants.ERROR_NUM.SERVER_INTERNAL_CONNECTION_ERROR, constants.ERROR_MSG.SERVER_INTERNAL_CONNECTION_ERROR);
    	return 	response_cb(err, {});
    }
}

Connector.prototype.connectServer = function(config, recv_data_cb, beat_request_func){
	var self = this;
	this.config = config;
	this.client = net.connect({host: config.address.ip, port:config.address.port});
	this.recv_msg_cb = recv_data_cb;
	this.client.on('connect', function(){
			logger.info(self.config.serverName ," connect ok ip:",self.client.remoteAddress," port:",self.client.remotePort);
			self.binarySpliter.reset();
            if (null != beat_request_func) {
                self.beat_request_func = beat_request_func;
            }
			self.sendkeepalive();
			self.setConnectedFlag();
			});

	this.client.on('data', function(data){
			logger.info(self.config.serverName ," recv raw data:", bufferToHex(data));
			var packets = self.binarySpliter.getPackets(data);	
				if(!self.binarySpliter.is_good){
					logger.error(self.config.serverName ," binarySpliter spliter packet error data:", bufferToHex(data));
					self.binarySpliter.reset();
					self.disconnect();
					self.setDisconnectedFlag();
					return;
				}else{
					for(var i = 0;i < packets.length;i++){
						//client.recv_msg_cb(packets[i]);
						//decode header
						var header = getHeader(packets[i]);
						if(!header){
							logger.error(self.config.serverName ," get packet header error ");
							return ;
						}
						var recv_cmd = header[constants.HEADER_OFFSET.CMD];
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
	this.client.on('end', function(){
			logger.warn(self.config.serverName ," disconnect ip:",self.getRemoteAddress()," port:",self.getRemotePort());
			self.setDisconnectedFlag();
			});
	this.client.setTimeout(self.config.timeout_interval);
	//check server timeout
	this.client.on('timeout', function(){
			logger.warn(self.config.serverName, " timeout ip:",self.client.remoteAddress," port:",self.client.remotePort);
			self.disconnect();
			self.setDisconnectedFlag();
			});
	this.client.on('error', function(err){
		logger.warn("socket error:",err);
		self.setConnectedFlag();
	});
	this.intervalId = setInterval(function(){
		if(self.client.destroyed){
			logger.warn("interval check connect destroyed remoteAddress:",self.config.address.ip," port:", self.config.address.port);
			self.setDisconnectedFlag();
			self.client.connect(self.config.address.port, self.config.address.ip);
		}else{
			self.sendkeepalive();
		}
	},self.config.keepalive_interval);
	return this;
}	

module.exports = Connector;
