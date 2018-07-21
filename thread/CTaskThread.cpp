#include "../include/thread/CTaskThread.h"
#include "../include/base/CUuid.h"

#include <iostream>

namespace easygo {
	namespace thread {

		const unsigned int CTaskThread::MAX_TASK_COUNT = 2048;

		CTaskThread::CTaskThread() : m_nNext(0), m_taskQueue(), m_bRunLoop(false), m_mutex(), m_cond()
		{
			m_threadId = easygo::base::CUuid::GenerateUUID();
		}

		CTaskThread::~CTaskThread()
		{
		}

		void CTaskThread::Start()
		{
			m_bRunLoop = true;
			CThread::Start();
		}

		void CTaskThread::Stop()
		{
			m_bRunLoop = false;
			m_cond.notify_all();
			CThread::Stop();
		}

		void CTaskThread::Run()
		{
			while (m_bRunLoop)
			{
				std::unique_lock <std::mutex> lck(m_mutex);
				while (m_taskQueue.size() <= 0 && m_bRunLoop)
				{
					m_cond.wait(lck);
				}

				std::queue<std::pair<unsigned int, easygo::base::CBase<CTask>>> tmpTaskQueue;
				while (m_taskQueue.size() > 0)
				{
					std::pair<unsigned int, easygo::base::CBase<CTask>> taskPair = m_taskQueue.front();
					m_taskQueue.pop();
					tmpTaskQueue.emplace(taskPair);
				}

				lck.unlock();
				while (tmpTaskQueue.size() > 0)
				{
					std::pair<unsigned int, easygo::base::CBase<CTask>> taskPair = tmpTaskQueue.front();
					tmpTaskQueue.pop();
					taskPair.second->Execute();
				}

			}
			m_bRunning = false;
		}

		bool CTaskThread::PushTask(const easygo::base::CBase<CTask> task)
		{
			std::unique_lock <std::mutex> lck(m_mutex);
			
			if (m_taskQueue.size() > CTaskThread::MAX_TASK_COUNT)
			{
				return false;
			}

			m_taskQueue.push(std::make_pair(m_nNext.load(), task));
			m_nNext++;
			m_cond.notify_all();

			return true;
		}

		void CTaskThread::PrintTaskStatics()
		{
			//if (m_taskQueue.size())
			//{
			//	std::cout << ">>>" << m_threadId << "<<<task queue size>>>" << m_taskQueue.size() << std::endl;
			//}
		}

	} // namespace thread
} // namespace easygo