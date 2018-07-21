#include "../include/network/CConnector.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CActor.h"
#include "../include/network/CSocket.h"
#include "../include/network/CListener.h"

namespace easygo {
	namespace network {

		CConnector::CConnector(PCNetBase netBase, const std::string remoteIp, const unsigned short remotePort) :
		m_netBase(netBase), m_remoteIP(remoteIp), m_remotePort(remotePort), m_actor()
		{
		}

		CConnector::~CConnector()
		{
		}

		bool CConnector::Connect()
		{
			m_actor = new CActor(m_netBase, this);
			m_actor->SetNetBase(m_netBase);
			m_actor->SetConnector(this);
			return m_actor->Connect(m_remoteIP, m_remotePort);
		}

		bool CConnector::Write(const char * buf, int len)
		{
			return GetActor()->Write(buf, len);;
		}

		bool CConnector::Post(int len)
		{
			return GetActor()->Post(len);
		}

		void CConnector::SetAutoPost(bool autoPost)
		{
			GetActor()->SetAutoPost(autoPost);
		}

		bool CConnector::GetAutoPost() const
		{
			return GetActor()->GetAutoPost();
		}

		PCNetBase CConnector::GetNetBase() const
		{
			return m_netBase;
		}

		std::string CConnector::GetRemoteIP() const
		{
			return m_remoteIP;
		}

		unsigned short CConnector::GetRemotePort() const
		{
			return m_remotePort;
		}

		PCActor CConnector::GetActor() const
		{
			return m_actor;
		}

	} // namespace network
} // namespace easygo
