#include <iostream>
#include <cstring>
#include "CAppConnector.h"
#include "CAppConnection.h"
#include "../include/network/CListener.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"
#include "../include/network/CNetBase.h"

namespace easygo {
	namespace network {

		CAppConnector::CAppConnector(PCNetBase netBase, const std::string remoteIp, const unsigned short remotePort) :
		CConnector(netBase, remoteIp, remotePort)
		{
		}

		CAppConnector::~CAppConnector()
		{
		}

		void CAppConnector::OnConnectComplete()
		{
			Write("0123456789\n", strlen("0123456789\n"));
		}

		void CAppConnector::OnReadComplete(const char* buf, int len)
		{
			std::cout << "OnReadComplete: " << len << std::endl;
			Write(buf, len);
			if (!GetAutoPost())
			{
				Post(len);
			}
		}

		void CAppConnector::OnWriteComplete(int len)
		{
			std::cout << "OnWriteComplete: " << len << std::endl;
		}

		void CAppConnector::OnCloseComplete()
		{
			std::cout << "OnCloseComplete: " << std::endl;
		}

	} // namespace network
} // namespace easygo