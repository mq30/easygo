#include <cstring>
#include "../include/base/CUuid.h"
#include "../include/network/CConnection.h"
#include "../include/network/CListener.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CNetBuffer.h"
#include <iostream>

namespace easygo {
	namespace network {

		CConnection::CConnection(PCNetBase netBase) : easygo::network::CSocket(netBase),
		m_recvBuffer(new CNetBuffer), m_sendBufferQueue(), m_sendQueueMutex(),
		m_recvBufferMutex(), m_maxCacheSize(MAX_CACHE_SIZE), m_autoPost(false)
		{
			m_uuid = easygo::base::CUuid::GenerateUUID();
		}

		CConnection::~CConnection()
		{
		}

		void CConnection::OnConnect()
		{
			OnConnectComplete();
		}

		void CConnection::OnRead()
		{
			int ret;
			unsigned int recvBytes = 0;
			char recvBuffer[MAX_RW_SIZE + 1];

			while (true)
			{
				if (GetSockState() != NET_SOCK_CONNECTED && GetSockState() != NET_SOCK_LISTENED)
				{
					return;
				}

				if (recvBytes > 0 && m_recvBuffer->GetReadableSize() >= m_maxCacheSize)
				{
					OnReadComplete(m_recvBuffer->GetBuffer(), m_recvBuffer->GetReadableSize());
					if (GetAutoPost())
					{
						Post(m_recvBuffer->GetReadableSize());
					}
					recvBytes = 0;
					continue;
				}

				ret = Recv(recvBuffer, sizeof(recvBuffer) - 1);
				if (-1 == ret)
				{
					if (CheckIsBlock())
					{
						if (recvBytes > 0)
						{
							break; // read complete
						}

						return;
					}

					if (CheckIsClose())
					{
						OnClose();
						return;
					}

					if (recvBytes > 0)
					{
						break; // read complete
					}

					return;
				}
				else if (0 == ret)
				{
					OnClose();
					return;
				}
				else
				{
					recvBytes += ret;
					std::lock_guard<std::recursive_mutex> lock(m_recvBufferMutex);
					m_recvBuffer->Write(recvBuffer, ret);
				}
			}
			
			if (recvBytes > 0)
			{
				OnReadComplete(m_recvBuffer->GetBuffer(), m_recvBuffer->GetReadableSize());
				if (GetAutoPost())
				{
					Post(m_recvBuffer->GetReadableSize());
				}
				return;
			}
		}

		void CConnection::OnWrite()
		{
			if (GetSockState() == NET_SOCK_CONNECTING)
			{
				SetSockState(NET_SOCK_CONNECTED);
				OnConnect();
				return;
			}

			std::vector<PCNetBuffer> completeSendBuffers;

			do
			{
				std::lock_guard<std::recursive_mutex> lock(m_sendQueueMutex);
				if (m_sendBufferQueue.size() > 0)
				{
					int ret;
					while (m_sendBufferQueue.size() > 0)
					{
						if (GetSockState() != NET_SOCK_CONNECTED && GetSockState() != NET_SOCK_LISTENED && GetSockState() != NET_SOCK_CONNECTING)
						{
							return;
						}

						PCNetBuffer sendBuffer = m_sendBufferQueue.front();
						ret = Send(sendBuffer->GetBuffer(), sendBuffer->GetReadableSize());
						if (sendBuffer->GetReadableSize() == ret)
						{
							completeSendBuffers.push_back(sendBuffer);
							m_sendBufferQueue.pop();
							continue;
						}
						else if (-1 == ret)
						{
							if (CheckIsClose())
							{
								OnClose();
								return;
							}
						}

						if (ret > 0)
						{
							sendBuffer->Read(NULL, ret);
						}

						break;
					}
				}
				else
				{
					GetNetBase()->RemoveEvent(this, NET_SOCK_WRITE);
					return;
				}

				if (m_sendBufferQueue.size() == 0)
				{
					GetNetBase()->RemoveEvent(this, NET_SOCK_WRITE);
				}

			} while (0);

			for (int i = 0; i < completeSendBuffers.size(); i++)
			{
				OnWriteComplete(completeSendBuffers[i]->GetBufferSize());
			}

		}

		void CConnection::OnClose()
		{
			Close();
			OnCloseComplete();
		}

		bool CConnection::Write(const char* buf, int len)
		{
			if (NULL == buf || len <= 0)
			{
				return false;
			}

			int ret = 0;
			int sendBytes = 0;
			int sendRetry = 0;
			
			while (sendBytes < len && sendRetry < 3)
			{
				ret = Send(buf + sendBytes, len - sendBytes);
				if (ret >= 0)
				{
					sendBytes += ret;
				}

				if (sendBytes == len)
				{
					OnWriteComplete(len);
					return true;
				}
				else if (-1 == ret)
				{
					if (CheckIsClose())
					{
						OnClose();
						return false;
					}

					sendRetry++;
				}
			}
			
			if (len - sendBytes > 0)
			{
				PCNetBuffer sendBuffer = new CNetBuffer();
				sendBuffer->Write(buf + sendBytes, len - sendBytes);
				do {
					std::lock_guard<std::recursive_mutex> lock(m_sendQueueMutex);
					m_sendBufferQueue.push(sendBuffer);
					GetNetBase()->AddEvent(this, NET_SOCK_WRITE);
				} while (0);
			}

			return true;
		}

		bool CConnection::Post(int len)
		{
			std::lock_guard<std::recursive_mutex> lock(m_recvBufferMutex);
			if (len > 0 && len <= m_recvBuffer->GetReadableSize())
			{
				if (len == m_recvBuffer->Read(NULL, len))
				{
					return true;
				}
			}

			return false;
		}

		void CConnection::SetAutoPost(bool autoPost)
		{
			std::lock_guard<std::recursive_mutex> lock(m_recvBufferMutex);
			m_autoPost = autoPost;
		}

		bool CConnection::GetAutoPost()
		{
			std::lock_guard<std::recursive_mutex> lock(m_recvBufferMutex);
			return m_autoPost;
		}

		void CConnection::SetMaxCacheSize(unsigned int maxCacheSize/* = MAX_CACHE_SIZE*/)
		{
			std::lock_guard<std::recursive_mutex> lock(m_recvBufferMutex);
			m_maxCacheSize = maxCacheSize;
		}

		unsigned int CConnection::GetMaxCacheSize()
		{
			std::lock_guard<std::recursive_mutex> lock(m_recvBufferMutex);
			return m_maxCacheSize;
		}

		std::string CConnection::GetUUID() const
		{
			return m_uuid;
		}

	} // namespace network
} // namespace easygo

