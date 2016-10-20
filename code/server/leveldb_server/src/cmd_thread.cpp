
#include "cmd_thread.h"
#include "comm.h"
#include "constants.h"
#include "log.h"
#include "slave_session.h"
#include "master_session.h"
#include "binlog.h"
#include "nobinlog_iterator.h"
#include "msg.h"
#include "leveldb_engine.h"
#include "leveldb_server_app.h"
#include "serverconfig.h"

using namespace common;
using namespace utils;

CmdThread::CmdThread(Net_Session* session, BinInputPacket<>& inpacket)
    : Task_Base()
    , m_is_run(false)
    , m_notify_stop(false)
    , m_session(session)
    , m_inpkg(inpacket)
{
}


CmdThread::~CmdThread()
{
}

int CmdThread::doPreStart()
{
    LOG(ERROR)("err, call base class doPreStart.");
    return -1;
}


int CmdThread::start()
{
	if (false != m_is_run) {
		return -1;
	}


	m_notify_stop = false;
	int rc = activate();
	if (rc != 0) {
        LOG(ERROR)("start cmd thread failed. ret:%d", rc);
		return -1;
	}

	m_is_run = true;
	return 0;
}

int CmdThread::stop()
{
	m_notify_stop = true;
	wait();

	m_is_run = false;
	return 0;
}


bool CmdThread::execute()
{
    LOG(INFO)("enter cmd thread.");
    
    int rc = doPreStart();
    
    rc |= start();
    return (0 == rc) ? true:false;
}


int CmdThread::svc()
{
    LOG(ERROR)("err, call base class svc.");
    return -1;
}


// SyncDataCmdThread
SyncDataCmdThread::SyncDataCmdThread(Net_Session* session, BinInputPacket<>& inpacket)
    : CmdThread(session,inpacket)
    , request_sync_msg_(NULL)
    , result_sync_msg_(NULL)
    , copy_begin_flag_(0)
    , db_iter_(NULL)
    , last_seq_(0)
    , last_key_("")
    , is_mirror_(false)
    , sync_buf_(NULL)
    , sync_max_size_(0)
    , sync_speed_(0)
    , handle_(0)
    , sync_flag_(NOT_START)
{
    // slave ����ܽӶ��٣������ֻ�ܷ�����
    sync_max_size_ = utils::ServerConfig::Instance()->max_buffer_size[SLAVE_SERVER_NAME];
    if (sync_max_size_ < _1MB)
    {
        LOG(INFO)("max send buf size < 1MB, set it 1MB.");
        sync_max_size_ = _1MB;
    }
    LOG(INFO)("max send buf size is :%u.", sync_max_size_);
    sync_buf_ = new char[sync_max_size_];

    // ͬ�������е����٣�����master��ҵ��
    sync_speed_ = utils::ServerConfig::Instance()->sync_speed_limit();
}

SyncDataCmdThread::~SyncDataCmdThread()
{
    SAFE_DELETE(db_iter_);
    SAFE_DELETE(sync_buf_);
    SAFE_DELETE(request_sync_msg_);
    SAFE_DELETE(result_sync_msg_);
}


int SyncDataCmdThread::doPreStart()
{
    int ret = 0;
    SlaveSession *slave_session = dynamic_cast<SlaveSession *>(m_session);
    if (NULL == slave_session) {
        LOG(ERROR)("[backend] slave session is null. exit data sync thread.");
        ret = -1;
        return ret;
    }
    handle_ = slave_session->handle();
    // 1. decode request
    request_sync_msg_ = new RequestSyncMsg(m_inpkg);
    ret = request_sync_msg_->decode();
    if (0 != ret) {
        LOG(ERROR)("[backend] input data format is error. exit data sync thread.");
    } else {
        LOG(INFO)("[backend] doPreStart sync data thread. peer addr:%s, handle:%u, send buf max size:%u.", 
            FromAddrTostring(slave_session->remote_addr()).c_str(), handle_, sync_max_size_);
    }
    return ret;

}

// sleep longer to reduce logs.find
#define TICK_INTERVAL_MS	300
#define NOOP_IDLES			(3000/TICK_INTERVAL_MS)

