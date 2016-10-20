/**
 * @filedesc: 
 * leveldb_server_app.cpp, app file
 * @author: 
 *  bbwang
 * @date: 
 *  2014/8/3 12:02:59
 * @modify:
 *
**/

#include "leveldb_server_app.h"
#include "data_type.h"
#include "comm.h"
#include "backend_sync.h"
#include "leveldb_engine.h"
#include "timecounter.h"

using namespace utils;
using namespace common;


#ifndef USED_TIME
#define USED_TIME "USED_TIME"
#define CAL_CNT   "CAL_CNT"
#endif



LevelDbApp::LevelDbApp(void)
    : net_manager_(NULL)
    , send_buffer_(NULL)
    , backend_sync_(NULL)
    , sync_data_send_queue_(NULL)
{
    send_buffer_ = new char[MAX_SEND_BUFF_LEN];
    sync_data_send_queue_ = new Data_Send_Queue;

    perfman_cal_[USED_TIME] = 0.0;
    perfman_cal_[CAL_CNT] = 0.0;
    show_perfman_interal_ = 3600; // 默认一小时打印一次
}

LevelDbApp::~LevelDbApp(void)
{
    SAFE_DELETE(net_manager_)
    SAFE_DELETE_ARRAY(send_buffer_)
    SAFE_DELETE(backend_sync_)
    SAFE_DELETE(sync_data_send_queue_)
}

int ClientSplitter_::split( const char *buf, int len, int &packet_begin, int &packet_len )
{
    if ((size_t) len < sizeof(COHEADER) )
    {
        return 0;
    }

    COHEADER *hdr = (COHEADER *) buf;
    int32_t pkglen = ntohl(hdr->len);
    int32_t cmd = ntohl(hdr->cmd);
    int32_t seq = ntohl(hdr->seq);
    int32_t head_len = ntohl(hdr->head_len);

    if( pkglen > (int32_t)utils::ServerConfig::max_buffer_size[CLIENT_NODE_NAME] || pkglen < (int32_t)sizeof(COHEADER))
    {
        LOG(ERROR) ("ClientSplitter_ split error, pkg too long or short. hdr.pkglen:%d, len:%d, limited len:%u, head_len:%d, cmd:%d, seq:%d",
            pkglen, len, utils::ServerConfig::max_buffer_size[CLIENT_NODE_NAME], head_len, cmd, seq);
        return -1;
    }

    if(pkglen > len)
    {
        return 0;
    }
    if (len >= pkglen)
    {
        packet_begin = 0;
        packet_len = pkglen;

        return 1;
    } 

    return 1; 
}

int MasterSplitter_::split( const char *buf, int len, int &packet_begin, int &packet_len )
{
    if ((size_t) len < sizeof(COHEADER) )
    {
        return 0;
    }

    COHEADER *hdr = (COHEADER *) buf;
    int32_t pkglen = ntohl(hdr->len);
    int32_t cmd = ntohl(hdr->cmd);
    int32_t seq = ntohl(hdr->seq);
    int32_t head_len = ntohl(hdr->head_len);

    if( pkglen > (int32_t)utils::ServerConfig::max_buffer_size[MASTER_SERVER_NAME] || pkglen < (int32_t)sizeof(COHEADER))
    {
        LOG(ERROR) ("MasterSplitter_ split error, pkg too long or short. hdr.pkglen:%d, len:%d, limited len:%u, head_len:%d, cmd:%d, seq:%d",
            pkglen, len, utils::ServerConfig::max_buffer_size[MASTER_SERVER_NAME], head_len, cmd, seq);
        return -1;
    }

    if(pkglen > len)
    {
        return 0;
    }
    if (len >= pkglen)
    {
        packet_begin = 0;
        packet_len = pkglen;

        return 1;
    } 

    return 1; 
}

int SlaveSplitter_::split( const char *buf, int len, int &packet_begin, int &packet_len )
{
    if ((size_t) len < sizeof(COHEADER) )
    {
        return 0;
    }

    COHEADER *hdr = (COHEADER *) buf;
    int32_t pkglen = ntohl(hdr->len);
    int32_t cmd = ntohl(hdr->cmd);
    int32_t seq = ntohl(hdr->seq);
    int32_t head_len = ntohl(hdr->head_len);

    if( pkglen > (int32_t)utils::ServerConfig::max_buffer_size[SLAVE_SERVER_NAME] || pkglen < (int32_t)sizeof(COHEADER))
    {
        LOG(ERROR) ("SlaveSplitter_ split error, pkg too long or short. hdr.pkglen:%d, len:%d, limited len:%u, head_len:%d, cmd:%d, seq:%d",
            pkglen, len, utils::ServerConfig::max_buffer_size[SLAVE_SERVER_NAME], head_len, cmd, seq);
        return -1;
    }

    if(pkglen > len)
    {
        return 0;
    }
    if (len >= pkglen)
    {
        packet_begin = 0;
        packet_len = pkglen;

        return 1;
    } 

    return 1; 
}

uint32_t LevelDbApp::get_new_loginseq()
{
    static uint32_t loginseq = 0;
    loginseq++;
    if(0 == loginseq)
        loginseq++;
    return loginseq;
}


