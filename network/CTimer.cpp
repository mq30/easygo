#include "../include/network/CTimer.h"

namespace easygo {
	namespace network {

		CTimer::CTimer(long long timeout, bool loop, void * data) : 
		CTask(), m_timeout(timeout), m_loop(loop), m_data(data), 
		m_escaped(0), m_remove(false), m_cancel(false), m_taskThread()
		{
		}

		CTimer::~CTimer()
		{
		}

		void CTimer::Execute()
		{
			OnTimer();
		}

		long long CTimer::GetTimeOut() const
		{
			return m_timeout;
		}

		long long CTimer::GetEscaped()
		{
			return m_escaped;
		}

		void CTimer::SetEscaped(long long escaped)
		{
			m_escaped = escaped;
		}

		bool CTimer::CheckExpired(long long netTime)
		{
			return netTime >= m_timeout + m_escaped;
		}

		bool CTimer::CheckLoop() const
		{
			return m_loop;
		}

		void CTimer::SetLoop(bool loop)
		{
			m_loop = loop;
		}

		void CTimer::SetRemove(bool remove)
		{
			m_remove = remove;
		}

		bool CTimer::CheckRemove() const
		{
			return m_remove;
		}

		void CTimer::SetCancel(bool cancel)
		{
			m_cancel = cancel;
		}

		bool CTimer::CheckCancel() const
		{
			return m_cancel;
		}

		void CTimer::SetTaskThread(PCTaskThread taskThread)
		{
			m_taskThread = taskThread;
		}

		PCTaskThread CTimer::GetTaskThread()
		{
			return m_taskThread;
		}

	} // namespace network
} // namespace easygo
