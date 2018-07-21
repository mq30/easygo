#include <cstring>
#include <chrono>
#include <thread>
#include <iostream>
#include "../include/network/CSocket.h"
#include "../include/network/CListener.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"

namespace easygo {
	namespace network {

		CSocket::CSocket() :
		m_state(NET_SOCK_INITED), m_socket(INVALID_SOCKET), m_localIP(""), m_localPort(0), m_remoteIP(""), m_remotePort(0),
		m_event(0), m_netBase(), m_listener(), m_connector(), m_socketMutex(), m_code(0)
		{
		}

		CSocket::CSocket(PCNetBase netBase) : 
		m_state(NET_SOCK_INITED), m_socket(INVALID_SOCKET), m_netBase(netBase), m_event(0),
		m_localIP(""), m_localPort(0), m_remoteIP(""), m_remotePort(0), m_listener(), m_connector(),
	    m_socketMutex(), m_code(0)
		{
		}

		CSocket::~CSocket()
		{
		}

		bool CSocket::Listen(PCListener listener, const std::string& ip, const unsigned short port, const unsigned int backlog /*= 2048*/)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

			if (!listener)
			{
				return false;
			}

			if (NET_SOCK_INITED != m_state)
			{
				return false;
			}

			m_socket = socket(AF_INET, SOCK_STREAM, 0);
			if (INVALID_SOCKET == m_socket)
			{
				return false;
			}

			if (!SetSockReuseAddr())
			{
				closesocket(m_socket);
				m_state = NET_SOCK_CLOSED;
				return false;
			}

			if (!SetSockNonBlock())
			{
				closesocket(m_socket);
				m_state = NET_SOCK_CLOSED;
				return false;
			}

			sockaddr_in serverAddr;
			SetSockAddr(ip, port, &serverAddr);
			if (SOCKET_ERROR == bind(m_socket, (sockaddr*)&serverAddr, sizeof(serverAddr)))
			{
				closesocket(m_socket);
				m_state = NET_SOCK_CLOSED;
				return false;
			}


			if (SOCKET_ERROR == listen(m_socket, backlog > SOMAXCONN? backlog : SOMAXCONN))
			{
				closesocket(m_socket);
				m_state = NET_SOCK_CLOSED;
				return false;
			}

			m_localIP = ip;
			m_localPort = port;
			m_state = NET_SOCK_LISTENED;
			m_listener = listener;

			m_netBase->AddEvent(this, NET_SOCK_READ | NET_SOCK_EXCEP);

			return true;
		}

		bool CSocket::Connect(std::string remoteIP, unsigned short remotePort)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

			if (NET_SOCK_CONNECTED == m_state)
			{
				return true;
			}

			m_socket = socket(AF_INET, SOCK_STREAM, 0);
			if (INVALID_SOCKET == m_socket)
			{
				return false;
			}

			SetSockNonBlock();
			SetSockNoDelay();

			sockaddr_in remoteAddr;
			SetSockAddr(remoteIP, remotePort, &remoteAddr);
			int ret = connect(m_socket, (sockaddr*)&remoteAddr, sizeof(remoteAddr));
			if ((ret == SOCKET_ERROR) && (!CheckIsBlock()))
			{
				closesocket(m_socket);
				return false;
			}

			m_state = NET_SOCK_CONNECTING;
			m_remoteIP = remoteIP;
			m_remotePort = remotePort;
			m_netBase->AddEvent(this, NET_SOCK_ALL);

			return true;
		}

		int CSocket::Send(const char* buf, int len)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

			if (NET_SOCK_CONNECTED != m_state)
			{
				return -1;
			}

#ifdef WIN32
			return send(m_socket, buf, len, MSG_PARTIAL);
#else
			return send(m_socket, buf, len, MSG_DONTWAIT);
#endif
		}

		int CSocket::Recv(char * buf, int len)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
#ifdef WIN32
			return recv(m_socket, (char*)buf, len, MSG_PARTIAL);
#else
			return recv(m_socket, (char*)buf, len, MSG_DONTWAIT);
