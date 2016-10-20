/**
 * @filedesc: 
 * 
 * @author: 
 *  bbwang
 * @date: 
 *  2015/11/3 12:02:59
 * @modify:
 *
**/

#include "dbp_session.h"
#include "server_app.h"
#include "cmd.h"
#include "server_config.h"
#include "protocols.h"


//using namespace CPPSocket;
using namespace common;

DbpSession::DbpSession(void)
    : last_time_(0)
    , b_connected_(false)
    , msg_center_id_(0)
{
    LOG(DEBUG)("NEW DbpSession");
}

DbpSession::~DbpSession(void)
{
    LOG(DEBUG)("DELETE DbpSession");
}

int DbpSession::open(void *arg, const INET_Addr &remote_addr)
{
    LOG(INFO)("DbpSession::open, ip:%s, handle:%u.", FromAddrTostring(remote_addr).c_str(), handle());

    Net_Session::open(arg, remote_addr);
    b_connected_ = true;
    last_time_ = time(NULL);
    keepalive();//连接成功则直接发送心跳
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.heart_beat_interval), NULL);
    return 0;
}



int DbpSession::on_receive_message(char *ptr, int len)
{
    int ret = 0;
    // offset pkglen word
    BinInputPacket<> inpkg(ptr, len);
    inpkg.offset_head(sizeof(COHEADER));
    COHEADER header;
    inpkg.get_head(header);
    LOG(INFO)("DbpSession receive msg. [%s]", header.print());

    last_time_ = time(NULL);
    switch (header.cmd)
    {
        case CMD_KEEPALIVE:
        {
            LOG(INFO)("[beat] dbp center server is alive.");
        }
        break;
        case CMD_LOGIN:
        {
            BackLoginCmd cmd(this, inpkg);
            cmd.execute();
        }
        break;
        case CMD_INNER_GET_INCOME: // 
        {
            BackGetIncomeCmd cmd(this, inpkg);
            cmd.execute();
        }
        break;
        case CMD_SYNC_DATA: // 
        {
            BackGetTaskCmd cmd(this, inpkg);
            cmd.execute();
        }
        break;
        case CMD_THIRD_PARTY_LOGIN: // 6000
        {
            BackThirdPartyLoginCmd cmd(this, inpkg);
            cmd.execute();
        }
        break;

        default:
            LOG(ERROR)("DbpSession, unsupported cmd.");
            break;
    }

    return ret;
}

int DbpSession::handle_close(uint32_t handle)
{
    LOG(ERROR)("DbpSession::handle_close, ip:%s, handle:%u, reconnect interval:%u."
              , FromAddrTostring(remote_addr()).c_str(), handle, serv_info_.reconnect_interval);
    //delete this;
    b_connected_ = false;
    last_time_ = time(NULL);
    //先取消之前的定时器
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
    return 0;
}

void DbpSession::handle_timeout(int id,void *dbpData)
{
    (void)id;
    (void)dbpData;
    time_t cur_time = time(NULL);
    if (!b_connected_ && ((cur_time - last_time_) >= serv_info_.reconnect_interval))
    {
        last_time_ = cur_time;
        LOG(INFO)("reconnect dbp center server, ip:%s:%s", serv_info_.server_ip.c_str(), serv_info_.server_port.c_str());
        //重连dbp center ，重连失败则再次注册定时器

        INET_Addr addr;
        string strAddr = serv_info_.server_ip+":"+serv_info_.server_port;
        FromStringToAddr(strAddr.c_str(), addr);
        DbpSession *dbp_center_session = this;
        if (ServerApp::Instance()->dbp_center_connector().connect(dbp_center_session, addr
                , ServerApp::Instance()->splitter(), serv_info_.conn_time_out, 
                utils::ServerConfig::max_buffer_size[DBP_SERVER_NAME]) != 0)
        {
            LOG(WARN)("reconnect dbp center server falied, ip:%s", FromAddrTostring(remote_addr()).c_str());
            TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
        }

        return ;
    }

    if (b_connected_ && (cur_time - last_time_ >= serv_info_.time_out))
    {
        LOG(WARN)("dbp center session time out, session_id:%s, curtime:%ld, last_time:%ld, difftime:%d"
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

void DbpSession::set_serv_info(const Server_Info &info)
{
    serv_info_ = info;
    set_session_id(serv_info_.server_id);
}

void DbpSession::keepalive()
{
    BinOutputPacket<> outpkg(ServerApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);
    COHEADER header;
    header.cmd = CMD_KEEPALIVE;
    header.seq = ++beat_seq;
    header.head_len = head_len;
    header.uid = 0;
    header.sender_coid = 0;
    header.receiver_coid = 0;
	outpkg << utils::ServerConfig::Instance()->cond_id();
    header.len = outpkg.length();
    outpkg.set_head(header);
    LOG(INFO)("DbpSession:%s send beat. [%s]", session_id().c_str(), header.print());
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);
    send_msg(outpkg.getData(), outpkg.length());
}

// 轮询
int DbpSession::SendToDbp(char *data, int len)
{
    static uint32_t polling = 0;
    vector<DbpSession*> dbp_servers = ServerApp::Instance()->get_dbp_session();
    int session_size = dbp_servers.size();
	if (session_size == 0)
	{
		LOG(ERROR)("send to dbp error. server num is 0");
		return -1;
	}
    for (int loop_index = 0; loop_index < session_size; ++loop_index)
    {
        int index = (++polling) % session_size;
        DbpSession* server = dbp_servers[index];
        if (NULL != server && server->connected())
        {
            LOG(INFO)("send to dbp, server index:%d", index);
            return server->send_msg(data, len);
        } else {
            LOG(WARN)("connection not exist or disconnected, index:%d.", loop_index);
            continue;
        }
    }
	return -1;
}


