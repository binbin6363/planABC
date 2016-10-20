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


// �����̳д���ģ�ֻ��ʵ��svc���ɣ�execute������Ų��svc��
class CmdThread : public Task_Base//, public Command
{
public:

    CmdThread(Net_Session* session, BinInputPacket<>& inpacket);
    virtual ~CmdThread();
	//! �����߳�
	int start();

	//! ֹͣ�߳�
	int stop();

    virtual bool execute();

    bool is_stop(){return m_notify_stop;}
private:
    // start ֮ǰ���߳�����֮ǰ��������
    virtual int doPreStart();

	//! �̺߳���
	virtual int svc();

private:
	//! ����״̬
	bool m_is_run;

	//! ֹ֪ͣͨ
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
    // start ֮ǰ���߳�����֮ǰ��������
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
    uint32_t    sync_speed_; // ͬ������
    uint32_t    handle_;     // session handle
    uint32_t    sync_flag_;  // ͬ���׶�


};


