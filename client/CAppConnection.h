#ifndef EASYGO_NETWORK_CAPPCONNECTION_H
#define EASYGO_NETWORK_CAPPCONNECTION_H
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/base/CBase.h"
#include "../include/network/CSocket.h"
#include "../include/network/CConnection.h"

namespace easygo {
	namespace network {

		class CSocket;
		class CConnection;
		class CNetBase;

		class CAppConnection : public easygo::network::CConnection
		{
		public:
			CAppConnection(easygo::base::CBase<CNetBase> netBase);
			virtual ~CAppConnection();

		public:
			virtual void OnConnectComplete();
			virtual void OnReadComplete(const char* buf, int len);
			virtual void OnWriteComplete(int len);
			virtual void OnCloseComplete();

		private:

		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CAPPCONNECTION_H
