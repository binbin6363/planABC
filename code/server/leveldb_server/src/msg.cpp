/**
 * @filedesc: 
 * msg.cpp, all msg format
 * @author: 
 *  bbwang
 * @date: 
 *  2014/12/23 16:02:59
 * @modify:
 *
**/

#include "msg.h"


// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)num + [(string)key + (string)value + ...]
int RequestPutMsg::decode()
{
    int ret = 0;

    ONCE_LOOP_ENTER
    if (NULL == p_inpkg_)
        {
        LOG(ERROR)("RequestPutMsg, inpkg is null.");
        ret = LEVELDB_INNER_ERR;
        break;
        }
    *p_inpkg_ >> bizcmd_ >> transfer_str_ >> transid_;
    LOG(DEBUG)("RequestPutMsg, bizcmd:%u, transfer:%s, transid:%u"
        , bizcmd_, transfer_str_.c_str(), transid_);
    uint32_t num = 0;
    string key;
    string value;
    *p_inpkg_ >> num;
    if (num > MAX_NUM)
    {
        LOG(ERROR)("inpkg mybe error, num:%u is more than max num:%u", num, MAX_NUM);
        ret = LEVELDB_PKG_ERROR;
        break;
    }
    for (uint32_t i = 0; i < num; ++i)
    {
        key.clear(); value.clear();
        *p_inpkg_ >> key >> value;
        inputdatas_.insert(std::make_pair(key, value));
    }
    if (!(*p_inpkg_).good())
    {
        LOG(ERROR)("decode error!");
        ret = LEVELDB_PKG_ERROR;
    }
    ONCE_LOOP_LEAVE
    return ret;
}

// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)ret
int ResultPutMsg::encode()
{
    int ret = 0;
    if (outpkg_.m_head_len == 0)
    {
        outpkg_.offset_head(sizeof(COHEADER));
    }

    outpkg_ << get_cmd() << get_transfer() << get_transid() << get_ret();
    header_.len = outpkg_.length();
    LOG(DEBUG)("ResultPutMsg, encode, [%s] bizcmd:%u, transfer:%s, transid:%u, ret:%u"
        , header_.print(), get_cmd(), get_transfer().c_str(), get_transid(), get_ret());
    // be careful!!! reverse coid!!!
    header_.ReverseCoid();
    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("ResultPutMsg, encode error!");
        ret = LEVELDB_INNER_ERR;
    }
    return ret;
}


// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)num + [(string)key + ...]
int RequestGetMsg::decode()
{
    int ret = 0;

    ONCE_LOOP_ENTER
    if (NULL == p_inpkg_)
        {
        LOG(ERROR)("RequestGetMsg, inpkg is null.");
        ret = LEVELDB_INNER_ERR;
        break;
        }
    *p_inpkg_ >> bizcmd_ >> transfer_str_ >> transid_ >> num_;
    LOG(DEBUG)("RequestGetMsg, bizcmd:%u, transfer:%s, transid:%u, num:%u"
        , bizcmd_, transfer_str_.c_str(), transid_, num_);
    std::string key;
    if (num_ > MAX_NUM)
    {
        LOG(ERROR)("inpkg mybe error, num:%u is more than max num:%u", num_, MAX_NUM);
        ret = LEVELDB_PKG_ERROR;
        break;
    }
    for (uint32_t i = 0; i < num_; ++i)
    {
        key.clear();
        *p_inpkg_ >> key;
        inputdatas_.push_back(key);
    }
    if (!(*p_inpkg_).good())
    {
        LOG(ERROR)("RequestGetMsg, decode error!");
        ret = LEVELDB_PKG_ERROR;
    }
    ONCE_LOOP_LEAVE
    return ret;
}


// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)ret + (uint32_t)num + [(string)key + (string)value + ...]
int ResultGetMsg::encode()
{
    int ret = 0;
    if (outpkg_.m_head_len == 0)
    {
        outpkg_.offset_head(sizeof(COHEADER));
    }

    outpkg_ << get_cmd() << get_transfer() << get_transid() << get_ret();
    uint32_t num = kv_result_.size();
    outpkg_ << num;
    smciter key_iter = kv_result_.begin();
    smciter key_end = kv_result_.end();
    for ( ; key_iter != key_end; ++key_iter)
    {
        outpkg_ << key_iter->first << key_iter->second;
    }
    header_.len = outpkg_.length();
    LOG(DEBUG)("ResultGetMsg, encode, [%s] bizcmd:%u, transfer:%s, transid:%u, ret:%u, num:%u"
        , header_.print(), get_cmd(), get_transfer().c_str(), get_transid(), get_ret(), num);
    // be careful!!! reverse coid!!!
    header_.ReverseCoid();
    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("ResultGetMsg, encode error!");
        ret = LEVELDB_INNER_ERR;
    }
    return ret;
}


// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (string)key_start_perfix + (string)key_end_perfix + (uint32_t)limit_num
int RequestRangeGetMsg::decode()
{
    int ret = 0;

    ONCE_LOOP_ENTER
    if (NULL == p_inpkg_)
        {
        LOG(ERROR)("RequestRangeGetMsg, inpkg is null.");
        ret = LEVELDB_INNER_ERR;
        break;
        }
    *p_inpkg_ >> bizcmd_ >> transfer_str_ >> transid_ >> start_perfix_ >> end_perfix_ >> limit_num_;
    LOG(DEBUG)("RequestRangeGetMsg, bizcmd:%u, transfer:%s, transid:%u, start_perfix:%s, end_perfix:%s, limit:%u"
        , bizcmd_, transfer_str_.c_str(), transid_, start_perfix_.c_str(), end_perfix_.c_str(), limit_num_);
    if (!(*p_inpkg_).good())
    {
        LOG(ERROR)("RequestRangeGetMsg, decode error!");
        ret = LEVELDB_PKG_ERROR;
    }
    ONCE_LOOP_LEAVE
    return ret;
}

// 数据太大就需要下次再来取
// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)ret + (uint32_t)num + [(string)key + (string)value + ...] + (uint32_t)end_flag
int ResultRangeGetMsg::encode()
{
    int ret = 0;
    if (outpkg_.m_head_len == 0)
    {
        outpkg_.offset_head(sizeof(COHEADER));
    }

    outpkg_ << get_cmd() << get_transfer() << get_transid() << get_ret();
    uint32_t num = 0;
    uint32_t *pnum = (uint32_t *)outpkg_.getCur();
    outpkg_ << num;
    smciter key_iter = kv_result_.begin();
    smciter key_end = kv_result_.end();
    for ( ; key_iter != key_end; ++key_iter)
    {
        if (outpkg_.length() + key_iter->first.size() + key_iter->second.size() > MAX_SEND_BUFF_LEN - 400)
        {
            LOG(INFO)("data too large, reply data num:%u , need client continue request."
                , num);
            end_flag_ = 0; // this flag need client request continue
            break;
        }
        outpkg_ << key_iter->first << key_iter->second;
        ++num;
    }
    outpkg_ << end_flag_;
    // fill num
    *pnum = htonl(num);
    header_.len = outpkg_.length();
    // change header cmd to bizcmd
//    header_.cmd = get_cmd();
    LOG(DEBUG)("ResultRangeGetMsg, encode, [%s] bizcmd:%u, transfer:%s, transid:%u, ret:%u, limit:%u, reply num:%u, endflag:%u"
        , header_.print(), get_cmd(), get_transfer().c_str(), get_transid(), get_ret(), get_limit(), num, end_flag_);
    // be careful!!! reverse coid!!!
    header_.ReverseCoid();
    
    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("ResultRangeGetMsg, encode error!");
        ret = LEVELDB_INNER_ERR;
    }
    return ret;
}

// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (string)key_start_perfix + (string)key_end_perfix
int RequestRangeDelMsg::decode()
{
    int ret = 0;

    ONCE_LOOP_ENTER
    if(NULL == p_inpkg_)
    {
        LOG(ERROR)("RequestRangeDelMsg, inpkg is null.");
        ret = LEVELDB_INNER_ERR;
        break;
    }

    *p_inpkg_ >> bizcmd_ >> transfer_str_ >> transid_ >> start_perfix_ >> end_perfix_;
    
    LOG(DEBUG)("RequestRangeDelMsg, bizcmd:%u, transfer:%s, transid:%u, start_perfix:%s, end_perfix:%s"
        , bizcmd_, transfer_str_.c_str(), transid_, start_perfix_.c_str(), end_perfix_.c_str());
    
    if(!(*p_inpkg_).good())
    {
        LOG(ERROR)("RequestRangeDelMsg, decode error!");
        ret = LEVELDB_PKG_ERROR;
    }

    ONCE_LOOP_LEAVE
    return ret;
}

// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid + (uint32_t)ret
int ResultRangeDelMsg::encode()
{
    int ret = 0;
    if (outpkg_.m_head_len == 0)
    {
        outpkg_.offset_head(sizeof(COHEADER));
    }
    outpkg_ << get_cmd() << get_transfer() << get_transid() << get_ret();
    
    header_.len = outpkg_.length();
    
    // be careful!!! reverse coid!!!
    header_.ReverseCoid();

    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("ResultRangeDelMsg, encode error!");
        ret = LEVELDB_INNER_ERR;
    }
    
    LOG(INFO)("ResultRangeDelMsg, encode, [%s] bizcmd:%u, transfer:%s, transid:%u, ret:%u", header_.print(),
        get_cmd(), get_transfer().c_str(), get_transid(), get_ret());

    return ret;
}

// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)num + [(string)key + ...]
int RequestDelMsg::decode()
{
    int ret = 0;

    ONCE_LOOP_ENTER
    if (NULL == p_inpkg_)
        {
        LOG(ERROR)("RequestDelMsg, inpkg is null.");
        ret = LEVELDB_INNER_ERR;
        break;
        }
    *p_inpkg_ >> bizcmd_ >> transfer_str_ >> transid_ >> num_;
    LOG(DEBUG)("RequestDelMsg, bizcmd:%u, transfer:%s, transid:%u, num:%u"
        , bizcmd_, transfer_str_.c_str(), transid_, num_);
    std::string key;
    if (num_ > MAX_NUM)
    {
        LOG(ERROR)("inpkg mybe error, num:%u is more than max num:%u", num_, MAX_NUM);
        ret = LEVELDB_PKG_ERROR;
        break;
    }
    for (uint32_t i = 0; i < num_; ++i)
    {
        key.clear();
        *p_inpkg_ >> key;
        inputdatas_.push_back(key);
    }
    if (!(*p_inpkg_).good())
    {
        LOG(ERROR)("RequestDelMsg, decode error!");
        ret = LEVELDB_PKG_ERROR;
    }
    ONCE_LOOP_LEAVE
    return ret;
}

// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)ret
int ResultDelMsg::encode()
{
    int ret = 0;
    if (outpkg_.m_head_len == 0)
    {
        outpkg_.offset_head(sizeof(COHEADER));
    }

    outpkg_ << get_cmd() << get_transfer() << get_transid() << get_ret();
    header_.len = outpkg_.length();
    LOG(DEBUG)("ResultDelMsg, encode, [%s] bizcmd:%u, transfer:%s, transid:%u, ret:%u"
        , header_.print(), get_cmd(), get_transfer().c_str(), get_transid(), get_ret());
    // be careful!!! reverse coid!!!
    header_.ReverseCoid();
    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("ResultDelMsg, encode error!");
        ret = LEVELDB_INNER_ERR;
    }
    return ret;

}


// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)num + [(string)key + (string)value + ...]
int RequestGetAndPutMsg::decode()
{
    int ret = 0;

    ONCE_LOOP_ENTER
    if (NULL == p_inpkg_)
        {
        LOG(ERROR)("RequestGetAndPutMsg, inpkg is null.");
        ret = LEVELDB_INNER_ERR;
        break;
        }
    *p_inpkg_ >> bizcmd_ >> transfer_str_ >> transid_;
    LOG(DEBUG)("RequestGetAndPutMsg, bizcmd:%u, transfer:%s, transid:%u"
        , bizcmd_, transfer_str_.c_str(), transid_);
    uint32_t num = 0;
    string key;
    string value;
    *p_inpkg_ >> num;
    if (num > MAX_NUM)
    {
        LOG(ERROR)("inpkg mybe error, num:%u is more than max num:%u", num, MAX_NUM);
        ret = LEVELDB_PKG_ERROR;
        break;
    }
    for (uint32_t i = 0; i < num; ++i)
    {
        key.clear(); value.clear();
        *p_inpkg_ >> key >> value;
        kv_datas_.insert(std::make_pair(key, value));
    }
    if (!(*p_inpkg_).good())
    {
        LOG(ERROR)("decode error!");
        ret = LEVELDB_PKG_ERROR;
    }
    ONCE_LOOP_LEAVE
    return ret;
}



// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)ret + (uint32_t)num + [(string)key + (string)value + ...]
int ResultGetAndPutMsg::encode()
{
    int ret = 0;
    if (outpkg_.m_head_len == 0)
    {
        outpkg_.offset_head(sizeof(COHEADER));
    }

    outpkg_ << get_cmd() << get_transfer() << get_transid() << get_ret();
    uint32_t num = kv_result_.size();
    outpkg_ << num;
    smciter key_iter = kv_result_.begin();
    smciter key_end = kv_result_.end();
    for ( ; key_iter != key_end; ++key_iter)
    {
        outpkg_ << key_iter->first << key_iter->second;
    }
    header_.len = outpkg_.length();
    LOG(DEBUG)("ResultGetAndPutMsg, encode, [%s] bizcmd:%u, transfer:%s, transid:%u, ret:%u, num:%u"
        , header_.print(), get_cmd(), get_transfer().c_str(), get_transid(), get_ret(), num);
    // be careful!!! reverse coid!!!
    header_.ReverseCoid();
    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("ResultGetAndPutMsg, encode error!");
        ret = LEVELDB_INNER_ERR;
    }
    return ret;
}


// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)num + [(string)key ] + (string)value
int RequestPutBySameValueMsg::decode()
{
    int ret = 0;

    ONCE_LOOP_ENTER
    if (NULL == p_inpkg_)
        {
        LOG(ERROR)("RequestPutBySameValueMsg, inpkg is null.");
        ret = LEVELDB_INNER_ERR;
        break;
        }
    *p_inpkg_ >> bizcmd_ >> transfer_str_ >> transid_;
    LOG(DEBUG)("RequestPutBySameValueMsg, bizcmd:%u, transfer:%s, transid:%u"
        , bizcmd_, transfer_str_.c_str(), transid_);
    uint32_t num = 0;
    string key;
    string value;
    *p_inpkg_ >> num;
    if (num > MAX_NUM)
    {
        LOG(ERROR)("inpkg mybe error, num:%u is more than max num:%u", num, MAX_NUM);
        ret = LEVELDB_PKG_ERROR;
        break;
    }
    for (uint32_t i = 0; i < num; ++i)
    {
        key.clear(); value.clear();
        *p_inpkg_ >> key ;
        inputdatas_[key] = "";
    }
    *p_inpkg_ >> value ;
     if (!(*p_inpkg_).good())
    {
        LOG(ERROR)("decode error!");
        ret = LEVELDB_PKG_ERROR;
    }
    for(smiter it = inputdatas_.begin(); it != inputdatas_.end(); ++it)
    {
        it->second = value;
    }

    ONCE_LOOP_LEAVE
    return ret;
}

// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)transid 
// + (uint32_t)ret
int ResultPutBySameValueMsg::encode()
{
    int ret = 0;
    if (outpkg_.m_head_len == 0)
    {
        outpkg_.offset_head(sizeof(COHEADER));
    }

    outpkg_ << get_cmd() << get_transfer() << get_transid() << get_ret();
    header_.len = outpkg_.length();
    LOG(DEBUG)("ResultPutBySameValueMsg, encode, [%s] bizcmd:%u, transfer:%s, transid:%u, ret:%u"
        , header_.print(), get_cmd(), get_transfer().c_str(), get_transid(), get_ret());
    // be careful!!! reverse coid!!!
    header_.ReverseCoid();
    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("ResultPutBySameValueMsg, encode error!");
        ret = LEVELDB_INNER_ERR;
    }
    return ret;
}


// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint64_t)last_seq 
// + (string)last_key + (string)signature
int RequestSyncMsg::decode()
{
    int ret = 0;

    ONCE_LOOP_ENTER
    if (NULL == p_inpkg_)
    {
        LOG(ERROR)("RequestSyncMsg, inpkg is null.");
        ret = LEVELDB_INNER_ERR;
        break;
    }
    *p_inpkg_ >> bizcmd_ >> transfer_str_ >> last_seq_ >> last_key_ >> signature_;
    LOG(DEBUG)("RequestSyncMsg, bizcmd:%u, transfer:'%s', last_seq:%lu, last_key:'%s'."
        , bizcmd_, transfer_str_.c_str(), last_seq_, last_key_.c_str());
    LOG_HEX(p_inpkg_->getData(), p_inpkg_->size(), utils::L_INFO);

    if (!(*p_inpkg_).good())
    {
        LOG(ERROR)("RequestSyncMsg, decode error!");
        ret = LEVELDB_PKG_ERROR;
    }
    ONCE_LOOP_LEAVE
    return ret;

}


