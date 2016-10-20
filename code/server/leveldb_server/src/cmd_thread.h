/**
 * @filedesc: 
 * cmd_thread.h, process cmd in thread
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/18 14:02:59
 * @modify:
 *
**/


#include "cmd.h"
#include "task_base.h"

using namespace utils;


// 后续继承此类的，只需实现svc即可，execute的内容挪到svc中
class CmdThread : public Task_Base//, public Command
{
public:

    CmdThread(Net_Session* session, BinInputPacket<>& inpacket);
    virtual ~CmdThread();
	//! 启动线程
	int start();

	//! 停止线程
	int stop();

    virtual bool execute();

    bool is_stop(){return m_notify_stop;}
private:
    // start 之前，线程启动之前做的事情
    virtual int doPreStart();

	//! 线程函数
	virtual int svc();

private:
	//! 运行状态
	bool m_is_run;

	//! 停止通知
	volatile bool m_notify_stop;

protected:
	Net_Session * m_session;
	BinInputPacket<>& m_inpkg;

};


class RequestSyncMsg;
class ResultSyncMsg;
class BinlogQueue;
class NoBinlogIterator;

// handle cmd in thread
class SyncDataCmdThread : public CmdThread
{
public:
    SyncDataCmdThread(Net_Session* session, BinInputPacket<>& inpacket);
    virtual ~SyncDataCmdThread();

private:
    // start 之前，线程启动之前做的事情
    virtual int doPreStart();
    
	virtual int svc();
    int SyncFromCacheQueue(ResultSyncMsg &result, const BinlogQueue *binlog_queue);
    int CopyFromDb(ResultSyncMsg &result);
    int copy_end(ResultSyncMsg &result);

    NoBinlogIterator *new_iter();
    void delete_iter();
    void reset();
    void init_sync(const uint64_t &last_seq, const string &last_key, ResultSyncMsg &result);

private:
    RequestSyncMsg *request_sync_msg_;
    ResultSyncMsg  *result_sync_msg_;
    uint8_t        copy_begin_flag_;

    NoBinlogIterator *db_iter_;
	uint64_t    last_seq_;
	std::string last_key_;
	bool        is_mirror_;

    char        *sync_buf_;
    uint32_t    sync_max_size_;
    uint32_t    sync_speed_; // 同步限速
    uint32_t    handle_;     // session handle
    uint32_t    sync_flag_;  // 同步阶段


};


