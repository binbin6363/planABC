//! @file net_manager.h


#ifndef _NET_MANAGER_H_
#define _NET_MANAGER_H_


#include "types.h"
#include "net_thread.h"
#include "id_manager.h"
#include "event_handler.h"
#include "inet_addr.h"
#include "net_packet.h"
#include "net_event.h"
#include "net_event_queue.h"
#include "send_task.h"
#include "send_task_queue.h"
#include "packet_splitter.h"

#ifdef WIN32	// for windows
  #include "select_reactor.h"
  //! windowsƽ̨��ʹ��select
  typedef Select_Reactor Net_Reactor;
#else			// for linux
  //! linuxƽ̨��ʹ��epoll
  #include "epoll_reactor.h"
  typedef Epoll_Reactor Net_Reactor;
#endif

enum kCMD_LEVEL {
	CMD_LEVEL_1 = 1,
	CMD_LEVEL_2 ,
	CMD_LEVEL_3 ,
};


//! @class Net_Manager
//! @brief ���������
class Net_Manager
{
public:
	Net_Manager();
	~Net_Manager();

public:
	//! ��������������߳�
	//! @return 0:�ɹ�, -1ʧ��
	int start();

	//! ֹͣ����������߳�
	//! @return 0:�ɹ�, -1ʧ��
	int stop();

	//! ����һ��updͨ��
	//! @param local_addr upd���ذ󶨵�ַ
	//! @param handler �����ӵ��¼�������ָ��
	//! @return ͨ��id, >0:�ɹ�, 0ʧ��
	uint32_t create_udp(const INET_Addr& local_addr, pfnNetEventHandler handler);

	//! ����һ��updͨ��
	//! @param local_ip upd���ذ�ip
	//! @param local_port upd���ذ�port
	//! @param handler �����ӵ��¼�������ָ��
	//! @return ͨ��id, >0:�ɹ�, 0ʧ��
	uint32_t create_udp(const char *local_ip, int local_port, pfnNetEventHandler handler);

	//! ����һ��tcp clientͨ��
	//! @param remote_addr tcp�Զ˵�ַ
	//! @param packet_splitter tcp�����
	//! @param handler �����ӵ��¼�������ָ��
	//! @param timeout ���ӳ�ʱʱ��(��)
	//! @return ͨ��id, >0:�ɹ�, 0ʧ��
	uint32_t create_tcp_client(const INET_Addr& remote_addr, Packet_Splitter *packet_splitter, 
	                pfnNetEventHandler handler, int timeout,
	                size_t recv_buff_len = RECV_BUFFER_LENGTH);

	//! ����һ��tcp clientͨ��
	//! @param remote_ip �Զ�ip
	//! @param remote_port �Զ�port
	//! @param packet_splitter tcp�����
	//! @param handler �����ӵ��¼�������ָ��
	//! @param timeout ���ӳ�ʱʱ��(��)
	//! @return ͨ��id, >0:�ɹ�, 0ʧ��
	uint32_t create_tcp_client(const char *remote_ip, int remote_port, Packet_Splitter *packet_splitter, 
	                pfnNetEventHandler handler, int timeout, size_t recv_buff_len = RECV_BUFFER_LENGTH);

	//! ����һ��tcp serverͨ��
	//! @param local_addr tcp���ؼ�����ַ
	//! @param packet_splitter tcp�����
	//! @param accpet_handler �ü����˿�accpet���¼�������ָ��
	//! @param handler �����ӵ��¼�������ָ��
	//! @return ͨ��id, >0:�ɹ�, 0ʧ��
	uint32_t create_tcp_server(const INET_Addr& local_addr, Packet_Splitter *packet_splitter, pfnNetEventHandler accept_handler, 
	            pfnNetEventHandler handler, size_t recv_buff_len = RECV_BUFFER_LENGTH);

	//! ����һ��tcp serverͨ��
	//! @param local_ip tcp���ؼ���ip
	//! @param local_port tcp���ؼ���port
	//! @param packet_splitter tcp�����
	//! @param accpet_handler �ü����˿�accpet���¼�������ָ��
	//! @param handler �����ӵ��¼�������ָ��
	//! @return ͨ��id, >0:�ɹ�, 0ʧ��
	uint32_t create_tcp_server(const char *local_ip, int local_port, Packet_Splitter *packet_splitter, 
	                pfnNetEventHandler accept_handler, pfnNetEventHandler handler, 
	                        size_t recv_buff_len = RECV_BUFFER_LENGTH);

