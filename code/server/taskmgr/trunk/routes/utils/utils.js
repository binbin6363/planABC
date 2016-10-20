var constants = require('./constants');
//var proto = require('./proto');
//

var RECV_BUFFER_LEN = 4 * 1024 * 1024;

function BinaryDecoder(buffer) { 
	if(!buffer instanceof Buffer){
		this.is_good = false;
		return;
	}
	this.buf = buffer;
	this.offset  = 0;
	this.is_good = true;
}


/*
* //@param	arr = [{"type":string},{"Name":"cond_ip"}, {"type":uint32},{"Name":"status"}}]
* @param
  @return       false; //fail
		res = [{"cond_ip":"123"}, {"status":4}]  ;//sucess
*/
/*
BinaryDecoder.prototype.readStreamUint32Array(arr){
	//for()
}
*/
BinaryDecoder.prototype.readStreamUint32 = function(){
	if(!this.is_good || this.buf.length - this.offset < 4){
		this.is_good = false;
		return false;
	}
	var value = this.buf.readUInt32BE(this.offset);
	this.offset += 4;
	return value;
}
/*
* @param null
* @return string;
*/
BinaryDecoder.prototype.readStreamString = function(){
	if(!this.is_good || this.buf.length - this.offset < 4){
		this.is_good = false;
		return false;
	}
	var len = this.buf.readUInt32BE(this.offset);
	this.offset += 4;
	if(this.buf.length - this.offset < len || len < 2){
		this.is_good = false;
		return false;
	}
	var value = this.buf.slice(this.offset, this.offset + len - 2);
	this.offset += len ;
	return value.toString();
}

BinaryDecoder.prototype.readStreamBuffer = function(){
	if(!this.is_good || this.buf.length - this.offset < 4){
		this.is_good = false;
		return false;
	}
	var len = this.buf.readUInt32BE(this.offset);
	this.offset += 4;
	if(this.buf.length - this.offset < len || len < 2){
		this.is_good = false;
		return false;
	}
	var value = this.buf.slice(this.offset, this.offset + len - 2);
	this.offset += len ;
	return value;
}

BinaryDecoder.prototype.getLeft = function(){
	if(!this.is_good){
		this.is_good = false;
		return false;
	}
	return this.buf.slice(this.offset);
}

BinaryDecoder.prototype.set_offset = function(value){
	if(!this.is_good || typeof value != "number"){
		this.is_good = false;
		return false;		
	}
	if(this.buf.length - this.offset < value){
		return false;
	}
	this.offset += value;
	return true;

}

function BinaryEncoder(buf) { 
	this.buf = buf;
	this.offset = 0;
	this.is_good = true;
}



/*
*@param value number 
* @return ture or false 
*/
BinaryEncoder.prototype.offset = function(value){
	if(!this.is_good || typeof value != "number"){
		this.is_good = false;
		return false;		
	}
	if(this.buf.length - this.offset < value){
		return false;
	}
	this.offset += value;
	return true;
}
BinaryEncoder.prototype.get_offset = function(){
	if(!this.is_good){
		this.is_good = false;
		return false;
	}
	return this.offset;
}
/*BinaryEncoder.prototype.is_good = function(){
	return this.is_good
}
*/
BinaryEncoder.prototype.writeStreamUint32Offset = function(value, pos){
	if(!this.is_good ||  typeof value != "number" ){
		this.is_good = false;
		return false;
	}
	if(this.buf.length  < pos + 4){
		this.is_good = false;
		return false;
	}
	this.buf.writeUInt32BE(value, pos);	
	return true;	
}

/*
*@param value number 
* @return ture or false 
*/
BinaryEncoder.prototype.writeStreamUint8 = function(value){
    if(!this.is_good || typeof value != "number"){
        this.is_good = false;
        return false;       
    }
    if(this.buf.length < this.offset + 4){
        this.is_good = false;
        return false;
    }   
    this.buf.writeUInt8(value, this.offset); 
    this.offset += 1;
    return true;
}

/*
*@param value number 
* @return ture or false 
*/
BinaryEncoder.prototype.writeStreamUint32 = function(value){
	if(!this.is_good || typeof value != "number"){
		this.is_good = false;
		return false;		
	}
	if(this.buf.length < this.offset + 4){
		this.is_good = false;
		return false;
	}	
	this.buf.writeUInt32BE(value, this.offset);	
	this.offset += 4;
	return true;
}

