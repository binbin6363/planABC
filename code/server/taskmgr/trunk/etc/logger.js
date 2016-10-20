var log4js = require('../node_modules/log4js');
log4js.configure({
    appenders: [
        {
            type: "dateFile",
            filename: "/home/log/taskmgr.log",
            //maxLogSize: 500000000,
            pattern: "-yyyyMMdd",      
            alwaysIncludePattern: true,
            category: [ 'taskmgr', 'net','uploadserver' ]
        },
        {
            type: "console"
        }
    ],
});

log4js.loadAppender('file');
var logger = log4js.getLogger('taskmgr');
logger.info("init config");

exports.log4js = log4js;
exports.logger = logger;
