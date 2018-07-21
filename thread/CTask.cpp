#include "../include/thread/CTask.h"
#include "../include/base/CUuid.h"

namespace easygo {
	namespace thread {

		CTask::CTask()
		{
		}

		CTask::~CTask()
		{
		}

		unsigned long long CTask::GetTaskId()
		{
			return m_taskId;
		}

		void CTask::SetTaskId(unsigned long long taskId)
		{
			m_taskId = taskId;
		}

	} // namespace thread
} // namespace easygo