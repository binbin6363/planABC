//! @file net_thread.h


#ifndef _NET_THREAD_H_
#define _NET_THREAD_H_


#include "task_base.h"


class Net_Manager;
class Reactor;


#include <sys/time.h>

//! @class StatusInfo
//! @brief ״̬ͳ����Ϣ
//!
//!  ����״̬ͳ����Ϣ
class StatusManager : public utils::Task_Base{
public:  

	StatusManager(Net_Manager *net_manager);
	virtual ~StatusManager();

	//! �����߳�
	int start();

	//! ֹͣ�߳�
	int stop();

	//! �̺߳���
	virtual int svc();

public:
	struct StatusInfo
	{
		int m_tcpClientNums;
		int m_pendingNetEvent;
		int m_netPacketNums;
		int m_netBuffer1Nums;
		int m_netBuffer2Nums;
	};
private:
	time_t starttime;
	//! ����״̬
	bool m_is_run;

	Net_Manager *m_net_manager;
	
	//! ֹ֪ͣͨ
	volatile bool m_notify_stop;
};



//! @class Net_Thread
//! @brief ���紦���߳�
//!
//! ���̸߳����������ݵ��շ�, ����ͨ����ʱ���, �����������ݷ��������ͨ���Ĵ����Ͷ���
class Net_Thread : public utils::Task_Base
{
public:
	//! ���캯��
	//! @param net_manager ���������
	//! @param reactor ��Ӧ��
	Net_Thread(Net_Manager *net_manager, Reactor *reactor);
	virtual ~Net_Thread();

public:
	//! �����߳�
	int start();

	//! ֹͣ�߳�
	int stop();

	//! �̺߳���
	virtual int svc();

public:
	//! ����״̬
	bool m_is_run;

	//! ֹ֪ͣͨ
	volatile bool m_notify_stop;

	//! ���������
	Net_Manager *m_net_manager;

	//! ��Ӧ��
	Reactor *m_reactor;
};


#endif // _NET_THREAD_H_
