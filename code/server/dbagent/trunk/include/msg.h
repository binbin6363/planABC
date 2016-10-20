/**
 * @filedesc: 
 * msg.h, all msg format
 * @author: 
 *  bbwang
 * @date: 
 *  2014/12/23 16:02:59
 * @modify:
 *
**/
#include "comm.h"
#include "data_type.h"
#include "log.h"
#include "binpacket_wrap.h"
#include "constants.h"

using namespace utils;
using namespace common;

//typedef com::adv:msg::MysqlRequest     QueryRequest;
//typedef com::adv:msg::MysqlResult      QueryResult;

namespace google{
namespace protobuf {
	class Message;
}
}

using namespace google::protobuf;

void PrintPbData(const Message &message, const char *desc);


class Msg
{
public:
    Msg(BinInputPacket<> &inpkg)
        {
        if (inpkg.m_head_len == 0)
        {
            inpkg.offset_head(sizeof(COHEADER));
        }
        inpkg.get_head(header_);
        if (!inpkg.good())
        {
            LOG(ERROR)("msg decode header failed. maybe did not call inpkg.offset_head().");
        }
        LOG(DEBUG)("msg head: [%s]", header_.print());
        }
    
    Msg()
        {
        memset(&header_, 0, sizeof(COHEADER));
		header_.head_len = sizeof(COHEADER);
        }
    virtual ~Msg()
        {
        }

    virtual int decode()
        {
        LOG(ERROR)("virtual! decode error!");
        return 1;
        }
    virtual int encode()
        {
        LOG(ERROR)("virtual! encode error!");
        return 1;
        }

    virtual char *getData()
        {
        LOG(ERROR)("virtual! getData error!");
        return NULL;
        }
    virtual uint32_t length()
        {
        LOG(ERROR)("virtual! length error!");
        return 0;
        }
    
    virtual const std::string &get_transfer()
        {
        static std::string null_str = "";
        LOG(ERROR)("virtual! get_transfer error!");
        return null_str;
        }
    
public:
    COHEADER        header_;
    
};


// common
class RequestPutMsg : public Msg
{
public:
    RequestPutMsg(BinInputPacket<> &inpkg)
        : Msg(inpkg)
        , p_inpkg_(&inpkg)
        , bizcmd_(0)
        , transid_(0)
        {}
    virtual ~RequestPutMsg()
        {}

    virtual int decode();
    const strmap &datas()
        {
        return inputdatas_;
        }
    virtual const std::string &get_transfer()
        {
	return transfer_str_;
        }
    uint32_t get_cmd()
        {
        return bizcmd_;
        }
    uint32_t get_transid()
        {
        return transid_;
        }
        
    
private:
    BinInputPacket<> *p_inpkg_;
    uint32_t bizcmd_;
    strmap inputdatas_;
    std::string transfer_str_;
    uint32_t transid_;
};

class ResultPutMsg : public Msg
{
public:
    ResultPutMsg(BinOutputPacket<> &outpkg, Msg &request)
        : outpkg_(outpkg)
        , ret_(0)
        {
        p_request_ = &request;
        header_ = request.header_;
        }

    virtual ~ResultPutMsg()
        {
        }

    virtual int encode();
    uint32_t get_cmd()
        {
        uint32_t bizcmd = 0;
	    RequestPutMsg *request = dynamic_cast<RequestPutMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultPutMsg, request_ is NULL");
            bizcmd = 0;
            }
        else
            {
            bizcmd = request->get_cmd();
            }
        return bizcmd;
        }
    virtual const std::string &get_transfer()
        {
	    RequestPutMsg *request = dynamic_cast<RequestPutMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            static std::string null_str = "";
            LOG(ERROR)("ResultPutMsg, request_ is NULL");
            return null_str;
            }
        return request->get_transfer();
        }
    uint32_t get_ret()
        {
        return ret_;
        }
    uint32_t get_transid()
        {
        uint32_t transid = 0;
	    RequestPutMsg *request = dynamic_cast<RequestPutMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultPutMsg, request_ is NULL");
            transid = 0;
            }
        else
            {
            transid = request->get_transid();
            }
        return transid;
        }
    
    virtual char *getData()
        {
        return outpkg_.getData();
        }
    virtual uint32_t length()
        {
        return outpkg_.length();
        }
    void err_code(uint32_t errrcode)
        {
        ret_ = errrcode;
        }

