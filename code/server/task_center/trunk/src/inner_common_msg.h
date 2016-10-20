/**
 * @filedesc: 
 * inner_common_msg.h, all request define here
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/14 20:02:59
 * @modify:
 *
**/
#ifndef _INNER_COMMON_MSG_H_
#define _INNER_COMMON_MSG_H_

#include <string>
#include "int_types.h"
#include "adv_base.pb.h"
#include "adv_protocol.pb.h"

using namespace std;

class SrvInnerCommonMsg
{
public:
	int AddTagValue(string tag, uint32_t value);
	int AddTagValue(string tag, uint64_t value);
	int AddTagValue(string tag, const string &value);

	int SetTagValue(string tag, uint32_t value, int index = 0);
	int SetTagValue(string tag, uint64_t value, int index = 0);
	int SetTagValue(string tag, const string &value, int index = 0);

	int SetTransid(uint32_t transid);
	int SetTransfer(const string &transfer);
	int SetTime(uint32_t time);
	
	uint32_t GetTransid() const;
	const string &GetTransfer() const;
	uint32_t GetTime() const;
	
	inline const com::adv::msg::InnerCommonMsg &pb_msg() const {return common_msg_;}
	inline com::adv::msg::InnerCommonMsg &mutable_pb_msg() {return common_msg_;}

private:
	com::adv::msg::InnerCommonMsg common_msg_;
};



#endif// _INNER_COMMON_MSG_H_


