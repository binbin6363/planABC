#include "block_buffer.h"
#include "config.h"

Block_Buffer_D::Block_Buffer_D()
{
	m_read_index = 0;
	m_write_index = 0;
	m_buffer = NULL;
	_length = 0;
}

Block_Buffer_D::~Block_Buffer_D()
{
	delete[] m_buffer;
}

void Block_Buffer_D::init_buffer(size_t buff_len)
{
    	m_buffer = new char[buff_len];
	_length = buff_len;
}

char* Block_Buffer_D::rd_ptr()
{
	return m_buffer + m_read_index;
}


void Block_Buffer_D::rd_ptr(int n)
{
	assert((m_write_index - m_read_index) >= n);
	m_read_index += n;
}


char* Block_Buffer_D::wr_ptr()
{
	return m_buffer + m_write_index;
}


void Block_Buffer_D::wr_ptr(int n)
{
	assert((_length - m_write_index) >= n);
	m_write_index += n;
}


int Block_Buffer_D::buffer_length()
{
	return _length;
}


int Block_Buffer_D::data_length()
{
	return (m_write_index - m_read_index);
}


int Block_Buffer_D::space_length()
{
	return (_length - m_write_index);
}

int Block_Buffer_D::length()
{
	return _length;
}


void Block_Buffer_D::recycle()
{
	int data_len = m_write_index - m_read_index;
	if (0 == data_len) {
		m_write_index = 0;
		m_read_index = 0;
	}
	else {
		memmove(m_buffer, m_buffer + m_read_index, data_len);
		m_read_index = 0;
		m_write_index = data_len;
	}
}


void Block_Buffer_D::extend(int new_length)
{
	int data_len = m_write_index - m_read_index;
	char* ext_buffer = new char[new_length+data_len];

	LOG_INFO("Block_Buffer_T extend from %d to %d for datalen:%d newlength:%d",
					_length, new_length+data_len,
					data_len, new_length);
	
	if(data_len == 0)
	{
		m_write_index = 0;
		m_read_index = 0;		
	}
	else
	{
		memmove(ext_buffer, m_buffer + m_read_index, data_len);
		m_read_index = 0;
		m_write_index = data_len;
	}
	
	delete[] m_buffer;
	m_buffer = ext_buffer;
	_length = new_length+data_len;
}



