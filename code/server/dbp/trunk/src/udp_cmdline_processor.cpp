/**
* file: udp_cmdline_processor.h
* desc: ban liao, udp_cmdline_processor file. process cmd
* auth: bbwang
* date: 2015/7/8
*/

#include "udp_cmdline_processor.h"
#include "cmd.h"
#include "client_session.h"
#include "constants.h"
#include "server_app.h"
#include "log.h"
#include "binpacket_wrap.h"
#include "xml_parser.h"
#include "libco_wrap.h"
#include <algorithm>

using namespace common;
using namespace utils;
using namespace std;


typedef void (*VOIDFUN)(const Param &);
typedef map<string, VOIDFUN> FUNMAP;

typedef std::map<std::string, std::string> strmap;
typedef strmap::iterator striter;

strmap cmd_args;
FUNMAP m_cmd;

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


UdpCmdLineProcessor *UdpCmdLineProcessor::Instance()
{
    static UdpCmdLineProcessor inst;
    return &inst;
}


UdpCmdLineProcessor::UdpCmdLineProcessor()
{
    initCmd();
}

UdpCmdLineProcessor::~UdpCmdLineProcessor()
{
}


// handle request event from udp client
int UdpCmdLineProcessor::ProcessData(const Param &param, Msg *msg) const
{
    int ret = 0;
    LOG(INFO)("UdpCmdLineProcessor received data. [name:%s, remote:%s, netid:%u]"
        , param.service_name, FromAddrTostring(param.remote_addr).c_str(), param.net_id);

    // 保存对端的ip地址!!!
    
    CmdLineMsg *cmdline_msg = dynamic_cast<CmdLineMsg *>(msg);
    if (NULL == cmdline_msg)
    {
        LOG(ERROR)("UdpCmdLineProcessor, cast msg failed.");
        return 0;
    }
    string &cmd_str = cmdline_msg->get_requset_str();

	cmd_str = parsecmd(cmd_str);
    if (cmd_str.empty())
    {
        LOG(DEBUG)("UdpCmdLineProcessor receive empty cmd. do nothing.");
        return 0;
    }
    
    if (0 != call_cmd(param, cmd_str))
    {
		cmd_str = "CAN NOT SUPPORT CMD :[" + cmd_str + "]\r\n";
		LOG(ERROR)("[%s]", cmd_str.c_str());
    }
    else
    {
        cmd_str = "EXECUTE [" + cmd_str + "] OK\r\n";
		LOG(DEBUG)("%s", cmd_str.c_str());
	}
    cmd_str += "udp> ";

    CmdLineMsg result;
    result.set_result_str(cmd_str);
    Reply(param, *cmdline_msg, result);
    
    return ret;
}

int UdpCmdLineProcessor::call_cmd(const Param &param, const string &cmd) const
{
    int ret = 0;
    if(m_cmd.end() != m_cmd.find(cmd))
    {
        m_cmd[cmd](param);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

string UdpCmdLineProcessor::parsecmd(string &strUdp) const
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

    // 有参数的需要解析出参数列表
    parse_args(cmd);
    
	return cmd;		
}	



int UdpCmdLineProcessor::parse_args(string &cmd) const
{
    int ret = 0;
    cmd_args.clear();

    string tmp_cmd = cmd;

    size_t pos = tmp_cmd.find_first_of("-");
    if (pos == string::npos)
    {
        return 0;
    }

    size_t blank_pos = tmp_cmd.find_first_of(" ");
    cmd = cmd.substr(0, blank_pos);

    size_t end_pos = pos;
    do
    {
        // find option
        pos += 1;
        end_pos = tmp_cmd.find_first_of(" ", pos);
        CHECK_WARN_BREAK((end_pos == string::npos), 1000, "next blank not exist.");
        std::string option = tmp_cmd.substr(pos, end_pos - pos);

        // find param
        pos = end_pos + 1;
        end_pos = tmp_cmd.find_first_of(" ", pos );
        if (end_pos == string::npos)
        {
            end_pos = tmp_cmd.length();
        }
        std::string param = tmp_cmd.substr(pos, end_pos - pos);

        cmd_args[option] = param;
        LOG(DEBUG)("udp cmd show option:%s, param:%s", option.c_str(), param.c_str());

        pos = tmp_cmd.find_first_of("-", end_pos );

    }while(pos != string::npos);

    return 0;
}



// handle cmd, declear
void help(const Param &param);
void logdebug(const Param &);
void loginfo(const Param &);
void logwarn(const Param &);
void logerror(const Param &);
void reload(const Param &);
void test(const Param &);

void UdpCmdLineProcessor::initCmd() const
{
    LOG(INFO)("UDP SESSION INIT CMD");

    m_cmd.insert(FUNMAP::value_type("HELP",help));
    m_cmd.insert(FUNMAP::value_type("LOG DEBUG",logdebug));
    m_cmd.insert(FUNMAP::value_type("LOG INFO",loginfo));
    m_cmd.insert(FUNMAP::value_type("LOG WARN",logwarn));
    m_cmd.insert(FUNMAP::value_type("LOG ERROR",logerror));
    m_cmd.insert(FUNMAP::value_type("RELOAD_CFG",reload));
    m_cmd.insert(FUNMAP::value_type("TEST",test));
}

void help(const Param &param)
{
    stringstream shelp;
    shelp << "USAGE:" << endl;
    FUNMAP::const_iterator cmd_iter = m_cmd.begin();
    for (; cmd_iter != m_cmd.end(); ++cmd_iter)
    {
        shelp << cmd_iter->first.c_str() << endl;
    }
    
    CmdLineMsg result;
    result.set_result_str(shelp.str());
    LOG(DEBUG)("reply client request.");
    UdpCmdLineProcessor::Instance()->Reply(param, result, result);
}

void logdebug(const Param &)
{
    LOG_SET_LEVEL(utils::L_DEBUG);
    LOG(INFO)("[ LOG_SET_LEVEL(DEBUG);]");
}

void loginfo(const Param &)
{
    LOG_SET_LEVEL(utils::L_INFO);
    LOG(INFO)("[ LOG_SET_LEVEL(INFO);]");
}

void logwarn(const Param &)
{
    LOG_SET_LEVEL(utils::L_WARN);
    LOG(INFO)("[ LOG_SET_LEVEL(WARN);]");
}

void logerror(const Param &)
{
    LOG_SET_LEVEL(utils::L_ERROR);
    LOG(INFO)("[ LOG_SET_LEVEL(ERROR);]");
}

void reload(const Param &)
{
    utils::XmlParser::Instance().ReloadConfig(config);
    // 设置co的最大数
    set_max_co_num(config.sys_max_co_num);
    LOG(INFO)("[ RELOAD CONFIG]");
}

#include "mysql_msg.h"

void test(const Param &)
{
    string str = "select * from 'asdfjhjksdfv'";
    string estr = EscapeString(str);
    LOG(DEBUG)("test EscapeString, str:%s, estr:%s", str.c_str(), estr.c_str());
}


