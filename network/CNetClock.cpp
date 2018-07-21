#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <iostream>
#include "../include/network/CNetClock.h"

namespace easygo {
	namespace network {

		CNetClock::CNetClock() : m_bStart(false), m_startPoint()
		{
		}

		CNetClock::~CNetClock()
		{
		}

		void CNetClock::Start()
		{
			m_bStart = true;
			m_startPoint = std::chrono::steady_clock::now();
		}

		long long CNetClock::End()
		{
			if (!m_bStart)
			{
				return 0L;
			}

			return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_startPoint).count();
		}

	} // namespace network
} // namespace easygo