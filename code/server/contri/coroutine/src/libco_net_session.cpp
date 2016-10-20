/**
 * @filedesc: 
 * libco_net_session.cpp
 * @author: 
 *  bbwang
 * @date: 
 *  2015/1/7 16:02:41
 * @modify:
 *
**/
#include "libco_net_session.h"

#include <algorithm>
#include "msg.h"
#include "libco_routine_manager.h"
#include "net_handle_manager.h"
#include "translater.h"
#include "option.h"	
#include "hashselector.h"	
#include "conshashselector.h"	
#include "directselector.h"	
#include "scoreselector.h"	
#include "comm.h"	



LibcoNetSession::LibcoNetSession()
    : option_(NULL)
    , last_time_(0)
    , b_connected_(false)
{
}

LibcoNetSession::~LibcoNetSession()
{
}

void LibcoNetSession::set_option(Option *option/* = NULL*/)
{
    if (NULL == option)
    {
        LOG(ERROR)("set option failed. option is null.");
    }
    option_ = option;
}

Option *LibcoNetSession::option() const
{
    return option_;
}


//Processor *LibcoNetSession::processor() const
//{
//    return processor_;
//}

//void LibcoNetSession::set_processor(Processor *processor)
//{
//    this->processor_ = processor;
//}


// callback func handle call this func
//int LibcoNetSession::process_event( Net_Event &ev )
//{
//	switch ( ev.net_event_type )
//	{
//	case TYPE_DATA:
//		{
//            // 从当前协程获取消息
//            Msg *msg = CoroutineMgr::Instance().GetCurMsg();
//            if (!msg)
//            {
//                LOG(ERROR)("get cur co msg failed. coid:%u", CoroutineMgr::Instance().GetCurCoId());
//                break;
//            }
//        	if (session_type_ != TCP_SESSION)//为udp保存对端地址
//        	{
//        		remote_addr(ev.remote_addr);
//        	}
//            on_receive_message(*msg);
//		}break;	
//	case TYPE_ACCEPT:
//		{
//			handle_accept(ev.new_id, ev.remote_addr);
//		}
//		break;
//	case TYPE_CONNECT:
//		{
//			handle_connect(ev.id, ev.remote_addr);
//		}
//		break;
//	case TYPE_EXCEPTION:
//	case TYPE_SYS_ERROR:
//	case TYPE_CLOSE:
//	case TYPE_TIMEOUT:
//		{
//			handle_close(ev.id);
//			Net_Handle_Manager::Instance()->RemoveHandle(ev.id);
//		}break;
//	default:
//		{
//			LOG(WARN)("process error, unknown net_event_type:%d", ev.net_event_type);
//		}break;
//	}

//	return 0;
//}




void LibcoNetSession::session_type( Session_Type session_type )
{
	session_type_ = session_type;
}

Session_Type &LibcoNetSession::session_type()
{
    return session_type_;
}

int LibcoNetSession::open( void *arg, const INET_Addr &remote_addr )
{
	(void)arg;
	remote_addr_ = remote_addr;
    last_time_ = time(NULL);
	return 0;
}

void LibcoNetSession::update_time()
{
    last_time_ = time(NULL);
    b_connected_ = true;
}


time_t LibcoNetSession::last_time()
{
    return last_time_;
}


bool LibcoNetSession::IsConnected()
{
    return b_connected_;
}



int LibcoNetSession::on_receive_message( Msg &msg )
{
	(void)msg;
	LOG(INFO)("LibcoNetSession::on_receive_message");
	return 0;
}


