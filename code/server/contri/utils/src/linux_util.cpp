//! @file linux_util.h
//! @brief linux���е�һЩ����

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

	if((iPID = fork()) < 0) /* -1�Ǵ���ʧ�ܵ���� */
	{
		fprintf(stderr, "fork1 failed! errno:%d, msg:%s\n", errno, strerror(errno));
		exit(0);
	}
	else if(iPID > 0){       /* ����0�ǽ��ӽ��̵�PID���ظ������� */
		exit(0);
	}
	/* �����µ�session��process group����Ϊ��leader������������ն� */
	setsid();

	/* 
	 * signal�ĵڶ���������SIG_IGNʱ����ʾ��һ��������ָ���źŽ�������
	 * �����ն�IO�źź�ֹͣ�źŵ�
	 */
	signal(SIGINT,  SIG_IGN);  /* �����ն˵�ctrl+c��delete */
	signal(SIGHUP,  SIG_IGN);  /* ����������ն������Ľ��̣���ʾ���ն˵����ӶϿ� */
	signal(SIGQUIT, SIG_IGN);  /* �����ն˵�ctrl+/ */
	signal(SIGPIPE, SIG_IGN);  /* ��û�ж����̵Ĺܵ�д���� */
	signal(SIGTTOU, SIG_IGN);  /* ��̨���ն�д */
	signal(SIGTTIN, SIG_IGN);  /* ��̨���ն˶� */

	signal(SIGTERM, SIG_IGN);  /* ��kill����������ȱʡ�ź� */

	if((iPID = fork()) < 0) 
	{
		fprintf(stderr, "fork2 failed! errno:%d, msg:%s\n", errno, strerror(errno));
		exit(0);
	}
	else if(iPID > 0){ 
		exit(0);
	}
	/*
	 * ����Ŀ¼����Ϊ��Ŀ¼������Ϊ�˱�֤���ǵĽ��̲�ʹ���κ�Ŀ¼���������ǵ��ػ����̽�һֱ
	 * ռ��ĳ��Ŀ¼������ܻ���ɳ����û�����ж��һ���ļ�ϵͳ�� 
	 */
	chdir("/");

	//�رմ򿪵��ĵ������������ض����׼���롢��׼����ͱ�׼����������ĵ���������
	//���̴Ӵ������ĸ���������̳��˴򿪵��ĵ������������粻�رգ������˷�ϵͳ��Դ��
	//�����޷�Ԥ�ϵĴ���getdtablesize()����ĳ���������ܴ򿪵������ĵ�����

	for (int fd=0,fdtablesize=getdtablesize();fd < fdtablesize;fd++)
	{
		close(fd);
	}


	/*
	 * ���ļ���ʽ��������������Ϊ"0"��������Ϊ�ɼ̳е������ļ�������ʽ�����ֿ��ܻ��ֹĳЩ���Ȩ��
	 * �������ǵ��ػ�������Ҫ����һ��ɶ���д���ļ��������ػ����̴Ӹ���������̳������ļ�������ʽ
	 * ������ȴ�п������ε������������Ȩ�����´�����һ���ļ������д�����Ͳ�����Ч�����Ҫ���ļ�
	 * ��ʽ��������������Ϊ"0"��
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

void LinuxUtil::partner(const char * lockname, char* argv[])  // argv ��������������
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
