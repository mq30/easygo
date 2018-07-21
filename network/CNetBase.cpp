#include "../include/network/CNetBase.h"
#include "../include/network/CListener.h"
#include "../include/network/CConnector.h"
#include "../include/network/CSocket.h"
#include "../include/network/CActor.h"
#include "../include/network/CNetReadTask.h"
#include "../include/network/CNetWriteTask.h"
#include "../include/network/CNetCloseTask.h"
#include "../include/network/CNetClock.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace easygo {
	namespace network {
		
#define MIN_TIMER_DURATION	10

		CNetBase::CNetBase() : m_bRunLoop(false), m_bInit(false), m_netBaseMutex(),
		m_closingSocks(), m_mainThreadMutex(), m_mainThreadCond(), m_threadPool(), 
		m_netClock(new easygo::network::CNetClock), m_inactiveTimers(), m_activeTimers(),
		m_netTime(0L)
#ifdef WIN32 
#elif defined(__APPLE__) || defined(__FreeBSD__)

#else
		, m_addSet(), m_modSet(), m_delSet()
#endif
		{
#ifdef WIN32
			FD_ZERO(&m_readSet);
			FD_ZERO(&m_writeSet);
			FD_ZERO(&m_excepSet);
#endif
			m_socketMap.clear();
			m_closingSocks.clear();
		}

		CNetBase::~CNetBase()
		{
		}

		bool CNetBase::Init()
		{
#ifdef WIN32
			WSADATA wsaData;
			WORD wReq = MAKEWORD(1, 1);
			if (0 != WSAStartup(wReq, &wsaData))
			{
				return false;
			}

			FD_ZERO(&m_readSet);
			FD_ZERO(&m_writeSet);
			FD_ZERO(&m_excepSet);

			m_bInit = true;

			return true;			
#elif defined(__APPLE__) || defined(__FreeBSD__)
			m_kqfd = kqueue();
			if (-1 == m_kqfd)
			{
				return false;
			}

			m_bInit = true;

			return true;
#else
			m_epfd = epoll_create(2048);
			if (-1 == m_epfd)
			{
				return false;
			}

			m_bInit = true;

			return true;
#endif
		}

		bool CNetBase::UnInit()
		{
			if (!m_bInit)
			{
				return true;
			}
#ifdef WIN32
			if (0 != WSACleanup())
			{
				return false;
			}
#elif defined(__APPLE__) || defined(__FreeBSD__)
			close(m_kqfd);
#else
			close(m_epfd);
#endif
			m_bInit = false;

			return true;
		}

		bool CNetBase::Listen(PCListener listener)
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return false;
			}

			return listener->Listen();
		}

		bool CNetBase::Connect(PCConnector connector)
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return false;
			}

			return connector->Connect();
		}

		void CNetBase::Run()
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return;
			}

			std::unique_lock <std::mutex> lck(m_mainThreadMutex);