	//! ֪ͨɾ������ͨ��
	//! @param id ͨ��id
	//! @return 0:�ɹ� <0:ʧ��
	int delete_net(uint32_t id);

	//! �������ݰ�
	//! @warning ����˵��óɹ�, packet����������������ͷ�; ����˵���ʧ��, packet���ɵ��÷������ͷ�
	//! @param id ͨ��id
	//! @param packet ���ݰ�
	//! @param remote_addr �Զ˵�ַ(udp��Ҫʹ�ô˵�ַ, tcp���Դ˵�ַ)
	//! @return ���ͽ��, 0:�ɹ�, -1:ʧ��(ͨ��������), -2:ʧ��(��������)
	int send_packet(uint32_t id, Net_Packet* packet, const INET_Addr& remote_addr);

	//! �������ݰ�
	//! @warning ����˵��óɹ�, packet����������������ͷ�; ����˵���ʧ��, packet���ɵ��÷������ͷ�
	//! @param id ͨ��id
	//! @param packet ���ݰ�
	//! @param remote_ip �Զ�ip
	//! @param remote_port �Զ�port
	//! @return ���ͽ��, 0:�ɹ�, -1:ʧ��(ͨ��������), -2:ʧ��(��������)
	int send_packet(uint32_t id, Net_Packet* packet, const char *remote_ip, int remote_port);

	//! �������ݰ�(ֻ������tcp)
	//! @warning ����˵��óɹ�, packet����������������ͷ�; ����˵���ʧ��, packet���ɵ��÷������ͷ�
	//! @param id ͨ��id
	//! @param packet ���ݰ�
	//! @return ���ͽ��, 0:�ɹ�, -1:ʧ��(ͨ��������), -2:ʧ��(��������)
	int send_packet(uint32_t id, Net_Packet* packet);

	//! ȡ�����¼�
	//! @warning �ͻ���Ӧѭ�����ô˺�����ȡ�����¼�, ���ڴ������ÿ�������¼������ͷ�
	//! @return �����¼�, û�������¼�ʱ����NULL
	Net_Event* get_event();

	//! ��ȡδ����ķ����¼�������
	//! @return �����¼�������
	int get_send_size();

	//! ��ȡδ����Ľ����¼�������
	//! @return �����¼�������
	int get_recv_size();

	int get_net_event_size();
	int set_water(float water1_p, float water2_p, float water3_p);

	float l1_water() {return lev1_water_percent;}
	float l2_water() {return lev2_water_percent;}
	float l3_water() {return lev3_water_percent;}

	int register_cmd_level(uint32_t cmd, uint32_t lev);
	int level(uint32_t cmd);

public:
	friend class Net_Thread;
	friend class SOCK_Dgram;
	friend class SOCK_Acceptor;
	friend class SOCK_Connector;
	friend class SOCK_Stream;

private:
	//! ����һ�������¼�
	//! @param net_event �����¼�
	void put_event(Net_Event* net_event);

	//! ȡ��������
	//! @return ��������, û�з�������ʱ����NULL
	Send_Task* get_send_task();

	//! �������(reactor����)
	void on_net_error();

private:
	//! ����״̬ 0:ֹͣ, 1:����, 2:�쳣
	int m_status;

	//! ����������߳�
	Net_Thread* m_net_thread;


	StatusManager* m_net_statusManager;

	//! ͨ��id������
	Id_Manager m_id_manager;

	//! ��Ӧ��
	Net_Reactor m_reactor;

	//! �����¼�����
	Net_Event_Queue *m_net_event_queue;

	//! �������������
	Net_Send_Task_Queue *m_send_task_queue;

	// ѹ������ʱ����ˮλ����
	// lev3_water_percent <= lev2_water_percent <= lev1_water_percent
	float lev1_water_percent; // �ڴ��������¼�ʱ����
	float lev2_water_percent; // �ڴ��������¼�ʱ����
	float lev3_water_percent; // �ڷְ�ʱ����

	map<uint32_t, uint32_t> cmd_level_map; // cmd -> level
};


#endif // _NET_MANAGER_H_
