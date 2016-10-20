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
#include <google/protobuf/text_format.h> // header for TextFormat

void PrintPbData(const Message &message, const char *desc)
{
    // log protobuf info
    const char *tag = "print protobuf info:";
    if (NULL != desc)
    {
        tag = desc;
    }
    std::string printer;
    TextFormat::PrintToString(message, &printer);
    LOG(DEBUG)("%s\n%s", tag, printer.c_str());
}




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
    LOG(INFO)("RequestPutMsg, bizcmd:%u, transfer:%s, transid:%u"
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
    LOG(INFO)("RequestGetMsg, bizcmd:%u, transfer:%s, transid:%u, num:%u"
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
    LOG(INFO)("RequestRangeGetMsg, bizcmd:%u, transfer:%s, transid:%u, start_perfix:%s, end_perfix:%s, limit:%u"
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
    header_.cmd = get_cmd();
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
    LOG(INFO)("RequestDelMsg, bizcmd:%u, transfer:%s, transid:%u, num:%u"
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
    LOG(INFO)("RequestGetAndPutMsg, bizcmd:%u, transfer:%s, transid:%u"
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