// 给前端回数据,Msg &msg必须要为on_receive_message中的参数msg
int LibcoNetSession::SendResponse(const Msg &msg, Msg &result)
{
	(void)msg;
	if (net_manager() == NULL)
	{
		LOG(WARN)("SendResponse, send msg failed, not initialized.");
		return NET_NOT_INIT_ERROR; // -1
	}
	Translater* translater = NULL;
    if (NULL == option_ || NULL == (translater = option_->translater))
    {
        LOG(ERROR)("SendResponse, no translater match this session. net id:%u.", handle());
        return CONFIG_NOT_INIT_ERROR;
    }
    // 1. reverse coid
    //result.ReverseCoId();
    result.SetReplyCoId();

    // 2. encode
    Net_Packet *net_packet = new Net_Packet(result.GetSize());
	int length = translater->Encode(net_packet->ptr(), net_packet->max_packet_length(), &result, option_->service_name);
	if (length == -1)
	{
		LOG(WARN)("SendResponse, encode message fail,Message type=%s,cmd=%d", result.GetTypeName().c_str(), result.GetCmd());
		delete net_packet;
		return ENCODE_ERROR;
	}
    net_packet->length(length);
    
    // 3. send packet
	int ret = 0;
	if (session_type_ == TCP_SESSION)
	{
		ret = net_manager()->send_packet(handle(), net_packet);
	}
	else
	{
		ret = net_manager()->send_packet(handle(), net_packet, remote_addr());
	}
    // 4. handle ret
	if (ret == 0)
	{
		LOG(DEBUG)("send buf handle:%u, len:%d", handle(), length);
	}
	else//发送失败则删除packet
	{
	    ret = NET_LOST_ERROR;
		delete net_packet;
	}
    return ret;
}

// 给后端发的数据
int LibcoNetSession::SendRequest(Msg &request)
{
	if (net_manager() == NULL)
	{
		LOG(WARN)("SendRequest, send msg failed, not initialized.");
		return NET_NOT_INIT_ERROR;
	}
	Translater* translater = NULL;
    if (NULL == option_ || NULL == (translater = option_->translater))
    {
        LOG(ERROR)("SendRequest, no translater match this session. net id:%u.", handle());
        return CONFIG_NOT_INIT_ERROR;
    }
    // 1. reverse coid
    request.SetRequestCoId();
    //request.ReverseCoId();
    
    // 2. encode
    Net_Packet *net_packet = new Net_Packet(request.GetSize());
	int length = translater->Encode(net_packet->ptr(), net_packet->max_packet_length(), &request, option_->service_name);
	if (length == ENCODE_ERROR)
	{
		LOG(WARN)("SendRequest, encode message fail,Message type=%s,cmd=%d", request.GetTypeName().c_str(), request.GetCmd());
		delete net_packet;
        net_packet = NULL;
		return ENCODE_ERROR;
	}
    net_packet->length(length);
    
    // 3. send packet
	int ret = 0;
	if (session_type_ == TCP_SESSION)
	{
		ret = net_manager()->send_packet(handle(), net_packet);
	}
	else
	{
		ret = net_manager()->send_packet(handle(), net_packet, remote_addr());
	}
    // 4. handle ret
	if (ret == 0)
	{
	    LOG(INFO)("handle:%u, send buf len:%u, remote addr:%s"
            , handle(), length, FromAddrTostring(remote_addr()).c_str());
	}
	else//发送失败则删除packet
	{
	    ret = NET_LOST_ERROR;
		delete net_packet;
        net_packet = NULL;
	}
    return ret;
}

#if 0
int LibcoNetSession::SendRequestAndRecv(Msg &request, Msg *&result)
{
    // 1. 发送请求
    int ret = SendRequest(request);
	if (0 != ret)
	{
		LOG(WARN)("send request failed");
		return ret;
	}

    // 2. 挂起
    LOG(INFO)("[yield] current coid:%u", CoroutineMgr::Instance().GetCurCoId());
    CoroutineMgr::Instance().CoroutineYieldCt();

    // 3. 唤醒之后从当前协程获取数据
    uint32_t cur_coid = CoroutineMgr::Instance().GetCurCoId();
    LOG(INFO)("[resume] current coid:%u", cur_coid);
    result = CoroutineMgr::Instance().GetCurMsg();
    // 4. 对收到的消息做校验，超时/错乱
    if (result->IsTimeout())
    {
        LOG(ERROR)("msg timeout. coid:%u", result->receiver_coid);
		return TIME_OUT_ERROR;
    }
    if (result->receiver_coid != cur_coid)
    {
        LOG(ERROR)("coroutine id not match. coid:%u", result->receiver_coid);
		return COID_ERROR;
    }
    // TODO: 可在此处检测是否出现异常，及时处理，需支持异常进入协程处理
//    if (result->IsAbnormal())
//    {
//        LOG(ERROR)("connection abnormal. routine id:%u", result->receiver_coid);
//		return NET_ABNORMAL_ERROR;
//    }
    return 0;
}
#endif

