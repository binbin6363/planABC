//! @file linux_util.h
//! @brief linux特有的一些操作

#include "linux_util.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

namespace utils {

void LinuxUtil::daemon()
{
	pid_t iPID;

	if((iPID = fork()) < 0) /* -1是创建失败的情况 */
	{
		fprintf(stderr, "fork1 failed! errno:%d, msg:%s\n", errno, strerror(errno));
		exit(0);
	}
	else if(iPID > 0){       /* 大于0是将子进程的PID传回给父进程 */
		exit(0);
	}
	/* 创建新的session和process group，成为其leader，并脱离控制终端 */
	setsid();

	/* 
	 * signal的第二个参数是SIG_IGN时，表示第一个参数所指明信号将被忽略
	 * 忽略终端IO信号和停止信号等
	 */
	signal(SIGINT,  SIG_IGN);  /* 来自终端的ctrl+c和delete */
	signal(SIGHUP,  SIG_IGN);  /* 发给与控制终端相连的进程，表示与终端的连接断开 */
	signal(SIGQUIT, SIG_IGN);  /* 来自终端的ctrl+/ */
	signal(SIGPIPE, SIG_IGN);  /* 向没有读进程的管道写错误 */
	signal(SIGTTOU, SIG_IGN);  /* 后台向终端写 */
	signal(SIGTTIN, SIG_IGN);  /* 后台从终端读 */

	signal(SIGTERM, SIG_IGN);  /* 由kill函数产生的缺省信号 */

	if((iPID = fork()) < 0) 
	{
		fprintf(stderr, "fork2 failed! errno:%d, msg:%s\n", errno, strerror(errno));
		exit(0);
	}
	else if(iPID > 0){ 
		exit(0);
	}
	/*
	 * 工作目录更改为根目录。这是为了保证我们的进程不使用任何目录。否则我们的守护进程将一直
	 * 占用某个目录，这可能会造成超级用户不能卸载一个文件系统。 
	 */
	chdir("/");

	//关闭打开的文档描述符，或重定向标准输入、标准输出和标准错误输出的文档描述符。
	//进程从创建他的父进程那里继承了打开的文档描述符。假如不关闭，将会浪费系统资源，
	//引起无法预料的错误。getdtablesize()返回某个进程所能打开的最大的文档数。

	for (int fd=0,fdtablesize=getdtablesize();fd < fdtablesize;fd++)
	{
		close(fd);
	}


	/*
	 * 将文件方式创建屏蔽字设置为"0"。这是因为由继承得来的文件创建方式屏蔽字可能会禁止某些许可权。
	 * 例如我们的守护进程需要创建一组可读可写的文件，而此守护进程从父进程那里继承来的文件创建方式
	 * 屏蔽字却有可能屏蔽掉了这两种许可权，则新创建的一组文件其读或写操作就不能生效。因此要将文件
	 * 方式创建屏蔽字设置为"0"。
	 */ 
	umask(0);
	signal(SIGCHLD, SIG_IGN); 

}

int LinuxUtil::lock_wait(const char * fname)
{
	//cout<<"--lock----[fname:]"<<fname<<endl;
	int fd = open(fname, O_RDWR | O_CREAT, 0666);

	if( fd < 0 ){
		//perror("open");
		return -1;
	}

	struct flock lock;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	lock.l_type = F_WRLCK;

	int error = -1;

	do{

		error = fcntl(fd, F_SETLKW, &lock);  	

	}while(-1==error && EINTR == errno);

	return 0;

} 

void LinuxUtil::partner(const char * lockname, char* argv[])  // argv 必须有两个参数
{
	char * arg[] = {argv[0], argv[1], (char *)0};	
	int ret = lock_wait(lockname);
	if(0 == ret )
	{
		int pid = fork();
		if(0==pid)
		{
			execv(arg[0], arg);
		}
		sleep(1);
	}	
}


void LinuxUtil::groupd_partner(const char * lockname, char*argv[], const char* cShellPath)
{
	char * arg[] = {argv[0], argv[1], (char*)0};
	int32_t nRet = lock_wait(lockname);
	if (0 == nRet)
	{
		//int32_t nSystemRet = system(cShellPath);
		int32_t nPid = fork();
		if (0 == nPid)
		{
			fprintf(stdout, "(framework), fork child thread nPid:%d.\n", nPid);
			//if( (-1 != nSystemRet) && (SHELL_COULD_NOT_BE_EXECUTED != nSystemRet))
			{
				execv(arg[0], arg);
			}

		}
		sleep(1);
	}
}

string LinuxUtil::getTime()//YYYYMMDDHHmmSS
{
    char tmbuf[64];
    time_t now = time(NULL);
    struct tm _ct;
    localtime_r(&now, &_ct);
    memset(tmbuf,0,sizeof(tmbuf));
    snprintf(tmbuf,sizeof(tmbuf),"%04d%02d%02d%02d%02d%02d",(int)(_ct.tm_year+1900), (int)(_ct.tm_mon+1), (int)(_ct.tm_mday),(int)_ct.tm_hour,(int)_ct.tm_min,(int)_ct.tm_sec);	
    return string(tmbuf);
}

string LinuxUtil::getTime1() //YYYY-mm-DD HH:MM:SS
{
    char tmbuf[64];
    time_t now = time(NULL);
    struct tm _ct;
    localtime_r(&now, &_ct);
    memset(tmbuf,0,sizeof(tmbuf));
    snprintf(tmbuf,sizeof(tmbuf),"[%04d-%02d-%02d %02d:%02d:%02d]",(int)(_ct.tm_year+1900), (int)(_ct.tm_mon+1), (int)(_ct.tm_mday),(int)_ct.tm_hour,(int)_ct.tm_min,(int)_ct.tm_sec);
    return string(tmbuf);
}

string LinuxUtil::getTime(time_t now)//YYYYMMDDHHmmSS
{
    char tmbuf[64];
    struct tm _ct;
    localtime_r(&now, &_ct);
    memset(tmbuf,0,sizeof(tmbuf));
    snprintf(tmbuf,sizeof(tmbuf),"%04d%02d%02d%02d%02d%02d",(int)(_ct.tm_year+1900), (int)(_ct.tm_mon+1), (int)(_ct.tm_mday),(int)_ct.tm_hour,(int)_ct.tm_min,(int)_ct.tm_sec);	
    return string(tmbuf);
}

string LinuxUtil::getTime1(time_t now) //YYYY-mm-DD HH:MM:SS
{
    char tmbuf[64];
    struct tm _ct;
    localtime_r(&now, &_ct);
    memset(tmbuf,0,sizeof(tmbuf));
    snprintf(tmbuf,sizeof(tmbuf),"[%04d-%02d-%02d %02d:%02d:%02d]",(int)(_ct.tm_year+1900), (int)(_ct.tm_mon+1), (int)(_ct.tm_mday),(int)_ct.tm_hour,(int)_ct.tm_min,(int)_ct.tm_sec);
    return string(tmbuf);
}


} // namespace utils
