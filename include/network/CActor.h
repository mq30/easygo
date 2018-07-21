#ifndef EASYGO_NETWORK_CACTOR_H
#define EASYGO_NETWORK_CACTOR_H
#include <string>
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/thread/CThread.h"
#include "../include/declexport.h"
#include "../include/network/CSocket.h"
#include "../include/network/CConnection.h"
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
		class CConnector;
		typedef easygo::base::CBase<CConnector> PCConnector;
		class CNetBase;
		typedef easygo::base::CBase<CNetBase>  PCNetBase;
		class CListener;
		typedef easygo::base::CBase<CListener>  PCListener;
	}
}

namespace easygo {
	namespace network {

		class CConnection;

		class EASYGO_NETWORK_DECL CActor : public CConnection
		{
		public:
			CActor(PCNetBase netBase, PCConnector connector);
			virtual ~CActor();

		public:
			virtual void OnConnectComplete();
			virtual void OnReadComplete(const char* buf, int len);
			virtual void OnWriteComplete(int len);
			virtual void OnCloseComplete();

		private:

		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CACTOR_H