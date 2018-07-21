#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <atomic>
#include "../include/network/CNetBuffer.h"

namespace easygo {
	namespace network {

		static std::atomic_uint64_t sgBufferId = 0;

		CNetBuffer::CNetBuffer() : m_buffer(NULL), m_bufferSize(0), m_rwOffset(0)
		{
			m_bufferId = ++sgBufferId;
		}

		CNetBuffer::~CNetBuffer()
		{
			if (NULL != m_buffer)
			{
				//std::cout << "CNetBuffer::~CNetBuffer(): " << "[" << m_bufferId << "]" << m_bufferSize << "," << m_rwOffset << std::endl;
				free(m_buffer);
				m_buffer = NULL;
			}
			m_bufferSize = 0;
			m_rwOffset = 0;
		}

		bool CNetBuffer::IncBufferSize(unsigned int incSize)
		{
			m_bufferSize = m_rwOffset + incSize;
			m_bufferSize += 1;
			char* buffer = NULL;
			buffer = (char*)realloc(m_buffer, m_bufferSize);
			if (NULL == buffer)
			{
				return false;
			}

			m_buffer = buffer;

			return true;
		}

		unsigned int CNetBuffer::Write(const char* buf, int len)
		{
			if (m_rwOffset + len > m_bufferSize)
			{
				IncBufferSize(len);
			}

			if (buf)
			{
				memcpy(m_buffer + m_rwOffset, buf, len);
			}

			m_rwOffset += len;

			return len;
		}

		unsigned int CNetBuffer::Read(char* buf, int len)
		{
			if (len > m_rwOffset)
			{
				len = m_rwOffset;
			}

			if (buf)
			{
				memcpy(buf, m_buffer, len);
			}

			m_rwOffset -= len;
			if (m_rwOffset > 0)
			{
				memmove(m_buffer, m_buffer + len, m_rwOffset);
			}

			return len;
		}


		char* CNetBuffer::GetBuffer() const
		{
			return m_buffer;
		}

		unsigned int CNetBuffer::GetBufferSize() const
		{
			return m_bufferSize;
		}

		unsigned int CNetBuffer::GetWritableSize()
		{
			return m_bufferSize - m_rwOffset > 0? m_bufferSize - m_rwOffset : 0;
		}

		unsigned int CNetBuffer::GetReadableSize() const
		{
			return m_rwOffset;
		}

	} // namespace network
} // namespace easygo

