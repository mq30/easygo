#ifndef EASYGO_NETWORK_CAPPLISTENER_H
#define EASYGO_NETWORK_CAPPLISTENER_H
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/base/CBase.h"
#include "../include/network/CSocket.h"
#include "../include/network/CListener.h"
#include "../include/network/CActor.h"

namespace easygo {
	namespace network {

		class CSocket;
		class CNetBase;

		class CAppListener : public easygo::network::CListener
		{
		public:
			CAppListener(PCNetBase netBase, const std::string& listenIP, const unsigned short listenPort);
			virtual ~CAppListener();

		public:
			virtual void OnEstablishConnection();
			virtual PCSocket OnAcceptNewSocket();

		private:

		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CAPPLISTENER_H
