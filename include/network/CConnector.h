#ifndef EASYGO_NETWORK_CCONNECTOR_H
#define EASYGO_NETWORK_CCONNECTOR_H
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
		class CActor;
		typedef easygo::base::CBase<CActor> PCActor;
		class CNetBase;
		typedef easygo::base::CBase<CNetBase>  PCNetBase;
		class CListener;
		typedef easygo::base::CBase<CListener>  PCListener;
	}
}

namespace easygo {
	namespace network {

		class EASYGO_NETWORK_DECL CConnector : public easygo::base::CCounter
		{
		public:
			CConnector(PCNetBase netBase, const std::string remoteIp, const unsigned short remotePort);
			virtual ~CConnector();

		public:
			bool Connect();
			bool Write(const char* buf, int len);
			bool Post(int len);
			void SetAutoPost(bool autoPost);
			bool GetAutoPost() const;

		public:
			PCNetBase GetNetBase() const;
			std::string GetRemoteIP() const;
			unsigned short GetRemotePort() const;
			PCActor GetActor() const;

		public:
			virtual void OnConnectComplete() = 0;
			virtual void OnReadComplete(const char* buf, int len) = 0;
			virtual void OnWriteComplete(int len) = 0;
			virtual void OnCloseComplete() = 0;

		private:
			std::string m_remoteIP;
			unsigned short m_remotePort;
			PCNetBase m_netBase;
			PCActor m_actor;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CCONNECTOR_H