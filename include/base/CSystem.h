#ifndef EASYGO_BASE_CSYSTEM_H
#define EASYGO_BASE_CSYSTEM_H
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

		class EASYGO_BASE_DECL CSystem
		{
		public:
			CSystem();
			virtual ~CSystem();
		public:
			static long GetProcessorCount();

		};

	} // namespace base
} // namespace easygo

#endif // EASYGO_BASE_CSYSTEM_H