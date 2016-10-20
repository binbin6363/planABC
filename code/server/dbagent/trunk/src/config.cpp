#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "config.h"
#include "log.h"

//
// ServerConfig class
//
ServerConfig* ServerConfig::m_pConfig = NULL;

ServerConfig::ServerConfig()
{
	
}

ServerConfig::~ServerConfig()
{
	m_setCondList.clear();
}

bool ServerConfig::Load(const char* pFile)
{
	TiXmlDocument Config;
	Config.LoadFile(pFile);

	TiXmlElement* pRootElement = Config.RootElement();
	if (!pRootElement)
	{
		return false;
	}

	LoadCondAllowList(pRootElement);
	
	LoadImdAllowList(pRootElement);
	
	LoadGroupdAllowList(pRootElement);

	LoadQGroupdAllowList(pRootElement);

	LoadWebagentdAllowList(pRootElement);

	return true;
}

bool ServerConfig::Reload(const char* pFile)
{
	TiXmlDocument Config;
	Config.LoadFile(pFile);

	TiXmlElement* pRootElement = Config.RootElement();
	if (!pRootElement)
	{
		return false;
	}

	LoadCondAllowList(pRootElement);

	LoadImdAllowList(pRootElement);

	LoadGroupdAllowList(pRootElement);

	LoadQGroupdAllowList(pRootElement);

	LoadWebagentdAllowList(pRootElement);

	return true;
}

bool ServerConfig::ReloadCacheStatusNum(const char* pFile)
{
	TiXmlDocument Config;
	Config.LoadFile(pFile);

	TiXmlElement* pRootElement = Config.RootElement();
	if (!pRootElement)
	{
		return false;
	}

	TiXmlElement *tcpElement  =  pRootElement->FirstChildElement("tcp");
	if (!tcpElement)
	{
		return false;
	}

	TiXmlElement* pImdNumElement = tcpElement->FirstChildElement("user_cache_status_num");
	string sStatusChageNum = pImdNumElement->GetText();
	g_user_cache_status_num = atoi(sStatusChageNum.c_str());
	LOG(INFO)("reload user_cache_status_num:%u", g_user_cache_status_num);

	return true;
}

