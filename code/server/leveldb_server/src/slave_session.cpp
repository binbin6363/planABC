/**
 * @filedesc: 
 * slave_session.cpp, accept slave connect
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/6 14:55:59
 * @modify:
 */
#include "slave_session.h"
#include "comm.h"
#include "leveldb_server_app.h"
#include "protocols.h"
#include "constants.h"
#include "binpacket_wrap.h"
#include "leveldb_engine.h"
#include "cmd_thread.h"
#include "msg.h"
#include "binlog.h"
#include "timecounter.h"

using namespace common;
using namespace utils;


SlaveSession::SlaveSession()
    : last_time_(0)
    , connected_flag_(CONNECTING)
    , sync_thread_(NULL)
{
    LOG(DEBUG)("new SlaveSession");
}


SlaveSession::~SlaveSession()
{
    LOG(DEBUG)("delete SlaveSession");
    if (sync_thread_) {
        sync_thread_->stop(); // 断线就停掉后端同步
        SAFE_DELETE(sync_thread_);
    }
}

    
int SlaveSession::open(void *arg, const INET_Addr &remote_addr)
{
    LOG(INFO)("SlaveSession::open, address:%s, handle:%u.", FromAddrTostring(remote_addr).c_str(), handle());

    connected_flag_ = CONNECTED;
    last_time_ = time(NULL);
    Net_Session::open(arg, remote_addr);
	TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(ServerConfig::Instance()->check_timeout_interval()), NULL);
    return 0;
}

// slave做为客户端连接本服务
int SlaveSession::on_receive_message(char *ptr, int len)
{
    int ret = 0;
    
    BinInputPacket<> inpkg(ptr, len);
    inpkg.offset_head(sizeof(COHEADER));
    COHEADER ldbhdr;
    inpkg.get_head(ldbhdr);
    LOG(INFO)("receive msg from slave, %s", ldbhdr.print());

	last_time_ = time(NULL);

    // time counter
//    string log_info = ldbhdr.print();
//    log_info += ", total ";
//    TimeCounterAssistant tca(log_info.c_str());

    switch (ldbhdr.cmd)
    {
        case KEEPALIVE_CMD: // 0
        {
            LOG(INFO)("slave is alive, addr:%s, handle:%u. response it.", FromAddrTostring(remote_addr()).c_str(), handle());
            // 心跳要及时回复，不能缓存到同步队列
            send_msg(ptr, len);
        }
        break;
        // 客户端连接上来发送LEVELDB_DETECT_MASTER协议。而后本服务一直给客户端推送数据。
        case LEVELDB_DETECT_MASTER: // 30100
        {
            if (NULL == sync_thread_)
            {
                LOG(INFO)("slave request sync data, enter sync data thread. peer addr:%s, handle:%u."
                    , FromAddrTostring(remote_addr()).c_str(), handle());
                sync_thread_ = new SyncDataCmdThread(this, inpkg);
                sync_thread_->execute();
            }
            else
            {
                LOG(ERROR)("error, already start sync thread!");
            }
        }
        break;
        default:
        {
             LOG(ERROR)("unsupported cmd from slave. cmd:%u", ldbhdr.cmd);
        }
        break;
    }

    return ret;
}


int SlaveSession::handle_close(uint32_t handle)
{
	LOG(WARN)("SlaveSession::handle_close, sessionid:%s, handle:%u, ip:%s."
        , session_id().c_str(), handle, FromAddrTostring(remote_addr()).c_str());
	TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    connected_flag_ = DISCONNECTED;
    if (sync_thread_) {
        sync_thread_->stop(); // 断线就停掉后端同步
        SAFE_DELETE(sync_thread_);
    }
    delete this;
    return 0;
}

void SlaveSession::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;

    time_t cur_time = time(NULL);
    if (difftime(cur_time, last_time_) >= ServerConfig::Instance()->slave_ttl())
    {
        LOG(WARN)("slave session time out, session_id:%s, curtime:%ld, last_time:%ld, difftime:%d, ttl:%u, ip:%s"
            , session_id().c_str(), cur_time, last_time_, (int)difftime(cur_time, last_time_)
            , ServerConfig::Instance()->slave_ttl(), FromAddrTostring(remote_addr()).c_str());
        connected_flag_ = DISCONNECTED;
        this->close();
        if (sync_thread_) {
            sync_thread_->stop(); // 断线就停掉后端同步
            SAFE_DELETE(sync_thread_);
        }
    }
    else
    {
        TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(ServerConfig::Instance()->check_timeout_interval()), NULL);
    }

}


// 这个迭代器是为同步数据准备的，涉及的数据量比较大，所以不能fill_cache
//leveldb::Iterator *SlaveSession::reset_dbiterator(const string &key)
//{
//    SAFE_DELETE(db_iter_);
//    
//	leveldb::ReadOptions iterate_options;
//	iterate_options.fill_cache = false;
//    leveldb::DB *ldb = LeveldbEngine::inst().GetDb();
//	db_iter_ = ldb->NewIterator(iterate_options);
//	db_iter_->Seek(key);
//	if(db_iter_->Valid() && db_iter_->key() == key){
//		db_iter_->Next();
//	}
//    
//    return db_iter_;
//}



int SlaveSession::send_msg_to_client(Msg &msg)
{
    return this->send_msg(msg.getData(), msg.length());
}