#ifdef WIN32
			fd_set readSet, writeSet, excepSet;
			timeval timeout;
			timeout.tv_sec = 0;		// seconds
			timeout.tv_usec = 100;  // microseconds
			int optVal = 0;
			int optLen = sizeof(int);

			while (m_bRunLoop)
			{
				PrepareTimer();

				if (!m_readSet.fd_count && !m_writeSet.fd_count && !m_excepSet.fd_count)
				{
					std::this_thread::sleep_for(std::chrono::microseconds(MIN_TIMER_DURATION));
					ProcessTimer();
					continue;
				}

				timeout.tv_sec  = 0;
				timeout.tv_usec = MIN_TIMER_DURATION;

				FD_ZERO(&readSet);
				FD_ZERO(&writeSet);
				FD_ZERO(&excepSet);

				do {
					std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
					memcpy(&readSet, &m_readSet, sizeof(fd_set));
					memcpy(&writeSet, &m_writeSet, sizeof(fd_set));
					memcpy(&excepSet, &m_excepSet, sizeof(fd_set));

					if (m_closingSocks.size() > 0)
					{
						std::set<TNetScoket>::iterator it = m_closingSocks.begin();
						while (it != m_closingSocks.end())
						{
							closesocket(*it);
							it++;
						}
						m_closingSocks.clear();
					}

				} while (0);

				int nfds = select(0, &readSet, &writeSet, &excepSet, &timeout);
				ProcessTimer();
				if (nfds == SOCKET_ERROR)
				{
					continue;
				}

				if (nfds == 0)
				{
					m_threadPool->PrintTaskStatics();
					continue;
				}

				for (int i = 0; i < readSet.fd_count; i++)
				{
					TNetScoket sockfd = readSet.fd_array[i];
					PCSocket baseSocket = FindBaseSocket(sockfd);
					if (baseSocket)
					{
						easygo::base::CBase<easygo::thread::CTask> task = new easygo::network::CNetReadTask(baseSocket);
						m_threadPool->PushTask(task);
					}
				}

				for (int i = 0; i < writeSet.fd_count; i++)
				{
					TNetScoket sockfd = writeSet.fd_array[i];
					PCSocket baseSocket = FindBaseSocket(sockfd);
					if (baseSocket)
					{
						easygo::base::CBase<easygo::thread::CTask> task = new easygo::network::CNetWriteTask(baseSocket);
						m_threadPool->PushTask(task);
					}
				}

				for (int i = 0; i < excepSet.fd_count; i++)
				{
					TNetScoket sockfd = excepSet.fd_array[i];
					PCSocket baseSocket = FindBaseSocket(sockfd);
					if (baseSocket)
					{
						if (NET_SOCK_CONNECTING == baseSocket->GetSockState())
						{
							optVal = 0;
							if (0 == getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *)&optVal, &optLen))
							{
								baseSocket->SetSockCode(optVal);
							}
						}
						easygo::base::CBase<easygo::thread::CTask> task = new easygo::network::CNetCloseTask(baseSocket);
						m_threadPool->PushTask(task);
					}
				}
			}

#elif defined(__APPLE__) || defined(__FreeBSD__)
			struct kevent events[2048];
			int nfds = 0;
			struct timespec timeout;
			timeout.tv_sec = 0;
			timeout.tv_nsec = 100;

			while (m_bRunLoop)
			{
				PrepareTimer();

				timeout.tv_sec = 0;
				timeout.tv_nsec = MIN_TIMER_DURATION;
				do {
					std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
					if (m_closingSocks.size() > 0)
					{
						std::set<TNetScoket>::iterator it = m_closingSocks.begin();
						while (it != m_closingSocks.end())
						{
							closesocket(*it);
							it++;
						}
						m_closingSocks.clear();
					}
					nfds = kevent(m_kqfd, NULL, 0, events, 2048, &timeout);
					ProcessTimer();
				} while (0);

				for (int i = 0; i < nfds; i++)
				{
					TNetScoket sockfd = events[i].ident;
					PCSocket baseSocket = FindBaseSocket(sockfd);
					if (!baseSocket)
					{
						continue;
					}

					if (events[i].filter == EVFILT_READ)
					{
						easygo::base::CBase<easygo::thread::CTask> task = new easygo::network::CNetReadTask(baseSocket);
						m_threadPool->PushTask(task);
					}

					if (events[i].filter == EVFILT_WRITE)
					{
						easygo::base::CBase<easygo::thread::CTask> task = new easygo::network::CNetWriteTask(baseSocket);
						m_threadPool->PushTask(task);
					}
				}
			}

