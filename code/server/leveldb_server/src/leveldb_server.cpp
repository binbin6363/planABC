/**
 * @filedesc: 
 * leveldb_server.cpp, main file, handle server event
 * @author: 
 *  bbwang
 * @date: 
 *  2014/8/3 12:02:59
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
#include "leveldb_server_app.h"
#include "leveldb_engine.h"
#include "serverconfig.h"
#include "linux_util.h"

using namespace std;
using namespace utils;

void ShowVersion(void);
int ShowInfo();
int LoadConfig(int argc, char *argv[]);
//int check_update();

void exit_main(int sig)
{
    LOG(ERROR)("leveldb: Got a Signal, sig:%d", sig);
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
        if (LevelDbApp::Instance()->open() != 0)
        {
            LOG(ERROR)("init falied.");
            fprintf(stderr, "\033[1;31minit falied.\n\n");
            ret = -1;
            break;
        }
        LOG(INFO)("open server succeed.");
        LOG(INFO)("\033[1;32mleveldb server start successfully. ^-^.\n\n");
        LOG(INFO)("run loop.");
        // 启动存储引擎
        LeveldbEngine::inst().startDb();
        // 显示一些信息
        ShowInfo();
        // todo: 启动服务,进入事件循环
        LevelDbApp::Instance()->run_service();
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
            // default is not daemon
            if (ServerConfig::Instance()->load_daemon_info() 
                    && ServerConfig::Instance()->isDaemon())
            {
                LinuxUtil::daemon();
            }
            
            // todo: file not exist cause Segmentation fault!!!!!!!!!!!!!!!!
            if (!ServerConfig::Instance()->init_log())
            {
                fprintf(stderr, "init log error");
                ret = -2;
                break;
            }
            LOG(INFO)("load config.");
            ServerConfig::Instance()->load_config();
        }
        catch ( std::exception & e ) 
        {
            printf("%s\n", e.what());
        }
    ONCE_LOOP_LEAVE
    return ret;
}


//int check_update()
//{
//    int ret = 0;
//    ONCE_LOOP_ENTER
//    // check update element, no update element, return false
//    if (!ServerConfig::Instance()->check_update_element())
//    {
//        LOG(DEBUG)("no update element, there is no use to update.");
//        break;
//    }
//    // when open update file failed, return false
//    if (!utils::ServerConfig::Instance()->load_update_info())
//    {
//        LOG(ERROR)("check update error.");
//        //ret = -1;
//        break;
//    }

//    ONCE_LOOP_LEAVE
//    return ret;
//}


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

extern int LOG_QUEUE_SIZE;

int ShowInfo()
{
#ifndef SERVER_VERSION
#define SERVER_VERSION ""
#endif
    utils::ServerConfig *sc = utils::ServerConfig::Instance();
    string data_db = sc->dbpath();
    const char *lcim = sc->ldb_create_if_missing();
    const char *leie = sc->ldb_error_if_exists();
    const char *lpc = sc->ldb_paranoid_checks();
    uint32_t lwbz = sc->ldb_write_buffer_size();
    uint32_t lmof = sc->ldb_max_open_files();
    uint32_t lbz = sc->ldb_block_size();
    uint32_t lbri = sc->ldb_block_restart_interval();
    uint32_t lbpk = sc->ldb_bits_per_key();
    uint32_t lcs = sc->ldb_cache_size();
    uint32_t lct = sc->ldb_compression_type();
    int ssl = sc->sync_speed_limit();
    const char *write_bin_log = sc->write_log() ? "true" : "false";
    LOG(INFO)("************************************************************************************");
    LOG(INFO)("* SERVER VERSION: %s", SERVER_VERSION);
    LOG(INFO)("* DATA DB PATH: %s", data_db.c_str());
    LOG(INFO)("* CREATE IF MISSING: %s", lcim);
    LOG(INFO)("* ERROR IF EXISTS: %s", leie);
    LOG(INFO)("* PARANOID CHECKS: %s", lpc);
    LOG(INFO)("* WRITE BUFFER SIZE: %uKB", lwbz/(1024));
    LOG(INFO)("* MAX OPEN FILES: %u", lmof);
    LOG(INFO)("* BLOCK SIZE: %uKB", lbz/(1024));
    LOG(INFO)("* BLOCK RESTART INTERVAL: %u", lbri);
    LOG(INFO)("* BITS PER KEY: %u", lbpk);
    LOG(INFO)("* CACHE SIZE: %uKB", lcs/(1024));
    LOG(INFO)("* COMPRESSION TYPE: %u", lct);
    LOG(INFO)("* SYNC SPEED LIMIT: %dMB/s(depends on master push speed)", ssl);
    LOG(INFO)("* WRITE BINLOG: %s", write_bin_log);
    LOG(INFO)("* BINLOG SIZE: %d", LOG_QUEUE_SIZE);
    LOG(INFO)("************************************************************************************");
    return 0;
}

