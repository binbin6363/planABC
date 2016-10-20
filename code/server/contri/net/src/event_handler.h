//! @file event_handler.h


#ifndef _EVENT_HANDLER_H_
#define _EVENT_HANDLER_H_


#include "types.h"
#include "os.h"


//! @enum EVENT_MASK
//! @brief �¼�����
enum EVENT_MASK
{
	MASK_READ = (1 << 0),		//!< �ɶ��¼�
	MASK_WRITE = (1 << 1),		//!< ��д�¼�
	MASK_TIMEOUT = (1 << 2),	//!< ��ʱ�¼�
};


class Reactor;
class Send_Task;


//! @class Event_Handler
//! @brief �¼�������
class Event_Handler
{
public:
	Event_Handler();
	virtual ~Event_Handler();

public:
	//! ��ȡ��Ӧ��
	//! @return ��Ӧ��ָ��
	Reactor* reactor();

	//! ���÷�Ӧ��
	//! @param reactor ��Ӧ��ָ��
	void reactor(Reactor* reactor);

	//! ��ȡͨ��id
	//! @return ͨ��id
	virtual uint32_t get_id() = 0;

	//! ��ȡsocket���
	//! @return socket���
	virtual SOCKET get_handle() = 0;

	//! ������
	//! @return ������� 0:��������, -1: ���ӱ��ر�, -2:�����쳣
	virtual int handle_input() = 0;

	//! ����д
	//! @return ������� 0:��������, -1: ���ӱ��ر�, -2:�����쳣
	virtual int handle_output() = 0;

	//! �����쳣
	virtual int handle_exception() = 0;

	//! ���ӹر�
	virtual int handle_close() = 0;

	//! ��ʱ
	virtual int handle_timeout() = 0;

	//! �ύ��������
	//! @param send_task �����͵�����
	virtual int send_data(Send_Task *send_task) = 0;

private:
	//! ��Ӧ��
	Reactor *m_reactor;
};


#endif // _EVENT_HANDLER_H_