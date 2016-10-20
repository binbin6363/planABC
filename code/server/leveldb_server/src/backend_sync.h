/**
 * @filedesc: 
 * backend_sync.h, backend sync data, in another netmanager instance
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/18 10:02:59
 * @modify:
 *
**/
#ifndef BACKEND_SYNC_H_
#define BACKEND_SYNC_H_

class Net_Manager;
class MasterSession;

class BackendSync
{
public:
    BackendSync();
    ~BackendSync();

public:
    // 打开监听slave的端口或者连接到master
	int Start(Net_Manager *net_manager);

private:
    int open_slave_acceptor();
    int open_master_service();

public:

private:
    Net_Manager        *net_manager_;
    MasterSession      *master_session_;

};



#endif //BACKEND_SYNC_H_

