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
var Connector = require('./connector');
var logger = require('../../etc/logger').logger;


function Connectors(){
	this.connectors = []; //N connector instance
	this.configs = []; //N connector config

}

/*
* @param {array} configs
	[
	  	{       
		  "address" : {"ip":'192.168.16.216',"port":2702},
		  "timeout_interval" : 10000 , // timeout interval
		  "keepalive_interval" : 5000,
		  "serverName" : "webagent",
		  "serverId" : "192.168.16.216:1234",
		  "serverType" : "web",
		  "timeout" : 2000
	  	}
	]	
* @param {function} callback (data)
* @return {bool} true means success, false means fail
*/
Connectors.prototype.init = function(configs, data_cb, beat_func){
		if(!(configs instanceof Array)){
			return false;
		}
		this.configs = configs;

		for(var i = 0;i< configs.length; i++){
			 this.connectors[i] =  this.create_connector(configs[i], data_cb, beat_func);
		}
		return true;
}	
/*
* send a message don't need response
* @param {number} uid
* @message {Buffer} message

*/
Connectors.prototype.send = function(uid, message){ 
	if(!Buffer.isBuffer(message)){
	                return;
	}
	if(this.connectors.length != 0){
	        var connector = this.connectors[uid % this.connectors.length];
	        if(connector.connected){
	         	connector.send(message);
	        }else{
	        	logger.error("send ", connector.config.serverName, " address:", connector.remoteAddress ," port:",connector.remotePort," error connnected is false");
	        }
	}	
}
/*
* need connector modify header's seq , and cache context for callback
* @param {string} key, decide connector distrubition
* @param {Array}  header 
* @param {Array}  body
* @param {function} response_cb(err,result)
* @return callcack
*/
Connectors.prototype.sendRequest = function(key, header, body, response_cb){ 
	var err = {
		errNum:constants.ERROR_NUM.SUCCESS,
		errMsg:constants.ERROR_MSG.SUCCESS
	}
	//check type
	if(typeof key != 'string' ||  !(header instanceof Array)){ //|| !(body instanceof Array) 
		utils.setResult(err,constants.ERROR_NUM.SERVER_INTERNAL_INVALID_PARAM,constants.ERROR_MSG.SERVER_INTERNAL_INVALID_PARAM);
		return response_cb(err, {});
	}

	if(this.connectors.length != 0){
        var connector = this.connectors[parseInt(key,'10') % this.connectors.length];
    	connector.sendRequest(header, body, response_cb);
	}	
}
/*
* create one connector
* @param {object} config
	  	{       
		  "address" : {"ip":'192.168.16.216',"port":2702},
		  "timeout_interval" : 10000 , // timeout interval
		  "keepalive_interval" : 5000, // 
		  "serverName" : "webagent",
		  "serverId" : "192.168.16.216:1234",
		  "serverType" : "web",
		  "timeout" : 2000 
	  	}
* @return {Connector}
*/
Connectors.prototype.create_connector = function(config, data_cb, beat_func){
	//todo, check config integrity
	var connector = new Connector;
	connector.connectServer(config, data_cb, beat_func);
	return connector;

}
function delete_connector(connectors,config){

	for(var i = 0 ; i < connectors.length; i++){
		if(connectors[i].config.address.ip == config.address.ip && 
			connectors[i].config.address.port == config.address.port){
			logger.info("delete connector, session id:"+connectors[i].getUniqId());
			connectors.splice(i,1);
		}
	}
}

function dumpObjAllConnectors(connectors){
	for(var i = 0 ;i < connectors.length; i++){
		logger.info("connector list:",connectors[i].getUniqId());
	}

}

function dumpObjAllKeys(obj){
	logger.info("connector list:",Object.keys(obj));
}
/*
* reload new config
* @param {object}
* @param {function} call_back
*/

Connectors.prototype.reload = function(new_config,func){
	//check new_config integrity
	//check which connector config is change
	var old_config_obj = {};
	var new_add_config = [];
	var del_config = [];
	for(var i = 0 ; i < this.connectors.length; i++){
		var key = this.connectors[i].getUniqId();
		old_config_obj[key] = this.connectors[i];
	}
	dumpObjAllKeys(old_config_obj);
	//console.log("config_obj:",);
	for(var i = 0 ; i < new_config.length ; i++){
		var key = new_config[i].address.ip + ":" + new_config[i].address.port;
		if(!old_config_obj.hasOwnProperty(key)){
			//如果不存在，则添加到new_add_config
			new_add_config.push(new_config[i]);
		}else{
			logger.info("reload self connector:",key);
			old_config_obj[key].reload(new_config[i]);
			delete old_config_obj[key];
		}
	}
	dumpObjAllKeys(old_config_obj);
	//old_config_obj is deleted config
	var keys = Object.keys(old_config_obj);
	for(var i = 0 ; i < keys.length; i++){
		logger.info("reload  destroy one connector:",keys[i]);
		old_config_obj[keys[i]].destroy();//销魂每一个connetor
		delete_connector(this.connectors, old_config_obj[keys[i]].config);
	}
	for(var i = 0 ; i < new_add_config.length; i++){
		logger.info("reload  add one connector:",new_add_config[i].address.ip + ":" + new_add_config[i].address.port);
		var connector = this.create_connector(new_config[i], this.cb);
		this.connectors.push(connector);
	}
	dumpObjAllConnectors(this.connectors);
}
module.exports = Connectors;
