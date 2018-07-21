#ifndef EASYGO_THREAD_CTHREADPOOL
#define EASYGO_THREAD_CTHREADPOOL
#include <vector>
#include <atomic>
#include "../include/thread/CTaskThread.h"
#include "../include/base/CBase.h"
#include "../include/base/CCounter.h"
#include "../include/declexport.h"

#ifndef EASYGO_THREAD_DECL
#	ifdef EASYGO_DYNAMIC_LINK
#		ifdef EXPORT_THREAD
#			define EASYGO_THREAD_DECL EASYGO_DECL_EXPORT
#		else
#			define EASYGO_THREAD_DECL EASYGO_DECL_IMPORT
#		endif
#	else
#		define EASYGO_THREAD_DECL
#	endif
#endif

namespace easygo {

	namespace thread {

		//class CTask;
		//class CTaskThread;
		//class CThread;

		class EASYGO_THREAD_DECL CThreadPool : public easygo::base::CCounter
		{
		public:
			CThreadPool();
			CThreadPool(int threadCount);
			virtual ~CThreadPool();

		public:
			void Start();
			void Stop();
			int GetThreadCount() const;
			bool PushTask(easygo::base::CBase<CTask> task);
			bool NotifyAcceptor(easygo::base::CBase<CTask> task);
			void PrintTaskStatics();

		private:
			int m_threadCount;
			std::vector<easygo::base::CBase<CTaskThread>> taskThreadList;
		};

	} // namespace thread
} // namespace easygo

#endif // EASYGO_THREAD_CTHREADPOOL
