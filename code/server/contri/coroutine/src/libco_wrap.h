/**
 * @filedesc: 
 * libco_wrap.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/1/6 16:02:41
 * @modify:
 *
**/

#ifndef LIBCO_WRAP_H_
#define LIBCO_WRAP_H_

#include "libco_routine_manager.h"
#include "libco_data_type.h"
#include "net_manager.h"


class Net_Manager;

int set_service_id(uint16_t serviceid);


int set_max_co_num(uint32_t max_co_num);


// start run service, loop
int co_run_service(Net_Manager *net_manager);


#endif //LIBCO_WRAP_H_

