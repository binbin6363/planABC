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
 *  ��ּ���ǿ��������ļ��������֣�һ����ȫ����������һ���ǲ����������״�����ͬ������ȫ��
 *  ͬ����֮��Ķ����������ܻ�ȫ��ͬ��Ҳ���ܻ����ͬ����
 *  1.�ͻ��˵���Ϊ:�������Ӻ�slave��������DETECTЭ�����̽�⣬DETECTЭ�������slave����ķ�����(32���ַ���)
 *    ��slave�����ݿ��պţ�
 *    ����˵���Ϊ:�жϷ������Ƿ���ҵ�һ�£�
 *    �����������һ�£���ظ��ܾ���
 *    ���������һ�£��������ж����ݿ��պ�(db_->GetSnapshot()->number_)�Ƿ���ҵ�ǰ�汾�����ݿ��պ��Ƿ�һ�£�
 *      �����ǰ���ݿ���һ�¾͸���slave���в���ͬ����
 *      �����ǰ���ݿ��ղ�һ�¾͸���slave����ȫ��ͬ��
 *  2.�ͻ��˷���ͬ��Э��
 *
 *
 *
**/

#ifndef __LEVELDB_SYNC_H__
#define __LEVELDB_SYNC_H__

#include <vector>
using namespace std;

// db����ͬ������
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

