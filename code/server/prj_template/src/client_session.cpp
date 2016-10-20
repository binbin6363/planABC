/**
 * @filedesc: 
 * client_session.cpp, handle client action
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/8 21:52:59
 * @modify:
 *
**/

#include "client_session.h"

#include "server_app.h"
#include "cmd.h"
#include "server_config.h"

using namespace common;
//using namespace WCCD;


ClientSession::ClientSession(void)
    : last_time_(0)
    , signing_seq_(0)
    , login_status_(0)
    , connect_status_(CONNECTING)
    , client_uid_(0)
    , m_unBuild(0)
    , m_unVersion(0)
{
    // 清空密钥
    memset(session_key_, 0, CLIENT_PASSWD_LENGTH);
    //signing_seq_ = ServerApp::Instance()->get_new_loginseq();
    //LOG(DEBUG)("ClientSession new a instant, login_seq:%d", signing_seq_);
}


ClientSession::~ClientSession(void)
{
    LOG(DEBUG)("ClientSession destruct. uid:%u, login_seq:%d, address:%s, handle:%u"
        , client_uid_, signing_seq_, FromAddrTostring(remote_addr()).c_str(), handle());
    // 未发送到客户端的消息回写
}

int ClientSession::open( void *arg, const INET_Addr &remote_addr )
{
    LOG(INFO)("ClientSession::open, address:%s, handle:%u.", FromAddrTostring(remote_addr).c_str(), handle());
    connect_status_ = CONNECTED;

    last_time_ = time(NULL);
    Net_Session::open(arg, remote_addr);

    return 0;
}

int ClientSession::handle_close( uint32_t handle )
{
    LOG(INFO)("ClientSession::handle_close, uid:%u, sessionid:%s, handle:%u, address:%s"
        , client_uid_, session_id().c_str(), handle, FromAddrTostring(remote_addr()).c_str());
    ServerApp::Instance()->remove_uid_session(this);
//    ServerApp::Instance()->remove_first_pkg_list(this);
//    ServerApp::Instance()->remove_seq_session(this); // todo: need to remove the session with the seq come from packet head
//    ServerApp::Instance()->remove_session(session_id());
    //TimeoutManager::Instance()->UnRegisterTimer(get_time_id());
    delete this;
    return 0;
}

int ClientSession::check_deciph_pkg(BinInputPacket<> &inpkg)
{
    int ret = 0;
    HEADER header;
    inpkg.get_head(header);
    if( (uint32_t)header.pkglen > sizeof(HEADER) )
    {
        LOG(DEBUG)("[%u] [start decip pkg] handle:%u, sessionkey:", client_uid_, handle());
        LOG_HEX(session_key(), CLIENT_PASSWD_LENGTH, utils::L_DEBUG);
        int out_len = 0;
        XTEA::decipher(session_key(), inpkg.getData() + sizeof(HEADER)
            , header.pkglen - sizeof(HEADER)
            , inpkg.getData() + sizeof(HEADER)
            , header.pkglen - sizeof(HEADER)
            , out_len);
        if (0 == out_len)
        {
            LOG(ERROR)("[%u] decipher failed, do not handle this cmd any more. handle:%u [%s]"
                , client_uid_, handle(), header.print());
            ret = -1;
            return ret;
        }
        inpkg.set_pkglen(out_len + sizeof(HEADER));
        LOG(DEBUG)("[%u] [recv decrypt mob pkg] len:%u", client_uid_, inpkg.size());
        LOG_HEX(inpkg.getData(), inpkg.size(), utils::L_DEBUG);
    } else {
        LOG(ERROR)("pkg len is smaller than Header len.");
        ret = -1;
    }
    return ret;
}


// 解密，密钥会更改，登录包密钥根据uid生成，后续密钥由服务端生成
int ClientSession::Decrypt(BinInputPacket<> &inpkg)
{
    int ret = 0;
    // 空包无需解密，如心跳包
    if (inpkg.size() == sizeof(HEADER))
    {
        return 0;
    }

    // 首次登录，根据uid生成密钥,以后的密钥是服务端生成的随机密钥
    HEADER header;
    inpkg.get_head(header);
    if ((uint32_t)header.cmd == CMD_LOGIN)
    {
        MakeKeyByUid();
    }

    ret = check_deciph_pkg(inpkg);
    return ret;
}