char * LibcoNetSession::get_buffer()
{
	return NULL;
}

int LibcoNetSession::handle_connect( uint32_t new_handle, const INET_Addr &remote_addr )
{
    (void)new_handle;
	if (open(this, remote_addr) != 0)//调用open通知上层
	{
        b_connected_ = false;
		this->close();
	}
    b_connected_ = true;
	
	return 0;
}



void LibcoNetSession::SetId(uint32_t )
{
}    
    
uint32_t LibcoNetSession::GetId()
{
    LOG(WARN)("call base LibcoNetSession GetId method.");
    return 0;
}

uint32_t LibcoNetSession::GetId() const
{
    LOG(WARN)("call base LibcoNetSession GetId const method.");
    return 0;
}


void LibcoNetSession::SetScore(uint32_t )
{
}    
    
uint32_t LibcoNetSession::GetScore()
{
    LOG(WARN)("call base LibcoNetSession GetScore method.");
    return 0;
}

uint32_t LibcoNetSession::GetScore() const
{
    LOG(WARN)("call base LibcoNetSession GetScore const method.");
    return 0;
}

uint32_t LibcoNetSession::NodeId() // 虚拟节点id
{
    LOG(WARN)("call base LibcoNetSession NodeId const method.");
    return 0;
}


void LibcoNetSession::SetNodeId(uint32_t )
{
    LOG(WARN)("call base LibcoNetSession SetNodeId const method.");
}

//
Cluster::Cluster()
    : group_selector_(NULL)
{
    groups_.clear();
}

Cluster::~Cluster()
{
    groups_.clear();
    SAFE_DELETE(group_selector_);
}


int Cluster::AddGroup(SessionGroup *session_group)
{
    int ret = 0;
    ret = internal_add_group(session_group);
    if (0 != ret)
    {
        LOG(ERROR)("add session group failed, id:%u, addr:%p", session_group->GetId(), session_group);
    }
    else 
    {
        LOG(INFO)("add session group ok, id:%u, addr:%p", session_group->GetId(), session_group);
    }
    return ret;
}

/*   C L U S T E R .   G E T   G R O U P   */
/*-------------------------------------------------------------------------
    通过指定组id直接获取
-------------------------------------------------------------------------*/
SessionGroup *Cluster::GetGroup(uint32_t group_id)
{
    SessionGroup *session_group = NULL;

    GroupCIter group_iter = groups_.find(group_id);
    if (group_iter != groups_.end())
    {
        LOG(DEBUG)("session group exist. group id:%u", group_id);
        session_group = group_iter->second;
    }
    else
    {
        LOG(WARN)("can not found session group with id:%u", group_id);
    }
    
    return session_group;
}


void Cluster::RemoveGroup(uint32_t group_id)
{
    LOG(DEBUG)("remove group from cluster, group id:%u", group_id);
    internal_remove_group(group_id);
    return ;
}

Cluster::GroupMap &Cluster::Groups()
{
    return groups_;
}

/*   C L U S T E R .   G E T   G R O U P   */
/*-------------------------------------------------------------------------
    通过策略获取
-------------------------------------------------------------------------*/
SessionGroup *Cluster::GetGroup()
{
    if (NULL != group_selector_)
    {
//        LOG(DEBUG)("get group in cluster by strategy. selector name:%s"
//            , group_selector_->GetTypeName().c_str());
        group_selector_->ShowInfo();
        return group_selector_->Select();
    }
    if (1 == groups_.size())
    {
        LOG(DEBUG)("get the only group in cluster directly.");
        GroupIter group_iter = groups_.begin();
        return group_iter->second;
    }
    else
    {
        LOG(ERROR)("group selector is null, get group failed.");
        return NULL;
    }
}

/*   C L U S T E R .   S E T   S E L E C T O R   */
/*-------------------------------------------------------------------------
    set selector
-------------------------------------------------------------------------*/
void Cluster::SetSelector(Selector<SessionGroup *> *selector)
{
    group_selector_ = selector;
}