char *LevelDbApp::get_buffer()
{
    return send_buffer_;
}

void LevelDbApp::reset_buffer()
{
    if (NULL != send_buffer_)
        memset(send_buffer_, '\0', MAX_SEND_BUFF_LEN);
}

int LevelDbApp::open()
{
    net_manager_ = new Net_Manager;
    // todo: start net thread, handle event
    if (0 != net_manager_->start())
    {
        LOG(ERROR)("[start] net_manager_ start failed.");
        return -1;
    }

    // todo: start leveldb and udp_cmd service
    LOG(INFO)("open acceptor");
    if (0 != open_acceptor())
    {
        LOG(ERROR)("[start] open_acceptor failed.");
        return -1;
    }

    //此处先写死了
    LOG(INFO)("[start] all start succeed. ^_^");
    TimeoutManager::Instance()->RegisterTimer(this, 1000, NULL);

    // 启动后端的同步线程，使用独立的Net_Manager
    backend_sync_ = new BackendSync;
    return backend_sync_->Start(net_manager_);
}




int LevelDbApp::open_acceptor()
{
    client_accept_.net_manager(net_manager_);
    udp_cmd_accept_.net_manager(net_manager_);
   
    INET_Addr inetAddr;
    string strAddr = ServerConfig::Instance()->ldb_ip()+":"+ServerConfig::Instance()->ldb_port();
    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1){
        LOG(ERROR)("[start] parse addr error,service address=%s", strAddr.c_str());
        return -1;
    }

    LOG(INFO)("[start] leveldb buf size:%u", utils::ServerConfig::max_buffer_size[CLIENT_NODE_NAME]);
    // todo: open leveldb server. transfer callback function
    LOG(INFO)("[start] init leveldb service, addr=%s.", strAddr.c_str());
    if (client_accept_.open(inetAddr, &splitter_
        , utils::ServerConfig::max_buffer_size[CLIENT_NODE_NAME]) != 0)
    {
        LOG(ERROR)("[start] leveldb service open failed.");
        return -1;
    }

    // udp client session
    strAddr = utils::ServerConfig::Instance()->udp_cmd_addr();
    if(FromStringToAddr(strAddr.c_str(), inetAddr) == -1){
        LOG(ERROR)("[start] parse addr error,udp cmd service address=%s", strAddr.c_str());
        return -1;
    }

    // todo: open udp cmd server. transfer callback function
    LOG(INFO)("[start] init udp cmd service, addr=%s.", strAddr.c_str());
    if (udp_cmd_accept_.open(inetAddr, NULL) != 0)
    {
        LOG(ERROR)("[start] udp cmd service open failed.");
        return -1;
    }

    return 0;
}

int LevelDbApp::run_service()
{
    if (!net_manager_)
    {
        LOG(WARN)("[start] leveldb app is not initialized.");
        return -1;
    }

    LOG(INFO)("enter run service loop.");
    while(1)
    {
        TIME_SPEC nowTime = gettimeofday();
        TimeoutManager::Instance()->UpdateTimeout(nowTime);
        
        // 把缓存的包发出去
        send_packet();
        
        Net_Event*  ev = net_manager_->get_event();
        if (ev)
        {
            ev->handler(*ev);
            delete ev;
        }
        else
        {
            usleep(10);
        }
    }
    LOG(INFO)("leave run service loop.");

}

// 打印数据库当前状态
// 数据量
// 数据条数
void LevelDbApp::DumpDatabaseInfo()
{
    std::string ss;
    char property[STACK_STR_MAX_LEN] = {0};
    int len = 0;

    // 1. dump seq. min seq and lastest seq.
    uint64_t min_seq     = LeveldbEngine::inst().get_min_seq();
    uint64_t lastest_seq = LeveldbEngine::inst().get_lastest_seq();
    len = snprintf(property, STACK_STR_MAX_LEN, "min_seq:%lu, ", min_seq);
    ss.append(property, len);
    len = snprintf(property, STACK_STR_MAX_LEN, "lastest_seq:%lu, ", lastest_seq);
    ss.append(property, len);
    
    const static char *PERFIX1 = "leveldb.";
    const static char *STATS1 = "stats";
    std::string value;
    // 2. dump leveldb.stats
    len = snprintf(property, STACK_STR_MAX_LEN, "%s%s", PERFIX1, STATS1);
    std::string stats(property);
    LeveldbEngine::inst().GetProperty(stats, &value);
    ss.append(property, len);
    ss.append("\n");
    ss.append(value);

    LOG(INFO)("[timer] dump database info:%s", ss.c_str());
}


