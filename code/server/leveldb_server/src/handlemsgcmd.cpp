/**
 * @filedesc: 
 * handlemsgcmd.cpp, handle msg cmd
 * @author: 
 *  bbwang
 * @date: 
 *  2014/12/17 15:02:59
 * @modify:
 *
**/

#include "cmd.h"
#include "comm.h"
#include "leveldb_server_app.h"
#include "leveldb_engine.h"
#include <sstream>
#include "msg.h"
#include "binlog.h"
#include "master_session.h"
#include "slave_session.h"
#include "string_util.h"
#include "timecounter.h"


/**
*
* msg content key format:
*single chat: Tuid_Fuid_SrvMsgid_CS
*qgroup chat: Qid_Fuid_SrvMsgid_CQ
*ngroup chat: Nid_Fuid_SrvMsgid_CN
*
*/
#define SINGLE_MSG_KEY_FORMAT "%u_%u_%lu_CS"
#define QGROUP_MSG_KEY_FORMAT "%u_%u_%lu_CQ"
#define NGROUP_MSG_KEY_FORMAT "%u_%u_%lu_CN"


// common cmd
bool BatchSetValueCmd::execute()
{
    uint32_t ret = 0;
    RequestPutMsg request(m_inpkg);
    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    ResultPutMsg result(outpkg, request);
    ClientSession *session = dynamic_cast< ClientSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("cast session failed.");
        return false;
    }
    ONCE_LOOP_ENTER
    if (0 != request.decode())
    {
        result.err_code(LEVELDB_PKG_ERROR);
        ret = LEVELDB_PKG_ERROR;
        break;
    }

    if (0 != LeveldbEngine::inst().BatchPut(request.datas()))
    {
        result.err_code(LEVELDB_INNER_ERR);
        ret = LEVELDB_INNER_ERR;
        break;
    }

    ONCE_LOOP_LEAVE
    result.encode();
    LOG(DEBUG)("send msg pkg [%s]", result.header_.print());
    LOG_HEX(result.getData(), result.length(), utils::L_DEBUG);
    session->send_msg_to_client(result);
    return true;
    
}



bool BatchGetValueCmd::execute()
{
    RequestGetMsg request(m_inpkg);
    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    ResultGetMsg result(outpkg, request);
    ClientSession *session = dynamic_cast< ClientSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("cast session failed.");
        return false;
    }
    ONCE_LOOP_ENTER
    if (0 != request.decode())
    {
        result.err_code(LEVELDB_PKG_ERROR);
        break;
    }

    if (0 != LeveldbEngine::inst().BatchGet(request.datas(), result.kvdata()))
    {
        result.err_code(LEVELDB_INNER_ERR);
        break;
    }
    ONCE_LOOP_LEAVE

    // TODO: encode 失败就不回包！2015/2/12
    if (0 != result.encode())
    {
        LOG(ERROR)("BatchGetValueCmd encode error.");
        return false;
    }
    LOG(DEBUG)("send msg pkg [%s]", result.header_.print());
    LOG_HEX(result.getData(), result.length(), utils::L_DEBUG);
    session->send_msg_to_client(result);
    return true;

}


bool RangeGetValueCmd::execute()
{
    RequestRangeGetMsg request(m_inpkg);
    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    ResultRangeGetMsg result(outpkg, request);
    ClientSession *session = dynamic_cast< ClientSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("cast session failed.");
        return false;
    }
    ONCE_LOOP_ENTER
    if (0 != request.decode())
    {
        result.err_code(LEVELDB_PKG_ERROR);
        break;
    }

    if (0 != LeveldbEngine::inst().GetByRange(request.key_start_perfix(), request.key_end_perfix(), result.kvdata(), result.end_flag(), request.get_limit()))
    {
        result.err_code(LEVELDB_INNER_ERR);
        break;
    }
    ONCE_LOOP_LEAVE

    // TODO: encode 失败就不回包！2015/2/12
    if (0 != result.encode())
    {
        LOG(ERROR)("RangeGetValueCmd encode error.");
        return false;
    }
    LOG(DEBUG)("send msg pkg [%s]", result.header_.print());
    LOG_HEX(result.getData(), result.length(), utils::L_DEBUG);
    session->send_msg_to_client(result);
    return true;

}


bool BatchDelCmd::execute()
{
    RequestDelMsg request(m_inpkg);
    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    ResultDelMsg result(outpkg, request);
    ClientSession *session = dynamic_cast< ClientSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("cast session failed.");
        return false;
    }
    ONCE_LOOP_ENTER
    if (0 != request.decode())
    {
        result.err_code(LEVELDB_PKG_ERROR);
        break;
    }

    if (0 != LeveldbEngine::inst().BatchDelete(request.datas()))
    {
        result.err_code(LEVELDB_INNER_ERR);
        break;
    }

    ONCE_LOOP_LEAVE
    result.encode();
    LOG(DEBUG)("send msg pkg [%s]", result.header_.print());
    LOG_HEX(result.getData(), result.length(), utils::L_DEBUG);
    session->send_msg_to_client(result);
    return true;
}


