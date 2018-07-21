#include "../include/network/CSocket.h"
#include "../include/network/CNetCloseTask.h"
#include "../include/network/CListener.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"

namespace easygo {
	namespace network {

		CNetCloseTask::CNetCloseTask(PCSocket socket)
		{
			SetTaskId((unsigned long long)socket->GetNetSocket());
			m_socket = socket;
		}

		CNetCloseTask::~CNetCloseTask()
		{
		}

		void CNetCloseTask::Execute()
		{
			if (m_socket)
			{
				m_socket->OnClose();
			}
		}

		void CNetCloseTask::SetSocket(PCSocket socket)
		{
			m_socket = socket;
		}

		PCSocket CNetCloseTask::GetSocket() const
		{
			return m_socket;
		}

	} // namespace network
} // namespace easygo