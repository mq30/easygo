#ifndef EASYGO_NETWORK_CSOCKET_H
#define EASYGO_NETWORK_CSOCKET_H
#include <string>
#include <mutex>
#include <condition_variable>
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
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

		typedef enum NET_SOCK_STATE
		{
			NET_SOCK_INITED			= 0,
			NET_SOCK_LISTENED		= 1,
			NET_SOCK_CONNECTING		= 2,
			NET_SOCK_CONNECTED		= 3,
			NET_SOCK_CLOSING		= 4,
			NET_SOCK_CLOSED			= 5
		} TNetSockState;

		class EASYGO_NETWORK_DECL CSocket : public easygo::base::CCounter
		{
		public:
			CSocket();
			CSocket(PCNetBase netBase);
			virtual ~CSocket();

		public:
			bool Listen(PCListener listener, const std::string& ip, const unsigned short port, const unsigned int backlog = 2048);
			bool Connect(std::string remoteIP, unsigned short remotePort);
			int  Send(const char* buf, int len);
			int  Recv(char *buf, int len);
			bool Close();
			
		public:
			void SetConnector(PCConnector connector);
			PCConnector GetConnector() const;
			void SetListener(PCListener listener);
			PCListener GetListener() const;
			void SetNetBase(PCNetBase netBase);
			PCNetBase GetNetBase() const;
			TNetScoket GetNetSocket() const;
			void SetNetSocket(TNetScoket socket);
			void SetSockState(TNetSockState state);
			TNetSockState GetSockState();
			void SetSockCode(int code);
			int GetSockCode();
			void SetSockEvent(int event);
			int GetSocketEvent();
			void SetRemoteIP(std::string remoteIP);
			std::string GetRemoteIP();
			void SetRemotePort(unsigned short remotePort);
			unsigned short GetRemotePort();
			std::string GetLocalIP();
			void SetLocalIP(std::string localIP);
			unsigned short GetLocalPort();
			void SetLocalPort(unsigned short localPort);

		public:
			virtual void OnConnect() = 0;
			virtual void OnRead() = 0;
			virtual void OnWrite() = 0;
			virtual void OnClose() = 0;

		public:
			bool SetSockReuseAddr(bool bReuseAddr = true);
			bool SetSockNonBlock(bool bNonBlock = true);
			bool SetSockNoDelay(bool bNoDelay = true);
			bool SetSockCloseOnExec(bool bCloseOnExec = true);
			bool SetSendBufSize(unsigned int sendBufSize);
			bool SetRecvBufSize(unsigned int recvBufSize);

		public:
			static int GetErrorCode();
			static bool CheckIsBlock();
			static bool CheckIsClose();

		protected:
			void SetSockAddr(const std::string& ip, const unsigned short port, sockaddr_in* pSockAddr);

		protected:
			int m_code;
			int m_event;
			TNetSockState m_state;
			TNetScoket m_socket;
			std::string m_localIP;
			unsigned short m_localPort;
			std::string m_remoteIP;
			unsigned short m_remotePort;
			PCListener m_listener;
			PCNetBase m_netBase;
			PCConnector m_connector;
			std::recursive_mutex m_socketMutex;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CSOCKET_H