private:
    Msg                  *p_request_;
    BinOutputPacket<>    outpkg_;
    uint32_t             ret_;
};



class RequestGetMsg : public Msg
{
public:
    RequestGetMsg(BinInputPacket<> &inpkg)
        : Msg(inpkg)
        , p_inpkg_(&inpkg)
        , bizcmd_(0)
        , num_(0)
        {
        }
    virtual ~RequestGetMsg()
        {
        }

    virtual int decode();
    const strvec &datas()
        {
        return inputdatas_;
        }
     virtual const std::string &get_transfer()
         {
	     return transfer_str_;
         }
    uint32_t get_cmd()
        {
        return bizcmd_;
        }
    uint32_t get_transid()
        {
        return transid_;
        }
        
        
private:
    BinInputPacket<>    *p_inpkg_;
    uint32_t            bizcmd_;
    uint32_t            num_;
    strvec              inputdatas_;
    std::string         transfer_str_;
    uint32_t            transid_;
};


class ResultGetMsg : public Msg
{
public:
    ResultGetMsg(BinOutputPacket<> &outpkg, Msg &request)
        : outpkg_(outpkg)
        , ret_(0)
        {
        p_request_ = &request;
        header_ = request.header_;
        }

    virtual ~ResultGetMsg()
        {
        }

    virtual int encode();
    virtual char *getData()
        {
        return outpkg_.getData();
        }
    virtual uint32_t length()
        {
        return outpkg_.length();
        }
    strmap &kvdata()
        {
        return kv_result_;
        }
    uint32_t get_cmd()
        {
        uint32_t bizcmd = 0;
	    RequestGetMsg *request = dynamic_cast<RequestGetMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultGetMsg, request_ is NULL");
            bizcmd = 0;
            }
        else
            {
            bizcmd = request->get_cmd();
            }
        return bizcmd;
        }
    virtual const std::string &get_transfer()
        {
	    RequestGetMsg *request = dynamic_cast<RequestGetMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            static std::string null_str = "";
            LOG(ERROR)("ResultGetMsg, request_ is NULL");
            return null_str;
            }
        return request->get_transfer();
        }
    uint32_t get_transid()
        {
        uint32_t transid = 0;
	    RequestGetMsg *request = dynamic_cast<RequestGetMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultGetMsg, request_ is NULL");
            transid = 0;
            }
        else
            {
            transid = request->get_transid();
            }
        return transid;
        }
    uint32_t get_ret()
        {
        return ret_;
        }
    
    void err_code(uint32_t errrcode)
	{
	ret_ = errrcode;
	}

private:
    Msg                  *p_request_;
    strmap               kv_result_;
    BinOutputPacket<>    outpkg_;
    uint32_t             ret_;
};


class RequestRangeGetMsg : public Msg
{
public:
    RequestRangeGetMsg(BinInputPacket<> &inpkg)
        : Msg(inpkg)
        , p_inpkg_(&inpkg)
        , bizcmd_(0)
        , num_(0)
        , transfer_str_("")
        , transid_(0)
        , start_perfix_("")
        , end_perfix_("")
        , limit_num_(20)
        {
        }
    virtual ~RequestRangeGetMsg()
        {
        }

    virtual int decode();
    const std::string &key_start_perfix()
        {
        return start_perfix_;
        }
    const std::string &key_end_perfix()
        {
        return end_perfix_;
        }
     virtual const std::string &get_transfer()
         {
	     return transfer_str_;
         }
    uint32_t get_cmd()
        {
        return bizcmd_;
        }
    uint32_t get_transid()
        {
        return transid_;
        }
    uint32_t get_limit()
        {
        return limit_num_;
        }
        
private:
    BinInputPacket<>    *p_inpkg_;
    uint32_t            bizcmd_;
    uint32_t            num_;
    std::string         transfer_str_;
    uint32_t            transid_;
    std::string         start_perfix_;
    std::string         end_perfix_;
    uint32_t            limit_num_; // Ä¬ÈÏÊÇ20
};


