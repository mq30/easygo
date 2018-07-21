#ifndef EASYGO_NETWORK_CCONNECTION_H
#define EASYGO_NETWORK_CCONNECTION_H
#include <mutex>
#include <vector>
#include <queue>
#include <deque>
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/thread/CThread.h"
#include "../include/network/CSocket.h"
#include "../include/network/CNetBuffer.h"
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

#define MAX_RW_SIZE		1024
#define MAX_CACHE_SIZE  4096

namespace easygo {
	namespace network {
		class CConnector;
		typedef easygo::base::CBase<CConnector> PCConnector;
		class CNetBase;
		typedef easygo::base::CBase<CNetBase>  PCNetBase;
		class CListener;
		typedef easygo::base::CBase<CListener>  PCListener;
		class CNetBuffer;
		typedef easygo::base::CBase<CNetBuffer>  PCNetBuffer;
	}
}
namespace easygo {
	namespace network {

		class EASYGO_NETWORK_DECL CConnection : public CSocket
		{
		public:
			CConnection(PCNetBase netBase);
			virtual ~CConnection();

		public:
			bool Write(const char* buf, int len);
			bool Post(int len);
			void SetAutoPost(bool autoPost);
			bool GetAutoPost();
			void SetMaxCacheSize(unsigned int maxCacheSize = MAX_CACHE_SIZE);
			unsigned int  GetMaxCacheSize();
			std::string GetUUID() const;
			
		public:
			virtual void OnConnectComplete() = 0;
			virtual void OnReadComplete(const char* buf, int len) = 0;
			virtual void OnWriteComplete(int len) = 0;
			virtual void OnCloseComplete() = 0;

		private:
			virtual void OnConnect();
			virtual void OnRead();
			virtual void OnWrite();
			virtual void OnClose();

		private:
			std::string m_uuid;
			bool m_autoPost;
			unsigned int m_maxCacheSize;
			PCNetBuffer m_recvBuffer;
			std::queue<PCNetBuffer> m_sendBufferQueue;
			std::recursive_mutex m_sendQueueMutex;
			std::recursive_mutex m_recvBufferMutex;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CCONNECTION_H
