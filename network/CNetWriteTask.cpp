#include "../include/network/CSocket.h"
#include "../include/network/CNetWriteTask.h"
#include "../include/network/CListener.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"

namespace easygo {
	namespace network {

		CNetWriteTask::CNetWriteTask(PCSocket socket)
		{
			SetTaskId((unsigned long long)socket->GetNetSocket());
			m_socket = socket;
		}

		CNetWriteTask::~CNetWriteTask()
		{
		}

		void CNetWriteTask::Execute()
		{
			if (m_socket)
			{
				m_socket->OnWrite();
			}
		}

		void CNetWriteTask::SetSocket(PCSocket socket)
		{
			m_socket = socket;
		}

		PCSocket CNetWriteTask::GetSocket() const
		{
			return m_socket;
		}

	} // namespace network
} // namespace easygo