bool GetAndPutValueCmd::execute()
{
    bool ret = true;
    LOG(INFO)("GetAndPutValueCmd execute.");
    RequestGetAndPutMsg request(m_inpkg);
    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    ResultGetAndPutMsg result(outpkg, request);
    ClientSession *session = dynamic_cast< ClientSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("cast session failed.");
        return false;
    }
    ONCE_LOOP_ENTER
    if (0 != request.decode())
    {
        result.err_code(LEVELDB_PKG_ERROR);
        break;
    }

    // 取值
    if (0 != LeveldbEngine::inst().BatchGet(request.key_datas(), result.kvdata()))
    {
        result.err_code(LEVELDB_INNER_ERR);
        break;
    }

    // 成功就覆盖
    if (0 != LeveldbEngine::inst().BatchPut(request.datas()))
    {
        result.err_code(LEVELDB_INNER_ERR);
        ret = LEVELDB_INNER_ERR;
        break;
    }
    
    ONCE_LOOP_LEAVE

    // TODO: encode 失败就不回包！2015/2/12
    if (0 != result.encode())
    {
        LOG(ERROR)("BatchGetValueCmd encode error.");
        return false;
    }
    LOG(DEBUG)("send msg pkg [%s]", result.header_.print());
    LOG_HEX(result.getData(), result.length(), utils::L_DEBUG);
    session->send_msg_to_client(result);
    return ret;
}

// common cmd
bool BatchSetSameValueCmd::execute()
{
    uint32_t ret = 0;
    RequestPutBySameValueMsg request(m_inpkg);
    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    ResultPutBySameValueMsg result(outpkg, request);
    ClientSession *session = dynamic_cast< ClientSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("cast session failed.");
        return false;
    }
    ONCE_LOOP_ENTER
    if (0 != request.decode())
    {
        result.err_code(LEVELDB_PKG_ERROR);
        ret = LEVELDB_PKG_ERROR;
        break;
    }
    if (0 != LeveldbEngine::inst().BatchPut(request.datas()))
    {
        result.err_code(LEVELDB_INNER_ERR);
        ret = LEVELDB_INNER_ERR;
        break;
    }

    ONCE_LOOP_LEAVE
    result.encode();
    LOG(DEBUG)("send msg pkg [%s]", result.header_.print());
    LOG_HEX(result.getData(), result.length(), utils::L_DEBUG);
    session->send_msg_to_client(result);
    return true;
    
}

bool RangeDelValueCmd::execute()
{
    RequestRangeDelMsg request(m_inpkg);

    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    
    ResultRangeDelMsg result(outpkg, request);
    
    ClientSession *session = dynamic_cast< ClientSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("range del cast session failed.");
        return false;
    }
    ONCE_LOOP_ENTER

    if (0 != request.decode())
    {
        result.err_code(LEVELDB_PKG_ERROR);
        break;
    }
    
    int ret = LeveldbEngine::inst().DelByRange(request.key_start_perfix(), request.key_end_perfix());
    if (ret != 0)
    {
        if(ret < 0)
            result.err_code(LEVELDB_INNER_ERR);
        else
            result.err_code((uint32_t)ret);
        break;
    }
    ONCE_LOOP_LEAVE

    // TODO: encode 失败就不回包！2015/2/12
    if (0 != result.encode())
    {
        LOG(ERROR)("range del encode error.");
        return false;
    }
    
    LOG(DEBUG)("send msg pkg [%s]", result.header_.print());
    LOG_HEX(result.getData(), result.length(), utils::L_DEBUG);
    session->send_msg_to_client(result);
    
    return true;
}

bool ReverseRangeGetValueCmd::execute()
{
    RequestRangeGetMsg request(m_inpkg);
    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    ResultRangeGetMsg result(outpkg, request);
    ClientSession *session = dynamic_cast< ClientSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("cast session failed.");
        return false;
    }
    ONCE_LOOP_ENTER
    if (0 != request.decode())
    {
        result.err_code(LEVELDB_PKG_ERROR);
        break;
    }

    if (0 != LeveldbEngine::inst().GetByRangeInReverse(request.key_start_perfix(), request.key_end_perfix(), result.kvdata(), result.end_flag(), request.get_limit()))
    {
        result.err_code(LEVELDB_INNER_ERR);
        break;
    }
    ONCE_LOOP_LEAVE

    // TODO: encode 失败就不回包！2015/2/12
    if (0 != result.encode())
    {
        LOG(ERROR)("ReverseRangeGetValueCmd encode error.");
        return false;
    }
    LOG(DEBUG)("send msg pkg [%s]", result.header_.print());
    LOG_HEX(result.getData(), result.length(), utils::L_DEBUG);
    session->send_msg_to_client(result);
    return true;

}



