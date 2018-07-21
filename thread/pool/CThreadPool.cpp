#include <iostream>
#include "../include/base/CSystem.h"
#include "../include/thread/pool/CThreadPool.h"

namespace easygo {
	namespace thread {

		CThreadPool::CThreadPool() : m_threadCount(), taskThreadList()
		{
			m_threadCount = easygo::base::CSystem::GetProcessorCount();
			taskThreadList.clear();
		}

		CThreadPool::CThreadPool(int threadCount) : m_threadCount(threadCount)
		{
			if (m_threadCount <= 0)
			{
				m_threadCount = easygo::base::CSystem::GetProcessorCount();
			}
			taskThreadList.clear();
		}

		CThreadPool::~CThreadPool()
		{
			taskThreadList.clear();
		}

		void CThreadPool::Start()
		{
			for (int i = 0; i < m_threadCount; i++)
			{
				easygo::base::CBase<easygo::thread::CTaskThread> taskThread = new easygo::thread::CTaskThread;
				taskThread->Start();
				taskThreadList.push_back(taskThread);
			}
		}

		void CThreadPool::Stop()
		{
			for (int i = 0; i < m_threadCount; i++)
			{
				taskThreadList[i]->Stop();
			}
		}

		int CThreadPool::GetThreadCount() const
		{
			return m_threadCount;
		}

		bool CThreadPool::PushTask(easygo::base::CBase<CTask> task)
		{
			unsigned int thread = task->GetTaskId() % (m_threadCount);

			if (thread >= (unsigned int)m_threadCount || thread < 0)
			{
				thread = 0;
			}

			return taskThreadList[thread]->PushTask(task);
		}

		bool CThreadPool::NotifyAcceptor(easygo::base::CBase<CTask> task)
		{
			return taskThreadList[m_threadCount - 1]->PushTask(task);
		}

		void CThreadPool::PrintTaskStatics()
		{
			for (int i = 0; i < m_threadCount; i++)
			{
				taskThreadList[i]->PrintTaskStatics();
			}
		}

	} // namespace thread
} // namespace easygo