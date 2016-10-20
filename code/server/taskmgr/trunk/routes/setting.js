var express = require('express');
var router = express.Router();
var Connectors = require("./client/connectors");
var logger = require('../etc/logger').logger;
var options = require('../etc/options').options;
var constants = require('./utils/constants');
var BinaryDecoder = require('./utils/utils').BinaryDecoder;
var BinaryEncoder = require('./utils/utils').BinaryEncoder;
var bufferToHex = require('./utils/utils').bufferToHex;


/**
 * post data to page. handle
 * 1. 请求转发到任务中心处理
 */
router.post('/', function(req, res, next) {
    // param: 
    // cmd=3003
    // &nick_name=hahawang
    // &passwd=ASDWQ231216876Js
    // &bind_mail=126783432@qq.com
    // &sign=yes%20I%20am
    // &verify_code=167523
    // &transid=12
    // &time=1444998292
    console.log("show /setting post param:");
    console.log(req.query);
    doSetting(req, res);
});

router.get('/', function(req, res, next) {
    // param: 
    // cmd=3003
    // &nick_name=hahawang
    // &passwd=ASDWQ231216876Js
    // &bind_mail=126783432@qq.com
    // &sign=yes%20I%20am
    // &verify_code=167523
    // &transid=12
    // &time=1444998292
    console.log("show /setting get param:");
    console.log(req.query);
    doSetting(req, res);
});


function doSetting (req, res) {

    var responseJsonObj = {};
    var retObj = {};
    retObj.ret = 0;
    retObj.err_msg = "done ok";
    responseJsonObj.res = retObj;
    responseJsonObj.nick_name = req.query.nick_name;
    responseJsonObj.transid = req.query.transid;
    responseJsonObj.time = req.query.time;

    logger.info("done setting request, show result:");
    logger.info(responseJsonObj);
    res.json(responseJsonObj);
    res.end();
}



module.exports = router;


