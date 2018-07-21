#ifdef WIN32
#include <Windows.h>
#else
#include<unistd.h>
#endif
#include "../include/base/CSystem.h"

namespace easygo {
	namespace base {

		CSystem::CSystem()
		{
		}

		CSystem::~CSystem()
		{
		}

		long CSystem::GetProcessorCount()
		{
#ifdef WIN32
			SYSTEM_INFO si;
			GetSystemInfo(&si);
			return si.dwNumberOfProcessors;
#else
			return sysconf(_SC_NPROCESSORS_ONLN);
#endif
		}

	} // namespace base
} // namespace easygo
