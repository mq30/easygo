#include "../include/network/CListener.h"
#include "../include/network/CSocket.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CAcceptor.h"
#include "../include/network/CSocket.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"

namespace easygo {
	namespace network {

		CListener::CListener(PCNetBase netBase, const std::string& listenIP, const unsigned short listenPort, const unsigned int backlog/* = 2048*/):
		m_netBase(netBase), m_listenIP(listenIP), m_listenPort(listenPort)
		{
		}

		CListener::~CListener()
		{
		}

		bool CListener::Listen()
		{
			m_acceptor = new CAcceptor(m_netBase, this);
			return m_acceptor->Listen(this, m_listenIP, m_listenPort);
		}

		PCSocket CListener::GetAcceptor() const
		{
			return m_acceptor;
		}

		PCNetBase CListener::GetNetBase() const
		{
			return m_netBase;
		}

		std::string CListener::GetListenIP() const
		{
			return m_listenIP;
		}

		unsigned short CListener::GetListenPort() const
		{
			return m_listenPort;
		}

	} // namespace network
} // namespace easygo