// 处理服务端推送过来的数据
bool RecvSyncDataCmd::execute()
{
//    string log_info = "RecvSyncDataCmd";
//    TimeCounterAssistant tca(log_info.c_str());
    SyncResponseMsg request(m_inpkg);
    
    MasterSession *session = dynamic_cast< MasterSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("receive sync data cmd cast session failed.");
        return false;
    }
    ONCE_LOOP_ENTER

    if (0 != request.decode())
    {
        LOG(ERROR)("data pkg from master is error.");
        break;
    }

    const Records &records = request.records();
    size_t cnt = records.size();
    // 处理收到的数据
    for (size_t i = 0; i < cnt; ++i)
    {
        HandOneRecord(records[i]);
    }

    LOG(INFO)("sync data from master, data num:%zu, last seq:%lu", cnt, LeveldbEngine::inst().get_lastest_seq());
    ONCE_LOOP_LEAVE
    
    return true;

}

int RecvSyncDataCmd::HandOneRecord(const Record &rcd)
{
    Binlog log;
	if(log.load(rcd.key) == -1){
		LOG(ERROR)("invalid binlog! key:%s, value:%s"
            , DATA2HEX_STR(rcd.key.data(), rcd.key.size())
            , DATA2HEX_STR(rcd.value.data(), rcd.value.size()));
		return -1;
	}
    MasterSession *session = dynamic_cast< MasterSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("receive sync data cmd cast session failed.");
        return -1;
    }

	const char *sync_type = "sync";
	switch(log.type()){
		case BinlogType::COPY:{
            sync_type = "copy";
            session->set_sync_flag(ON_COPY);
			LOG(DEBUG)("[%s] %s", sync_type, log.dumps().c_str());
			this->ProcCopy(log, rcd);
			break;
		}
		case BinlogType::SYNC:
		case BinlogType::MIRROR:{
            session->set_sync_flag(ON_SYNC);
			LOG(DEBUG)("[%s] %s", sync_type, log.dumps().c_str());
			this->ProcSync(log, rcd);
			break;
		}
		default:
			LOG(ERROR)("[%s] unsupported log type. %s", sync_type, log.dumps().c_str());
			break;
	}
	return 0;
}

int RecvSyncDataCmd::ProcCopy(Binlog &log, const Record &rcd)
{
    MasterSession *session = dynamic_cast< MasterSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("receive sync data cmd cast session failed.");
        return -1;
    }

	switch(log.cmd()){
		case BinlogCommand::BEGIN:
        {
			LOG(INFO)("copy begin. backup db...");
            // 改变策略。此处先关闭leveldb引擎，备份已有的db，再打开新的目录
            LeveldbEngine &db_engine = LeveldbEngine::inst();
            db_engine.closeDb();
            string old_db = utils::ServerConfig::Instance()->dbpath();
            uint32_t timestamp = time(NULL);
            string backup_db = old_db + "_bk_" + utils::StringUtil::u32tostr(timestamp);
            int ret = rename(old_db.c_str(), backup_db.c_str());
            if (0 != ret) {
                LOG(ERROR)("backup db failed. errno:%d. open and flush db.", errno);
                db_engine.startDb();
                db_engine.Flushdb();
            } else {
                LOG(INFO)("backup db ok, open db.");
                db_engine.startDb();
            }
			LOG(INFO)("end backup db.");
        }
        break;
		case BinlogCommand::END:
        {
			LOG(INFO)("copy end, last_seq: %lu, seq: %lu", session->last_seq_, log.seq());
            session->set_sync_flag(ON_SYNC);
			session->last_key_ = "";
            // 拷贝完成，将中途记录的key覆盖
			session->save_status_to_meta_db();
        }
		break;
		default:
			return ProcSync(log, rcd);
			break;
	}
	return 0;

}


int RecvSyncDataCmd::ProcSync(Binlog &log, const Record &rcd)
{
    MasterSession *session = dynamic_cast< MasterSession *>(m_session);
    if (NULL == session)
    {
        LOG(ERROR)("receive sync data cmd cast session failed.");
        return -1;
    }
    LeveldbEngine &db_engine = LeveldbEngine::inst();
	switch(log.cmd()){
		case BinlogCommand::KSET:
		{
			LOG(DEBUG)("KSET, set %s", log.key().c_str());//DATA2HEX_STR(rcd.key.data(), rcd.key.size())
			if(db_engine.Put(log.key(), rcd.value) != 0){
				return -1;
			}
		}
		break;
		case BinlogCommand::KDEL:
		{
			LOG(DEBUG)("KDEL, del %s", log.key().c_str());//DATA2HEX_STR(rcd.key.data(), rcd.key.size())
			if(db_engine.Delete(log.key()) != 0){
				return -1;
			}
		}
		break;
		default:
			LOG(ERROR)("unknown binlog, type=%d, cmd=%d", log.type(), log.cmd());
		break;
	}

    // 每次拷贝需要记录收到的key，防止断线从头开始；同理要记录每次的seq
    // 拷贝数据的过程中伴随着sync Binlog
	session->last_seq_ = log.seq();
	if(log.type() == BinlogType::COPY){
		session->last_key_ = log.key();
	}
	session->save_status_to_meta_db();
	return 0;

}