#else
			struct epoll_event events[2048];
			int nfds = 0;

			while (m_bRunLoop)
			{
				PrepareTimer();

				do {
					std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
					std::map<TNetScoket, uint32_t>::iterator itAdd = m_addSet.begin();
					while (itAdd != m_addSet.end())
					{
						struct epoll_event ev;
						ev.events = itAdd->second;
						ev.data.fd = itAdd->first;
						epoll_ctl(m_epfd, EPOLL_CTL_ADD, itAdd->first, &ev);
						itAdd++;
					}
					m_addSet.clear();

					std::map<TNetScoket, uint32_t>::iterator itMod = m_modSet.begin();
					while (itMod != m_modSet.end())
					{
						struct epoll_event ev;
						ev.events  = itMod->second;
						ev.data.fd = itMod->first;
						epoll_ctl(m_epfd, EPOLL_CTL_MOD, itMod->first, &ev);
						itMod++;
					}
					m_modSet.clear();

					std::map<TNetScoket, uint32_t>::iterator itDel = m_delSet.begin();
					while (itDel != m_delSet.end())
					{
						struct epoll_event ev;
						ev.events = itDel->second;
						ev.data.fd = itDel->first;
						// ev not NULL to be portable to kernels before 2.6.9
						epoll_ctl(m_epfd, EPOLL_CTL_DEL, itDel->first, &ev);
						itDel++;
					}	
					m_delSet.clear();

					if (m_closingSocks.size() > 0)
					{
						std::set<TNetScoket>::iterator it = m_closingSocks.begin();
						while (it != m_closingSocks.end())
						{
							closesocket(*it);
							it++;
						}
						m_closingSocks.clear();
					}
				} while (0);

				nfds = epoll_wait(m_epfd, events, 2048, MIN_TIMER_DURATION);
				ProcessTimer();
				if (nfds <= 0)
				{
					continue;
				}

				for (int i = 0; i < nfds; i++)
				{
					TNetScoket sockfd = events[i].data.fd;
					PCSocket baseSocket = FindBaseSocket(sockfd);
					if (!baseSocket)
					{
						continue;
					}

					if (events[i].events & (EPOLLPRI | EPOLLERR | EPOLLHUP))
					{
						easygo::base::CBase<easygo::thread::CTask> task = new easygo::network::CNetCloseTask(baseSocket);
						m_threadPool->PushTask(task);
					}

					if (events[i].events & EPOLLIN)
					{
						easygo::base::CBase<easygo::thread::CTask> task = new easygo::network::CNetReadTask(baseSocket);
						m_threadPool->PushTask(task);
					}

					if (events[i].events & EPOLLOUT)
					{
						easygo::base::CBase<easygo::thread::CTask> task = new easygo::network::CNetWriteTask(baseSocket);
						m_threadPool->PushTask(task);
					}
				}
			}

#endif
			m_mainThreadCond.notify_all();
		}

		void CNetBase::Start(int threadCount)
		{
			if (!m_bInit)
			{
				return;
			}

			m_threadPool = new easygo::thread::CThreadPool(threadCount + 1);
			m_threadPool->Start();
			m_bRunLoop = true;
			easygo::thread::CThread::Start();
		}

		void CNetBase::Stop()
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return;
			}

			m_bRunLoop = false;
			easygo::thread::CThread::Stop();
		}

		void CNetBase::Wait()
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return;
			}

			std::unique_lock <std::mutex> lck(m_mainThreadMutex);
			while (m_bRunLoop)
			{
				m_mainThreadCond.wait(lck);
			}
		}

		bool CNetBase::AddTimer(PCTimer timer)
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return false;
			}

			if (!timer || timer->GetTimeOut() < 0)
			{
				return false;
			}

			std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
			m_inactiveTimers.emplace(timer);

			return true;
		}

		bool CNetBase::RemoveTimer(PCTimer timer)
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return false;
			}

			std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
			timer->SetRemove();

			return true;
		}

		void CNetBase::PrepareTimer()
		{
			m_netClock->Start();
		}

		void CNetBase::ProcessTimer()
		{
			m_netTime += m_netClock->End();
			do {

				std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
				while (m_inactiveTimers.size() > 0)
				{
					PCTimer timer = m_inactiveTimers.top();
					if (timer->CheckExpired(m_netTime))
					{
						m_inactiveTimers.pop();
						m_activeTimers.emplace(timer);
						continue;
					}
					
					break;
				}

				while (m_activeTimers.size() > 0)
				{
					PCTimer timer = m_activeTimers.top();
					if (!timer->CheckCancel() && !timer->CheckRemove())
					{
						if (!timer->GetTaskThread())
						{
							m_threadPool->PushTask(dynamic_cast<easygo::thread::CTask*>(timer.GetSelf()));
						}
						else
						{
							timer->GetTaskThread()->PushTask(dynamic_cast<easygo::thread::CTask*>(timer.GetSelf()));
						}
					}
					
					m_activeTimers.pop(); 
					// pop timer before update escaped time of timer
					timer->SetEscaped(m_netTime);
					if (!timer->CheckRemove() && timer->CheckLoop())
					{
						m_inactiveTimers.emplace(timer);
					}
				}

			} while (0);
		}

		void CNetBase::AddEvent(PCSocket socket, int event)
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return;
			}

			socket->SetSockEvent(socket->GetSocketEvent() | event);

			std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
			PCSocket baseSocket = FindBaseSocket(socket->GetNetSocket());
			if (!baseSocket)
			{
				m_socketMap.emplace(socket->GetNetSocket(), socket);
			}
