#include <iostream>
#include <map>
#include <queue>
#include "CAppConnector.h"
#include "CAppConnection.h"
#include "CAppListener.h"
#include "../include/network/CNetBase.h"
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

void ServerMain()
{
	CNetBase::ProcSignal();

	PCNetBase netBase = new CNetBase();
	netBase->Init();
	netBase->Start(49);

	PCListener listener = new CAppListener(netBase, "0.0.0.0", 9999);
	netBase->Listen(listener);

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
	netBase->Start(2);

	PCConnector connector = new CAppConnector(netBase, "192.168.1.102", 9999);
	netBase->Connect(connector);

	netBase->Wait();
	netBase->UnInit();
}

int main()
{
	ClientMain();
	return 0;
}