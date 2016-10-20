#include "udp_cmd_session.h"
#include "server_config.h"
#include <algorithm>
#include <sstream>
#include "client_session.h"

using namespace std;

typedef void (*VOIDFUN)();
typedef map<string, VOIDFUN> FUNMAP;
FUNMAP m_cmd;

UdpCmdSession *UdpCmdSession::g_udpcmd = NULL;

#define UDP_CMD_SESSION UdpCmdSession::g_udpcmd


UdpCmdSession::UdpCmdSession(void)
{
    LOG(INFO)("create udp cmd session.");
    initCmd();
    UDP_CMD_SESSION = this;
}

UdpCmdSession::~UdpCmdSession(void)
{
    LOG(INFO)("remove udp cmd session.");
    m_cmd.clear();
    UDP_CMD_SESSION = NULL;
}

int UdpCmdSession::send_udp_data(const char *data, int length)
{
    int ret = 0;
    ret = send_msg(const_cast<char *>(data), length);
    return ret;
}

int UdpCmdSession::open( void *arg, const INET_Addr &remote_addr )
{
	(void)arg;
	(void)remote_addr;
	LOG(INFO)("create udpcmd service success.");
	return 0;
}

int UdpCmdSession::handle_close( uint32_t handle )
{
    LOG(INFO)("UdpCmdSession::handle_close, ip:%s, handle:%u."
              , FromAddrTostring(remote_addr()).c_str(), handle);
    return 0;
}

void UdpCmdSession::handle_timeout( int id,void *userData )
{
    (void)id;
    (void)userData;
    LOG(INFO)("UdpCmdSession timeout. udpcmd session will be closed.");
}




bool blankadjacent(int i, int j)
{

	if( i==j && ' ' == i ) return true;
	return false;  

}

void trim(string& line)
{
    LOG(DEBUG)("[udp] trim str:%s", line.c_str());
	string sblank = " ";
	string snul = "";
	basic_string<char>::iterator ite = line.begin();
	basic_string<char>::iterator iter = line.begin();

	while( ite != line.end()  ){
		if( (ite = find(line.begin(),line.end(),'\t')) != line.end() ){
			iter = ite + 1 ;
			line.replace(ite,iter,sblank);       
			ite = line.begin();
		}else if( (ite = find(line.begin(),line.end(),'\r')) != line.end() ){ //去掉'\r'(回车),兼容windows txt格式
			iter = ite + 1 ;
			line.replace(ite,iter,snul);       
			ite = line.begin();
		}
		else if( (ite = find(line.begin(),line.end(),'\n')) != line.end() ){ //去掉'\r'(回车),兼容windows txt格式
			iter = ite + 1 ;
			line.replace(ite,iter,snul);       
			ite = line.begin();
		}
	}
}

string UdpCmdSession::parsecmd(string &strUdp)
{
	string cmd = "";
    LOG(DEBUG)("[udp] parsecmd:%s", strUdp.c_str());
	for(size_t i = 0; i < strUdp.length(); ++i)
	{
		cmd += toupper(strUdp.at(i));
	}

	trim(cmd);
    LOG(DEBUG)("[udp] done trim");
	cmd.resize(unique(cmd.begin(),cmd.end(), blankadjacent) - cmd.begin());
	if(cmd.length()<=0 ) return "";
	if( ' ' == cmd.at(0)) cmd = cmd.substr(1,cmd.length()-1);			
	return cmd;		
}	


int UdpCmdSession::on_receive_message( char *ptr, int len )
{
    string inpkg(ptr, len);
    LOG(DEBUG)("[udp] receive cmd");
	LOG_HEX(inpkg.c_str(), inpkg.length(), utils::L_DEBUG);
	
	try
	{
    	string vcmd = parsecmd(inpkg);
        if (vcmd.empty())
        {
            LOG(DEBUG)("udp receive null cmd. do nothing.");
            return 0;
        }
        if (0 != call_cmd(vcmd))
        {
    		vcmd = "CAN NOT SUPPORT CMD :[" + vcmd + "]\r\n";
    		LOG(ERROR)("[%s]", vcmd.c_str());
        }
        else
        {
            vcmd = "EXECUTE [" + vcmd + "] OK\r\n";
    		LOG(DEBUG)("%s", vcmd.c_str());
    	}
        send_udp_data(vcmd.c_str(), vcmd.length());
	}
	catch (...)
	{
		LOG(WARN)("load config failed, Unknown Error");
	}

	return 0;
}