#ifdef WIN32
			if ((event & NET_SOCK_READ) != 0)
			{
				FD_SET(socket->GetNetSocket(), &m_readSet);
			}

			if ((event & NET_SOCK_WRITE) != 0)
			{
				FD_SET(socket->GetNetSocket(), &m_writeSet);
			}

			if ((event & NET_SOCK_EXCEP) != 0)
			{
				FD_SET(socket->GetNetSocket(), &m_excepSet);
			}

#elif defined(__APPLE__) || defined(__FreeBSD__)
			struct kevent ke;

			if ((event & NET_SOCK_READ) != 0)
			{
				EV_SET(&ke, socket->GetNetSocket(), EVFILT_READ, EV_ADD, 0, 0, NULL);
				kevent(m_kqfd, &ke, 1, NULL, 0, NULL);
			}

			if ((event & NET_SOCK_WRITE) != 0)
			{
				EV_SET(&ke, socket->GetNetSocket(), EVFILT_WRITE, EV_ADD, 0, 0, NULL);
				kevent(m_kqfd, &ke, 1, NULL, 0, NULL);
			}
#else
			m_addSet.emplace(socket->GetNetSocket(), EPOLLIN | EPOLLOUT | EPOLLET | EPOLLPRI | EPOLLERR | EPOLLHUP);
#endif
		}

		void CNetBase::RemoveEvent(PCSocket socket, int event)
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return;
			}

			int oldEvent;
			int sockEvent;

			oldEvent = sockEvent = socket->GetSocketEvent();

#ifdef WIN32
			do {
				std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
				if ((event & NET_SOCK_READ) != 0)
				{
					sockEvent -= NET_SOCK_READ;
					FD_CLR(socket->GetNetSocket(), &m_readSet);
				}

				if ((event & NET_SOCK_WRITE) != 0)
				{
					sockEvent -= NET_SOCK_WRITE;
					FD_CLR(socket->GetNetSocket(), &m_writeSet);
				}

				if ((event & NET_SOCK_EXCEP) != 0)
				{
					sockEvent -= NET_SOCK_EXCEP;
					FD_CLR(socket->GetNetSocket(), &m_excepSet);
				}

				if ((event & NET_SOCK_READ) == 0 && (event & NET_SOCK_WRITE) == 0)
				{
					m_closingSocks.insert(socket->GetNetSocket());
					m_socketMap.erase(socket->GetNetSocket());
				}
			} while (0);

