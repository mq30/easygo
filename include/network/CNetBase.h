#ifndef EASYGO_NETWORK_CNETBASE_H
#define EASYGO_NETWORK_CNETBASE_H
#include <map>
#include <vector>
#include <set>
#include <atomic>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/thread/CThread.h"
#include "../include/thread/CTaskThread.h"
#include "../include/thread/pool/CThreadPool.h"
#include "../include/network/CSocket.h"
#include "../include/declexport.h"
#include "../include/base/CBase.h"
#include "../include/network/CNetClock.h"
#include "../include/network/CTimer.h"

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
		class CSocket;
		typedef easygo::base::CBase<CSocket> PCSocket;
		class CListener;
		typedef easygo::base::CBase<CListener>  PCListener;
		class CNetClock;
		typedef easygo::base::CBase<CNetClock> PCNetClock;
		class CTimer;
		typedef easygo::base::CBase<CTimer> PCTimer;
	}
}

namespace easygo {

	namespace thread {
		class CThreadPool;
	}

	namespace network {	

		typedef enum NET_SOCK_EVENT 
		{
			NET_SOCK_READ	= 0x1,
			NET_SOCK_WRITE	= 0x2,
			NET_SOCK_EXCEP	= 0x4,
			NET_SOCK_ALL	= 0x7
		} TNetSockEvent;

		class EASYGO_NETWORK_DECL CNetBase : public easygo::thread::CThread
		{
		public:
			CNetBase();
			virtual ~CNetBase();

		public:
			bool Init();
			bool UnInit();
			bool Listen(PCListener listener);
			bool Connect(PCConnector connector);

		public:
			virtual void Run();
			void Start(int threadCount);
			void Stop();
			void Wait();
			bool AddTimer(PCTimer timer);
			bool RemoveTimer(PCTimer timer);
			void PrepareTimer();
			void ProcessTimer();
			void AddEvent(PCSocket socket, int event);
			void RemoveEvent(PCSocket socket, int event);
			PCSocket FindBaseSocket(TNetScoket sockfd);
			bool PushTask(easygo::base::CBase<easygo::thread::CTask> task);
			bool NotifyAcceptor(easygo::base::CBase<easygo::thread::CTask> task);

		public:
			static void ProcSignal();
			static bool Daemonize();

		private:
			int m_nThreadCount;
#ifdef WIN32
			fd_set m_readSet;
			fd_set m_writeSet;
			fd_set m_excepSet;
#elif defined(__APPLE__) || defined(__FreeBSD__)
			int m_kqfd;
#else
			int m_epfd;
			std::map<TNetScoket, uint32_t> m_addSet;
			std::map<TNetScoket, uint32_t> m_modSet;
			std::map<TNetScoket, uint32_t> m_delSet;
#endif
			std::priority_queue<PCTimer, std::deque<PCTimer>, CTimerComparer> m_inactiveTimers;
			std::priority_queue<PCTimer, std::deque<PCTimer>, CTimerComparer> m_activeTimers;
			long long m_netTime;
			PCNetClock m_netClock;
			std::atomic<bool> m_bRunLoop;
			std::atomic<bool> m_bInit;
			std::map<TNetScoket, PCSocket> m_socketMap;
			std::set<TNetScoket> m_closingSocks;
			std::recursive_mutex m_netBaseMutex;
			std::mutex m_mainThreadMutex;
			std::condition_variable m_mainThreadCond;
			easygo::base::CBase<easygo::thread::CThreadPool> m_threadPool;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CNETBASE_H
