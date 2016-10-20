/**
 * @filedesc: 
 * server.cpp, main file, handle server event
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/8 21:52:59
 * @modify:
 *
**/
#include <sys/time.h>
#include <sstream>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <string>
#include <malloc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <iostream>
#include "log.h"
#include "server_app.h"
#include "server_config.h"
#include "tinyxml.h"
#include "linux_util.h"


using namespace std;

void ShowVersion(void);
int LoadConfig(int argc, char *argv[]);
int check_update();

string g_str_config;

void exit_main(int sig)
{
    LOG(ERROR)("mobiled: Got a Signal, sig:%d", sig);
}

#if 0
const char* option_format = "f:vh";

static option long_options[] = {
    {"config-file", 1, 0, 'f'},    
    {"version", 0, 0, 'v'},
    {NULL, 0, 0, 0}
};
#endif 


int main(int argc,char *argv[])
{
    signal(SIGCHLD, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    int ret = 0;
    ONCE_LOOP_ENTER
        // check argv
        if (argc < 2)
        {
            fprintf(stderr, "\033[1;31m!!!error params num.\n");
            fprintf(stdout, "\033[1;32m%s cfg.xml\n", argv[0]);
            return 0;
        }
        // load config
        if (LoadConfig(argc, argv) != 0)
        {
            fprintf(stderr, "\033[1;31mload config failed, please check config.\n\n");
            ret = -2;
            break;
        }
        LOG(INFO)("load config succeed.");

        // check update
        //check_update();
        
        // todo: 打开连接
        if (ServerApp::Instance()->open() != 0)
        {
            LOG(ERROR)("init falied.");
            fprintf(stderr, "\033[1;31minit falied.\n\n");
            ret = -1;
            break;
        }
        LOG(INFO)("open server succeed.");

        fprintf(stdout, "\033[1;32mmobiled server start successfully. ^-^.\n\n");
        LOG(INFO)("run loop.");
        // todo: 启动服务,进入事件循环
        ServerApp::Instance()->run_service();
    ONCE_LOOP_LEAVE
    return 0;
}


bool InitConfig(int argc, char *argv[])
{
    (void)argc;
    if ( strcmp("-v", argv[1]) == 0 ) 
    {
        ShowVersion();
        exit(0);
    }

    return utils::ServerConfig::Instance()->load_file(argv[1]);
    
    //return config.LoadFile(argv[1]);
}

bool SetLogger(const TiXmlElement& setting)
{
    const TiXmlElement *cfgLog = setting.FirstChildElement("log");
    if (!cfgLog && !cfgLog->FirstChildElement("file") && !cfgLog->FirstChildElement("rank"))
        return false;

    string logFile = cfgLog->FirstChildElement("file")->GetText();
    string logRank = cfgLog->FirstChildElement("rank")->GetText();
    cout<<"open log file:"<<(const char*)logFile.c_str()<<endl;
    LOG_INIT((const char*)logFile.c_str(),500000000,utils::L_TRACE);
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

//改为loadconfig
int LoadConfig(int argc, char *argv[])
{
    int ret = 0;
    ONCE_LOOP_ENTER
        try 
        {
            mallopt(M_MMAP_MAX, 0);
            mallopt(M_TRIM_THRESHOLD, 0);
            //load file
            if (!InitConfig(argc, argv))
            {
                fprintf(stderr, "load cfg file error, path=%s", argv[1]);
                ret = -1;
                break;
            }
            // load damenon info
            if (utils::ServerConfig::Instance()->load_daemon_info())
            {
                //LOG(INFO)("Change to daemon mode.");
                setuid(utils::ServerConfig::Instance()->uid());

				if (utils::ServerConfig::Instance()->uid() == 0)
				{
	                utils::LinuxUtil::daemon();
				}

                //common::T::partner(utils::ServerConfig::Instance()->partfile(), argv);
            }
            
            // todo: file not exist cause Segmentation fault!!!!!!!!!!!!!!!!
            if (!utils::ServerConfig::Instance()->init_log())
            {
                fprintf(stderr, "init log error");
                ret = -2;
                break;
            }
            LOG(INFO)("load config.");
            utils::ServerConfig::Instance()->load_config();
        }
        catch ( std::exception & e ) 
        {
            printf("%s\n", e.what());
        }
    ONCE_LOOP_LEAVE
    return ret;
}

#if 0
int check_update()
{
    int ret = 0;
    ONCE_LOOP_ENTER
    // check update element, no update element, return false
    if (!utils::ServerConfig::Instance()->check_update_element())
    {
        LOG(DEBUG)("no update element, there is no use to update.");
        break;
    }
    // when open update file failed, return false
    if (!utils::ServerConfig::Instance()->load_update_info())
    {
        LOG(ERROR)("check update error.");
        //ret = -1;
        break;
    }

    ONCE_LOOP_LEAVE
    return ret;
}
#endif

void ShowVersion( ) 
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

