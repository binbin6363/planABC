//! @file os.h
//! @brief ����socket�������


#ifndef _OS_H_
#define _OS_H_


#include "types.h"


#ifdef WIN32	// for windows

#include <winsock2.h>
#include <direct.h>
#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>


//! ����Ϊ������ģʽ
inline int set_socket_nonblocking(SOCKET socket)
{
	int iMode = 1;
	ioctlsocket(socket, FIONBIO, (u_long FAR*)&iMode);

	return 0;
}


//! �ر�socket
inline int close(SOCKET socket)
{
	return closesocket(socket);
}


//! ������
inline int error_no()
{
	return WSAGetLastError();
}


#define EAGAIN		WSAEWOULDBLOCK		//<! �����ݿɶ����д
#define EINPROGRESS	WSAEWOULDBLOCK			//<! ��������


//! ˯�� usec ΢��
inline void usleep(unsigned long usec)
{
	Sleep(usec / 1000);
}


//! �����ļ���
inline int mkdir(const char *dirname, int)
{
	return _mkdir(dirname);
}


#define F_OK 0
#define W_OK 2
#define R_OK 4


//! ����ļ�
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
//! @brief socket����
typedef int SOCKET;


//! @def INVALID_SOCKET
//! @brief ��Чsocket
#define INVALID_SOCKET -1


//! ����Ϊ������ģʽ
inline int set_socket_nonblocking(SOCKET socket)
{
	int val = fcntl(socket, F_GETFL, 0);
	fcntl(socket, F_SETFL, val | O_NONBLOCK);

	return 0;
}

//! ������
inline int error_no()
{
	return errno;
}


#endif


#endif // _OS_H_

