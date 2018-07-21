#ifndef EASYGO_NETWORK_CTIMER_H
#define EASYGO_NETWORK_CTIMER_H
#include <chrono>
#include <ratio>
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/declexport.h"
#include "../include/base/CBase.h"
#include "../include/thread/CTask.h"
#include "../include/thread/CTaskThread.h"

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
		class CTimerComparer;
		class CTaskThread;
		typedef easygo::base::CBase<easygo::thread::CTaskThread> PCTaskThread;
	} // namespace easygo
} // namespace network

namespace easygo {
	namespace network {

		class EASYGO_NETWORK_DECL CTimer : public easygo::thread::CTask
		{
		public:
			CTimer(long long timeout, bool loop = true, void* data = NULL);
			virtual ~CTimer();

		public:
			virtual void Execute();

		public:
			long long GetTimeOut() const;
			long long GetEscaped();
			void SetEscaped(long long escaped);
			bool CheckExpired(long long netTime);
			bool CheckLoop() const;
			void SetLoop(bool loop = true);
			void SetRemove(bool remove = true);
			bool CheckRemove() const;
			void SetCancel(bool cancel = true);
			bool CheckCancel() const;
			void SetTaskThread(PCTaskThread taskThread);
			PCTaskThread GetTaskThread();

		public:
			virtual void OnTimer() = 0;

		private:
			bool m_loop;
			bool m_remove;
			bool m_cancel;
			void* m_data;
			long long m_escaped;
			long long m_timeout;
			PCTaskThread m_taskThread;
			friend class CTimerComparer;
		};

	} // namespace network
} // namespace easygo

namespace easygo {
	namespace network {

		class CTimer;
		typedef easygo::base::CBase<CTimer> PCTimer;

	} // namespace network
} // namespace easygo

namespace easygo {
	namespace network {

		class CTimerComparer
		{
		public:
			bool operator()(const PCTimer& leftTimer, const PCTimer& rightTimer) const
			{
				if (!leftTimer || !rightTimer)
				{
					return false;
				}

				return (leftTimer->m_timeout + leftTimer->m_escaped) > (rightTimer->m_timeout + rightTimer->m_escaped);
			}
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CTIMER_H