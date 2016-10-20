/**
 * @filedesc: 
 * master_session.cpp, connect master session
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/6 14:55:59
 * @modify:
 */
#include "master_session.h"
#include "protocols.h"
#include "constants.h"
#include "binpacket_wrap.h"
#include "string_util.h"
#include "cmd.h"
#include "log.h"
#include "serverconfig.h"
#include "leveldb_server_app.h"
#include "backend_sync.h"
#include "timecounter.h"
#include "leveldb/cache.h"

using namespace common;
using namespace utils;

/// 斜杠定义
#define BSLASH_CHR					'\\'
#define BSLASH_STR					"\\"
#define SLASH_CHR					'/'
#define SLASH_STR					"/"

/// 路径区分符
#ifndef PATH_CHR
# ifdef WIN32
#  define PATH_CHR					BSLASH_CHR
#  define PATH_STR					BSLASH_STR
#else
#  define PATH_CHR					SLASH_CHR
#  define PATH_STR					SLASH_STR
# endif // WIN32
#endif  // PATH_CHR

static std::string parent_path(const string &path)
{
	std::string cdir;
	if ( path.empty() || path.compare(".") == 0 ) {
        LOG(ERROR)("sorry, can not find parent dir. cur path:%s", path.c_str());
		cdir = "";
    } else {
		cdir = path;
    }
    
	int len = (int)cdir.length() - 1;
	if ( len <= 0 )
		return path;

	for ( int i = len - 1; i >= 0; i-- )
	{
		if ( cdir.at(i) == PATH_CHR )
		{
			cdir.resize(i);
			cdir += PATH_STR;
			break;
		}
	}
	return cdir;
}



MasterSession::MasterSession()
    : last_time_(0)
    , b_connected_(false)
    , sync_flag_(NOT_START)
    , meta_path_("")
    , meta_db_(NULL)
    , meta_open_flag_(false)
    , last_seq_(0)
    , last_key_("")
{
    LOG(DEBUG)("new MasterSession");
    
    meta_options_.create_if_missing = true;
    meta_options_.error_if_exists   = false;
    meta_options_.block_cache = leveldb::NewLRUCache(50 * _1MB);
    meta_options_.block_size = 32 * _1MB;
    meta_options_.write_buffer_size = 64 * _1MB;
    
    string main_db_path = utils::ServerConfig::Instance()->dbpath();
    string parent_dir = parent_path(main_db_path);
    meta_path_ = parent_dir + "db_meta";
}


MasterSession::~MasterSession()
{
    LOG(DEBUG)("delete MasterSession");
    close_meta_db();
}

    
int MasterSession::open(void *arg, const INET_Addr &remote_addr)
{
    LOG(INFO)("MasterSession::open, ip:%s, handle:%u.", FromAddrTostring(remote_addr).c_str(), handle());

    Net_Session::open(arg, remote_addr);
    b_connected_ = true;
    last_time_ = time(NULL);
    keepalive();//连接成功则直接发送心跳

    open_meta_db();

    // load status
    load_status_from_meta_db();

    detect_master_for_sync_data();
    
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.heart_beat_interval), NULL);
    return 0;

}


// 本服务作为客户端去连接master
int MasterSession::on_receive_message(char *ptr, int len)
{
    int ret = 0;
    
    BinInputPacket<> inpkg(ptr, len);
    inpkg.offset_head(sizeof(COHEADER));
    COHEADER ldbhdr;
    inpkg.get_head(ldbhdr);
    LOG(DEBUG)("receive msg from master, %s", ldbhdr.print());
    LOG_HEX(ptr, len, utils::L_DEBUG);

	last_time_ = time(NULL);

    // time counter
    string log_info = "packet from master";
    TimeCounterAssistant tca(log_info.c_str());

    switch (ldbhdr.cmd)
    {
        case KEEPALIVE_CMD: // 0
        {
            LOG(INFO)("master server is alive. peer addr:%s, handle:%u"
                , FromAddrTostring(remote_addr()).c_str(), handle());
        }
        break;
        case LEVELDB_SYNC: // 30101,数据同步，主机推过来的
        {
            RecvSyncDataCmd cmd(this, inpkg);
            cmd.execute();
        }
        break;
        default:
        {
            LOG(ERROR)("unsupported cmd from master.");
        }
        break;
    }
    
    LevelDbApp::Instance()->add_perfman(tca.getUsecUsed());

    return ret;
}

int MasterSession::handle_close(uint32_t handle)
{
    LOG(WARN)("MasterSession::handle_close, ip:%s, handle:%u, reconnect interval:%u."
              , FromAddrTostring(remote_addr()).c_str(), handle, serv_info_.reconnect_interval);
    //delete this;
    b_connected_ = false;
    last_time_ = time(NULL);
    close_meta_db();
    //先取消之前的定时器
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
    return 0;
}

