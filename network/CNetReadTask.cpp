#include "../include/network/CSocket.h"
#include "../include/network/CNetReadTask.h"
#include "../include/network/CListener.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"

namespace easygo {
	namespace network {

		CNetReadTask::CNetReadTask(PCSocket socket) 
		{
			SetTaskId((unsigned long long)socket->GetNetSocket());
			m_socket = socket;
		}

		CNetReadTask::~CNetReadTask()
		{
		}

		void CNetReadTask::Execute()
		{
			if (m_socket)
			{
				m_socket->OnRead();
			}
		}

		void CNetReadTask::SetSocket(PCSocket socket)
		{
			m_socket = socket;
		}

		PCSocket CNetReadTask::GetSocket() const
		{
			return m_socket;
		}

	} // namespace network
} // namespace easygo