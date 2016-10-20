#include "sock_stream.h"
#include "net_manager.h"
#include "log.h"
#include <assert.h>
#include "block_buffer.h"


// class SOCK_Stream
SOCK_Stream::SOCK_Stream(uint32_t id, Net_Manager *net_manager, 
                Packet_Splitter *packet_splitter, SOCKET socket,
                const INET_Addr& local_addr, const INET_Addr& remote_addr, 
                pfnNetEventHandler in_handler,size_t buff_len)
{
	m_id = id;
	m_socket = socket;
	m_local_addr = local_addr;
	m_remote_addr = remote_addr;
	m_net_manager = net_manager;
	m_packet_splitter = packet_splitter;
	m_current_send_task = NULL;
	m_current_send_length = 0;
	handler = in_handler;
	m_recv_buffer.init_buffer(buff_len);
}

SOCK_Stream::~SOCK_Stream()
{
	if (0 != m_id) {
		m_net_manager->m_id_manager.release(m_id);
	}
	if (m_current_send_task != NULL) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}
	while (true == m_send_task_queue.read(m_current_send_task)) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}
}

int SOCK_Stream::objectPoolSize(){
	return m_pool_allocator.size(); 
}

void SOCK_Stream::close_stream()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

uint32_t SOCK_Stream::get_id()
{
	return m_id;
}

SOCKET SOCK_Stream::get_handle()
{
	return m_socket;
}

int SOCK_Stream::handle_input()
{
	while (true)
	{
		int rc = recv(m_socket, m_recv_buffer.wr_ptr(), m_recv_buffer.space_length(), 0);
		if (rc > 0)
		{
			m_recv_buffer.wr_ptr(rc);
			int packet_begin = 0;
			int packet_len = 0;

			// 循环拆包
			while (true)
			{
				packet_len = 0;
				int split_result = m_packet_splitter->split(m_recv_buffer.rd_ptr(), m_recv_buffer.data_length(), packet_begin, packet_len);
				if (1 == split_result)
				{
					// 此处进行限流，当压力过大时，主动丢掉包
 					if (m_net_manager->get_net_event_size() > 
						(int)(m_net_manager->l3_water() * 200000)) {
						uint32_t cmd = 
						ntohl(*reinterpret_cast<uint32_t*>(m_recv_buffer.rd_ptr() 
						+ sizeof(uint32_t)));
						if (CMD_LEVEL_3 == m_net_manager->level(cmd)) {
							LOG(WARN)("request is too many, lost level3 request. cmd:%u", cmd);
							continue;
						}
					} 
					if (m_net_manager->get_net_event_size() > 
					(int)(m_net_manager->l2_water() * 200000)) {
						uint32_t cmd = 
						ntohl(*reinterpret_cast<uint32_t*>(m_recv_buffer.rd_ptr() 
						+ sizeof(uint32_t)));
						if (CMD_LEVEL_2 == m_net_manager->level(cmd)) {
							LOG(WARN)("request is too many, lost level2 request. cmd:%u", cmd);
							continue;
						}
					}  
					if (m_net_manager->get_net_event_size() > 
					(int)(m_net_manager->l1_water() * 200000)) {
						uint32_t cmd = 
						ntohl(*reinterpret_cast<uint32_t*>(m_recv_buffer.rd_ptr() 
						+ sizeof(uint32_t)));
						if (CMD_LEVEL_1 == m_net_manager->level(cmd)) {
							LOG(ERROR)("request is too many, lost level1 request. cmd:%u", cmd);
							continue;
						}
					}

					// got a packet
					Net_Event *net_event = new Net_Event(packet_len);
					
					net_event->net_event_type = TYPE_DATA;
					net_event->id = m_id;
					net_event->new_id = 0;
					net_event->local_addr = m_local_addr;
					net_event->remote_addr = m_remote_addr;
					net_event->handler = handler;
//						memcpy(net_event->packet.ptr(), m_recv_buffer.rd_ptr() + packet_begin, packet_len);
//						net_event->packet.length(packet_len);
					net_event->setBuff(m_recv_buffer.rd_ptr() + packet_begin, packet_len);

					m_net_manager->put_event(net_event);

					m_recv_buffer.rd_ptr(packet_begin + packet_len);
				}
				else if (0 == split_result)
				{
					// no packet in buf
					if(packet_len > m_recv_buffer.length())
					{
						//重新生成一个较大的缓冲区,并将当前缓冲区数据转移过去
						m_recv_buffer.extend(packet_len);
					}
					
					break;
				}
				else
				{
					// 协议错
					LOG(ERROR)("SOCK_Stream::handle_input error, split error, split_result:%d", split_result);
					return -2;
				}

				if (0 == m_recv_buffer.data_length())
				{
					// buf is empty, no packet in buf
					break;
				}
			}

			if ((m_recv_buffer.data_length() == 0) || (m_recv_buffer.space_length() == 0)
				|| (m_recv_buffer.data_length()+m_recv_buffer.space_length())<packet_len)
			{
				m_recv_buffer.recycle();
			}
		}
		else if (rc == 0)
		{
			// close
			LOG(WARN)("SOCK_Stream::handle_input, socket close by peer space_len:%d max_len:%d",
                                    m_recv_buffer.space_length(), m_recv_buffer.length());
			return -1;
		}
		else
		{
			if (EAGAIN == error_no()) {
				return 0;
			}
			else {
				// exception
				LOG(WARN)("SOCK_Stream::handle_input error, recv error, errno:%d", error_no());
				return -2;
			}
		}
	}

	return 0;
}