#endif
		}

		bool CSocket::Close()
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			m_state = NET_SOCK_CLOSING;
			m_netBase->RemoveEvent(this, NET_SOCK_ALL);
			m_state = NET_SOCK_CLOSED;
			return true;
		}

		void CSocket::SetNetBase(PCNetBase netBase)
		{
			m_netBase = netBase;
		}

		PCNetBase CSocket::GetNetBase() const
		{
			return m_netBase;
		}

		void CSocket::SetConnector(PCConnector connector)
		{
			m_connector = connector;
		}

		PCConnector CSocket::GetConnector() const
		{
			return m_connector;
		}

		void CSocket::SetListener(PCListener listener)
		{
			m_listener = listener;
		}

		PCListener CSocket::GetListener() const
		{
			return m_listener;
		}

		bool CSocket::SetSockReuseAddr(bool bReuseAddr)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

			int reuseAddr = 1;
			if (false == bReuseAddr)
			{
				reuseAddr = 0;
			}

			if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&reuseAddr, sizeof(reuseAddr)))
			{
				return false;
			}

			return true;
		}

		bool CSocket::SetSockNonBlock(bool bNonBlock)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

#ifdef WIN32
			unsigned long nonBlock = 1;

			if (false == bNonBlock)
			{
				nonBlock = 0;
			}

			if (SOCKET_ERROR == ioctlsocket(m_socket, FIONBIO, &nonBlock))
			{
				return false;
			}
#else
			if (bNonBlock)
			{
				if (SOCKET_ERROR == fcntl(m_socket, F_SETFL, O_NONBLOCK | fcntl(m_socket, F_GETFL)))
				{
					std::cout << "set nonblock false......" << std::endl;
					return false;
				}
			}
			else
			{
				if (SOCKET_ERROR == fcntl(m_socket, F_SETFL, (~O_NONBLOCK) & fcntl(m_socket, F_GETFL)))
				{
					return false;
				}
			}

#endif
			return true;
		}

		bool CSocket::SetSockNoDelay(bool bNoDelay)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

			int noDelay = 1;
			if (false == bNoDelay)
			{
				noDelay = 0;
			}

			if (SOCKET_ERROR == setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&noDelay, sizeof(noDelay)))
			{
				return false;
			}

			return true;
		}

		bool CSocket::SetSockCloseOnExec(bool bCloseOnExec)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

#ifdef WIN32
			HANDLE handle = (HANDLE)m_socket;//TODO

			const DWORD dwFlags = bCloseOnExec ? 1 : 0;
			if (!SetHandleInformation(handle, HANDLE_FLAG_INHERIT, dwFlags))
			{
				return false;
			}

#else
			if (closeOnExec)
			{
				if (SOCKET_ERROR == fcntl(m_socket, F_SETFL, FD_CLOEXEC | fcntl(m_socket, F_GETFL)))
				{
					std::cout << "set closeonexec false......" << std::endl;
					return false;
				}
			}
			else
			{
				if (SOCKET_ERROR == fcntl(m_socket, F_SETFL, (~FD_CLOEXEC) & fcntl(m_socket, F_GETFL)))
				{
					return false;
				}
			}

