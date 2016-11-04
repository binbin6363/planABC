/**
 * @filedesc: 
 * udp_cmd_session.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/13 23:02:59
 * @modify:
 *
**/

#ifndef _UDP_CMD_SESSION_H_
#define _UDP_CMD_SESSION_H_
#include "headers.h"


class UdpCmdSession : public Net_Session
{
public:

	UdpCmdSession(void);
	~UdpCmdSession(void);
	virtual int open(void *arg, const INET_Addr &remote_addr);
	virtual int on_receive_message(char *ptr, int len);
	string parsecmd(string &strUdp);
    static UdpCmdSession *g_udpcmd;
    int send_udp_data(const char *data, int length);
    int handle_close( uint32_t handle );
    void handle_timeout( int id,void *userData );


private:
    int call_cmd(const string &cmd);
    void initCmd();


};

#endif // _UDP_CMD_SESSION_H_