// ����ͬ���̴߳���
// 1.У��ͻ���
// 2.�ж�����ͬ������
// 3.ͬ������
int SyncDataCmdThread::svc()
{
    int ret = 0;
    int add_num = 0;
    int interval = 0;
    uint8_t copy_flag = 0;
    uint32_t push_cnt = 0;

    if (NULL == request_sync_msg_) {
        LOG(ERROR)("[backend] request is null. exit data sync thread.");
        ret = -1;
        return ret;
    }

    uint64_t request_last_seq = request_sync_msg_->last_seq_;
    uint64_t binlog_last_seq = LeveldbEngine::inst().get_lastest_seq();
    if (request_last_seq > binlog_last_seq) {
        LOG(ERROR)("[backend] sync request last seq(%lu) > binlog last seq(%lu), refuse sync data. exit data sync thread."
            , request_last_seq, binlog_last_seq);
        return -1;
    }
    
    BinOutputPacket<> outpkg(sync_buf_, sync_max_size_);
    outpkg.offset_head(sizeof(COHEADER));
    result_sync_msg_ = new ResultSyncMsg(outpkg, *request_sync_msg_);
    
    // �ж�ͬ���׶�
    init_sync(request_sync_msg_->last_seq_, request_sync_msg_->last_key_, *result_sync_msg_);

    LevelDbApp *app = LevelDbApp::Instance();
    BinlogQueue *binlog_queue = LeveldbEngine::inst().Binlogs();
    if (NULL == binlog_queue) {
        LOG(ERROR)("[backend] log queue is null, exit data sync thread.");
        ret = -1;
        return ret;
    }

    // ������ON_COPY�׶λ���ON_SYNC�׶Σ���Ҫͬ������������������
    // ֻ��ON_COPY�׶λ���Ҫ��db����ȫ������
    while (!is_stop()) {
        ++interval;
        push_cnt = 0;
        
		if(sync_flag_ == OUT_OF_SYNC){
			reset();
	        Binlog log(0, BinlogType::COPY, BinlogCommand::BEGIN, "");
            result_sync_msg_->AddRecord(log.repr(), "copy_begin");
            result_sync_msg_->set_copy_begin_flag(BinlogCommand::BEGIN);
			continue;
		}
        copy_flag = result_sync_msg_->copy_begin_flag();

        // ���Ҫ���¿������Ȳ�����ͬ�����ݣ�ֱ�ӷ������¿�ʼ�ı�־���ȴ�һ��
        if (BinlogCommand::BEGIN != copy_flag) {
    		bool is_empty = true;
            // 1.��ͬ���������������
            // �ӻ���Ķ���ͬ�����µ�д������û�������ݿ�ͬ��������
            add_num = SyncFromCacheQueue(*result_sync_msg_, binlog_queue);
            if (0 != add_num) {
                is_empty = false;
            }

            // 2.�������COPY�׶Σ���ҪCOPY�������ݣ���key����
            if (sync_flag_ == ON_COPY) {
                add_num += CopyFromDb(*result_sync_msg_);
                if (0 != add_num) {
                    is_empty = false;
                }
            }

            // û����Ҫͬ�������ݣ�sleepһ�£���ֹ��cpuռ��
            if (is_empty) {
                // sleep for a while
                usleep(TICK_INTERVAL_MS * 1000);
                if (interval % 100 == 0) {
                    LOG(INFO)("[backend] sync data thread, sleep ...");
                }
                continue;
            }
        }
        
        // 3.����ȡ�����ͷ���
        if (0 != result_sync_msg_->encode()) {
            LOG(ERROR)("[backend] sync data pkg encode error. exit data sync thread.");
            break;
        }
        do {
            ++push_cnt;
            ret = app->push_back_packet(handle_, result_sync_msg_->getData(), result_sync_msg_->length());
            if (0 != ret) {
                LOG(WARN)("[backend] sync data queue is full, wait ...");
                usleep(TICK_INTERVAL_MS * 1000);
            }
            if (push_cnt > 100) {
                LOG(ERROR)("[backend] push sync data pkg error, exit data sync thread.");
                return ret;
            }
        } while(0 != ret);
        LOG(INFO)("[backend] sync data num:%u, last seq:%lu, data len:%u, handle:%u"
            , add_num, last_seq_, result_sync_msg_->length(), handle_);
        LOG_HEX(result_sync_msg_->getData(), result_sync_msg_->length(), utils::L_DEBUG);
        
        // 4.������û������
		float data_size_mb = result_sync_msg_->block_size() / 1024.0 / 1024.0;
        result_sync_msg_->reset();
        add_num = 0;
        int speed_limit = utils::ServerConfig::Instance()->sync_speed_limit();
        if (speed_limit > 0) {
            int sleep_time = (int)((data_size_mb / speed_limit) * 1000 * 1000);
            if (sleep_time > 0) {
                LOG(INFO)("[backend] limit the data sync speed, sleep %u us...", sleep_time);
                usleep(sleep_time);
            }
        }

        // 5. �����BinlogCommand::BEGIN�����ȴ�
        if (BinlogCommand::BEGIN == copy_flag) {
            LOG(INFO)("[backend] copy begin, sleep 60s...");
            sleep(60);
        }
    }

    LOG(ERROR)("[backend] quit sync data.");
    return ret;
}