/*
* @param value [number1,number2,...]
* @return ture or false 
*/
BinaryEncoder.prototype.writeStreamUint32Array = function(values){
	if((!this.is_good) || !(values instanceof Array)){
		this.is_good = false;
		return false;		
	}
	var i = 0;
	for(i = 0; i < values.length; i++){
		if(typeof values[i] != "number"	){
			this.is_good = false;
			return false;
		}
		if(!this.writeStreamUint32(values[i])){
			this.is_good = false;
			return false;
		}		
	}
	return true;
}

/*
*@param value string
* @return ture or false 
*/
BinaryEncoder.prototype.writeStreamString = function(value){
	if(!this.is_good || typeof value != "string"){
		this.is_good = false;
		return false;		
	}
    var tmp_buf = new Buffer(value);
	if(this.buf.length < 4 + tmp_buf.length + 2 + this.offset){		
		this.is_good = false;
		return false;
	}
	this.buf.writeUInt32BE(tmp_buf.length + 2, this.offset);
	this.offset+= 4;
	this.buf.write(value + '\0\0', this.offset);
	this.offset += tmp_buf.length + 2;
	return true;
}

/*
*@param value buffer
* @return ture or false 
*/
BinaryEncoder.prototype.writeStreamBuffer = function(value){
	if(!this.is_good || !(value instanceof Buffer)){
		this.is_good = false;
		return false;		
	}
	if(this.buf.length < 4 + value.length + 2 + this.offset){		
		this.is_good = false;
		return false;
	}
	this.buf.writeUInt32BE(value.length + 2, this.offset);
	this.offset+= 4;
	value.copy(this.buf, this.offset, 0 , value.length);	
	this.offset += value.length;
	this.buf.write('\0\0', this.offset);
	this.offset += 2;
	return true;
}

/*
*@param value buffer
* @return ture or false 
*/
BinaryEncoder.prototype.writeStreamRawBuffer = function(value){
	if(!this.is_good || !(value instanceof Buffer)){
		this.is_good = false;
		return false;		
	}
	if(this.buf.length < value.length + this.offset){		
		this.is_good = false;
		return false;
	}
	value.copy(this.buf, this.offset, 0 , value.length);	
	this.offset += value.length;
	return true;
}

exports.bufferToHex = function(buffer){
    if(typeof buffer == 'string'){
        var i = 0;
    	var str = '[' + buffer.length + ']' + '< ';
        var change_row_count = 0;
        for(i = 0; i<buffer.length ;i++){
            var val = buffer.charCodeAt(i);
            if(val < 16){
                    str+= '0' + val.toString(16);
            }else{
                    str+=  val.toString(16);
            }
            str+=" ";
        }
        str += '>';
    }
    if( buffer instanceof Buffer){
        var i = 0;
		var str = '[' + buffer.length + ']' + '< ';
        var change_row_count = 0;
        for(i = 0; i<buffer.length ;i++){
            var val = buffer[i];
            if(val < 16){
                    str+= '0' + val.toString(16);
            }else{
                    str+=  val.toString(16);
            }
            str+=" ";
        }
        str += '>';
    }
    return str;
}
/*
* binary spliter, 
*/
function BinarySpliter() { 
	this.already_recv_buff = new Buffer('');
	this.is_good = true;
	this.packets = [];
}
BinarySpliter.prototype.reset = function()
{
	this.already_recv_buff = new Buffer('');
	this.is_good = true;
	this.packets = [];
}
BinarySpliter.prototype.getPackets = function(data){
	this.packets = [];
	if(!Buffer.isBuffer(data)){
		this.is_good = false;
		return [];
	}
	var buffer = Buffer.concat([this.already_recv_buff, data]);
	this.already_recv_buff = new Buffer('');
	var left_len = buffer.length;
	var pos = 0;
	var binaryDecode = new BinaryDecoder(buffer);
	while(left_len > 0){
		if(left_len < 4){
			this.already_recv_buff = new Buffer(buffer.slice(pos, pos + left_len));
			break;
		}
		var len = binaryDecode.readStreamUint32();
		if(!binaryDecode.is_good || len == 0 || len > constants.RECV_BUFFER_LEN){
			this.is_good = false;
			return [];
		}
		if(left_len < len){
			this.already_recv_buff = new Buffer(buffer.slice(pos, pos + left_len));
			break;
		}else{
			this.packets.push(buffer.slice(pos, pos + len));
			left_len -= len;
			pos += len;
			binaryDecode.set_offset(len - 4);
			if(!binaryDecode.is_good){
				this.is_good = false;
				return [];
			}
		}
		//assert (left_len >=0)
		/*
		if(left_len - len >= 0){
			this.packets.push(buffer.slice(pos, pos + len));
			left_len -= len;
			pos += len;
			binaryDecode.set_offset(len - 4);
			if(!binaryDecode.is_good){
				this.is_good = false;
				return [];
			}
		}
		*/
		/*
		if(left_len <= 0 || left_len < len){
			this.already_recv_buff = new Buffer(buffer.slice(pos, pos + left_len));
			break;
		}
		*/
	}	
	return this.packets;	
}