int SOCK_Stream::handle_output()
{
	if (m_current_send_task != NULL)
	{
		int rc = send(m_socket, m_current_send_task->packet->ptr() + m_current_send_length, m_current_send_task->packet->length() - m_current_send_length, 0);
		if (rc > 0) {
			// 发送成功
			m_current_send_length += rc;

			if (m_current_send_length == m_current_send_task->packet->length()) {
				// 当前包发送完毕
				delete m_current_send_task;
				m_current_send_task = NULL;
				m_current_send_length = 0;
			}
			else {
				// 短写
				return -3;
			}
		}
		else if (rc == 0) {
			// close
			LOG(WARN)("SOCK_Stream::handle_output, socket close by peer send curr, send m_current_send_task %d for net:%d needlen:%d return rc:%d",
						m_current_send_task->packet->length() - m_current_send_length,
						get_id(), m_current_send_task->packet->length(), rc);
			return -1;
		}
		else {
			if (EAGAIN == error_no()) {
				LOG(DEBUG)("SOCK_Stream::handle_output, send return EAGAIN, send m_current_send_task %d for net:%d needlen:%d return rc:%d",
							m_current_send_task->packet->length() - m_current_send_length,
							get_id(), m_current_send_task->packet->length(), rc);
				return -3;	//数据未写完，需要等待后续写入
			}
			else {
				LOG(WARN)("SOCK_Stream::handle_output, error, send error, errno:%d, send m_current_send_task %d for net:%d needlen:%d return rc:%d",
							error_no(), m_current_send_task->packet->length() - m_current_send_length,
							get_id(), m_current_send_task->packet->length(), rc);
				return -2;
			}
		}
	}

	assert(m_current_send_task == NULL);
	assert(m_current_send_length == 0);

	while (true)
	{
		bool brc = m_send_task_queue.read(m_current_send_task);
		if (false == brc) {
			// 发送队列已空
			break;
		}

		int rc = send(m_socket, m_current_send_task->packet->ptr(), m_current_send_task->packet->length(), 0);
		if (rc > 0) {
			if (rc == m_current_send_task->packet->length()) {
				// 当前包发送完毕
				delete m_current_send_task;
				m_current_send_task = NULL;
			}
			else {
				// 短写
				m_current_send_length = rc;
				LOG(DEBUG)("SOCK_Stream::handle_output, send m_send_task_queue %d for net:%d needlen:%d return rc:%d",
							m_current_send_task->packet->length() - m_current_send_length,
							get_id(), m_current_send_task->packet->length(), rc);
				return -3;
			}
		}
		else if (rc == 0) {
			// close
			LOG(WARN)("SOCK_Stream::handle_output, socket close by peer, send m_send_task_queue %d for net:%d needlen:%d return rc:%d",
						m_current_send_task->packet->length() - m_current_send_length,
						get_id(), m_current_send_task->packet->length(), rc);
			return -1;
		}
		else {
			if (EAGAIN == error_no()) {
				return -3;	//数据未写完，需要等待后续写入
			}
			else {
				LOG(WARN)("SOCK_Stream::handle_outputerror, send error, errno:%d, socket close by peer, send m_send_task_queue %d for net:%d needlen:%d return rc:%d",
							error_no(), m_current_send_task->packet->length() - m_current_send_length,
							get_id(), m_current_send_task->packet->length(), rc);
				return -2;
			}
		}
	}

	assert(m_current_send_task == NULL);
	assert(m_current_send_length == 0);

	int rc = reactor()->remove_handler(this, MASK_WRITE);
	if (0 != rc) {
		// 设置reactor失败,认为socket异常
		LOG(ERROR)("SOCK_Stream::handle_output error, remove_handler error");
		return -2;
	}

	return 0;
}

