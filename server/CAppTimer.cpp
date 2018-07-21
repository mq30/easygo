#include <iostream>
#include "CAppTimer.h"

namespace easygo {
	namespace network {

		CAppTimer::CAppTimer(std::string name, long long timeout, bool bLoop, void * pData) : m_timerName(name), CTimer(timeout * 1000000, bLoop, pData)
		{
			//std::cout << name << std::endl;
		}

		CAppTimer::~CAppTimer()
		{
		}

		void CAppTimer::OnTimer()
		{
			//if (m_timerName == "timer10")
			//{
			//	std::cout << m_timerName << " says hello." << std::endl;
			//}
			//std::this_thread::sleep_for(std::chrono::seconds(100));
		}

	} // namespace network
} // namespace easygo