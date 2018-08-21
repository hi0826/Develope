#pragma once
#include "Include.h"
#include "Processor/UserProcessor.h"
#include "Processor/NPCProcessor.h"
class CCore 
{
private:
	HANDLE         HIOCP;
	SOCKET         ListenSock;
	SOCKADDR_IN    BindAddr;
	UINT           WorkerSize;
	CNPCProcessor  NP;
	CUserProcessor UP;

public:
	CCore() {};
	virtual ~CCore() {};

public:
	void       Initialize();
	void       InitNetwork();
	void       Run();
	void       Close();

public:
	 void      CoreWorkerThread();
	 void      CoreEventThread();
	 void      CoreAcceptThread();
	 void      CoreSimulationThread();
public:
	std::thread   CreateAcceptThread()  { return std::thread([this] { this->CoreAcceptThread();  }); }
	std::thread   CreateWorkerThread()  { return std::thread([this] { this->CoreWorkerThread(); }); }
	std::thread   CreateEventThread()   { return std::thread([this] { this->CoreEventThread();  }); }
	std::thread   CreateSimulationThread() { return std::thread([this] { this->CoreSimulationThread();  }); }
};


