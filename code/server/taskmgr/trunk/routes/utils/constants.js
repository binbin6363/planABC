module.exports = {
    ERROR_NUM : {
        SUCCESS : "0",
        INVALID_PARAM : "1",  
        BACKEND_TIMEOUT : "2",
        DB_ERROR : "3",
        SERVER_INTERNAL_INVALID_PARAM : "4", 
        SERVER_INTERNAL_ENCODE_ERROR : "5",
        SERVER_INTERNAL_CONNECTION_ERROR : "6"
    },      
    ERROR_MSG : {
        SUCCESS : "success",
        INVALID_PARAM : "param invalid",
        BACKEND_TIMEOUT : "backend timeout",
        DB_ERROR : "db error",
        SERVER_INTERNAL_INVALID_PARAM : "internal invalid param",
        SERVER_INTERNAL_ENCODE_ERROR : "internal encode param",
        SERVER_INTERNAL_CONNECTION_ERROR : "connection error"
    },
    SERVICE_NAME : {
        MESSAGE : "Message",
        STATUS  : "Status",
        SYSTEM  : "System"
    },
    SERVER_PROTO_TYPE :{
        WEBSOCKET : "websocket",
        BINARY : "binary",
        HTTP : "http"
    },
    HEADER_OFFSET :{
        CMD : 1,
        SEQ : 2,
    },
    JS_DATA_TYPES : {
        NUMBER : 'number',
        STRING : 'string',
        OBJECT : 'object',
        ARRAY  :  'Array'
    },
    BUSSINESS_TYPE : {
        AUDIO : "audio",
        CHATPIC   : "chatpic",
        QGROUPPIC : "qgrouppic",
        NGROUPPIC : "ngrouppic",
        CHATFILE : "chatfile",
        QGROUPFILE : "qgroupfile",
        NGROUPFILE : "ngroupfile",

    },
    CLIENT_TYPE : {
        PC : 0 ,
        WEB : 1 ,
        IOS : 2 ,
        ANDROID : 3
    },
    PROTO_HEADER_LEN : 7,
    PROTO_HEADER_BINARY_LEN : 7*4,
    SEND_BUFFER_LEN : 4*1024*1024,
    RECV_BUFFER_LEN : 4*1024*1024,
    MAX_NOTICE_MSG_GET_NUM : 100,
    FILE_BUSSINESS : {
        SUCCESS : 0,
        FILE_ALREADY_EXISTS : 1,
        WRONG_RANGE_START   : 2,
        OPEN_FILE_FAIL   : 3,
        APPEND_FILE_FAIL   : 4,
        BUSSINESS_TYPE_NOT_EXISTS   : 5,
        FILE_NOT_EXIST : 6,
        RANGE_START_TOO_LONG : 7,
    }
}
