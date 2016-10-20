
var config = {

host : '139.196.42.222',
port : 6001,

header : {
    len: 0,
    cmd: 100,
    seq: 88,
    head_len: 20,
    uid: 500001
},

cmd : {
	BEAT:100,
	LOGIN:1000,
	SYNC:2000,
	REPORT:3001
},

login_request : {
    request: {
        devid: 'sdfguikltyui',
        dev_type: 1,
        version: 10,
        passwd: 'vvfvfdssf',
		transid: 12,
        time: 1444998292
    }
},

beat_request : {
    request: {
		transid: 22,
        time: 1445054155
    }
},

sync_request : {
    request: {
        count: 3,
        items: [
            {
                k: 1,
                v: 641845799
            },
            {
                k: 2,
                v: 441165795
            },
            {
                k: 3,
                v: 141165795
            }
        ],
		transid: 32,
		time: 1444993118
    }
},


report_request : {
    request: {
        id: 5634378,
        type: 0,
        done_step: 1,
        desc: 'task decsribe',
		transid: 42,
        time: 1444998292
    }
}
}

exports.config = config;