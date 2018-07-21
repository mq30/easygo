#ifndef EASYGO_THREAD_CTASK_H
#define EASYGO_THREAD_CTASK_H

#include <iostream>
#include <functional>
#include <string>
#include "base/CCounter.h"
#include "base/CBase.h"
#include "../include/declexport.h"

#ifndef EASYGO_THREAD_DECL
	#ifdef EASYGO_DYNAMIC_LINK
		#ifdef EXPORT_THREAD
			#define EASYGO_THREAD_DECL EASYGO_DECL_EXPORT
		#else
			#define EASYGO_THREAD_DECL EASYGO_DECL_IMPORT
		#endif
	#else
		#define EASYGO_THREAD_DECL
	#endif
#endif

namespace easygo {
	namespace thread {

		class EASYGO_THREAD_DECL CTask : public easygo::base::CCounter
		{
		public:
			CTask();
			virtual ~CTask();
			virtual void Execute() = 0;
			unsigned long long GetTaskId();
			void SetTaskId(unsigned long long taskId);

		private:
			unsigned long long m_taskId;
		};

		

	} // namespace thread
} // namespace easygo

#endif // EASYGO_THREAD_CTASK_H
