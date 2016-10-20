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
 */
router.post('/', function(req, res, next) {
    // param: type=mail&value=126783432%CEqq%2Acom
    console.log("show /verify post param:");
    console.log(req.body);
    doVerify(req, res);
});

router.get('/', function(req, res, next) {
    // param: type=mail&value=126783432%CEqq%2Acom
    console.log("show /verify get param:");
    console.log(req.query);
    doVerify(req, res);
});


function doVerify (req, res) {

    logger.warn("done setting request, not implements.");
    res.end();
}



module.exports = router;