int Cluster::CreateGroupSelector(int strategy)
{
    Selector<SessionGroup *> *selector = NULL;
    switch (strategy)
    {
        case MOD_HASH_STRATEGY:// 0,按模 HASH 策略
        {
            selector = new ModHashSelector<SessionGroup *>();
        }
        break;
    	case POLL_STRATEGY: // 1,轮询 策略
        {
             LOG(ERROR)("strategy:%u not implements.", strategy);
            //selector = new PollSelector<SessionGroup *>();
        }   
        break;
    	case SPECIFIED_STRATEGY: // 2,指定server id
        {
            selector = new DirectSelector<SessionGroup *>();
        }   
        break;
    	case CONS_HASH_STRATEGY: // 3,一致性 HASH 策略
        {
            selector = new ConsHashSelector<SessionGroup *>();
        }   
        break;
    	case DYN_SCORE_STRATEGY: // 4, 动态得分策略
        {
            LOG(ERROR)("strategy:%u not implements.", strategy);
            //selector = new ModHashSelector<SessionGroup *>();
        }   
        break;
        default:
        {
            LOG(WARN)("not supported strategy:%d, use default mod hash selector.", strategy);
            selector = new ModHashSelector<SessionGroup *>();
        }
        break;
    }
    SAFE_DELETE(group_selector_);
    group_selector_ = selector;
    return 0;
}

/*   C L U S T E R .   S E T   H A S H   I D   */
/*-------------------------------------------------------------------------
    must be call after method SetSelector
-------------------------------------------------------------------------*/
void Cluster::SetHashId(uint32_t hash_id)
{
    if (NULL == group_selector_)
    {
        LOG(ERROR)("selector is null, can not set hash id.");
        return ;
    }
    group_selector_->SetHashId(hash_id);
}

Selector<SessionGroup *> *Cluster::GetSelector()
{
    return group_selector_;
}


int Cluster::internal_add_group(SessionGroup *session_group)
{
    int ret = 0;
    if (NULL == session_group)
    {
        LOG(WARN)("add group, session group is null, add group failed.");
        return -1;
    }
    if (NULL == group_selector_)
    {
        LOG(WARN)("add group, selector is null.");
        return -1;
    }

    uint32_t group_id = session_group->GetId();

    GroupCIter group_iter = groups_.find(group_id);
    if (group_iter != groups_.end())
    {
        LOG(DEBUG)("session group exist already. group id:%u", group_id);
        return 0;
    }

    ret = group_selector_->AddElement(group_id, session_group);
    if (0 != ret)
    {
        LOG(ERROR)("add group to selector failed.");
    }
    else
    {
        LOG(DEBUG)("add group to selector ok.");
        groups_[group_id] = session_group;
    }

    return ret;
}


int Cluster::internal_remove_group(uint32_t group_id)
{
    GroupIter group_iter = groups_.find(group_id); 
    if (group_iter != groups_.end())
    {
        groups_.erase(group_iter);
        group_selector_->RemoveElement(group_id);
    }
    else
    {
        LOG(WARN)("remove group with id:%u not exist", group_id);
    }
    return 0;
}


// sessiongroup
SessionGroup::SessionGroup()
    : group_id_(0)
    , session_selector_(NULL)
{
    sessions_.clear();
}

SessionGroup::~SessionGroup()
{
    // session在业务层处理，此处仅仅清空map
    sessions_.clear();
    SAFE_DELETE(session_selector_);
}



int SessionGroup::AddSession(LibcoNetSession *session)
{
    int ret = 0;
    ret = internal_add_session(session);
    if (0 != ret)
    {
        LOG(ERROR)("add session failed, id:%u, addr:%p", session->GetId(), session);
    }
    else 
    {
        LOG(INFO)("add session ok, id:%u, addr:%p", session->GetId(), session);
    }
    return ret;
}


void SessionGroup::RemoveSession(uint32_t session_id)
{
    LOG(DEBUG)("remove session from group, session id:%u", session_id);
    internal_remove_session(session_id);
    return ;
}


/*   S E S S I O N   G R O U P .   G E T   S E S S I O N   */
/*-------------------------------------------------------------------------
    通过策略获取session
-------------------------------------------------------------------------*/
LibcoNetSession *SessionGroup::GetSession()
{
    if (NULL != session_selector_)
    {
//        LOG(DEBUG)("get the session in group by strategy. group id:%u, selector name:%s"
//            , group_id_, session_selector_->GetTypeName().c_str());
        session_selector_->ShowInfo();
        return session_selector_->Select();
    }
    if (1 == sessions_.size())
    {
        LOG(DEBUG)("get the only session in group directly.");
        SessionIter session_iter = sessions_.begin();
        return session_iter->second;
    }
    else
    {
        LOG(ERROR)("session selector is null, get session failed.");
        return NULL;
    }
}

