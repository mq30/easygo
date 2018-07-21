#ifndef EASYGO_NETWORK_CACCEPTOR_H
#define EASYGO_NETWORK_CACCEPTOR_H
#include <mutex>
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/thread/CThread.h"
#include "../include/network/CSocket.h"
#include "../include/declexport.h"
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

		class EASYGO_NETWORK_DECL CAcceptor : public CSocket, public easygo::thread::CThread
		{
		public:
			CAcceptor(PCNetBase netBase, PCListener listener);
			virtual ~CAcceptor();

		public:
			virtual void Run();

		public:
			virtual void OnConnect();
			virtual void OnRead();
			virtual void OnWrite();
			virtual void OnClose();

		private:
			std::mutex m_mutex;
			std::condition_variable m_cond;
			bool m_bAccept;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CACCEPTOR_H