class ResultRangeGetMsg : public Msg
{
public:
    ResultRangeGetMsg(BinOutputPacket<> &outpkg, Msg &request)
        : outpkg_(outpkg)
        , ret_(0)
        , end_flag_(1)
        {
        p_request_ = &request;
        header_ = request.header_;
        }

    virtual ~ResultRangeGetMsg()
        {
        }

    virtual int encode();
    virtual char *getData()
        {
        return outpkg_.getData();
        }
    virtual uint32_t length()
        {
        return outpkg_.length();
        }
    strmap &kvdata()
        {
        return kv_result_;
        }
    uint32_t get_cmd()
        {
        uint32_t bizcmd = 0;
	    RequestRangeGetMsg *request = dynamic_cast<RequestRangeGetMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultRangeGetMsg, request_ is NULL");
            bizcmd = 0;
            }
        else
            {
            bizcmd = request->get_cmd();
            }
        return bizcmd;
        }
    virtual const std::string &get_transfer()
        {
	    RequestRangeGetMsg *request = dynamic_cast<RequestRangeGetMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            static std::string null_str = "";
            LOG(ERROR)("ResultRangeGetMsg, request_ is NULL");
            return null_str;
            }
        return request->get_transfer();
        }
    uint32_t get_transid()
        {
        uint32_t transid = 0;
	    RequestRangeGetMsg *request = dynamic_cast<RequestRangeGetMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultRangeGetMsg, request_ is NULL");
            transid = 0;
            }
        else
            {
            transid = request->get_transid();
            }
        return transid;
        }
    uint32_t get_ret()
        {
        return ret_;
        }
    uint32_t get_limit()
        {
        uint32_t max_num = 0;
	    RequestRangeGetMsg *request = dynamic_cast<RequestRangeGetMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultRangeGetMsg, request_ is NULL");
            max_num = MAX_SYNC_MSG_NUM;
            }
        else
            {
            max_num = request->get_limit();
            }
        return max_num;
        }
    void err_code(uint32_t errrcode)
	{
	ret_ = errrcode;
	}
    uint32_t &end_flag()
    {
        return end_flag_;
    }

private:
    Msg                  *p_request_;
    strmap               kv_result_;
    BinOutputPacket<>    outpkg_;
    uint32_t             ret_;
    uint32_t             end_flag_;
};





class RequestDelMsg : public Msg
{
public:
    RequestDelMsg(BinInputPacket<> &inpkg)
        : Msg(inpkg)
        , p_inpkg_(&inpkg)
        , bizcmd_(0)
        , num_(0)
        , transid_(0)
        {}
    virtual ~RequestDelMsg()
        {}

    virtual int decode();
    const strvec &datas()
        {
        return inputdatas_;
        }
    virtual const std::string &get_transfer()
        {
	return transfer_str_;
        }
    uint32_t get_cmd()
        {
        return bizcmd_;
        }
    uint32_t get_transid()
        {
        return transid_;
        }
        
    
private:
    BinInputPacket<> *p_inpkg_;
    uint32_t         bizcmd_;
    uint32_t         num_;
    strvec           inputdatas_;
    std::string      transfer_str_;
    uint32_t         transid_;
};



class ResultDelMsg : public Msg
{
public:
    ResultDelMsg(BinOutputPacket<> &outpkg, Msg &request)
        : outpkg_(outpkg)
        , ret_(0)
        {
        p_request_ = &request;
        header_ = request.header_;
        }

    virtual ~ResultDelMsg()
        {
        }