/*
* encodeMesaage
* @param {Buffer} send_buf, 
* @param {Array} header,
* @parma {Array, Buffer} body,or
*/

exports.encodeMessage = function(send_buf, header, body){
		var ret = {
			is_good : true
		}
		var binaryEncode = new BinaryEncoder(send_buf);
        binaryEncode.writeStreamUint32Array(header);			
		if(!binaryEncode.is_good){
			ret.is_good = false;
			return ret;
		}

        if (body instanceof Array) {
            for(var i = 0; i < body.length ; i++){
                if(typeof body[i] == "number"){
                    binaryEncode.writeStreamUint32(body[i]);
                }else if(typeof body[i] == "string"){
                    binaryEncode.writeStreamString(body[i]);
                }else if(body[i] instanceof Buffer){
                    binaryEncode.writeStreamBuffer(body[i]);
                }else if(typeof body[i] == "object"){
                    binaryEncode.writeStreamString(JSON.stringify(body[i]));
                }else {
                    ret.is_good = false;
                    return ret;
                }
            }
        } else if (body instanceof Buffer) {
            binaryEncode.writeStreamRawBuffer(body);
        } else {
            ret.is_good = false;
            return ret;
        }
        
		if (!binaryEncode.is_good)
		{
			ret.is_good = false;
			return ret;
		}

		binaryEncode.writeStreamUint32Offset(binaryEncode.get_offset(), 0);
		ret.msg = send_buf.slice(0, binaryEncode.get_offset());
		return  ret;
}
/*
* encodeMesaage
* @param {Array} header,
* @parma {Array} body
*/

exports.encodeMessageNoInitBuf = function(header, body){
		var ret = {
			is_good : true
		}
		var buf_len = 0;
		buf_len += (header.length) * 4;
		for(var i = 0; i < body.length ; i++){
			if(typeof body[i] == "number"){
				buf_len += 4;
			}else if(typeof body[i] == "string"){
				buf_len += Buffer(body[i]).length + 4 + 2;
			}else if(body[i] instanceof Buffer){
				buf_len += body[i].length + 4 + 2;
			}else if(typeof body[i] == "object"){
				buf_len += Buffer(JSON.stringify(body[i])).length + 4 + 2;
			}else {
				ret.is_good = false;
				return ret;
			}
		}
		var send_buf = new Buffer(buf_len);
		var binaryEncode = new BinaryEncoder(send_buf);

        binaryEncode.writeStreamUint32Array(header);			
		if(!binaryEncode.is_good){
			ret.is_good = false;
			return ret;
		}
		for(var i = 0; i < body.length ; i++){
			if(typeof body[i] == "number"){
				binaryEncode.writeStreamUint32(body[i]);
			}else if(typeof body[i] == "string"){
				binaryEncode.writeStreamString(body[i]);
			}else if(body[i] instanceof Buffer){
				binaryEncode.writeStreamBuffer(body[i]);
			}else if (typeof body[i] == "object"){
				binaryEncode.writeStreamString(JSON.stringify(body[i]));
			}else {
				ret.is_good = false;
				return ret;
			}
		}
		binaryEncode.writeStreamUint32Offset(binaryEncode.get_offset(), 0);
		ret.msg = send_buf.slice(0, binaryEncode.get_offset());
		return  ret;
}
/*
* get header packet from Buffer packet
* @param {Buffer} packet
* @return fail return false, success return [len, cmd, seq] 
*/
exports.getHeader = function(packet){
	if(!Buffer.isBuffer(packet)){
		return false;
	}
	var binaryDecode = new BinaryDecoder(packet);
	var len = binaryDecode.readStreamUint32();
	var cmd = binaryDecode.readStreamUint32();
	var seq = binaryDecode.readStreamUint32();
	if(!binaryDecode.is_good){
		return false;
	}
	return [len, cmd, seq];
}

function setResult(obj,num,msg){
    obj.errNum = num;
    obj.errMsg = msg;
}
exports.setResult = setResult;

exports.StringToNumber = function (data){
	return parseInt(data,'10');
}

