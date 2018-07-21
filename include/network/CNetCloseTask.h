#ifndef EASYGO_NETWORK_CNETCLOSETASK_H
#define EASYGO_NETWORK_CNETCLOSETASK_H
#include "../include/base/CCounter.h"
#include "../include/thread/CTask.h"
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
		class CSocket;
		typedef easygo::base::CBase<CSocket> PCSocket;
	}
}

namespace easygo {
	namespace network {

		class EASYGO_NETWORK_DECL CNetCloseTask : public easygo::thread::CTask
		{
		public:
			CNetCloseTask(PCSocket socket);
			virtual ~CNetCloseTask();
			virtual void Execute();
			void SetSocket(PCSocket socket);
			PCSocket GetSocket() const;

		private:
			PCSocket m_socket;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CNETCLOSETASK_H