int ClientSession::on_receive_message( char *ptr, int len )
{
    int ret = 0;
    // 1. make inpkg, get head
    BinInputPacket<> inpkg(ptr, len);
    inpkg.offset_head(sizeof(HEADER));
    HEADER header;
    inpkg.get_head(header);
    LOG(INFO)("ClientSession, receive client msg. [%s]", header.print());
    LOG(WARN)("CHECK CLIENT SESSION SEQ NO.:%d", signing_seq_);

    // 2. decrypt
    ret = Decrypt(inpkg);
    if (0 != ret)
    {
        LOG(ERROR)("client decrypt failed. [%s]", header.print());
        return 0;
    }

    // 3. handle with cmd
    switch (header.cmd)
    {
        case CMD_KEEPALIVE:    // 0, empty body
        {
            BeatCmd cmd(this, inpkg);
            cmd.execute();
        }
        break;
        case CMD_LOGIN: // 1000
        {
            LOG(WARN)("CHECK CLIENT SESSION SEQ NO.:%d", signing_seq_);
            LoginCmd cmd(this, inpkg);
            if (cmd.execute())
            {
                login_status_ = LOGINING;
            } else {
                login_status_ = LOGIN_FAILED;
            }
        }
        break;
        case CMD_LOGOUT: // 1001
        {
            LogoutCmd cmd(this, inpkg);
            if (cmd.execute())
            {
                login_status_ = LOGOUT;
            }
        }
        break;
        case CMD_NOTIFY_TASK:
        {
            LOG(WARN)("ClientSession, not supported yet. %u", CMD_NOTIFY_TASK);
        }
        break;
        case CMD_SYNC_TASK:
        {
            LOG(WARN)("ClientSession, not supported yet. %u", CMD_SYNC_TASK);
        }
        break;
        case CMD_REPORT_TASK:
        {
            LOG(WARN)("ClientSession, not supported yet. %u", CMD_REPORT_TASK);
        }
        break;
        case CMD_GET_INCOME:
        {
            LOG(WARN)("ClientSession, not supported yet. %u", CMD_GET_INCOME);
        }
        break;
        default:
            LOG(ERROR)("ClientSession, unsupported client cmd.");
            break;
    }

    return ret;

}

void ClientSession::handle_timeout(int id,void *userData)
{
    (void)id;
    (void)userData;

    time_t cur_time = time(NULL);
    if (difftime(cur_time, last_time_) >= utils::ServerConfig::Instance()->client_ttl())
    {
        LOG(WARN)("client session time out, session_id:%s, curtime:%ld, last_time:%ld, difftime:%d, ttl:%u, ip:%s"
            , session_id().c_str(), cur_time, last_time_, (int)difftime(cur_time, last_time_)
            , utils::ServerConfig::Instance()->client_ttl(), FromAddrTostring(remote_addr()).c_str());
        destory_connect();
    }
    else
    {
        TimeoutManager::Instance()->RegisterTimer(this, MAKE_SECOND_INTERVAL(utils::ServerConfig::Instance()->session_timeout()), NULL);
    }

}


void ClientSession::login_ok()
{
    login_status_ = LOGIN_OK;
    ServerApp::Instance()->add_uid_session(this);
    LOG(INFO)("login ok, add session. uid:%u, login seq:%d, handle:%u."
        , client_uid_, signing_seq_, handle());
}

void ClientSession::destory_connect()
{
    // close handle immediately
    login_status_   = LOGIN_FAILED;
    connect_status_ = DISCONNECTED;
    //ServerApp::Instance()->remove_uid_session(this);
    this->close();
    LOG(INFO)("destory connect. uid:%u, login seq:%d, handle:%u."
        , client_uid_, signing_seq_, handle());
}

void ClientSession::MakeKeyByUid()
{
    // TODO: 根据uid生成key,目前使用假数据
    LOG(INFO)("just make fake key by uid. need to implements!");
//    for (int i = 0; i < (int)CLIENT_PASSWD_LENGTH; ++i)
//    {
//        session_key_[i] = i + 'a';
//    }
    // TODO: 此处被坑过，泥马使用strcpy会复制最后一个'\0'
    // TODO: ，直接把signing_seq_值修改为0了.囧^._.^. BBWANG
    strncpy(session_key_, "0123456789abcdef", CLIENT_PASSWD_LENGTH);
}

void ClientSession::MakeKeyRand()
{
    LOG(INFO)("make random key.");
    struct timeval tv;
    uint32_t seed = 0;
    int ret = gettimeofday(&tv, NULL);
    if (0 != ret)
    {
        ret = errno;
        seed = rand();     // 调用失败就用上次的随机值作为种子
        LOG(ERROR)("MakeKeyRand error. call gettimeofday failed. ret:%d, msg:%s", ret, strerror(ret));
    } 
    else 
    {
        seed = tv.tv_usec; // 微秒时间作为种子
    }
    srand(seed);
    for (int i = 0; i < (int)CLIENT_PASSWD_LENGTH; ++i)
    {
        session_key_[i] = (rand() % 57)+65;
    }
    LOG(INFO)("make random key");
    LOG_HEX(session_key_, CLIENT_PASSWD_LENGTH, utils::L_DEBUG);
}