// ����ͬ������������
int SyncDataCmdThread::SyncFromCacheQueue(ResultSyncMsg &result, const BinlogQueue *binlog_queue)
{    
    // ��һ��Binlog
	Binlog log;
	while(1){
		int ret = 0;
        uint64_t &last_seq = last_seq_;
        const string   &last_key = last_key_;
        const uint32_t &sync_flag = sync_flag_;
		const uint64_t expect_seq = last_seq + 1;
        
		if(sync_flag == ON_COPY && last_seq == 0){
			ret = binlog_queue->find_last(&log);
		}else{
            // ͬ���ٶȸ�����logɾ���ٶȣ�����expect_seq < binlog_queue->get_min_seq()
		    if (expect_seq < binlog_queue->get_min_seq()) {
    			LOG(WARN)("[backend] handle: %u, OUT_OF_SYNC! min seq: %lu, expect_seq: %lu",
    				handle_,
    				binlog_queue->get_min_seq(),
    				expect_seq);
    			sync_flag_ = OUT_OF_SYNC;
    			return 0;
            }
			ret = binlog_queue->find_next(expect_seq, &log);
		}
        // û�ҳɹ���ֱ�ӷ���
		if(ret != 0){
			return 0;
		}
		if(sync_flag == ON_COPY && log.key() > last_key){
            // ����seq
			last_seq = log.seq();
			// ����µ�д������key�ڵ�ǰ������ָ���key֮�󣬾ͱ����½�һ����������
			// ��Ϊ�Ѿ����ڵĵ������Ҳ����¼�������ݡ�������ɾ��֮ǰ�ĵ�����
			// ��������²���Ҫ��binlogָ��Ĳ���ͬ����slave������key�ĵ��������ҵ�������¼������д
			delete_iter();
			continue;
		}
        // ͬ���ٶȸ�����logɾ���ٶȣ�����log.seq() != expect_seq
//		if(last_seq != 0 && log.seq() != expect_seq){
//			LOG(WARN)("[backend] handle: %u, OUT_OF_SYNC! log.seq: %lu, expect_seq: %lu",
//				handle_,
//				log.seq(),
//				expect_seq);
//			sync_flag_ = OUT_OF_SYNC;
//			return 1;
//		}
	
		// update last_seq
		last_seq = log.seq();

		char type = log.type();
		if(type == BinlogType::MIRROR && is_mirror_){
            LOG(DEBUG)("[backend] slave is mirror, do nothing.");
			continue;
		}
		
		break;
	}

    // ������������������ݵ������ͻ�����
    LeveldbEngine &db_engine = LeveldbEngine::inst();
	int ret = 0;
	std::string val;
	switch(log.cmd()){
		case BinlogCommand::KSET:
		case BinlogCommand::HSET:
		case BinlogCommand::ZSET:
		case BinlogCommand::QSET:
		case BinlogCommand::QPUSH_BACK:
		case BinlogCommand::QPUSH_FRONT:
			ret = db_engine.raw_get(log.key(), &val);
			if(ret == -1){
				LOG(ERROR)("[backend] handle: %u, raw_get error!", handle_);
			}else if(ret == 1){
				LOG(DEBUG)("[backend] handle: %u, skip not found: %s", handle_, log.dumps().c_str());
			}else{
				LOG(DEBUG)("[backend] handle: %u, %s", handle_, log.dumps().c_str());
                result.AddRecord(log.repr(), val);
			}
			break;
		case BinlogCommand::KDEL:
		case BinlogCommand::HDEL:
		case BinlogCommand::ZDEL:
		case BinlogCommand::QPOP_BACK:
		case BinlogCommand::QPOP_FRONT:
			LOG(DEBUG)("[backend] handle: %u, %s", handle_, log.dumps().c_str());
            result.AddRecord(log.repr(), "");
			break;
	}
    LOG(DEBUG)("[backend] sync a data to slave, last_seq:%lu.", last_seq_);
	return 1;
}