void stop()
{
    LOG(INFO)("[ RECV CMD stop cmd,  mobiled must stop........]");
    sleep(1);
    exit(0);
}

void logdebug()
{
    LOG_SET_LEVEL(utils::L_DEBUG);
    LOG(INFO)("[ LOG_SET_LEVEL(DEBUG);]");
}

void loginfo()
{
    LOG_SET_LEVEL(utils::L_INFO);
    LOG(INFO)("[ LOG_SET_LEVEL(INFO);]");
}

void logwarn()
{
    LOG_SET_LEVEL(utils::L_WARN);
    LOG(INFO)("[ LOG_SET_LEVEL(WARN);]");
}

void logerror()
{
    LOG_SET_LEVEL(utils::L_ERROR);
    LOG(INFO)("[ LOG_SET_LEVEL(ERROR);]");
}

void help()
{
    stringstream shelp;
    shelp << "help mobiled cmd:" << endl;
    FUNMAP::const_iterator cmd_iter = m_cmd.begin();
    for (; cmd_iter != m_cmd.end(); ++cmd_iter)
    {
        shelp << cmd_iter->first.c_str() << endl;
    }
    UDP_CMD_SESSION->send_udp_data(shelp.str().c_str(),shelp.str().length());
}

void reload_cfg()
{
    LOG(INFO)("reload config.");
    utils::ServerConfig::Instance()->reload_config();
    //LOG(INFO)(sret.str().c_str());
    //UdpCmdSession::g_udpcmd->send_udp_data(sret.str().c_str(),sret.str().length());
}

void reload_clientpolicy_cfg()
{
    LOG(INFO)("reload client policy config.");
    //utils::ServerConfig::Instance()->reload_client_policy_cfg();
}

void reload_err_msg()
{
    LOG(INFO)("reload err msg.");
    utils::ServerConfig::Instance()->load_err_msg();
}

void reload_token_key()
{
    LOG(INFO)("reload token key.");
    //utils::ServerConfig::Instance()->load_token_key();
}

void usernum()
{
/*
    stringstream sret;
	sret << " curent user-num: " << cur_user_num << endl;
    LOG(INFO)(sret.str().c_str());
    UDP_CMD_SESSION->send_udp_data(sret.str().c_str(),sret.str().length());
    */
}

void userlist()
{
/*
    stringstream sret;
	sret << "curent user-list" << endl;
    list_lru_hash<ClientSession> &session_list = MobileApp::Instance()->client_session_list();
    list_head* pPos;
    list_head* pHead = session_list.begin();
    for( pPos = pHead->next; pPos != pHead; )
    {
        list_head *pCurPos = pPos;
        pPos = pPos->next;

        ClientSession* pHeadSession = list_entry(pCurPos, ClientSession, item_);
        if(NULL != pHeadSession)
        {
        sret << " cid:" << pHeadSession->cid() 
            << ", uid:" << pHeadSession->uid() 
            << ", status:" << (uint32_t)pHeadSession->status()
            << ", login_seq:" << pHeadSession->login_seq() 
            //<< ", corp_uc:" << pHeadSession->corp_uc()
            //<< ", regular_auth:" << pHeadSession->regular_authority()
            //<< ", im_auth:" << pHeadSession->im_authority()
            << " ,alive_time:" <<  pHeadSession->alive_time()
            << endl;
        }
    }
    sret << endl;
    LOG(INFO)(sret.str().c_str());
    UDP_CMD_SESSION->send_udp_data(sret.str().c_str(),sret.str().length());
    */
}

void reload_update()
{
    utils::ServerConfig::Instance()->load_update_info();
    LOG(INFO)("[RELOAD_UPDATE(INFO);]");
}



