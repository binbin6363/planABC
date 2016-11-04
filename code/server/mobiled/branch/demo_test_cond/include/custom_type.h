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
    uint64_t  taskid        ; // ����id��
    uint32_t  tasktype      ; // ��������
    string taskname         ; // ��������
    string tasklink         ; // �������ӵ�ַ
    string taskdesc         ; // ��������
    string taskpkgname      ; // �������
    uint32_t tasksize       ; // �����С
    string taskprice        ; // ����۸�
    uint32_t  taskptstep     ; // ������
    uint32_t  taskpcstep     ; // ����ǰ�������
    uint32_t  taskstatus     ; // ����ǰ״̬
    string taskpublisher    ; // ���񷢲���
    string taskicon         ; // ����ͼ������
    uint32_t  tasktotalnum  ; // ��������
    uint32_t  taskusednum   ; // �������ʹ�õ���
    uint64_t  taskstime     ; // ���񷢲�ʱ��
    uint64_t  tasketime     ; // �����ֹʱ��

};


typedef vector<CustomTaskInfo *> TaskInfoVec;
typedef TaskInfoVec::const_iterator TaskInfoCIter;
typedef TaskInfoVec::iterator TaskInfoIter;


#endif // _CUSTOM_TYPE_H_

