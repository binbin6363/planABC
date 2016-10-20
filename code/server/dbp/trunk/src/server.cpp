/**
* file: status_server.cpp
* desc: server
* auth: bbwang
* date: 2015/11/3
*/
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>
#include <sstream>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <iostream>

#include "comm.h"
#include "xml_parser.h"
#include "constants.h"
#include "linux_util.h"
#include "server_app.h"

using namespace common;

void ShowVersion();


bool IsFileExist(const std::string &file_path)
{
    if (access(file_path.c_str(), F_OK) != 0)
    {
        LOG(ERROR)("file not exist. path:%s", file_path.c_str());
        return false;
    }
    return true;
}

// =========== main ===========
int InitConfig(const std::string &cfg_path, Config& config)
{
    int ret = 0;
    ONCE_LOOP_ENTER
    if (!IsFileExist(cfg_path))
    {
        ret = -1;
        break;
    }

    utils::XmlParser &xml_parser = utils::XmlParser::Instance();
    if (!xml_parser.LoadXmlFile(cfg_path))
    {
        fprintf(stderr, "load xml file failed.");
        ret = -1;
        break;
    }

    // log
    if (!xml_parser.LoadLogInfo(config))
    {
        ret = -1;
        fprintf(stderr, "Load Log ServerInfo failed.");
        break;
    }

    // daemon
    if (!xml_parser.LoadDaemonInfo(config))
    {
        ret = -1;
        fprintf(stderr, "Load Daemon ServerInfo failed.");
        break;
    }

    // system
    if (!xml_parser.LoadSystemInfo(config))
    {
        ret = -1;
        fprintf(stderr, "Load System ServerInfo failed.");
        break;
    }

    // load udp cmdline service
    string name("");
    if (!xml_parser.LoadCommonServiceInfo(UDP_CMD_NAME, config.udp_cmd_ip, config.udp_cmd_port, config.udp_cmd_timeout, name))
    {
        ret = -1;
        fprintf(stderr, "Load udp cmdline service info failed.");
        break;
    }


    Server_Info default_info;
    default_info.service_name = "UNKNOW_SERVICE";
    default_info.heart_beat_interval = 2;
    default_info.reconnect_interval = 3;
    default_info.time_out = 10;
    default_info.conn_time_out = 3;
    // load dbagent server
    if (!xml_parser.LoadServerInfo(DBAGENT_SERVER_NAME, config.dbagent_srv_infos, config.dbagent_service_name, default_info))
    {
        ret = -1;
        fprintf(stderr, "Load dbagent ServerInfo failed.");
        break;
    }
    
    // load redis proxy server
//    if (!xml_parser.LoadServerInfo(REDIS_PROXY_SERVER_NAME, config.redis_proxy_infos, config.redis_proxy_service_name, default_info))
//    {
//        ret = -1;
//        fprintf(stderr, "Load redis proxy ServerInfo failed.");
//        break;
//    }
    ONCE_LOOP_LEAVE
    return ret;
}

bool InitLogger(const Config& config)
{

    string logFile = config.log_path;
    string logRank = config.log_rank;
    fprintf(stdout, "open log file:%s", logFile.c_str());
    LOG_INIT((const char*)logFile.c_str(),config.log_size,utils::L_TRACE);
    LOG_OPEN();
    LOG_SET_USEC(true);

    if (logRank == "DEBUG")
    {
        LOG_SET_LEVEL(utils::L_DEBUG);
    }
    else if (logRank == "INFO")
    {
        LOG_SET_LEVEL(utils::L_INFO);
    }
    else if (logRank == "ERROR")
    {
        LOG_SET_LEVEL(utils::L_ERROR);
    }
    return true;
}

void Daemon(const Config& config, char *argv[])
{
    (void)argv;
    // load daemon info
    setuid(config.dae_uid);
    if (0 == config.dae_uid)
    {
        fprintf(stdout, "Change to daemon mode. uid:%u.\n" , config.dae_uid);
        utils::LinuxUtil::daemon();
    }
//    common::T::partner(config.dae_partfile.c_str(), argv);
}


int main(int argc, char *argv[])
{
    int ret = 0;
    (void)argc;
    // signal 
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    ONCE_LOOP_ENTER
    if (argc != 2)
    {
        fprintf(stderr, "\033[1;31mcmd error!\n");
        fprintf(stdout, "\033[1;32musage:\n");
        fprintf(stdout, "\033[1;32m\t%s cfg.xml\n", argv[0]);
        fprintf(stdout, "\033[1;30m");
        return 0;
    }

    if ( strcmp("-v", argv[1]) == 0 ) 
    {
        ShowVersion();
        exit(0);
    }

    std::string cfg_path = argv[1];
    fprintf(stdout, "InitConfig. file:%s\n", cfg_path.c_str());
    ret = InitConfig(cfg_path, config);
    if (0 != ret)
    {
        fprintf(stderr, "\033[1;31m init config failed.");
        break;
    }

    //fprintf(stdout, "Daemon.\n");
    Daemon(config, argv);

    fprintf(stdout, "InitLogger.\n");
    if (!InitLogger(config))
    {
        fprintf(stderr, "\033[1;31minit log failed.");
        break;
    }
    
    fprintf(stdout, "InstallFactory.\n");
    ServerApp::Instance()->RegistFactory(config);

    fprintf(stdout, "open config.\n");
    // todo: 打开连接
    if (ServerApp::Instance()->open(config) != 0)
    {
        LOG(ERROR)("init falied.");
        fprintf(stderr, "\033[1;31minit falied.\n\n");
        ret = -1;
        return ret;
    }
    LOG(INFO)("open server.");

    fprintf(stdout, "\033[1;32mserver start successfully. ^-^.\n\n");
    LOG(INFO)("run loop.");
    // todo: 启动服务,进入事件循环
    fprintf(stdout, "run_service.\n");
    ret = ServerApp::Instance()->run_service();
    if (0 != ret)
    {
        LOG(ERROR)("run_service failed.");
        break;
    }
    
    fprintf(stdout, "UninstallFactory.\n");
    ServerApp::Instance()->UnregistFactory(config);
    ONCE_LOOP_LEAVE
    return ret;
}

void ShowVersion() 
{
#ifndef SERVER_VERSION
#define SERVER_VERSION ""
#endif
#ifndef USER_BUILD
#define USER_BUILD ""
#endif
#ifndef MACHINE_IP
#define MACHINE_IP ""
#endif
#ifndef GCC_VERSION
#define GCC_VERSION ""
#endif
#ifndef MAKEFILE_TIME
#define MAKEFILE_TIME ""
#endif

    printf("VERSION    : %s\n", SERVER_VERSION);
    printf("BUILD BY   : %s\n", USER_BUILD);
    printf("BUILD TIME : %s %s\n", __DATE__, __TIME__);
    printf("ON MACHINE : %s\n", MACHINE_IP);
    printf("USE GCC    : %s\n", GCC_VERSION);
    printf("MAKEFILE   : %s\n", MAKEFILE_TIME);
}


