#ifndef EASYGO_THREAD_CTHREAD
#define EASYGO_THREAD_CTHREAD

#include <thread>
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

		class EASYGO_THREAD_DECL CThread : public easygo::base::CCounter
		{
		public:
			CThread();
			virtual ~CThread();
			virtual void Start();
			virtual void Stop();
			bool IsRunning() const;
			void SetRunning(bool isRunning);
			
		public:
			virtual void Run() = 0;

		private:
			void Join();

		protected:
			std::thread *m_pThread;
			std::atomic<bool> m_bRunning;
		};

		void StartRoutine(void* arg);
	} // namespace thread
} // namespace easygo

#endif // EASYGO_THREAD_CTHREAD
