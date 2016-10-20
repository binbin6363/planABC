var net = require('net');
var config = require('./config').config;

var beatinterval;


var now = Date.now();
console.log("now:"+now);

var buf_beat = new Buffer(100);
var buf_sync = new Buffer(1024);
var buf_report = new Buffer(1024);
var buf_login = new Buffer(1024);

var beat = function() {
	var size = 0;
	buf_beat.writeUInt32BE(config.cmd.BEAT, 4);
	buf_beat.writeUInt32BE(config.header.seq, 8);
	buf_beat.writeUInt32BE(config.header.head_len, 12);
	buf_beat.writeUInt32BE(config.header.uid, 16);
	size = buf_beat.write(JSON.stringify(config.beat_request), 20);
	config.header.len = buf_beat.length;
	buf_beat.writeUInt32BE(size+20, 0);
	client.write(buf_beat.slice(0, size+20));
	console.log("beat:", buf_beat.slice(0, size+20).toString("hex"));
}

var sync = function() {
	var size = 0;
	buf_sync.writeUInt32BE(config.cmd.SYNC, 4);
	buf_sync.writeUInt32BE(config.header.seq, 8);
	buf_sync.writeUInt32BE(config.header.head_len, 12);
	buf_sync.writeUInt32BE(config.header.uid, 16);
	size = buf_sync.write(JSON.stringify(config.sync_request), 20);
	config.header.len = buf_sync.length;
	buf_sync.writeUInt32BE(size+20, 0);
	client.write(buf_sync.slice(0, size+20));
	console.log("sync:", buf_beat.slice(0, size+20).toString("hex"));
}

var report = function() {
	var size = 0;
	buf_report.writeUInt32BE(config.cmd.REPORT, 4);
	buf_report.writeUInt32BE(config.header.seq, 8);
	buf_report.writeUInt32BE(config.header.head_len, 12);
	buf_report.writeUInt32BE(config.header.uid, 16);
	size = buf_report.write(JSON.stringify(config.report_request), 20);
	config.header.len = buf_report.length;
	buf_report.writeUInt32BE(size+20, 0);
	client.write(buf_report.slice(0, size+20));
	console.log("report:", buf_beat.slice(0, size+20).toString("hex"));
}

var login = function() {
	var size = 0;
	buf_login.writeUInt32BE(config.cmd.LOGIN, 4);
	buf_login.writeUInt32BE(config.header.seq, 8);
	buf_login.writeUInt32BE(config.header.head_len, 12);
	buf_login.writeUInt32BE(config.header.uid, 16);
	buf_login.write("0123456789abcdef", 20);
	size = buf_login.write(JSON.stringify(config.login_request), 36);
	config.header.len = buf_login.length;
	buf_login.writeUInt32BE(size+36, 0);
	client.write(buf_login.slice(0, size+36));
	console.log("login:", buf_beat.slice(0, size+36).toString("hex"));
}

var connectcb = function (){
    console.log('conneced to addr: ' + config.host + ':' + config.port);
    logintimeout = setTimeout(login, 2000);
    // 建立连接后启动心跳定时器 
    beatinterval = setInterval(beat, 30000);
    syncinterval = setTimeout(sync, 40000);
    reportinterval = setTimeout(report, 40000);
}

var client = new net.Socket();


client.connect(config.port, config.host, connectcb);

// 为客户端添加“data”事件处理函数
// data是服务器发回的数据
client.on('data', function(data) {

    console.log('DATA: ' + data);
    // 完全关闭连接
    //client.destroy();

});

// 为客户端添加“close”事件处理函数
client.on('close', function() {
    console.log('Connection closed');
	process.exit(0);
});

client.on('error', function (e) {
	if (e.code == 'EADDRINUSE') {
		console.log('connection error, retrying connect...');
		setTimeout(function () {
			client.close();
			clearInterval(beatinterval);
			client.connect(config.port, config.host, connectcb);
		}, 1000);
	}
});
