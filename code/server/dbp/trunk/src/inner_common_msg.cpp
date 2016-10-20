/**
 * @filedesc: 
 * inner_common_msg.cpp, all common msg define here
 * @author: 
 *  bbwang
 * @date: 
 *  2015/10/14 20:02:59
 * @modify:
 *
**/
#include "inner_common_msg.h"


int SrvInnerCommonMsg::AddTagValue(string tag, uint32_t value)
{
    com::adv::msg::PDataCell* pdata_cell = common_msg_.add_items();
    pdata_cell->set_tag(tag);
    pdata_cell->set_i32_value(value);
    return 0;
}

int SrvInnerCommonMsg::AddTagValue(string tag, uint64_t value)
{
    com::adv::msg::PDataCell* pdata_cell = common_msg_.add_items();
    pdata_cell->set_tag(tag);
    pdata_cell->set_i64_value(value);
    return 0;
}

int SrvInnerCommonMsg::AddTagValue(string tag, const string &value)
{
    com::adv::msg::PDataCell* pdata_cell = common_msg_.add_items();
    pdata_cell->set_tag(tag);
    pdata_cell->set_str_value(value);
    return 0;
}

int SrvInnerCommonMsg::SetTagValue(string tag, uint32_t value, int index)
{
    com::adv::msg::PDataCell* pdata_cell = common_msg_.mutable_items(index);
    pdata_cell->set_tag(tag);
    pdata_cell->set_i32_value(value);
    return 0;
}

int SrvInnerCommonMsg::SetTagValue(string tag, uint64_t value, int index)
{
    com::adv::msg::PDataCell* pdata_cell = common_msg_.mutable_items(index);
    pdata_cell->set_tag(tag);
    pdata_cell->set_i64_value(value);
    return 0;
}

int SrvInnerCommonMsg::SetTagValue(string tag, const string &value, int index)
{
    com::adv::msg::PDataCell* pdata_cell = common_msg_.mutable_items(index);
    pdata_cell->set_tag(tag);
    pdata_cell->set_str_value(value);
    return 0;
}

int SrvInnerCommonMsg::SetTransid(uint32_t transid)
{
    common_msg_.set_transid(transid);
    return 0;
}


int SrvInnerCommonMsg::SetTransfer(const string &transfer)
{
    common_msg_.set_transfer(transfer);
    return 0;
}

int SrvInnerCommonMsg::SetTime(uint32_t time)
{
    common_msg_.set_time(time);
    return 0;
}


uint32_t SrvInnerCommonMsg::GetTransid() const
{
    return common_msg_.transid();
}

const string &SrvInnerCommonMsg::GetTransfer() const
{
    return common_msg_.transfer();
}

uint32_t SrvInnerCommonMsg::GetTime() const
{
    return common_msg_.time();
}


