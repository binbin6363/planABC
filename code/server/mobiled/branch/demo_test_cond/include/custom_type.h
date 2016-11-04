/**
 * @filedesc: 
 * custom_type.h
 * @author: 
 *  bbwang
 * @date: 
 *  2015/11/13 20:02:59
 * @modify:
 *
**/
#ifndef _CUSTOM_TYPE_H_
#define _CUSTOM_TYPE_H_
#include "headers.h"
#include "MemoryPool.h"


struct CustomTaskInfo {
DECLARE_SLAB(CustomTaskInfo);
    uint64_t  taskid        ; // 任务id号
    uint32_t  tasktype      ; // 任务类型
    string taskname         ; // 任务名称
    string tasklink         ; // 任务链接地址
    string taskdesc         ; // 任务描述
    string taskpkgname      ; // 任务包名
    uint32_t tasksize       ; // 任务大小
    string taskprice        ; // 任务价格
    uint32_t  taskptstep     ; // 任务步骤
    uint32_t  taskpcstep     ; // 任务当前处理进度
    uint32_t  taskstatus     ; // 任务当前状态
    string taskpublisher    ; // 任务发布者
    string taskicon         ; // 任务图标链接
    uint32_t  tasktotalnum  ; // 任务总量
    uint32_t  taskusednum   ; // 任务可以使用的量
    uint64_t  taskstime     ; // 任务发布时间
    uint64_t  tasketime     ; // 任务截止时间

};


typedef vector<CustomTaskInfo *> TaskInfoVec;
typedef TaskInfoVec::const_iterator TaskInfoCIter;
typedef TaskInfoVec::iterator TaskInfoIter;


#endif // _CUSTOM_TYPE_H_

