/**
 * @filedesc: 
 * leveldb_sync.h, sync data
 * @author: 
 *  bbwang
 * @date: 
 *  2015/8/6 9:22:59
 * @modify:
 *
 * @desc:
 *  describe sync implements
 *  主旨就是拷贝数据文件，分两种，一种是全量拷贝，另一种是差量拷贝。首次连接同步采用全量
 *  同步，之后的短线重连可能会全量同步也可能会差量同步。
 *  1.客户端的行为:建立连接后，slave主动发送DETECT协议进行探测，DETECT协议包含本slave服务的服务名(32个字符串)
 *    和slave的数据快照号；
 *    服务端的行为:判断服务名是否和我的一致，
 *    如果服务名不一致，则回复拒绝；
 *    如果服务名一致，接下来判断数据快照号(db_->GetSnapshot()->number_)是否和我当前版本的数据快照号是否一致，
 *      如果当前数据快照一致就告诉slave进行差量同步，
 *      如果当前数据快照不一致就告诉slave进行全量同步
 *  2.客户端发送同步协议
 *
 *
 *
**/

#ifndef __LEVELDB_SYNC_H__
#define __LEVELDB_SYNC_H__

#include <vector>
using namespace std;

// db数据同步引擎
class LeveldbSyncEngine
{

struct File
{
    const char *full_file_name;
    uint32_t file_time_stamp;
};
typedef vector<File> FileList ;

public:


private:
    uint32_t ReadFile(const char *file_name, uint32_t offset, char *out_data, uint32_t out_len);
    uint32_t WriteFile(const char *file_name, uint32_t offset, char *in_data, uint32_t in_len);

    uint32_t GetFileLists(const char *dir_name, const char *suffix = NULL, const char *prefix = NULL);

private:
    FileList file_lists_;
    uint32_t file_index_;
    
};

#endif // __LEVELDB_SYNC_H__

