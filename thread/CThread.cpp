#include "../include/thread/CThread.h"
#include "../include/thread/CTaskThread.h"
#include <functional>
#include <iostream>

namespace easygo {
	namespace thread {

		CThread::CThread() : m_bRunning(false), m_pThread(NULL)
		{
		}

		CThread::~CThread()
		{
			if (NULL != m_pThread)
			{
				delete m_pThread;
			}
		}

		void CThread::Start()
		{
			m_pThread = new std::thread(StartRoutine, (void*)this);
		}

		void CThread::Stop()
		{
			m_bRunning = false;
			Join();
		}

		bool CThread::IsRunning() const
		{
			return m_bRunning;
		}

		void CThread::SetRunning(bool isRunning)
		{
			m_bRunning = isRunning;
		}

		void CThread::Join()
		{
			m_pThread->join();
		}

		void StartRoutine(void* arg)
		{
			CThread* pThread = (CThread*)arg;
			pThread->SetRunning(true);
			pThread->Run();
			return;
		}

	} // namespace easygo
} // namespace thread
