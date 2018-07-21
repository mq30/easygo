#ifndef EASYGO_NETWORK_CAPPCONNECTOR_H
#define EASYGO_NETWORK_CAPPCONNECTOR_H
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/base/CBase.h"
#include "../include/network/CSocket.h"
#include "../include/network/CConnector.h"

namespace easygo {
	namespace network {

		class CSocket;
		class CNetBase;

		class CAppConnector : public easygo::network::CConnector
		{
		public:
			CAppConnector(PCNetBase netBase, const std::string remoteIp, const unsigned short remotePort);
			virtual ~CAppConnector();

		public:
			virtual void OnConnectComplete();
			virtual void OnReadComplete(const char* buf, int len);
			virtual void OnWriteComplete(int len);
			virtual void OnCloseComplete();

		private:

		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CAPPCONNECTOR_H
