#include "../include/network/CSocket.h"
#include "../include/network/CNetConnectTask.h"
#include "../include/network/CListener.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"

namespace easygo {
	namespace network {

		CNetConnectTask::CNetConnectTask(PCSocket socket)
		{
			SetTaskId((unsigned long long)socket->GetNetSocket());
			m_socket = socket;
		}

		CNetConnectTask::~CNetConnectTask()
		{
		}

		void CNetConnectTask::Execute()
		{
			if (m_socket)
			{
				m_socket->OnConnect();
			}
		}

		void CNetConnectTask::SetSocket(PCSocket socket)
		{
			m_socket = socket;
		}

		PCSocket CNetConnectTask::GetSocket() const
		{
			return m_socket;
		}

	} // namespace network
} // namespace easygo