#endif
			return true;
		}

		bool CSocket::SetSendBufSize(unsigned int sendBufSize)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

			if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char *)&sendBufSize, 4)) 
			{
				return false;
			}

			socklen_t len = 4;
			unsigned int size = 0;
			getsockopt(m_socket, SOL_SOCKET, SO_SNDBUF, (char *)&size, &len);
			if (size != sendBufSize)
			{
				return false;
			}

			return true;
		}

		bool CSocket::SetRecvBufSize(unsigned int recvBufSize)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

			if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char *)&recvBufSize, 4))
			{
				return false;
			}

			socklen_t len = 4;
			unsigned int size = 0;
			getsockopt(m_socket, SOL_SOCKET, SO_RCVBUF, (char *)&size, &len);
			if (size != recvBufSize)
			{
				return false;
			}

			return true;
		}

		TNetScoket CSocket::GetNetSocket() const
		{
			return m_socket;
		}

		void CSocket::SetNetSocket(TNetScoket socket)
		{
			m_socket = socket;
		}

		void CSocket::SetSockState(TNetSockState state)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			m_state = state;
		}

		TNetSockState CSocket::GetSockState()
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			return m_state;
		}

		void CSocket::SetSockCode(int code)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			m_code = code;
		}

		int CSocket::GetSockCode()
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			return m_code;
		}

		void CSocket::SetSockEvent(int event)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			m_event = event;
		}

		int CSocket::GetSocketEvent()
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			return m_event;
		}

		void CSocket::SetRemoteIP(std::string remoteIP)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			m_remoteIP = remoteIP;
		}

		std::string CSocket::GetRemoteIP()
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			return m_remoteIP;
		}

		void CSocket::SetRemotePort(unsigned short remotePort)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			m_remotePort = remotePort;
		}

		unsigned short CSocket::GetRemotePort()
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			return m_remotePort;
		}

		std::string CSocket::GetLocalIP()
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

			if (!m_localIP.empty())
			{
				return m_localIP;
			}

			sockaddr_in localAddr;
			socklen_t addrLen = sizeof(sockaddr_in);
			if (INVALID_SOCKET == getsockname(m_socket, (struct sockaddr *)&localAddr, &addrLen))
			{
				return "0.0.0.0";
			}

			uint32_t ip = ntohl(localAddr.sin_addr.s_addr);
			m_localPort = ntohs(localAddr.sin_port);

			char strIP[64];
			snprintf(strIP, sizeof(strIP), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
			m_localIP = strIP;

			return m_localIP;
		}

		void CSocket::SetLocalIP(std::string localIP)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			m_localIP = localIP;
		}

		unsigned short CSocket::GetLocalPort()
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);

			if (0 != m_localPort)
			{
				return m_localPort;
			}

			sockaddr_in localAddr;
			socklen_t addrLen = sizeof(sockaddr_in);
			if (INVALID_SOCKET == getsockname(m_socket, (struct sockaddr *)&localAddr, &addrLen))
			{
				return 0;
			}

			uint32_t ip = ntohl(localAddr.sin_addr.s_addr);
			m_localPort = ntohs(localAddr.sin_port);

			char strIP[64];
			snprintf(strIP, sizeof(strIP), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
			m_localIP = strIP;

			return m_localPort;
		}

		void CSocket::SetLocalPort(unsigned short localPort)
		{
			std::lock_guard<std::recursive_mutex> lock(m_socketMutex);
			m_localPort = localPort;
		}

		int CSocket::GetErrorCode()
		{
#ifdef WIN32
			return WSAGetLastError();
#else
			return errno;
#endif
		}

		bool CSocket::CheckIsBlock()
		{
			int errCode = GetErrorCode();
#ifdef WIN32
			return ((WSAEINPROGRESS == errCode) || (WSAEWOULDBLOCK == errCode));
#else
			return ((EINPROGRESS == errCode) || (EWOULDBLOCK == errCode) || (EAGAIN == errCode));
#endif
		}

		bool CSocket::CheckIsClose()
		{
			int errCode = GetErrorCode();
#ifdef WIN32
			return ((WSAECONNABORTED == errCode) || (WSAETIMEDOUT == errCode) || (WSAECONNRESET == errCode)   || 
				    (WSAECONNABORTED == errCode) || (WSAENOTSOCK == errCode)  || (WSAEHOSTUNREACH == errCode) || (WSAENOTCONN == errCode));
#else
			return ((ENOTCONN == errCode) || (EWOULDBLOCK == errCode) || (EBADF == errCode) || (ECONNREFUSED == errCode));
#endif
		}

		void CSocket::SetSockAddr(const std::string& ip, const unsigned short port, sockaddr_in* pSockAddr)
		{
			memset(pSockAddr, 0x00, sizeof(sockaddr_in));
			pSockAddr->sin_family = AF_INET;
			pSockAddr->sin_port = htons(port);
			pSockAddr->sin_addr.s_addr = inet_addr(ip.c_str());
			if (pSockAddr->sin_addr.s_addr == INADDR_NONE)
			{
				hostent* host = gethostbyname(ip.c_str());
				if (host == NULL)
				{
					return;
				}

				pSockAddr->sin_addr.s_addr = *(uint32_t*)host->h_addr;
			}
		}

	} // namespace network
} // namespace easygo