// 精确获取session
LibcoNetSession *SessionGroup::GetSession(uint32_t session_id)
{
    SessionCIter session_iter = sessions_.find(session_id);
    SessionCIter session_end_iter = sessions_.end();
    if (session_iter != session_end_iter)
    {
        return session_iter->second;
    } 
    else 
    {
        LOG(WARN)("not found session with id:%u", session_id);
        return NULL;
    }
}


SessionGroup::SessionMap &SessionGroup::Sessions()
{
    return sessions_;
}

/*   S E S S I O N   G R O U P .   G E T   I D   */
/*-------------------------------------------------------------------------
   for selector    
-------------------------------------------------------------------------*/
uint32_t SessionGroup::GetId()
{
    return group_id_;
}

uint32_t SessionGroup::GetId() const
{
    return group_id_;
}

void SessionGroup::SetId(uint32_t group_id)
{
    group_id_ = group_id;
}


uint32_t SessionGroup::NodeId() // 虚拟节点id
{
    return node_id_;
}


void SessionGroup::SetNodeId(uint32_t node_id)
{
    node_id_ = node_id;
}


void SessionGroup::SetSelector(Selector<LibcoNetSession *> *selector)
{
    session_selector_ = selector;
}

Selector<LibcoNetSession *> *SessionGroup::GetSelector()
{
    return session_selector_;
}


void SessionGroup::SetHashId(uint32_t hash_id)
{
    session_selector_->SetHashId(hash_id);
}


int SessionGroup::CreateSessionSelector(int strategy)
{
    Selector<LibcoNetSession *> *selector = NULL;
    switch (strategy)
    {
        case MOD_HASH_STRATEGY:// 0,按模 HASH 策略
        {
            selector = new ModHashSelector<LibcoNetSession *>();
        }
        break;
    	case POLL_STRATEGY: // 1,轮询 策略
        {
             LOG(ERROR)("strategy:%u not implements.", strategy);
            //selector = new PollSelector<LibcoNetSession *>();
        }
        break;
    	case SPECIFIED_STRATEGY: // 2,指定server id
        {
            selector = new DirectSelector<LibcoNetSession *>();
        }
        break;
    	case CONS_HASH_STRATEGY: // 3,一致性 HASH 策略
        {
            selector = new ConsHashSelector<LibcoNetSession *>();
        }   
        break;
    	case DYN_SCORE_STRATEGY: // 4, 动态得分策略
        {
            selector = new ScoreSelector<LibcoNetSession *>();
        }   
        break;
        default:
        {
            LOG(WARN)("not supported strategy:%d, not use selector.", strategy);
            selector = NULL;
        }
        break;
    }
    SAFE_DELETE(session_selector_);
    session_selector_ = selector;
    return 0;
}


int SessionGroup::internal_add_session(LibcoNetSession *session)
{
    if (NULL == session)
    {
        LOG(WARN)("add session, session is null.");
        return -1;
    }
    if (NULL == session_selector_)
    {
        LOG(WARN)("add session, selector is null.");
        return -1;
    }

    uint32_t session_id = session->GetId();
    SessionCIter session_iter = sessions_.find(session_id);
    if (session_iter != sessions_.end())
    {
        LOG(WARN)("session exist already. sesison id:%u", session_id);
        return -2;
    }
    sessions_[session_id] = session;
    LOG(INFO)("add session, id:%u, session addr:%p", session_id, session);
    int ret = session_selector_->AddElement(session_id, session);
    if (0 != ret)
    {
        LOG(ERROR)("add session to selector failed.");
    }
    else
    {
        LOG(DEBUG)("add session to selector ok.");
        sessions_[session_id] = session;
    }

    return ret;
}


int SessionGroup::internal_remove_session(uint32_t session_id)
{
    SessionIter session_iter = sessions_.find(session_id);
    if (session_iter != sessions_.end())
    {
        sessions_.erase(session_iter);
        session_selector_->RemoveElement(session_id);
    }
    else
    {
        LOG(WARN)("remove session not exist with id:%u", session_id);
    }
    return 0;
}



