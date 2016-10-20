//! @file os.h
//! @brief 定义socket相关类型


#ifndef _OS_H_
#define _OS_H_


#include "types.h"


#ifdef WIN32	// for windows

#include <winsock2.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>


//! 设置为非阻塞模式
inline int set_socket_nonblocking(SOCKET socket)
{
	int iMode = 1;
	ioctlsocket(socket, FIONBIO, (u_long FAR*)&iMode);

	return 0;
}


//! 关闭socket
inline int close(SOCKET socket)
{
	return closesocket(socket);
}


//! 错误码
inline int error_no()
{
	return WSAGetLastError();
}


#define EAGAIN		WSAEWOULDBLOCK		//<! 无数据可读或可写
#define EINPROGRESS	WSAEWOULDBLOCK			//<! 正在连接


//! 睡眠 usec 微秒
inline void usleep(unsigned long usec)
{
	Sleep(usec / 1000);
}


//! 创建文件夹
inline int mkdir(const char *dirname, int)
{
	return _mkdir(dirname);
}


#define F_OK 0
#define W_OK 2
#define R_OK 4


//! 检测文件
inline int access(const char *path, int mode)
{
	return _access(path, mode);
}




#else			// for linux

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


//! @typedef SOCKET
//! @brief socket类型
typedef int SOCKET;


//! @def INVALID_SOCKET
//! @brief 无效socket
#define INVALID_SOCKET -1


//! 设置为非阻塞模式
inline int set_socket_nonblocking(SOCKET socket)
{
	int val = fcntl(socket, F_GETFL, 0);
	fcntl(socket, F_SETFL, val | O_NONBLOCK);

	return 0;
}

//! 错误码
inline int error_no()
{
	return errno;
}


#endif


#endif // _OS_H_