// ���ؿ�������������
int SyncDataCmdThread::CopyFromDb(ResultSyncMsg &result)
{
	int copy_num = 0;

    const uint64_t &last_seq = last_seq_;
	if(db_iter_ == NULL){
        new_iter();
	}
	int iterate_count = 0;
	uint32_t stime = time(NULL);
	while(true){
		// ����.�ְ��Ƚ��ѿ��ơ��˴���Ĭ�ϴ�С����_1MB�ͷ��ɡ�������bug
		if(++iterate_count > 100 || result.block_size() + 4096 > _1MB){
            LOG(INFO)("[backend] copy blocks full. flush data to slave. iterate cnt:%u, data size:%u"
                , iterate_count, result.block_size());
			break;
		}
        // ��ʱ
		if(time(NULL) - stime > 2){
			LOG(INFO)("[backend] copy blocks too long, flush");
			break;
		}
		
		if(!db_iter_->next()){
    		copy_num += copy_end(result);
			return copy_num;
		}
        
		leveldb::Slice key = db_iter_->key();
		if(key.ToString().size() == 0){
            LOG(DEBUG)("[backend] copy, skip empty key");
			continue;
		}
		leveldb::Slice val = db_iter_->val();
		last_key_ = key.ToString();
		char cmd = BinlogCommand::KSET;
		copy_num += 1;

        LOG(DEBUG)("[backend] copy a data to slave, last_seq:%lu.", last_seq);
		Binlog log(last_seq, BinlogType::COPY, cmd, key);
        result.AddRecord(log.repr(), val.ToString());
	}
	return copy_num;

}


// ���ؿ�������������
int SyncDataCmdThread::copy_end(ResultSyncMsg &result)
{
    const uint64_t &last_seq = last_seq_;

	LOG(INFO)("[backend] handle: %u, copy end", handle_);
	sync_flag_ = ON_SYNC;
    delete_iter();

	Binlog log(last_seq, BinlogType::COPY, BinlogCommand::END, "");
    result.AddRecord(log.repr(), "copy_end");
	return 1;
}


void SyncDataCmdThread::init_sync(const uint64_t &last_seq, const string &last_key, ResultSyncMsg &result)
{
    const char *type = "sync";
    this->last_seq_ = last_seq;
    this->last_key_ = last_key;
    LOG(INFO)("[backend] init sync. slave last seq:%lu, last key:%s.", last_seq_, last_key_.c_str());
	// a slave must reset its last_key when receiving 'copy_end' command
	if(last_key == "" && last_seq_ != 0){
		LOG(INFO)("[backend %s] handle: %d, sync recover, seq: %lu, key: '%s'",
			type,
			handle_,
			last_seq_, DATA2HEX_STR(last_key.data(), last_key.size())
			);
		this->sync_flag_ = ON_SYNC;
		
		Binlog log(last_seq_, BinlogType::COPY, BinlogCommand::END, "");
        result.AddRecord(log.repr(), "copy_end");
	}else if(last_key == "" && last_seq_ == 0){
	    type = "copy";
		LOG(INFO)("[backend %s] handle: %d, copy begin, seq: %lu, key: '%s'",
			type,
			handle_,
			last_seq_, DATA2HEX_STR(last_key.data(), last_key.size())
			);
		this->reset();
		Binlog log(last_seq_, BinlogType::COPY, BinlogCommand::BEGIN, "");
        result.AddRecord(log.repr(), "copy_begin");

        result.set_copy_begin_flag(BinlogCommand::BEGIN);
	}else{
	    type = "copy";
		LOG(INFO)("[backend %s] handle: %d, copy recover, seq: %lu, key: '%s'",
			type,
			handle_,
			last_seq_, DATA2HEX_STR(last_key.data(), last_key.size())
			);
		this->sync_flag_ = ON_COPY;
	}
}
    

NoBinlogIterator *SyncDataCmdThread::new_iter()
{
	if(db_iter_){
        LOG(ERROR)("[backend] failed. new db iter but iter is not null.");
        return db_iter_;
    }
	std::string key = last_key_;

    LeveldbEngine &db_engine = LeveldbEngine::inst();
	db_iter_ = db_engine.iterator(key, "", -1);

	LOG(INFO)("[backend] iterator created, last_key: '%s'"
        , DATA2HEX_STR(last_key_.data(), last_key_.size()));
    return db_iter_;
}

void SyncDataCmdThread::delete_iter()
{
	if(db_iter_){
        LOG(DEBUG)("[backend] delete db iter.");
		delete db_iter_;
		db_iter_ = NULL;
	}
}

void SyncDataCmdThread::reset()
{
	LOG(INFO)("[backend] handle:%u, copy begin", handle_);
	this->sync_flag_ = ON_COPY;
	this->last_seq_ = 0;
	this->last_key_ = "";
}