void reload_iplist()
{
   //utils::ServerConfig::Instance()->Reload(g_cfg.c_str());
   string sret = " Reload allow ip list . not implements";
   LOG(INFO)(sret.c_str());
   UDP_CMD_SESSION->send_udp_data(sret.c_str(),sret.length());
}


void close_force_update()
{
/*
    stringstream sret;
    sret << endl;
	sret << "close_force_update:" << endl;
    force_strategy = 0;
    sret << force_strategy << endl;
    LOG(INFO)("%s", sret.str().c_str());
    UDP_CMD_SESSION->send_udp_data(sret.str().c_str(),sret.str().length());
    */
}

void reload_maintenance()
{
    utils::ServerConfig::Instance()->load_maintenance();
    LOG(INFO)("[RELOAD_MAINTENANCE(INFO);]");
}

void UdpCmdSession::initCmd()
{
    LOG(INFO)("UDP SESSION INIT CMD");

    //m_cmd.insert(FUNMAP::value_type("GET_CID", getCid));
    m_cmd.insert(FUNMAP::value_type("STOP MOBILED",stop));
    m_cmd.insert(FUNMAP::value_type("LOG DEBUG",logdebug));
    m_cmd.insert(FUNMAP::value_type("LOG INFO",loginfo));
    m_cmd.insert(FUNMAP::value_type("LOG WARN",logwarn));
    m_cmd.insert(FUNMAP::value_type("LOG ERROR",logerror));
    
    m_cmd.insert(FUNMAP::value_type("USER NUM",usernum));
    m_cmd.insert(FUNMAP::value_type("USER LIST",userlist));
    
    m_cmd.insert(FUNMAP::value_type("CLOSE FORCE UPDATE",close_force_update));
    
    m_cmd.insert(FUNMAP::value_type("RELOAD_IP_LIST", reload_iplist));
    m_cmd.insert(FUNMAP::value_type("RELOAD_UPDATE",reload_update));
    m_cmd.insert(FUNMAP::value_type("RELOAD_MAINTENANCE",reload_maintenance));
	m_cmd.insert(FUNMAP::value_type("RELOAD_CFG",reload_cfg));
    m_cmd.insert(FUNMAP::value_type("RELOAD_CLIENTPOLICY",reload_clientpolicy_cfg));
	m_cmd.insert(FUNMAP::value_type("RELOAD_ERR_MSG",reload_err_msg));
    m_cmd.insert(FUNMAP::value_type("RELOAD_TOKEN_KEY",reload_token_key));
    
    m_cmd.insert(FUNMAP::value_type("HELP",help));

}

/*
	m_cmd.insert(FUNMAP::value_type("RELOAD_CACHE_STATUS_NUM", ReloadCacheStatusNum));
	m_cmd.insert(FUNMAP::value_type("SWITCH_FAILURE_MODE", SwitchFailureMode));
	m_cmd.insert(FUNMAP::value_type("RELOAD_LOGIN_MAX_NUM", ReloadLoginMaxNum));
	m_cmd.insert(FUNMAP::value_type("DISABLE_QGROUP_NGROUP", DisableQgroupNgroup));
	m_cmd.insert(FUNMAP::value_type("RELOAD_LOGIN_TIME", ReloadLoginTime));
	m_cmd.insert(FUNMAP::value_type("RELOAD_WHITE_LIST_LOGIN_MAX_NUM", ReloadWhiteListLoginMaxNum));
	m_cmd.insert(FUNMAP::value_type("RELOAD_WHITE_LIST_USER", ReloadWhiteListUser));
	m_cmd.insert(FUNMAP::value_type("RELOAD_TTL", ReloadClientTtl));
	m_cmd.insert(FUNMAP::value_type("RELOAD_DBPROXY",ReloadDbProxyIp));
	m_cmd.insert(FUNMAP::value_type("RELOAD_NGROUPD",ReloadNgroupdId));
	m_cmd.insert(FUNMAP::value_type("RELOAD_QGROUPD",ReloadQgroupdId));
	m_cmd.insert(FUNMAP::value_type("RELOAD_MSGCENTER",ReloadMsgcenter));
	m_cmd.insert(FUNMAP::value_type("RELOAD_NP1SYNC",ReloadNp1SyncSwitch));
	m_cmd.insert(FUNMAP::value_type("SHOW_NP1_INFO",ShowNp1Info));
	m_cmd.insert(FUNMAP::value_type("RELOAD_IDCOUNTER",ReloadIdCounter));
*/

