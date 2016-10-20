#ifndef serverconfig_h__
#define serverconfig_h__
#include "comm.h"
#include "tinyxml.h"
#include <vector>
#include <string>
#include <list>
#include <map>
#include "binpacket_wrap.h"
#include "constants.h"
#include "errcodes.h"
#include "protocols.h"

using namespace common;
using namespace utils;
using namespace std;


namespace utils{



class ServerConfig
{

// ensure singleton
private:
    ServerConfig(void);
    ServerConfig &operator=(const ServerConfig &other);
    ServerConfig(const ServerConfig &other);

public:
    ~ServerConfig(void);
    static ServerConfig *Instance()
    {
        static ServerConfig server_config_;
        return &server_config_;
    }

    bool reset_config();
    bool init_log();
    bool load_sys_info();
    bool load_daemon_info();
    bool load_servers_info();
    // load server info, copy the 3rd parameters, can not modify generate info 
    bool load_server_info(const char *section, vector<Server_Info> &server_info, Server_Info default_info);
    // load ip and port info
    bool load_addr_info(const char *section, string &addr);
    bool load_leveldb_info();
    bool load_slave_info();
    bool load_allow_list();
    
    string udp_cmd_addr() {return udp_cmd_addr_;}

    // get server info
    const vector<Server_Info> &server_info(const char *server_name) const;// {return vtImdInfo_;}
    
    bool IsAllow(const string &remote_ip);
    string file_path();
    bool  load_file(string file);
    bool load_config();
    
    uint32_t check_timeout_interval() const {return check_timeout_interval_;};
    void  check_timeout_interval(uint32_t check_timeout_interval){check_timeout_interval_ = check_timeout_interval;};

    
    const string &ldb_ip() const {return ldb_ip_;}
    const string &ldb_port() const {return ldb_port_;}
    const string &for_slave_ip() const {return for_slave_ip_;}
    const string &for_slave_port() const {return for_slave_port_;}
    bool isDaemon() const {return daemon_ !=0 ? true:false;}
    uint32_t client_ttl() const {return client_ttl_;}
    uint32_t slave_ttl() const {return slave_ttl_;}

    
    string dbpath() {return ldb_dbpath_;}
    const char *ldb_create_if_missing() {return ldb_create_if_missing_.c_str();}
    const char *ldb_error_if_exists() {return ldb_error_if_exists_.c_str();}
    const char *ldb_paranoid_checks() {return ldb_paranoid_checks_.c_str();}
    uint32_t ldb_write_buffer_size() {return ldb_write_buffer_size_;}
    uint32_t ldb_max_open_files() {return ldb_max_open_files_;}
    uint32_t ldb_block_size() {return ldb_block_size_;}
    uint32_t ldb_block_restart_interval() {return ldb_block_restart_interval_;}
    uint32_t ldb_bits_per_key() {return ldb_bits_per_key_;}
    uint32_t ldb_cache_size() {return ldb_cache_size_;}
    uint32_t ldb_compression_type() {return ldb_compression_type_;}
    int sync_speed_limit() {return sync_speed_limit_;}
    bool write_log(){return write_log_;}

    string  server_id() {return server_id_;}
    // buffer size setting, key:client, imd, dbproxyd, qgroupd, ngroupd, qgroupd
    static map<string, uint32_t> max_buffer_size;
    
    
 private:
    string                    cfg_file_path_;
    TiXmlElement              *RootElement;
    TiXmlDocument             *appConfig;
    list<string>              allow_list_;
    Server_Info               tGeneralInfo_;

    vector<Server_Info>       vtLevelDbInfo_;
    vector<Server_Info>       vtMasterInfo_;
    uint32_t                  check_timeout_interval_;      //会话超时时间,单位s
    string                    ldb_ip_;
    string                    ldb_port_;
    string                    for_slave_ip_;  // slave 连接的ip
    string                    for_slave_port_;// slave 连接的端口
    string                    server_id_;

    uint32_t                  client_ttl_;
    uint32_t                  slave_ttl_;
    
    uint32_t                  daemon_; // 1 daemon, 0 not daemon

//    string                    udp_addr_;
//    string                    udp_log_addr_;
    string                    udp_cmd_addr_;

    string                    ldb_dbpath_;
    string                    ldb_create_if_missing_;
    string                    ldb_error_if_exists_;
    string                    ldb_paranoid_checks_;

    // -------------------
    // Parameters that affect performance as follows

    // Amount of data to build up in memory (backed by an unsorted log
    // on disk) before converting to a sorted on-disk file.
    //
    // Larger values increase performance, especially during bulk loads.
    // Up to two write buffers may be held in memory at the same time,
    // so you may wish to adjust this parameter to control memory usage.
    // Also, a larger write buffer will result in a longer recovery time
    // the next time the database is opened.
    //
    // Default: 4MB
    uint32_t                  ldb_write_buffer_size_;

    // Number of open files that can be used by the DB.  You may need to
    // increase this if your database has a large working set (budget
    // one open file per 2MB of working set).
    //
    // Default: 1000
    uint32_t                  ldb_max_open_files_;
    
    // Approximate size of user data packed per block.  Note that the
    // block size specified here corresponds to uncompressed data.  The
    // actual size of the unit read from disk may be smaller if
    // compression is enabled.  This parameter can be changed dynamically.
    //
    // Default: 4K
    uint32_t                  ldb_block_size_;

    // Number of keys between restart points for delta encoding of keys.
    // This parameter can be changed dynamically.  Most clients should
    // leave this parameter alone.
    //
    // Default: 16
    uint32_t                  ldb_block_restart_interval_;

    // Compress blocks using the specified compression algorithm.  This
    // parameter can be changed dynamically.
    //
    // Default: kSnappyCompression, which gives lightweight but fast
    // compression.
    //
    // Typical speeds of kSnappyCompression on an Intel(R) Core(TM)2 2.4GHz:
    //    ~200-500MB/s compression
    //    ~400-800MB/s decompression
    // Note that these speeds are significantly faster than most
    // persistent storage speeds, and therefore it is typically never
    // worth switching to kNoCompression.  Even if the input data is
    // incompressible, the kSnappyCompression implementation will
    // efficiently detect that and will switch to uncompressed mode.
    uint32_t                  ldb_compression_type_;

    // If non 0, use the specified filter policy to reduce disk reads.
    // Many applications will benefit from passing the result of
    // NewBloomFilterPolicy() here.
    //
    // Default: 10
    uint32_t                  ldb_bits_per_key_;

    // Control over blocks (user data is stored in a set of blocks, and
    // a block is the unit of reading from disk).
    // If non 0, use the specified cache for blocks.
    // If 0, leveldb will automatically create and use an 8MB internal cache.
    // Default: 0
    uint32_t                  ldb_cache_size_;

    // 同步限速,-1表示不限速
    int                       sync_speed_limit_;

    // 是否写binlog
    bool                      write_log_;

};



}
#endif // serverconfig_h__

