//! @file sock_stream.h


#ifndef _SOCK_STREAM_H_
#define _SOCK_STREAM_H_

#include "net_event.h"
#include "event_handler.h"
#include "inet_addr.h"
#include "send_task.h"
#include "send_task_queue.h"

#include "pool_allocator.h"
#include "config.h"
#include "block_buffer.h"

class Net_Manager;
class Packet_Splitter;
//class Block_Buffer_D;

//! @class SOCK_Stream
//! @brief tcpͨ��������
class SOCK_Stream : public Event_Handler
{
private:
	//! ���캯��
	//! @param id ͨ��id
	//! @param net_manager ���������
	//! @param packet_splitter �����
	//! @param socket �׽���
	//! @param local_addr ���ص�ַ
	//! @param remote_addr �Զ˵�ַ
	SOCK_Stream(uint32_t id, Net_Manager *net_manager, Packet_Splitter *packet_splitter, SOCKET socket, const INET_Addr& local_addr, const INET_Addr& remote_addr
	            , pfnNetEventHandler in_handler, size_t buff_len = RECV_BUFFER_LENGTH);

public:
	virtual ~SOCK_Stream();

	DECLARE_ALLOCATOR(SOCK_Stream, SOCK_STREAM_POOL_COUNT)

public:

	static int objectPoolSize();

	//! �ر�����
	void close_stream();

	//! ��ȡͨ��id
	//! @return ͨ��id
	virtual uint32_t get_id();

	//! ��ȡsocket���
	//! @return socket���
	virtual SOCKET get_handle();

	//! ��
	//! @return ������ 0:��������, -1: ���ӱ��Է��ر�, -2:�����쳣
	virtual int handle_input();

	//! д
	//! @return ������ 0:��������, -1: �����쳣
	virtual int handle_output();

	//! ���ӶϿ�
	virtual int handle_exception();

	//! ��ʱ
	virtual int handle_timeout();

	//! ���ӹر�
	virtual int handle_close();

	//! �ύ��������
	virtual int send_data(Send_Task *send_task);

public:
	friend class SOCK_Acceptor;
	friend class SOCK_Connector;

private:
	//! ͨ��id
	uint32_t m_id;

	//! socket���
	SOCKET m_socket;

	//! ���ؼ�����ַ
	INET_Addr m_local_addr;

	//! �Զ˵�ַ
	INET_Addr m_remote_addr;

	//! ���������
	Net_Manager *m_net_manager;

	//! �����
	Packet_Splitter *m_packet_splitter;

	//! ���ܻ���
	//Block_Buffer_T<RECV_BUFFER_LENGTH> m_recv_buffer;
	
       //�޸�Ϊ��̬buffer����Ӧ�ó���ָ��
	Block_Buffer_D m_recv_buffer;

	//! �������������
	Socket_Send_Task_Queue m_send_task_queue;

	//! ��ǰ���ڷ��͵�����
	Send_Task *m_current_send_task;

	//! ��ǰ���ڷ��͵������Ѿ����͵��ֽ���
	int m_current_send_length;
	
	//! �����ӵ��¼�������ָ��
	pfnNetEventHandler handler;
};


#endif // _SOCK_STREAM_H_
