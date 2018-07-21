#ifndef EASYGO_BASE_CUUID_H
#define EASYGO_BASE_CUUID_H

#include <string>
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

#pragma pack(push)
#pragma pack(4)
		typedef union {
			unsigned char data[32];
			struct {
				unsigned char group1[8];
				unsigned char group2[4];
				unsigned char group3[4];
				unsigned char group4[4];
				unsigned char group5[12];
			} groups;
		} UUID;
#pragma pack(pop)

		class EASYGO_BASE_DECL CUuid
		{
		public:
			CUuid();
			CUuid(const CUuid& uuid);
			explicit CUuid(const std::string& uuid);
			CUuid& operator=(const CUuid& uuid);
			virtual ~CUuid();
			
		public:
			bool IsNil() const;
			std::string String() const;
			unsigned char GetVersion() const;

		public:
			static std::string GenerateUUID();

		private:
			void GenerateRandomUUID();

		private:
			UUID m_uuid;
		};

	} // namespace base
} // namespace easygo

#endif // EASYGO_BASE_CUUID_H
