/**
 * @filedesc: 
 * client_session.cpp, handle client action
 * @author: 
 *  bbwang
 * @date: 
 *  2014/8/3 12:02:59
 * @modify:
 *
**/

#include "comm.h"
#include "client_session.h"
#include "leveldb_server_app.h"
#include "leveldb_engine.h"
#include "cmd.h"
#include "timecounter.h"
#include "msg.h"

using namespace common;


void ClientSession::log_msg(char *data, int len)
{
    BinInputPacket<> inpkg(data, len);
    inpkg.offset_head(sizeof(COHEADER));
    COHEADER mshdr;
    inpkg.get_head(mshdr);
    LOG(INFO)("ClientSession:[%s] serverid:%s, handle:%u, ip:%s"
    , mshdr.print(), session_id().c_str(), handle(), FromAddrTostring(remote_addr()).c_str());
    LOG_HEX( inpkg.getData(), inpkg.size(), utils::L_DEBUG );
}



ClientSession::ClientSession(void)
    : last_cli_alive_time_(0)
    , last_time_(0)
    , transfer_str_("")
{
}


ClientSession::~ClientSession(void)
{
}

int ClientSession::open( void *arg, const INET_Addr &remote_addr )
{
    LOG(INFO)("ClientSession::open, address:%s, handle:%u.", FromAddrTostring(remote_addr).c_str(), handle());

    last_time_ = time(NULL);
    last_cli_alive_time_ = last_time_;
    Net_Session::open(arg, remote_addr);
	TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(ServerConfig::Instance()->check_timeout_interval()), NULL);
    return 0;
}

int ClientSession::handle_close( uint32_t handle )
{
	LOG(WARN)("ClientSession::handle_close, sessionid:%s, handle:%u, ip:%s."
        , session_id().c_str(), handle, FromAddrTostring(remote_addr()).c_str());
//    LevelDbApp::Instance()->remove_handle_session(this);
//	LevelDbApp::Instance()->remove_session(session_id());
	TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    delete this;
    return 0;
}

/* 
 * inpkg format:
 * (uint32)bizcmd + (string)transfer + (uint32)transid + (uint32_t)kv_num + [(string)key + (string)value + ...]
 * (uint32)bizcmd + (string)transfer + (uint32)transid + (uint32_t)k_num + [(string)key + ...]
 *
 * outpkg format:
 * (uint32)bizcmd + (string)transfer + (uint32)transid + (uint32)ret
 * (uint32)bizcmd + (string)transfer + (uint32)transid + (uint32)ret + (uint32_t)kv_num + [(string)key + (string)value + ...]
 *
 *
 **/
int ClientSession::on_receive_message( char *ptr, int len )
{
    int ret = 0;
    BinInputPacket<> inpkg(ptr, len);
    inpkg.offset_head(sizeof(COHEADER));
    COHEADER mshdr;
    inpkg.get_head(mshdr);

    log_msg(ptr, len);

    if( mshdr.len > inpkg.size() )
    {
        LOG(ERROR)("receive pkg length is error, head.pkglen:%u, pkg len:%u, session_id:%s", mshdr.len, len, session_id().c_str());
        return ret;
    }

	last_time_ = time(NULL);
    if (KEEPALIVE_CMD == mshdr.cmd)
    {
		AliveCommand cmd(this, inpkg);
		return cmd.execute();
    }

    // time counter
    string log_info = mshdr.print();
    log_info += ", total ";
    TimeCounterAssistant tca(log_info.c_str());

    switch (mshdr.cmd)
    {

    // common protocol
    case LEVELDB_GET:// 30000
    {
        BatchGetValueCmd cmd(this, inpkg);
        cmd.execute();
    }
    break;
    case LEVELDB_PUT: // 30001
    {
        BatchSetValueCmd cmd(this, inpkg);
        cmd.execute();
    }
    break;
    case LEVELDB_DEL: // 30002
    {
        BatchDelCmd cmd(this, inpkg);
        cmd.execute();
    }
    break;
    case LEVELDB_RANGE_GET: // 30003
    {
        RangeGetValueCmd cmd(this, inpkg);
        cmd.execute();
    }
    break;
    case LEVELDB_RANGE_DEL: // 30004
    {
        RangeDelValueCmd cmd(this, inpkg);
        cmd.execute();
    }
    break;
    case LEVELDB_PUT_BY_SAME_VALUE: // 30005
    {
        BatchSetSameValueCmd cmd(this, inpkg);
        cmd.execute();
    }
    break;
    case LEVELDB_REVERSE_RANGE_GET: // 30006
    {
        ReverseRangeGetValueCmd cmd(this, inpkg);
        cmd.execute();
    }
    break;
    default:
        LOG(ERROR)("can not support procotol, %s", mshdr.print());
    break;
    }

    LevelDbApp::Instance()->add_perfman(tca.getUsecUsed());
    return ret;

}


void ClientSession::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;

    uint32_t cur_time = time(NULL);
    if (cur_time >= ServerConfig::Instance()->client_ttl()+last_time_)
    {
        LOG(WARN)("client session time out, session_id:%s, curtime:%u, last_time:%u, difftime:%d, ttl:%u, ip:%s"
            , session_id().c_str(), cur_time, last_time_, (int)difftime(cur_time, last_time_)
            , ServerConfig::Instance()->client_ttl(), FromAddrTostring(remote_addr()).c_str());
        this->close();
    }
    else
    {
        TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(ServerConfig::Instance()->check_timeout_interval()), NULL);
    }

}

int ClientSession::send_msg_to_client(char *data, uint32_t length)
{
    int ret = this->send_msg(data, length);
    return ret;
}


int ClientSession::send_msg_to_client(Msg &msg)
{
    return this->send_msg(msg.getData(), msg.length());
}
/*
void ClientSession::destory_connect()
{
    LOG(INFO)("destory connect. cid:%u, uid:%u, login seq:%u, handle:%u."
        , m_unCid, m_unUid, login_seq_, handle());
    // close handle immediately
    this->close();
}
*/


