#ifndef EASYGO_NETWORK_CNETBUFFER_H
#define EASYGO_NETWORK_CNETBUFFER_H
#include <vector>
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

		class EASYGO_NETWORK_DECL CNetBuffer : public easygo::base::CCounter
		{
		public:
			CNetBuffer();
			virtual ~CNetBuffer();

		public:
			bool IncBufferSize(unsigned int incSize);
			unsigned int Write(const char *buf, int len);
			unsigned int Read(char *buf, int len);

		public:
			char* GetBuffer() const;
			unsigned int GetBufferSize() const;
			unsigned int GetWritableSize();
			unsigned int GetReadableSize() const;

		private:
			char* m_buffer;
			unsigned int m_bufferSize;
			unsigned int m_rwOffset;
			unsigned long long m_bufferId;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CNETBUFFER_H