#elif defined(__APPLE__) || defined(__FreeBSD__)
			do {
				std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
				struct kevent ke;

				if ((event & NET_SOCK_READ) != 0)
				{
					sockEvent &= ~NET_SOCK_READ;
					EV_SET(&ke, socket->GetNetSocket(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
					kevent(m_kqfd, &ke, 1, NULL, 0, NULL);
				}

				if ((event & NET_SOCK_WRITE) != 0)
				{
					sockEvent &= ~NET_SOCK_WRITE;
					EV_SET(&ke, socket->GetNetSocket(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
					kevent(m_kqfd, &ke, 1, NULL, 0, NULL);
				}

				sockEvent &= ~NET_SOCK_EXCEP;
				if ((sockEvent & NET_SOCK_WRITE) == 0 && (sockEvent & NET_SOCK_READ) == 0)
				{
					m_closingSocks.insert(socket->GetNetSocket());
					m_socketMap.erase(socket->GetNetSocket());
				}
				
			} while (0);
#else
			do {
				std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);
				uint32_t epollEvent = EPOLLIN | EPOLLOUT | EPOLLET | EPOLLPRI | EPOLLERR | EPOLLHUP;
				if ((event & NET_SOCK_READ) != 0)
				{
					sockEvent &= ~NET_SOCK_READ;
					epollEvent &= ~EPOLLIN;
				}

				if ((event & NET_SOCK_WRITE) != 0)
				{
					sockEvent &= ~NET_SOCK_WRITE;
					epollEvent &= ~EPOLLOUT;
				}

				if ((event & NET_SOCK_EXCEP) != 0)
				{
					sockEvent &= ~NET_SOCK_EXCEP;
					epollEvent &= ~EPOLLERR;
					epollEvent &= ~EPOLLHUP;
				}

				if ((epollEvent & EPOLLIN) == 0 && (epollEvent & EPOLLOUT) == 0)
				{
					m_closingSocks.insert(socket->GetNetSocket());
					m_socketMap.erase(socket->GetNetSocket());
					m_delSet.emplace(socket->GetNetSocket(), epollEvent);
				}
				else
				{
					m_modSet.emplace(socket->GetNetSocket(), epollEvent);
				}
			} while (0);

#endif

			socket->SetSockEvent(sockEvent);
		}

		PCSocket CNetBase::FindBaseSocket(TNetScoket sockfd)
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return PCSocket();
			}

			std::lock_guard<std::recursive_mutex> lock(m_netBaseMutex);

			std::map<TNetScoket, PCSocket>::iterator it = m_socketMap.find(sockfd);
			if (it != m_socketMap.end())
			{
				return it->second;
			}

			return PCSocket();
		}

		bool CNetBase::PushTask(easygo::base::CBase<easygo::thread::CTask> task)
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return false;
			}

			return m_threadPool->PushTask(task);
		}

		bool CNetBase::NotifyAcceptor(easygo::base::CBase<easygo::thread::CTask> task)
		{
			if (!m_bInit || !m_bRunLoop)
			{
				return false;
			}

			return m_threadPool->NotifyAcceptor(task);
		}

		void CNetBase::ProcSignal()
		{
#ifndef WIN32
			struct sigaction act;
			act.sa_handler = SIG_IGN;
			sigaction(SIGPIPE, &act, NULL);
#endif
		}

		bool CNetBase::Daemonize()
		{
#ifndef  WIN32
			int i, fd0, fd1, fd2;
			pid_t pid;
			struct rlimit rl;
			struct sigaction sa;

			umask(0);
			if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
			{
				return false;
			}

			if ((pid = fork()) < 0)
			{
				return false;
			}
			else if (pid != 0)
			{
				exit(0);
			}

			setsid();

			sa.sa_handler = SIG_IGN;
			sigemptyset(&sa.sa_mask);
			sa.sa_flags = 0;
			if (sigaction(SIGHUP, &sa, NULL) < 0)
			{
				return false;
			}

			if ((pid = fork()) < 0)
			{
				return false;
			}
			else if (pid != 0)
			{
				exit(0);
			}

			if (chdir("/") < 0)
			{
				return false;
			}

			if (rl.rlim_max == RLIM_INFINITY)
			{
				rl.rlim_max = 1024;
			}

			for (int i = 0; i < rl.rlim_max; i++)
			{
				close(i);
			}

			fd0 = open("/dev/null", O_RDWR);
			fd1 = dup(0);
			fd2 = dup(0);

			if (fd0 != 0 || fd1 != 1 || fd2 != 2)
			{
				return false;
			}

			
#endif
			return true;
		}

	} // namespace network
} // namespace easygo