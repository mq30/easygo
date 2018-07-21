#include "CAppConnection.h"
#include "CAppListener.h"
#include "../include/network/CListener.h"
#include "../include/network/CConnector.h"
#include "../include/network/CNetBase.h"

namespace easygo {
	namespace network {

		CAppListener::CAppListener(PCNetBase netBase, const std::string& listenIP, const unsigned short listenPort) : 
		CListener(netBase, listenIP, listenPort)
		{
		}

		CAppListener::~CAppListener()
		{
		}

		void CAppListener::OnEstablishConnection()
		{
		}

		PCSocket CAppListener::OnAcceptNewSocket()
		{
			return new CAppConnection(GetNetBase());
		}

	} // namespace network
} // namespace easygo
