#ifndef EASYGO_NETWORK_CAPPTIMER_H
#define EASYGO_NETWORK_CAPPTIMER_H
#include <string>
#include "../include/network/NetType.h"
#include "../include/base/CCounter.h"
#include "../include/base/CBase.h"
#include "../include/network/CTimer.h"

namespace easygo {
	namespace network {

		class CAppTimer : public easygo::network::CTimer
		{
		public:
			CAppTimer(std::string name, long long timeout, bool bLoop = true, void* pData = NULL);
			virtual ~CAppTimer();

		public:
			virtual void OnTimer();

		private:
			std::string m_timerName;
		};

	} // namespace network
} // namespace easygo

#endif // EASYGO_NETWORK_CAPPTIMER_H
