/**
 * @filedesc: 
 * cmd.cpp, handle command
 * @author: 
 *  bbwang
 * @date: 
 *  2015/11/13 20:02:59
 * @modify:
 *
**/

#include "cmd.h"
#include "comm.h"
#include "server_app.h"
#include "mysqloperator.h"
#include "mysql_msg.h"
#include "binpacket_wrap.h"
#include "errcodes.h"

using namespace utils;
using namespace common;

int BeatCmd:: execute(void *)
{
	uint32_t id = 0;
	inpkg_ >> id;
	if( !inpkg_.good())
	{
		LOG(WARN)("alive pkg error.");
		return -1;
	}

    COHEADER coheader;
    inpkg_.get_head(coheader);
    coheader.ReverseCoid();

    BinOutputPacket<> outpkg(inpkg_.getData(), inpkg_.size());
    outpkg.offset_head(sizeof(COHEADER));
    outpkg.set_head(coheader);

	session_->send_msg(outpkg.getData(), outpkg.size());
	LOG(INFO)("client session id:%u is allive, send back alive pkg.", id);
	LOG_HEX(inpkg_.getData(), inpkg_.size(), utils::L_DEBUG);

	return 0;
}




int MysqlOperateCmd::execute(void *)
{
    int ret = true;

    BinOutputPacket<> outpkg(ServerApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    MysqlDatabaseImpl *data_base_impl = ServerApp::Instance()->db_impl();
    QueryRequest request(inpkg_);
    QueryResult result(outpkg);
    result.header_ = request.header_;

    ONCE_LOOP_ENTER
    ret = request.decode();
    if (0 != ret) {
        LOG(ERROR)("mysql operate, decode request failed.");
//        result.ret_code = ret;
//        result.ret_msg = "decode request failed.";
        break;
    }

    result.header_ = request.header_;
    ret = data_base_impl->Process(request, result);
    if ((ret != 0) && (result.get_ret() == ERR_MYSQL_GOAWAY))
    {
        LOG(INFO)("mysql have gone away, try again.");
        ret = data_base_impl->Process(request, result);
    } else if (0 != ret) {
        LOG(ERROR)("mysql operate, process request failed.");
        break;
    }
    ONCE_LOOP_LEAVE

    result.encode();
    LOG(INFO)("done operate mysql, response client. data len:%u", result.length());
    LOG_HEX(result.getData(), result.length(), utils::L_DEBUG);
	session_->send_msg(result.getData(), result.length());

    return ret;
}

