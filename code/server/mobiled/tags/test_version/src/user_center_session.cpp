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

#include "user_center_session.h"
#include "server_app.h"
#include "cmd.h"
#include "server_config.h"


//using namespace CPPSocket;
//using namespace WCCD;

UserCenterSession::UserCenterSession(void)
    : last_time_(0)
    , b_connected_(false)
    , msg_center_id_(0)
{
    LOG(DEBUG)("NEW UserCenterSession");
}

UserCenterSession::~UserCenterSession(void)
{
    LOG(DEBUG)("DELETE UserCenterSession");
}

int UserCenterSession::open(void *arg, const INET_Addr &remote_addr)
{
    LOG(INFO)("UserCenterSession::open, ip:%s, handle:%u.", FromAddrTostring(remote_addr).c_str(), handle());

    Net_Session::open(arg, remote_addr);
    b_connected_ = true;
    last_time_ = time(NULL);
    keepalive();//连接成功则直接发送心跳
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.heart_beat_interval), NULL);
    return 0;
}



int UserCenterSession::on_receive_message(char *ptr, int len)
{
    int ret = 0;
    // offset pkglen word
    BinInputPacket<> inpkg(ptr, len);
    inpkg.offset_head(sizeof(COHEADER));
    COHEADER header;
    inpkg.get_head(header);
    LOG(INFO)("UserCenterSession receive msg. [%s]", header.print());

    last_time_ = time(NULL);
    switch (header.cmd)
    {
        case CMD_KEEPALIVE:
            LOG(INFO)("[beat] user center server is alive.");
            break;
		/*
        case CMD_SEND_MESSAGE:
        {
            BackMessageCmd cmd(this, inpkg);
            cmd.Execute();
        }
        break;
        case CMD_SYNC_NOTICE: // 通知客户端同步消息
        {
            BackSyncNoticeCmd cmd(this, inpkg);
            cmd.Execute();
        }
        break;
        */

        default:
            LOG(ERROR)("UserCenterSession, unsupported cmd.");
            break;
    }

    return ret;
}

int UserCenterSession::handle_close(uint32_t handle)
{
    LOG(ERROR)("UserCenterSession::handle_close, ip:%s, handle:%u, reconnect interval:%u."
              , FromAddrTostring(remote_addr()).c_str(), handle, serv_info_.reconnect_interval);
    //delete this;
    b_connected_ = false;
    last_time_ = time(NULL);
    //先取消之前的定时器
    TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
    return 0;
}

void UserCenterSession::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;
    time_t cur_time = time(NULL);
    if (!b_connected_ && ((cur_time - last_time_) >= serv_info_.reconnect_interval))
    {
        last_time_ = cur_time;
        LOG(INFO)("reconnect user center server, ip:%s:%s", serv_info_.server_ip.c_str(), serv_info_.server_port.c_str());
        //重连user center ，重连失败则再次注册定时器

        INET_Addr addr;
        string strAddr = serv_info_.server_ip+":"+serv_info_.server_port;
        FromStringToAddr(strAddr.c_str(), addr);
        UserCenterSession *user_center_session = this;
        if (ServerApp::Instance()->user_center_connector().connect(user_center_session, addr
                , ServerApp::Instance()->splitter(), serv_info_.conn_time_out, 
                utils::ServerConfig::max_buffer_size[USER_CENTER_SERVER_NAME]) != 0)
        {
            LOG(WARN)("reconnect user center server falied, ip:%s", FromAddrTostring(remote_addr()).c_str());
            TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(serv_info_.reconnect_interval), NULL);
        }

        return ;
    }

    if (b_connected_ && (cur_time - last_time_ >= serv_info_.time_out))
    {
        LOG(WARN)("user center session time out, session_id:%s, curtime:%ld, last_time:%ld, difftime:%d"
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

void UserCenterSession::set_serv_info(const Server_Info &info)
{
    serv_info_ = info;
    set_session_id(serv_info_.server_id);
}

void UserCenterSession::keepalive()
{
    BinOutputPacket<> outpkg(ServerApp::Instance()->get_buffer(), MAX_SEND_BUFF_LEN);
    int head_len = sizeof(COHEADER);
    outpkg.offset_head(head_len);
    COHEADER header;
    header.cmd = CMD_KEEPALIVE;
    header.seq = 0;
    header.head_len = head_len;
    header.uid = 0;
    header.sender_coid = 0;
    header.receiver_coid = 0;
	outpkg << utils::ServerConfig::Instance()->cond_id();
    header.len = outpkg.length();
    outpkg.set_head(header);
    LOG(INFO)("UserCenterSession:%s send beat. [%s]", session_id().c_str(), header.print());
    LOG_HEX(outpkg.getData(), outpkg.length(), utils::L_DEBUG);
    send_msg(outpkg.getData(), outpkg.length());
}

int UserCenterSession::SendToUserCenter(int hashid, char *data, int len)
{
    int ret = 0;
    vector<UserCenterSession*> user_servers = ServerApp::Instance()->get_user_center_session();
    int session_size = user_servers.size();
	if (session_size <= 0)
	{
		LOG(ERROR)("send to user center error. server num is:%d", session_size);
		return -1;
	}
	int index = hashid % session_size;
    UserCenterSession* server = user_servers[index];
    if (NULL != server && server->connected())
    {
        LOG(INFO)("send to user center, server id:%d", index);
        ret = server->send_msg(data, len);
    } else {
        LOG(ERROR)("send to user center failed. connection not exist or disconnected.");
        ret = -2;
    }
	return ret;
}


