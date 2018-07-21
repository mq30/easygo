#ifndef EASYGO_THREAD_CTASKTHREAD_H
#define EASYGO_THREAD_CTASKTHREAD_H
#include <queue>
#include <atomic>
#include <string>
#include <mutex>
#include <condition_variable>
#include "CThread.h"
#include "CTask.h"
#include "../include/base/CBase.h"
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

		class EASYGO_THREAD_DECL CTaskThread : public CThread
		{
		public:
			CTaskThread();
			virtual ~CTaskThread();

		public:
			virtual void Start();
			virtual void Stop();
			virtual void Run();
			bool PushTask(const easygo::base::CBase<CTask> task);
			void PrintTaskStatics();

		public:
			static const unsigned int MAX_TASK_COUNT;

		private:
			std::queue<std::pair<unsigned int, easygo::base::CBase<CTask>>> m_taskQueue;
			std::atomic<unsigned int> m_nNext;
			std::atomic<bool> m_bRunLoop;
			std::mutex m_mutex; 
			std::string m_threadId;
			std::condition_variable m_cond;
		};

	} // namespace thread
} // namespace easygo

#endif // EASYGO_THREAD_CTASKTHREAD_H
