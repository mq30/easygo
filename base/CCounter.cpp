#include <cassert>
#include <cstddef>
#include <iostream>
#include "../include/base/CCounter.h"

namespace easygo {
	namespace base {

		CCounter::CCounter() : m_plCounterPtx(0)
		{
		}

		CCounter::~CCounter()
		{
			m_plCounterPtx = 0;
		}

		void CCounter::IncRef()
		{
			m_plCounterPtx.fetch_add(1);
		}

		bool CCounter::DecRef()
		{
			return (m_plCounterPtx.fetch_sub(1) - 1) == 0;
		}

	} // namespace base
} // namespace easygo