void MasterSession::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;
    time_t cur_time = time(NULL);
    if (!b_connected_ && ((cur_time - last_time_) >= serv_info_.reconnect_interval))
    {
        last_time_ = cur_time;
        LOG(WARN)("reconnect Master server, ip:%s:%s", serv_info_.server_ip.c_str(), serv_info_.server_port.c_str());
        //重连MsgCenter ，重连失败则再次注册定时器

        INET_Addr addr;
        string strAddr = serv_info_.server_ip+":"+serv_info_.server_port;
        FromStringToAddr(strAddr.c_str(), addr);
        MasterSession *master_session = this;
        if (LevelDbApp::Instance()->master_connector().connect(master_session, addr
                , &(LevelDbApp::Instance()->master_splitter()), serv_info_.conn_time_out, utils::ServerConfig::max_buffer_size["master"]) != 0)
        {
            LOG(WARN)("reconnect Master server falied, ip:%s", FromAddrTostring(remote_addr()).c_str());
            TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
        }

        return ;
    }

    if (b_connected_ && (cur_time >= serv_info_.time_out + last_time_))
    {
        LOG(WARN)("Master session time out, session_id:%s, curtime:%ld, last_time:%ld, difftime:%d"
                  , session_id().c_str(), cur_time, last_time_, (int)difftime(cur_time, last_time_));
        this->close();
        return;
    }

    if (b_connected_)
    {
        keepalive();
    }

    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.heart_beat_interval), NULL);

}


void MasterSession::keepalive()
{
    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    COHEADER ldbhdr;
    outpkg.offset_head(sizeof(COHEADER));

    ldbhdr.len = outpkg.length();
    ldbhdr.cmd = KEEPALIVE_CMD;
    ldbhdr.seq = 0;
    ldbhdr.uid= 0;
    ldbhdr.head_len = 16;
    outpkg.set_head(ldbhdr);

    LOG(INFO)("send master alive pkg, peer addr:%s:%s. [%s]"
        , serv_info_.server_ip.c_str(), serv_info_.server_port.c_str(), ldbhdr.print());
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);
    send_msg(outpkg.getData(), outpkg.length());
}


void MasterSession::set_sync_flag(uint32_t flag)
{
    sync_flag_ = flag;
}

uint32_t MasterSession::sync_flag()
{
    return sync_flag_;
}

const std::string &MasterSession::status_key(){
	static std::string key;
	if(key.empty()){
		key = "~slave.status." + utils::ServerConfig::Instance()->server_id();
	}
	return key;
}


int MasterSession::load_status_from_meta_db()
{
	const std::string &key = status_key();
    string kk = key + ".last_key";
    string sk = key + ".last_seq";
    string last_seq_str("");
    leveldb::Status status = meta_db_->Get(leveldb::ReadOptions(), kk, &last_key_);
    if (!status.ok()) {
        LOG(WARN)("load last key from meta db failed. msg:%s", status.ToString().c_str());
        return -1;
    }
    status = meta_db_->Get(leveldb::ReadOptions(), sk, &last_seq_str);
    if (!status.ok()) {
        LOG(WARN)("load last seq from meta db failed. msg:%s", status.ToString().c_str());
        return -1;
    }

    if (!last_seq_str.empty()) {
        last_seq_ = StringUtil::strtou64(last_seq_str);
    }
    
    LOG(INFO)("load status from meta db, last_seq:%lu, last key:'%s'", last_seq_, last_key_.c_str());
    return 0;
}


int MasterSession::save_status_to_meta_db()
{
	const std::string &key = status_key();
    string kk = key + ".last_key";
    string sk = key + ".last_seq";
	std::string last_seq_str = StringUtil::u64tostr(last_seq_);

    leveldb::WriteBatch wb;
    wb.Put(kk, last_key_);
    wb.Put(sk, last_seq_str);
	leveldb::WriteOptions write_opts;
	leveldb::Status s = meta_db_->Write(write_opts, &wb);
	if(s.ok()){
        LOG(DEBUG)("save status ok. last seq:%lu, last key:%s", last_seq_, last_key_.c_str());
	} else {
        LOG(ERROR)("save status falied. last seq:%lu, last key:%s", last_seq_, last_key_.c_str());
        return -1;
    }
    return 0;
}

void MasterSession::set_serv_info(Server_Info server_info)
{
    serv_info_ = server_info;
}

int MasterSession::open_meta_db()
{
    int ret = 0;
    leveldb::Status status = leveldb::DB::Open(meta_options_
        , meta_path_, &meta_db_);
    if(!status.ok()) {
        ret = -1;
        LOG(ERROR)("open meta db file failed. path:%s, msg:%s"
            , meta_path_.c_str(), status.ToString().c_str());
    } else {
        LOG(INFO)("open meta db succeed. path:%s", meta_path_.c_str());
        meta_open_flag_ = true;
    }
    return ret;
}


int MasterSession::close_meta_db()
{
    if (meta_open_flag_) {
        SAFE_DELETE(meta_db_);
        LOG(INFO)("close meta db, path:%s", meta_path_.c_str());
        meta_open_flag_ = false;
    }
    return 0;
}


// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint64_t)last_seq 
// + (string)last_key + (string)signature
int MasterSession::detect_master_for_sync_data()
{
    BinOutputPacket<> outpkg(LevelDbApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    COHEADER ldbhdr;
    outpkg.offset_head(sizeof(COHEADER));
    
    uint32_t bizcmd = 1;
    string signature = utils::ServerConfig::Instance()->server_id();
    string transfer_str = signature + " sync data from master";
    outpkg << bizcmd << transfer_str << last_seq_ << last_key_ << signature;
    ldbhdr.len = outpkg.length();
    ldbhdr.cmd = LEVELDB_DETECT_MASTER;
    ldbhdr.seq = 0;
    ldbhdr.head_len = 16;
    ldbhdr.uid= 0;
    outpkg.set_head(ldbhdr);

    LOG(INFO)("send master sync data pkg, [%s], peer addr:%s:%s.", ldbhdr.print()
        , serv_info_.server_ip.c_str(), serv_info_.server_port.c_str());
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);
    send_msg(outpkg.getData(), outpkg.length());
    return 0;
}


