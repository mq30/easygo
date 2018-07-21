#include <iostream>
#include <map>
#include <queue>
#include <thread>
#include <chrono>
#include "CAppConnector.h"
#include "CAppConnection.h"
#include "CAppListener.h"
#include "CAppTimer.h"
#include "../include/network/CNetBase.h"
#include "../include/network/CTimer.h"
#include "../include/thread/CTaskThread.h"
using namespace easygo::network;

void TestCCounter_1()
{
	PCNetBase netBase = new CNetBase();
}

void TestCCounter_2()
{
	PCNetBase netBase = new CNetBase();
	std::queue<std::pair<unsigned int, PCNetBase>> bases;
	bases.emplace(1, netBase);
	bases.pop();
}

void TestMain()
{
	TestCCounter_1();
	TestCCounter_2();
}

void TestTimer(PCNetBase netBase)
{
#define TIMER_THREAD_COUNT 10
	//std::cout << sizeof(long long) << std::endl;

	char timerName[64];
	PCTimer timers[TIMER_THREAD_COUNT];

	for (int i = 1; i <= TIMER_THREAD_COUNT; i++)
	{
		sprintf(timerName, "timer%d", i);
		timers[i-1] = new CAppTimer(timerName, 1);
		//if (i % 2 == 0)
		{
			PCTaskThread taskThread = new easygo::thread::CTaskThread;
			taskThread->Start();
			timers[i - 1]->SetTaskThread(taskThread);
		}
		netBase->AddTimer(timers[i - 1]);
		
	}

	std::this_thread::sleep_for(std::chrono::seconds(10 * TIMER_THREAD_COUNT + 1));
	for (int i = 0; i < TIMER_THREAD_COUNT; i++)
	{
		netBase->RemoveTimer(timers[i]);
		timers[i]->GetTaskThread()->Stop();
		//std::this_thread::sleep_for(std::chrono::seconds(1));
	}

}

void ServerMain()
{
	//if (!CNetBase::Daemonize())
	//{
	//}

	CNetBase::ProcSignal();

	PCNetBase netBase = new CNetBase();
	netBase->Init();
	netBase->Start(4);

	PCListener listener = new CAppListener(netBase, "0.0.0.0", 9999);
	netBase->Listen(listener);

	TestTimer(netBase);

	netBase->Wait();
	netBase->UnInit();
}

void ClientMain()
{
	if (!CNetBase::Daemonize())
	{
	}

	CNetBase::ProcSignal();

	PCNetBase netBase = new CNetBase();
	netBase->Init();
	netBase->Start(49);

	PCConnector connector = new CAppConnector(netBase, "192.168.1.102", 9999);
	netBase->Connect(connector);

	netBase->Wait();
	netBase->UnInit();
}

int main()
{
	ServerMain();
	return 0;
}