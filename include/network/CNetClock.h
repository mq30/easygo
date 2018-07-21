#ifndef EASYGO_NETWORK_CNETCLOCK_H
#define EASYGO_NETWORK_CNETCLOCK_H
#include <chrono>
#include <ratio>
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/declexport.h"
#include "../include/base/CBase.h"

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

		class EASYGO_NETWORK_DECL CNetClock : public easygo::base::CCounter
		{
		public:
			CNetClock();
			virtual ~CNetClock();

		public:
			void Start();
			long long End();

		private:
			bool m_bStart;
			std::chrono::steady_clock::time_point m_startPoint;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CNETCLOCK_H