int UdpCmdSession::call_cmd(const string &cmd)
{
    int ret = 0;
    if(m_cmd.end() != m_cmd.find(cmd))
    {
        m_cmd[cmd]();
    }
    else
    {
        ret = -1;
    }
    return ret;
}



#if 0
void getCid()
{
    if(!IS_MAIN_POS(g_imd_pos))
    {
    	return;
    }
	
    ofstream corpinfo("/usr/local/imo_server/monitor/corps.info");
    ofstream uidinfo("/usr/local/imo_server/monitor/uid.info");
    ofstream cidinfo("/usr/local/imo_server/monitor/cid.info");
    if (!corpinfo)
    {
        LOG(DEBUG)("Cannot open  corpinfo file");
        bcorp_flag = false;
    }
    if (!uidinfo)
    {
        LOG(DEBUG)(" Cannot open uid info:");
        buid_flag = false;
    }
    if (!cidinfo)
    {
        LOG(DEBUG)(" Cannot open cid info: ");
        bcid_flag = false;
    }
    // todo:
    LOG(DEBUG)(" curent user num:%u, corp num:%u", 1, 1);

    if (bcorp_flag)
    {
        corpinfo("CORP_NUM:%u, USER_NUM:%u", Corp::CORP_NUM[g_imd_pos - 1], User::USER_NUM[g_imd_pos - 1]);
    }

    if (buid_flag)
    {
        uidinfo("CORP_NUM:%u, USER_NUM:%u", Corp::CORP_NUM[g_imd_pos - 1], User::USER_NUM[g_imd_pos - 1]);
    }

    list_head * pos;
    list_head * head = &CorpHead[g_imd_pos - 1];
    list_for_each(pos, head)
    {
        Corp * p = list_entry(pos, Corp, listItem);
        LOG(DEBUG)(" Corp cid: " <<  p->key);
        {
            list_head * ppos;
            list_head * phead = &(p->linkItem);
            if (buid_flag)
            {
                uidinfo("Cid: " << p->key
                    (" Online uid as follows:");
            }

            if (bcid_flag)
            {
                cidinfo << p->key);
            }

            list_for_each(ppos, phead)
            {
                User * pp = list_entry(ppos, User, linkItem);
                user_count = user_count + 1;
                LOG(DEBUG)(" Current cid online UID: "
                                <<  pp->key("user num:"
                                << user_count);
                if (buid_flag)
                {
                    for(int i = 0 ; i <  DBCMD::DEV_TYPE_MAX; i++)
                    {
                        int push_online_flag = 0;
                        if(pp->userInfo[i].status != -1){
                            if(pp->userInfo[i].isPushOnline){
                                push_online_flag = 1;
                            }else{
                                push_online_flag = 0;
                            }
                            uidinfo << pp->key << '_' << i << '_' << push_online_flag("_" << p->key);
                        }
                    }
                    /*
                    if(pp->isOnlyMobilePushOnline()){
                        continue;
                    }else{
                        uidinfo << pp->key);
                    }
                    */
                }
            }

            if (bcorp_flag)
            {
                corpinfo("Cid:  "  << p->key("  onlineNum:  "
                     << user_count);
            }

            if (buid_flag)
            {
                uidinfo("Cid:  " << p->key("  onlineNum:  "
                    << user_count);
            }

            LOG(DEBUG)("Corp cid:" << p->key("Total user num:"
                            << user_count);
            user_count = 0;
        }
    }
	
    corpinfo.close();
    uidinfo.close();
    cidinfo.close();
}

void ReloadDbProxyIp(void)
{
	if (g_old_dbsession_array)
	{
		LOG(WARN)("old session don't destory,old_session="<<g_old_dbsession_array);
		for (size_t i=0;i<g_old_dbproxy_info.size();i++)
		{
			LOG(WARN)("old ip="<<g_old_dbproxy_info[i].ip(",port="<<g_old_dbproxy_info[i].port(",id="<<g_old_dbproxy_info[i].id);
		}
		return;
	}
	g_last_update_time = time(NULL);
	DBSession::init();
	LOG(INFO)("reload db proxy ip";
}


void ReloadNgroupdId()
{
	NgroupSession::init();
	LOG(INFO)("reload ngroupd id");
}


void ReloadQgroupdId()
{
	QgroupSession::init();
	LOG(INFO)("reload qgroupd id");
}


void ReloadMsgcenter()
{
	if (UserCenterSession::init() == -1)
	{
		LOG(ERROR)("reload qgroupd id failed");
	}
	LOG(INFO)("reload qgroupd id ok");
}


void ReloadIdCounter()
{
	LOG(INFO)("start reload idcounter");
	int ret = IdCounterSession::reload();
	LOG(INFO)("end reload idcounter,ret="<<ret);
}

void ReloadClientTtl()
{
	LOG(INFO)("reload client ttl");
	TiXmlDocument *myDocument = new TiXmlDocument(g_cfg.c_str());
	if(!myDocument->LoadFile())
	{
		LOG(ERROR)("reload file="<<g_cfg(" error");
		return;
	}
	TiXmlElement *RootElement = myDocument->RootElement();
	if (!RootElement)
	{
		LOG(ERROR)("open RootElement file="<<g_cfg(" error");
		return;
	}
	
	TiXmlElement *tcpElement  =  RootElement->FirstChildElement("tcp");
	if (!tcpElement)
	{
		LOG(ERROR)("open tcpElement file="<<g_cfg(" error");
		return;
	}
	
		
	TiXmlElement *cttlElement = tcpElement->FirstChildElement("cttl");
	if (!cttlElement)
	{
		LOG(ERROR)("open cttlElement file="<<g_cfg(" error");
		return;
	}
	
	string cttl = cttlElement->GetText();
	g_client_ttl = atoi(cttl.c_str());
	LOG(INFO)("reload client ttl: "<<g_client_ttl);

	TiXmlElement *musttlElement = tcpElement->FirstChildElement("musttl");
	if (!musttlElement)
	{
		LOG(ERROR)("open musttlElement file="<<g_cfg(" error");
		return;
	}
	
	string musttl = musttlElement->GetText();
	g_mobusersess_ttl = atoi(musttl.c_str());
	LOG(INFO)("reload mob user session ttl: " << g_mobusersess_ttl);
		
	TiXmlElement* pMaxContextElement = tcpElement->FirstChildElement("maxContextNums");
	if (pMaxContextElement)
	{
		string maxContextElement = pMaxContextElement->GetText();
		ContextManager::Instantce()->SetMaxContextNums(atoi(maxContextElement.c_str()));
		LOG(INFO)("reload set maxContextElement:" <<maxContextElement<< endl;
	}
	
	
	TiXmlElement* pdisablesendstatusElement = tcpElement->FirstChildElement("disablesendstatus");
	if (pdisablesendstatusElement)
	{
		string disablesendstatus = pdisablesendstatusElement->GetText();
		g_disable_status = atoi(disablesendstatus.c_str());
	}
	LOG(INFO)("set g_disable_status:" << g_disable_status);
}




void ReloadCacheStatusNum()
{
	string strLog;
	bool bRet = CServerConfig::GetInstance()->ReloadCacheStatusNum(g_cfg.c_str());
	strLog = bRet ? " Reload CacheStatusNum Succeed.\n" : " Reload CacheStatusNum Failed.\n";
	//g_udpcmd->send_udp_data(strLog.c_str(), strLog.length());
	LOG(INFO) << strLog.c_str());

	if (bRet)
	{
		uint32_t unTotallNum = StatusChangeCache::getInstance()->CheckALLUserStatusNum();

		//stringstream sret;
		//sret(" delete cache status completed, num: "<< unTotallNum("\n";
		//g_udpcmd->send_udp_data(sret.str().c_str(),sret.str().length());
	}
}

void SwitchFailureMode()
{
	LOG(INFO)("switch failure mode");
	TiXmlDocument *myDocument = new TiXmlDocument(g_cfg.c_str());
	if(!myDocument->LoadFile())
	{
		LOG(ERROR)("reload file="<<g_cfg(" error");
		return;
	}
	TiXmlElement *RootElement = myDocument->RootElement();
	if (!RootElement)
	{
		LOG(ERROR)("open RootElement file="<<g_cfg(" error");
		return;
	}

	TiXmlElement *tcpElement  =  RootElement->FirstChildElement("tcp");
	if (!tcpElement)
	{
		LOG(ERROR)("open tcpElement file="<<g_cfg(" error");
		return;
	}

	bool fault_mode = g_fault_mode;
	TiXmlElement* pFaultModeElement = tcpElement->FirstChildElement("fault_mode");
	if (pFaultModeElement)
	{
		string sFaultModeNum = pFaultModeElement->GetText();
		g_fault_mode = (sFaultModeNum == "true")?true:false;
	}
	else
	{
		LOG(INFO)("switch failure mode not config");
	}

	
	LOG(INFO)("switch failure mode complete("<<(fault_mode?"true":"false")("===="<<(g_fault_mode?"true":"false")(")."<< endl;
}


void ReloadLoginMaxNum()
{
	LOG(INFO)("reload login max num");
	TiXmlDocument *myDocument = new TiXmlDocument(g_cfg.c_str());
	if(!myDocument->LoadFile())
	{
		LOG(ERROR)("reload file="<<g_cfg(" error");
		return;
	}
	TiXmlElement *RootElement = myDocument->RootElement();
	if (!RootElement)
	{
		LOG(ERROR)("open RootElement file="<<g_cfg(" error");
		return;
	}

	TiXmlElement *tcpElement  =  RootElement->FirstChildElement("tcp");
	if (!tcpElement)
	{
		LOG(ERROR)("open tcpElement file="<<g_cfg(" error");
		return;
	}

	uint32_t login_max_num = g_logining_max_num;
	TiXmlElement* pLoginMaxNumElement = tcpElement->FirstChildElement("login_max_num");
	if (pLoginMaxNumElement)
	{
		string sLoginMaxNum = pLoginMaxNumElement->GetText();
		g_logining_max_num = atoi(sLoginMaxNum.c_str());
	}
	else
	{
		LOG(INFO)("reload failure not config");
	}


	LOG(INFO)("reload complete,"<<login_max_num("===>"<<g_logining_max_num<< endl;
}

void ReloadWhiteListLoginMaxNum()
{
	LOG(INFO)("reload white list login max num");
	TiXmlDocument *myDocument = new TiXmlDocument(g_cfg.c_str());
	if(!myDocument->LoadFile())
	{
		LOG(ERROR)("reload file="<<g_cfg(" error");
		return;
	}
	TiXmlElement *RootElement = myDocument->RootElement();
	if (!RootElement)
	{
		LOG(ERROR)("open RootElement file="<<g_cfg(" error");
		return;
	}

	TiXmlElement *tcpElement  =  RootElement->FirstChildElement("tcp");
	if (!tcpElement)
	{
		LOG(ERROR)("open tcpElement file="<<g_cfg(" error");
		return;
	}

	uint32_t logining_white_list_max_num = g_logining_white_list_max_num;
	TiXmlElement* pLoginMaxNumElement = tcpElement->FirstChildElement("white_list_login_max_num");
	if (pLoginMaxNumElement)
	{
		string sLoginMaxNum = pLoginMaxNumElement->GetText();
		g_logining_white_list_max_num = atoi(sLoginMaxNum.c_str());
	}
	else
	{
		LOG(INFO)("reload failure not config");
	}


	LOG(INFO)("reload complete,"<<logining_white_list_max_num("===>"<<g_logining_white_list_max_num<< endl;
}

void ReloadWhiteListUser()
{
	LOG(INFO)("reload white list user");
	bool bRet = CServerConfig::GetInstance()->ReoadWhiteList(g_cfg.c_str());
	LOG(INFO)("reload complete, status:"<<(bRet?"succeed":"failed")<< endl;
}

void ReloadLoginTime()
{
	LOG(INFO)("reload login time");
	TiXmlDocument *myDocument = new TiXmlDocument(g_cfg.c_str());
	if(!myDocument->LoadFile())
	{
		LOG(ERROR)("reload file="<<g_cfg(" error");
		return;
	}
	TiXmlElement *RootElement = myDocument->RootElement();
	if (!RootElement)
	{
		LOG(ERROR)("open RootElement file="<<g_cfg(" error");
		return;
	}

	TiXmlElement *tcpElement  =  RootElement->FirstChildElement("tcp");
	if (!tcpElement)
	{
		LOG(ERROR)("open tcpElement file="<<g_cfg(" error");
		return;
	}

	uint32_t user_login_time = g_user_login_time;
	TiXmlElement* pLoginTimeElement = tcpElement->FirstChildElement("login_time");
	if (pLoginTimeElement)
	{
		string sLoginTime = pLoginTimeElement->GetText();
		g_user_login_time = atoi(sLoginTime.c_str());
	}
	else
	{
		LOG(INFO)("reload failure not config");
	}


	LOG(INFO)("reload complete,"<<user_login_time("===>"<<g_user_login_time<< endl;
}


void DisableQgroupNgroup()
{
	LOG(INFO)("disable qgroup ngroup");
	TiXmlDocument *myDocument = new TiXmlDocument(g_cfg.c_str());
	if(!myDocument->LoadFile())
	{
		LOG(ERROR)("reload file="<<g_cfg(" error");
		return;
	}
	TiXmlElement *RootElement = myDocument->RootElement();
	if (!RootElement)
	{
		LOG(ERROR)("open RootElement file="<<g_cfg(" error");
		return;
	}

	TiXmlElement *tcpElement  =  RootElement->FirstChildElement("tcp");
	if (!tcpElement)
	{
		LOG(ERROR)("open tcpElement file="<<g_cfg(" error");
		return;
	}

	uint32_t disable_qgroup_ngroup = g_disable_qgroup_ngroup;

	TiXmlElement* pDisableElement = tcpElement->FirstChildElement("disable_qgroup_ngroup");
	if (pDisableElement)
	{
		string disable = pDisableElement->GetText();
		g_disable_qgroup_ngroup = (disable == "true")?true:false;
	}

	LOG(INFO)("disable qgroup ngroup complete,"<<(disable_qgroup_ngroup?"true":"false")("===>"<<(g_disable_qgroup_ngroup?"true":"false")<< endl;
}

void ReloadNp1SyncSwitch()
{
	LOG(INFO)("reload Np1SyncSwitch");
	TiXmlDocument *myDocument = new TiXmlDocument(g_cfg.c_str());
	myDocument->LoadFile();
	TiXmlElement *RootElement = myDocument->RootElement();
	TiXmlElement *CCSElement = RootElement->FirstChildElement("CCS");
	TiXmlElement * childElement = CCSElement->FirstChildElement("IsSync");
	string sSync = childElement->GetText();
	uint32_t unSync = atoi(sSync.c_str());
	g_bIsSync2Back = (bool)unSync;
	LOG(INFO)("reload Np1SyncSwitch end, value:"<< g_bIsSync2Back );
}

void ShowNp1Info()
{
	LOG(INFO)("start ShowNp1Info");
	LOG(INFO)("ImdPos:%u" << g_imd_pos(", ImdId:" << g_imd_id );
    	map<uint32_t, uint32_t>::iterator itor = g_imd_rounter.begin();
       for(; itor != g_imd_rounter.end(); itor++)
       {
       		LOG(INFO)("Router key: "<< (*itor).first(" val:"<<(*itor).second<< endl;
       }
}

#endif