exports.getCookie = function(cookie, c_name){
	if (cookie.length>0)
	{
		c_start=cookie.indexOf(c_name + "=")
			if (c_start!=-1)
			{ 
				c_start=c_start + c_name.length+1 
					c_end=cookie.indexOf(";",c_start)
					if (c_end==-1) c_end=cookie.length
						return cookie.substring(c_start,c_end)
			} 
	}
	return ""
}
exports.checkToken = function(token,uid){
	return true;
	/*
	var key = "pfur894kfuts292d";
	var buf = new Buffer(token,'base64');
	var t = xtea.decrypt(key,buf);
	var txt = t.split('_');
	if(txt[0] == uid){
		return true;
	}else{
		return true;
	}
	*/
}
/*
* @param {string } bId 业务id  (00~ff),必须两位
* @param {string } pId 机器id  (00~ff),必须两位
	2 + 8 + 4 + 2
* hex(业务名) + hex(time) + hex(incr) + hex(pc_id);
*/
function IdGenerator(bId, pId){
	this.begin = 4378;
	this.incr = this.begin;  //起始 111A, 终止 FDE8，并且生成的位数都相同
	this.max_num = 65000;    //默认一秒钟不可能产生100000个id

	this.is_good = true;
	if(bId.length != 2 || pId.length != 2){
		this.is_good = false;
		return false;
	}
	this.bId = bId;
	this.pId = pId;
}

IdGenerator.prototype.createUniqId = function(){
	var now = Math.floor(Date.now()/1000);
    var hex_now = now.toString("16");
	var hex_incr =  this.incr.toString("16");
	var id = this.bId + hex_now + hex_incr + this.pId;
	this.incr++;
	if(this.incr >= this.max_num){
		this.incr = this.begin;
	}
	return id;
}
/*
* @param {array} dst array
* @return {array} uniq array
*/
exports.uniqArray = function(arr){
	if(!(arr instanceof Array)){
		return [];
	}
    var res = [];
    var obj = {};
    for(var i = 0; i < arr.length; i++){
        if(!obj[arr[i]]){
            res.push(arr[i]);
            obj[arr[i]] = true;
        }
    }
    return res;
}
exports.IdGenerator = IdGenerator;
exports.BinarySpliter = BinarySpliter;
exports.BinaryDecoder = BinaryDecoder;
exports.BinaryEncoder = BinaryEncoder;


/* Parse the given cookie header string into an object
* The object has the various cookies as keys(names) => values
* @param {String} str
* @return {Object}
* for exmaple:
  var str = 'uid=1001000381; sid=s%3AX8LCA8hK5l3or4DIorYcdD4v.AYQjdEe5hA4fqq1Wbhp9W9vOx4pySlnF3d3D9rL9VsQ; mp_e0d974281500090bb0d68
4cc2df6a6ae_mixpanel=%7B%22distinct_id%22%3A%20%22146adcc5ee41-042b12d12-69161677-100200-146adcc5ee855%22%2C%22%24initial_referrer%2
2%3A%20%22%24direct%22%2C%22%24initial_referring_domain%22%3A%20%22%24direct%22%7D';

  var res = parseCookie(str, {decode: function(value) { return value; });

  //output:
  res:
  {
   	uid: '1001000381',
	sid: 's%3AX8LCA8hK5l3or4DIorYcdD4v.AYQjdEe5hA4fqq1Wbhp9W9vOx4pySlnF3d3D9rL9VsQ',
	mp_e0d974281500090bb0d684cc2df6a6ae_mixpanel: '%7B%22distinct_id%22%3A%20%22146adcc5ee41-042b12d12-69161677-100200-146adcc5ee855%22%2C%22%24initial_referrer%22%3A%20%22%24direct%22%2C%22%24initial_referring_domain%22%3A%20%22%24direct%22%7D'
   }
*/
exports.parseCookie = function(str, opt) {
    opt = opt || {};
    var obj = {}
    var pairs = str.split(/[;,] */);
    var dec = opt.decode || decode;

    pairs.forEach(function(pair) {
        var eq_idx = pair.indexOf('=')

        // skip things that don't look like key=value
        if (eq_idx < 0) {
            return;
        }

        var key = pair.substr(0, eq_idx).trim()
        var val = pair.substr(++eq_idx, pair.length).trim();

        // quoted values
        if ('"' == val[0]) {
            val = val.slice(1, -1);
        }

        // only assign once
        if (undefined == obj[key]) {
            try {
                obj[key] = dec(val);
            } catch (e) {
                obj[key] = val;
            }
        }
    });

    return obj;
};
