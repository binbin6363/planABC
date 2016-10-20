/**
 * @filedesc: 
 * 
 * @author: 
 *  bbwang
 * @date: 
 *  2014/8/3 12:02:59
 * @modify:
 *
**/

#ifndef udp_cmd_session_h__
#define udp_cmd_session_h__
#include "comm.h"
#include "inet_addr.h"
#include "net_session.h"
#include <string>
#include <algorithm>

using namespace std;

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

#endif // udp_cmd_session_h__