void LevelDbApp::handle_timeout( int id,void *userData )
{
    (void)id;
    (void)userData;
    LOG(DEBUG)("enter SessionManager onTimer");
    static uint32_t cnt = 0;

    // dump database info
    if ( ++cnt % 10 == 0) {
        DumpDatabaseInfo();
    }

    // show_perfman_interal_打印一次性能 
    if (cnt % show_perfman_interal_ == 0) {
        double perfman = get_perfman();
        LOG(INFO)("[perfman] all request average cost time:%s %f (s/request)",getTimeColor(perfman), perfman);
    }
    
    uint32_t curTime = time(NULL);
    uint32_t iCount = 0;
    list_head* pPos;
    list_head* pHead = client_session_list_.begin();
    for( pPos = pHead->next; pPos != pHead; )
    {
        list_head *pCurPos = pPos;
        pPos = pPos->next;

        ClientSession* pHeadSession = list_entry(pCurPos, ClientSession, item_);
        if(NULL != pHeadSession)
        {
            if( curTime > ServerConfig::Instance()->client_ttl() + pHeadSession->last_time())
            {
                iCount ++;
                LOG(ERROR)("[clean] user session_id:%s timeout and do auto clean, cur_time:%u, last_time:%u."
                    , pHeadSession->session_id().c_str(), (uint32_t)curTime, (uint32_t)pHeadSession->last_time());
                pHeadSession->close();
            }
            else
            {
                break;
            }
        }
    }
    if( iCount > 0 )
    {
        LOG(INFO)("[sum] SUM CLEAN user session num:%u.", iCount);
    }

    //此处先写死了
    TimeoutManager::Instance()->RegisterTimer(this, 1000, NULL);
}


Net_Session * LevelDbApp::get_session( const string &session_id )
{
    std::map<string, Net_Session*>::iterator it = map_session_.find(session_id);
    if (it != map_session_.end())
        return it->second;

    return NULL;
}


int LevelDbApp::add_session( const string &session_id, Net_Session *session )
{
    LOG(DEBUG)("add_session id:%s", session_id.c_str());
    std::map<string, Net_Session*>::iterator it = map_session_.find(session_id);
    if (it != map_session_.end())
    {
        LOG(WARN)("session is exists, session_id:%s", session_id.c_str());
        return -1;
    }

    map_session_.insert(make_pair(session_id, session));
    return 0;
}

void LevelDbApp::remove_session( const string &session_id )
{
    if (map_session_.find(session_id) != map_session_.end())
    {
        map_session_.erase(session_id);
        LOG(DEBUG)("remove_session id:%s", session_id.c_str());
    }
    else
    {
        LOG(DEBUG)("remove_session failed, session with id:%s not exist.", session_id.c_str());
    }
}


int LevelDbApp::push_back_packet(uint32_t handle, const char *data, uint32_t len)
{
    int ret = 0;
    if (NULL == data || 0 == len) {
        LOG(ERROR)("push back packet failed. data is null or len is 0.");
        return -1;
    }
	Net_Packet *packet = new Net_Packet(len);

	memcpy(packet->ptr(), data, len);
	packet->length(len);

    bool rc = true;
    DataSendPacket *psend_packet = new DataSendPacket(handle, packet);
    
    rc = sync_data_send_queue_->write(psend_packet);
	if (!rc)	// 队列用尽
	{
		LOG(WARN)("push_back_packet error, data send queue is full");
        SAFE_DELETE(packet);
        SAFE_DELETE(psend_packet);
		return -2;
	} else {
		LOG(DEBUG)("send buf handle:%u, len:%d", handle, len);        
    }
    LOG(DEBUG)("sync thread push back a packet to queue.");
    return ret;
}

DataSendPacket *LevelDbApp::pop_packet()
{
    DataSendPacket *psend_packet = NULL;
    bool rc = sync_data_send_queue_->read(psend_packet);
    if (!rc) {
        psend_packet = NULL;
    }
    return psend_packet;
}

// 一次发10个包，防止阻塞主逻辑其他业务
int LevelDbApp::send_packet()
{
    uint32_t cnt = 0;
    while (true) {
        ++cnt;
        if (cnt >= 10) {
            LOG(DEBUG)("send 10 packet, wait for next sending.");
            break;
        }
        DataSendPacket *psend_packet = pop_packet();
        if (NULL == psend_packet) {
            break;
        }
        uint32_t handle = psend_packet->handle;
    	Net_Handler *net_handle = Net_Handle_Manager::Instance()->GetHandle(handle);
        Net_Session *session = dynamic_cast<Net_Session *>(net_handle);
        if (NULL == session) {
            LOG(ERROR)("get session failed.");
        } else {
            session->send_msg(psend_packet->packet->ptr(), psend_packet->packet->length());
        }
        SAFE_DELETE(psend_packet->packet);
        SAFE_DELETE(psend_packet);
        LOG(DEBUG)("main thread send a packet.");
    }
    return 0;
}


void LevelDbApp::add_perfman(double used_time)
{
    perfman_cal_[USED_TIME] += used_time;
    perfman_cal_[CAL_CNT] += 1.0;
}

double LevelDbApp::get_perfman()
{
    double perfman = 0.0;
    if (perfman_cal_[CAL_CNT] > -0.0001 && perfman_cal_[CAL_CNT] < 0.0001) {
        return perfman;
    }
    perfman = perfman_cal_[USED_TIME] / perfman_cal_[CAL_CNT];
    perfman_cal_[USED_TIME] = 0.0;
    perfman_cal_[CAL_CNT] = 0.0;
    return perfman;
}
    
