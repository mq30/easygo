#include <cstring>
#include "CAppConnection.h"
#include "../include/network/CListener.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CNetBuffer.h"
#include <iostream>

namespace easygo {
	namespace network {

		CAppConnection::CAppConnection(easygo::base::CBase<CNetBase> netBase) : easygo::network::CConnection(netBase)
		{
			SetAutoPost(true);
		}

		CAppConnection::~CAppConnection()
		{
		}

		void CAppConnection::OnConnectComplete()
		{
		}

		void CAppConnection::OnReadComplete(const char * buf, int len)
		{
			std::cout << "OnReadComplete: " << len << std::endl;
			Write(buf, len);
			if (!GetAutoPost())
			{
				Post(len);
			}
		}

		void CAppConnection::OnWriteComplete(int len)
		{
			std::cout << "OnWriteComplete: " << len << std::endl;
		}

		void CAppConnection::OnCloseComplete()
		{
		}

	} // namespace network
} // namespace easygo

