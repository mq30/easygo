#include <iostream>
#include "../include/network/CNetBase.h"
#include "../include/network/CAcceptor.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CListener.h"
#include "../include/network/CSocket.h"
#include "../include/network/CNetConnectTask.h"
#include "../include/network/CConnector.h"
#include "../include/network/CActor.h"

namespace easygo {
	namespace network {

		CAcceptor::CAcceptor(PCNetBase netBase, PCListener listener) : m_bAccept(false)
		{
			SetNetBase(netBase);
			SetListener(listener);

			CThread::Start();
		}

		CAcceptor::~CAcceptor()
		{
		}

		void CAcceptor::Run()
		{
			char strIP[64];
			TNetScoket socket = 0;
			sockaddr_in peerAddr;
			socklen_t addrLen = sizeof(sockaddr_in);
			while (IsRunning())
			{
				do {
					std::unique_lock <std::mutex> lck(m_mutex);
					while (!m_bAccept && IsRunning())
					{
						m_cond.wait(lck);
					}
					m_bAccept = false;
				} while (0);

				if (INVALID_SOCKET == (socket = accept(m_socket, (sockaddr*)&peerAddr, &addrLen)))
				{
					//std::this_thread::sleep_for(std::chrono::milliseconds(100));
					continue;
				}

				easygo::base::CBase<CSocket> baseSocket = m_listener->OnAcceptNewSocket();
				uint32_t ip   = ntohl(peerAddr.sin_addr.s_addr);
				uint16_t port = ntohs(peerAddr.sin_port);

				snprintf(strIP, sizeof(strIP), "%d.%d.%d.%d", ip >> 24, (ip >> 16) & 0xFF, (ip >> 8) & 0xFF, ip & 0xFF);
				if (!SetSockNoDelay())
				{
					closesocket(socket);
					continue;
				}
				
				if (!SetSockNonBlock())
				{
					closesocket(socket);
					continue;
				}

				SetSockCloseOnExec();

				baseSocket->SetNetSocket(socket);
				baseSocket->SetSockState(NET_SOCK_CONNECTED);
				baseSocket->SetRemoteIP(strIP);
				baseSocket->SetRemotePort(port);
				baseSocket->SetListener(m_listener);
				baseSocket->SetNetBase(m_netBase);
				baseSocket->SetLocalIP(m_listener->GetListenIP());
				baseSocket->SetLocalPort(m_listener->GetListenPort());

				m_netBase->AddEvent(baseSocket, NET_SOCK_READ | NET_SOCK_EXCEP);
				easygo::base::CBase<easygo::thread::CTask> task = new easygo::network::CNetConnectTask(baseSocket);
				m_netBase->PushTask(task);
			}

			Close();
		}

		void CAcceptor::OnConnect()
		{
		}

		void CAcceptor::OnRead()
		{
			std::unique_lock <std::mutex> lck(m_mutex);
			m_bAccept = true;
			m_cond.notify_all();
		}

		void CAcceptor::OnWrite()
		{
		}

		void CAcceptor::OnClose()
		{
			m_cond.notify_all();
			m_bRunning = false;
			Stop();
		}

	} // namespace network
} // namespace easygo