// output protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)ret 
// + (uint32_t)num + [(string)key + (string)value + ...] 
// + (uint32_t)check_sum
int ResultSyncMsg::encode()
{
    int ret = 0;
    if (outpkg_.m_head_len == 0)
    {
        outpkg_.offset_head(sizeof(COHEADER));
    }

    // 校验和计算数据的起点终点
    const char *check_start = NULL;
    const char *check_end = NULL;
    
    outpkg_ << get_cmd() << get_transfer() << get_ret();
    uint32_t num = 0;
    uint32_t *pnum = (uint32_t *)outpkg_.getCur();
    outpkg_ << num;
    check_start = outpkg_.getCur();
    RcdCIter record_iter = records_.begin();
    RcdCIter record_end = records_.end();
    for ( ; record_iter != record_end; ++record_iter)
    {
        // 这个条件基本不会遇到。遇到就是灾难，丢数据
        // 16 = 2 * len + 2 * 2zero + check_sum
        if (outpkg_.length() + record_iter->key.size() + record_iter->value.size() + 16 > outpkg_.size())
        {
            LOG(ERROR)("data too large, reply num:%u, total num:%zu, lost data. cur pkg len:%u, pkg size:%u."
                , num, records_.size(), outpkg_.length(), outpkg_.size());
            break;
        }
        outpkg_ << record_iter->key << record_iter->value;
        ++num;
    }
    check_end = outpkg_.getCur();
    // 暂时未计算校验和，占位
    outpkg_ << check_sum_;
    // fill num
    *pnum = htonl(num);
    header_.len = outpkg_.length();
    LOG(DEBUG)("ResultSyncMsg, encode, [%s] bizcmd:%u, transfer:%s, ret:%u, sync data num:%u"
        , header_.print(), get_cmd(), get_transfer().c_str(), get_ret(), num);
    // be careful!!! reverse coid!!!
    header_.ReverseCoid();
    
    outpkg_.set_head(header_);
    if (!outpkg_.good())
    {
        LOG(ERROR)("ResultSyncMsg, encode error!");
        ret = LEVELDB_INNER_ERR;
    }
    return ret;

}


// 这里的key是Binlog的值，包含比较多的信息
int ResultSyncMsg::AddRecord(const string &key, const string &value)
{
    // 性能问题暂不考虑
    Record rcd;
    rcd.key = key;
    rcd.value = value;
    records_.push_back(rcd);
        
    block_size_ += (key.size() + value.size() + 12);
    return 0;
}


// input protocol: (uint32_t)bizcmd + (string)transfer_str + (uint32_t)ret 
// + (uint32_t)num + [(string)key + (string)value + ...] 
// + (uint32_t)check_sum
int SyncResponseMsg::decode()
{
    int ret = 0;

    ONCE_LOOP_ENTER
    if (NULL == p_inpkg_)
    {
        LOG(ERROR)("RequestSyncMsg, inpkg is null.");
        ret = LEVELDB_INNER_ERR;
        break;
    }
    *p_inpkg_ >> bizcmd_ >> transfer_str_ >> ret_ >> record_num_;
    LOG(DEBUG)("SyncResponseMsg, bizcmd:%u, transfer:%s, ret:%u, record num:%u."
        , bizcmd_, transfer_str_.c_str(), ret_, record_num_);
    
    LOG_HEX(p_inpkg_->getData(), p_inpkg_->size(), utils::L_DEBUG);
    if (record_num_ > 1024 * 1024) {
        LOG(ERROR)("SyncResponseMsg, record num is too large, maybe inpkg is error.");
        ret = LEVELDB_PKG_ERROR;
        break;
    }

    if (!p_inpkg_->good())
    {
        LOG(ERROR)("SyncResponseMsg, decode error!");
        ret = LEVELDB_PKG_ERROR;
        break;
    }

    Record one_record;
    for (uint32_t i = 0; i < record_num_; ++i){
        *p_inpkg_ >> one_record.key >> one_record.value;
        records_.push_back(one_record);
    }

    if (!p_inpkg_->good())
    {
        LOG(ERROR)("SyncResponseMsg, decode error!");
        ret = LEVELDB_PKG_ERROR;
        break;
    }

    ONCE_LOOP_LEAVE
    return ret;

}


