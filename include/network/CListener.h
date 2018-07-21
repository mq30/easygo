#ifndef EASYGO_NETWORK_CLISTENER_H
#define EASYGO_NETWORK_CLISTENER_H
#include <string>
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/declexport.h"
#include "../include/network/CSocket.h"
#include "../include/base/CBase.h"

#ifndef EASYGO_NETWORK_DECL
#	ifdef EASYGO_DYNAMIC_LINK
#		if defined(EXPORT_NETWORK)
#			define EASYGO_NETWORK_DECL EASYGO_DECL_EXPORT
#		else
#			define EASYGO_NETWORK_DECL EASYGO_DECL_IMPORT
#		endif
#	else
#		define EASYGO_NETWORK_DECL
#	endif
#endif

namespace easygo {
	namespace network {
		class CSocket;
		typedef easygo::base::CBase<CSocket> PCSocket;
		class CNetBase;
		typedef easygo::base::CBase<CNetBase>  PCNetBase;
	}
}

namespace easygo {
	namespace network {

		class EASYGO_NETWORK_DECL CListener : public easygo::base::CCounter
		{
		public:
			CListener(PCNetBase netBase, const std::string& listenIP, const unsigned short listenPort, const unsigned int backlog = 2048);
			virtual ~CListener();

		public:
			bool Listen();

		public:
			virtual void OnEstablishConnection() = 0;
			virtual PCSocket OnAcceptNewSocket() = 0;

		public:
			PCSocket GetAcceptor() const;
			PCNetBase GetNetBase() const;
			std::string GetListenIP() const;
			unsigned short GetListenPort() const;

		private:
			std::string m_listenIP;
			unsigned short m_listenPort;
			PCSocket m_acceptor;
			PCNetBase m_netBase;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CLISTENER_H