    virtual int encode();
    virtual char *getData()
        {
        return outpkg_.getData();
        }
    virtual uint32_t length()
        {
        return outpkg_.length();
        }
    uint32_t get_cmd()
        {
        uint32_t bizcmd = 0;
	    RequestDelMsg *request = dynamic_cast<RequestDelMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultDelMsg, request_ is NULL");
            bizcmd = 0;
            }
        else
            {
            bizcmd = request->get_cmd();
            }
        return bizcmd;
        }
    virtual const std::string &get_transfer()
        {
	    RequestDelMsg *request = dynamic_cast<RequestDelMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            static std::string null_str = "";
            LOG(ERROR)("ResultDelMsg, request_ is NULL");
            return null_str;
            }
        return request->get_transfer();
        }
    uint32_t get_transid()
        {
        uint32_t transid = 0;
	    RequestDelMsg *request = dynamic_cast<RequestDelMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultDelMsg, request_ is NULL");
            transid = 0;
            }
        else
            {
            transid = request->get_transid();
            }
        return transid;
        }
    uint32_t get_ret()
        {
        return ret_;
        }
    
    void err_code(uint32_t errrcode)
	{
	ret_ = errrcode;
	}

private:
    Msg                  *p_request_;
    strvec               inputdatas_;
    BinOutputPacket<>    outpkg_;
    uint32_t             ret_;
};


class RequestGetAndPutMsg : public Msg
{
public:
    RequestGetAndPutMsg(BinInputPacket<> &inpkg)
        : Msg(inpkg)
        , p_inpkg_(&inpkg)
        , bizcmd_(0)
        , num_(0)
        {
        }
    virtual ~RequestGetAndPutMsg()
        {
        }

    virtual int decode();

    const strvec &key_datas()
        {
        inputdatas_.clear();
        smciter sm_iter = kv_datas_.begin();
        smciter end_iter = kv_datas_.end();
        for (; sm_iter != end_iter; ++sm_iter)
            {
            inputdatas_.push_back(sm_iter->first);
            }
        return inputdatas_;
        }
    const strmap &datas()
        {
        return kv_datas_;
        }
     virtual const std::string &get_transfer()
         {
	     return transfer_str_;
         }
    uint32_t get_cmd()
        {
        return bizcmd_;
        }
    uint32_t get_transid()
        {
        return transid_;
        }
        
        
private:
    BinInputPacket<>    *p_inpkg_;
    uint32_t            bizcmd_;
    uint32_t            num_;
    strmap              kv_datas_;
    strvec              inputdatas_;
    std::string         transfer_str_;
    uint32_t            transid_;
};


class ResultGetAndPutMsg : public Msg
{
public:
    ResultGetAndPutMsg(BinOutputPacket<> &outpkg, Msg &request)
        : outpkg_(outpkg)
        , ret_(0)
        {
        p_request_ = &request;
        header_ = request.header_;
        }

    virtual ~ResultGetAndPutMsg()
        {
        }

    virtual int encode();
    virtual char *getData()
        {
        return outpkg_.getData();
        }
    virtual uint32_t length()
        {
        return outpkg_.length();
        }
    strmap &kvdata()
        {
        return kv_result_;
        }
    uint32_t get_cmd()
        {
        uint32_t bizcmd = 0;
	    RequestGetAndPutMsg *request = dynamic_cast<RequestGetAndPutMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultGetAndPutMsg, request_ is NULL");
            bizcmd = 0;
            }
        else
            {
            bizcmd = request->get_cmd();
            }
        return bizcmd;
        }
    virtual const std::string &get_transfer()
        {
	    RequestGetAndPutMsg *request = dynamic_cast<RequestGetAndPutMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            static std::string null_str = "";
            LOG(ERROR)("ResultGetAndPutMsg, request_ is NULL");
            return null_str;
            }
        return request->get_transfer();
        }
    uint32_t get_transid()
        {
        uint32_t transid = 0;
	    RequestGetAndPutMsg *request = dynamic_cast<RequestGetAndPutMsg *>(p_request_);
        if (NULL == p_request_ || NULL == request)
            {
            LOG(ERROR)("ResultGetAndPutMsg, request_ is NULL");
            transid = 0;
            }
        else
            {
            transid = request->get_transid();
            }
        return transid;
        }
    uint32_t get_ret()
        {
        return ret_;
        }
    
    void err_code(uint32_t errrcode)
	{
	ret_ = errrcode;
	}

private:
    Msg                  *p_request_;
    strmap               kv_result_;
    BinOutputPacket<>    outpkg_;
    uint32_t             ret_;
};