int SOCK_Stream::handle_exception()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}

	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_EXCEPTION;
	net_event->id = m_id;
	net_event->new_id = 0;
	net_event->local_addr = m_local_addr;
	net_event->remote_addr = m_remote_addr;
	net_event->handler = handler;

	m_net_manager->put_event(net_event);

	if (m_current_send_task != NULL) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}
	while (true == m_send_task_queue.read(m_current_send_task)) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}

	return 0;
}

int SOCK_Stream::handle_close()
{
	if (INVALID_SOCKET != m_socket) {
		LOG(DEBUG)("SOCK_Stream::handle_close, net:%d, m_current_send_task:%p, m_send_task_queue:%d",
					get_id(),
					m_current_send_task, m_send_task_queue.read(m_current_send_task));
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}

	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_CLOSE;
	net_event->id = m_id;
	net_event->new_id = 0;
	net_event->local_addr = m_local_addr;
	net_event->remote_addr = m_remote_addr;
	net_event->handler = handler;

	m_net_manager->put_event(net_event);

	if (m_current_send_task != NULL) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}
	while (true == m_send_task_queue.read(m_current_send_task)) {
		delete m_current_send_task;
		m_current_send_task = NULL;
	}

	return 0;
}

int SOCK_Stream::handle_timeout()
{
	if (INVALID_SOCKET != m_socket) {
		close(m_socket);
		m_socket = INVALID_SOCKET;
	}

	Net_Event *net_event = new Net_Event;

	net_event->net_event_type = TYPE_TIMEOUT;
	net_event->id = m_id;
	net_event->new_id = 0;
	net_event->local_addr = m_local_addr;
	net_event->remote_addr = m_remote_addr;
	net_event->handler = handler;

	m_net_manager->put_event(net_event);

	return 0;
}

int SOCK_Stream::send_data(Send_Task *send_task)
{
	bool brc = m_send_task_queue.write(send_task);
	if (true != brc) {
		// 队列用尽
		LOG(ERROR)("SOCK_Stream::send_data error, send task queue is full, packet lost,net_id%d,peer ip:0X%08x,port:%u",m_id,m_remote_addr.get_addr(),m_remote_addr.get_port());
		delete send_task;
		return 0;
	}


	int rc = handle_output();
		
	if (-1 == rc)
	{
		// 连接关闭
		LOG(WARN)("SOCK_Stream::send_data handle_output ret=%d for id=%d", rc, send_task->id);
		reactor()->delete_handler(this);
		handle_close();
		return -1;
	}
	else if (-2 == rc)
	{
		// 连接异常
		LOG(WARN)("SOCK_Stream::send_data handle_output ret=%d for id=%d", rc, send_task->id);
		reactor()->delete_handler(this);
		handle_exception();
		return -2;
	}
	else if(-3 == rc)
	{	
		LOG(WARN)("SOCK_Stream::send_data handle_output ret=%d for id=%d", rc, send_task->id);
		rc = reactor()->register_handler(this, MASK_WRITE);
		if (0 != rc) {
			// 设置reactor失败,认为socket异常
			LOG(ERROR)("SOCK_Stream::send_data error, register_handler error");
			// 连接异常
			reactor()->delete_handler(this);
			handle_exception();
			return -3;
		}
	}
	return 0;
}

IMPLEMENT_ALLOCATOR(SOCK_Stream)