bool ServerConfig::IsCondAllow(uint32_t uCondIp)
{
	if (m_setCondList.find(uCondIp) != m_setCondList.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ServerConfig::IsImdAllow(uint32_t uImdIp)
{
	if (m_setImdList.find(uImdIp) != m_setImdList.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ServerConfig::IsGroupdAllow(uint32_t uGroupdIp)
{
	if (m_setGroupdList.find(uGroupdIp) != m_setGroupdList.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ServerConfig::IsQGroupdAllow(uint32_t uQGroupdIp)
{
	if (m_setQGroupdList.find(uQGroupdIp) != m_setQGroupdList.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ServerConfig::IsWebagentdAllow(uint32_t uWebagentdIp)
{
	if (m_setWebagentdList.find(uWebagentdIp) != m_setWebagentdList.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool ServerConfig::LoadCondAllowList(TiXmlElement* pRoot)
{
	TiXmlElement* pAllowList = pRoot->FirstChildElement("CondAllowList");
	if (!pAllowList)
	{
		return false;
	}

	m_setCondList.clear();

	TiXmlElement* pAllow = pAllowList->FirstChildElement("ip");

	uint32_t uAllowIp = 0;
	while (pAllow)
	{
		inet_pton(AF_INET, pAllow->GetText(), &uAllowIp);
		m_setCondList.insert(uAllowIp);

		pAllow = pAllow->NextSiblingElement();
	}

    TiXmlElement* udpLogServer = pRoot->FirstChildElement("udplogserver");
    if(!udpLogServer)
        return false;

    string ip,port;
    TiXmlElement* ipElement = udpLogServer->FirstChildElement("ip");
    if(ipElement != NULL)
    {
        ip = ipElement->GetText();
    }
    TiXmlElement* portElement = udpLogServer->FirstChildElement("port");
    if(portElement != NULL)
    {
        port = portElement->GetText();
    }

    if(g_udplogcmd != NULL)
    {
        delete g_udplogcmd;
        g_udplogcmd =  NULL;
    }
    g_udplogcmd = new SendUdpLog(ip,atoi(port.c_str()));
    if(g_udplogcmd != NULL)
        LOG(DEBUG)("Reload udp log server cfg, ip:%u, port:%u is ok!!", ip, port);
    else
        LOG(DEBUG)("Reload udp log server cfg is failed!!");

	return true;
}

bool ServerConfig::LoadImdAllowList(TiXmlElement* pRoot)
{
	TiXmlElement* pAllowList = pRoot->FirstChildElement("ImdAllowList");
	if (!pAllowList)
	{
		return false;
	}

	m_setImdList.clear();

	TiXmlElement* pAllow = pAllowList->FirstChildElement("ip");

	uint32_t uAllowIp = 0;
	while (pAllow)
	{
		inet_pton(AF_INET, pAllow->GetText(), &uAllowIp);
		m_setImdList.insert(uAllowIp);

		pAllow = pAllow->NextSiblingElement();
	}

	return true;
}

bool ServerConfig::LoadGroupdAllowList(TiXmlElement* pRoot)
{
	TiXmlElement* pAllowList = pRoot->FirstChildElement("GroupdAllowList");
	if (!pAllowList)
	{
		return false;
	}

	m_setGroupdList.clear();

	TiXmlElement* pAllow = pAllowList->FirstChildElement("ip");

	uint32_t uAllowIp = 0;
	while (pAllow)
	{
		inet_pton(AF_INET, pAllow->GetText(), &uAllowIp);
		m_setGroupdList.insert(uAllowIp);

		pAllow = pAllow->NextSiblingElement();
	}

	return true;
}

bool ServerConfig::LoadQGroupdAllowList(TiXmlElement* pRoot)
{
	TiXmlElement* pAllowList = pRoot->FirstChildElement("QGroupdAllowList");
	if (!pAllowList)
	{
		return false;
	}

	m_setQGroupdList.clear();

	TiXmlElement* pAllow = pAllowList->FirstChildElement("ip");

	uint32_t uAllowIp = 0;
	while (pAllow)
	{
		inet_pton(AF_INET, pAllow->GetText(), &uAllowIp);
		m_setQGroupdList.insert(uAllowIp);

		pAllow = pAllow->NextSiblingElement();
	}

	return true;
}

bool ServerConfig::LoadWebagentdAllowList(TiXmlElement* pRoot)
{
	TiXmlElement* pAllowList = pRoot->FirstChildElement("WebagentdAllowList");
	if (!pAllowList)
	{
		return false;
	}

	m_setWebagentdList.clear();

	TiXmlElement* pAllow = pAllowList->FirstChildElement("ip");

	uint32_t uAllowIp = 0;
	while (pAllow)
	{
		inet_pton(AF_INET, pAllow->GetText(), &uAllowIp);
		m_setWebagentdList.insert(uAllowIp);

		pAllow = pAllow->NextSiblingElement();
	}

	return true;
}

bool ServerConfig::ReoadWhiteList(const char *szFile)
{
	TiXmlDocument Config;
	Config.LoadFile(szFile);

	TiXmlElement* pRootElement = Config.RootElement();
	if (!pRootElement)
	{
		LOG(WARN)("reload error, get root failed.");
		return false;
	}

	TiXmlElement *whiteListElement  =  pRootElement->FirstChildElement("whitelist");
	if (!whiteListElement)
	{
		LOG(WARN)("reload error, get whitelist failed.");
		return false;
	}

	TiXmlElement* pFileElement = whiteListElement->FirstChildElement("file");
	if (!pFileElement)
	{
		LOG(WARN)("reload error, get file failed.");
		return false;
	}

	string sWhiteListFile = pFileElement->GetText();
	LOG(INFO)("white list file:%s", sWhiteListFile.c_str());

	TiXmlDocument whitelist;
	if (!whitelist.LoadFile(sWhiteListFile.c_str()))
	{
		LOG(WARN)("load whitelist file failed, error:%u, file:%s", whitelist.ErrorDesc(), sWhiteListFile.c_str());
		return false;
	}

	TiXmlElement *pWhiteListRootElement = whitelist.RootElement();
	if (!pWhiteListRootElement)
	{
		LOG(WARN)("reload white list error, get root failed.");
		return false;
	}

	set_white_list.clear();

	TiXmlElement* pCorpCid = pWhiteListRootElement->FirstChildElement("corpcid");
	while(pCorpCid != NULL)
	{
		string strCid = pCorpCid->GetText();
		set_white_list.insert(atoi(strCid.c_str()));
		pCorpCid = pCorpCid->NextSiblingElement("corpcid");
		LOG(INFO)("corp cid:%u", strCid.c_str());
	}
	
	LOG(INFO)("reload white list succeed, white list num:%u", set_white_list.size());

	return true;
}

bool ServerConfig::isWhiteListUser( uint32_t cid)
{
	return (set_white_list.find(cid) != set_white_list.end());
}

