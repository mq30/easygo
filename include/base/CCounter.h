#ifndef EASYGO_BASE_CCOUNTER_H
#define EASYGO_BASE_CCOUNTER_H

#include <atomic>
#include "../include/declexport.h"

#ifndef EASYGO_BASE_DECL
#	ifdef EASYGO_DYNAMIC_LINK
#		ifdef EXPORT_BASE
#			define EASYGO_BASE_DECL EASYGO_DECL_EXPORT
#		else
#			define EASYGO_BASE_DECL EASYGO_DECL_IMPORT
#		endif
#	else
#		define EASYGO_BASE_DECL
#	endif
#endif

namespace easygo {
	namespace base {

		class EASYGO_BASE_DECL CCounter
		{
		public:
			CCounter();
			virtual ~CCounter();
			void IncRef();
			bool DecRef();

		protected:
			std::atomic<uint32_t> m_plCounterPtx;
		};

	} // namespace base
} // namespace easygo

#endif // EASYGO_BASE_CCOUNTER_H