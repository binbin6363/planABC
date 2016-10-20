//! @file sock_dgram.h


#ifndef _SOCK_DGRAM_H_
#define _SOCK_DGRAM_H_

#include "net_event.h"
#include "event_handler.h"
#include "inet_addr.h"
#include "send_task.h"
#include "send_task_queue.h"


class Net_Manager;


//! @class SOCK_Dgram
//! @brief updͨ��������
class SOCK_Dgram : public Event_Handler
{
public:
	//! ���캯��
	//! @param net_manager ���������
	SOCK_Dgram(Net_Manager *net_manager, pfnNetEventHandler in_handler);

	//! ��������
	virtual ~SOCK_Dgram();

public:
	//! ����updͨ��
	//! @param local_addr upd���ذ󶨵�ַ
	//! @param handler �����ӵ��¼�������ָ��
	//! @return ���, 0:�ɹ�, -1ʧ��
	int create_udp(const INET_Addr& local_addr);

	//! �ر�ͨ��
	void close_udp();

	//! ��ȡͨ��id
	//! @return ͨ��id
	virtual uint32_t get_id();

	//! ��ȡsocket���
	//! @return socket���
	virtual SOCKET get_handle();

	//! �����
	//! @return ������ 0:��������, -1: ���ӱ��ر�, -2:�����쳣
	virtual int handle_input();

	//! ����д
	//! @return ������ 0:��������, -1: ���ӱ��ر�, -2:�����쳣
	virtual int handle_output();

	//! �����쳣
	virtual int handle_exception();

	//! ���ӹر�
	virtual int handle_close();

	//! ��ʱ
	virtual int handle_timeout();

	//! �ύ��������
	//! @param send_task �����͵�����
	virtual int send_data(Send_Task *send_task);

private:
	//! ͨ��id
	uint32_t m_id;

	//! socket���
	SOCKET m_socket;

	//! ���ذ󶨵�ַ
	INET_Addr m_local_addr;

	//! ���������
	Net_Manager *m_net_manager;

	//! �������������
	Socket_Send_Task_Queue m_send_task_queue;

	//! ��ǰ���ڷ��͵�����
	Send_Task *m_current_send_task;
	
	//! �����ӵ��¼�������ָ��
	pfnNetEventHandler handler;
};


#endif // _SOCK_DGRAM_H_
