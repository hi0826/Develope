#include "Core.h"
#include "Util/MapData.h"
#include "Util/VectorData.h"
#include "Util\ErrorUtil.h"
#include "DB\DB.h"

void CCore::InitNetwork()
{
	wcout.imbue(locale("Korean"));
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	HIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);
	ListenSock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN BindAddr;
	ZeroMemory(&BindAddr, sizeof(SOCKADDR_IN));

	BindAddr.sin_family = AF_INET;
	BindAddr.sin_port = htons(MY_SERVER_PORT);
	BindAddr.sin_addr.s_addr = INADDR_ANY;

	::bind(ListenSock, reinterpret_cast<sockaddr*>(&BindAddr), sizeof(SOCKADDR));
	::listen(ListenSock, SOMAXCONN);
}

void CCore::Initialize()
{
	InitNetwork();

	if (!CVData::GET()->Initialize()) std::cout << "VData Read failed!\n" << std::endl;
	if (!CMapData::GET()->Initialize())std::cout << "MapData Read failed!\n" << std::endl;
	if (!UP.Initialize()) std::cout << "Processor Initialize Failed..." << std::endl;
	if (!NP.Initialize()) std::cout << "NPC Manager Initialize Failed..." << std::endl;
}

void CCore::Run()
{
	std::vector<thread> WorkerThread;
	std::thread AcceptThread;
	std::thread EventThread;
	std::thread SimulationThread;

	std::cout << "WORKER THREAD  POOL SIZE:: ";
	std::cin >> WorkerSize;

	AcceptThread = CreateAcceptThread();

	for (int i = 0; i < WorkerSize; ++i)
		WorkerThread.push_back(CreateWorkerThread());

	EventThread = CreateEventThread();
	SimulationThread = CreateSimulationThread();

	for (auto& th : WorkerThread) th.join();

	AcceptThread.join();
	EventThread.join();
	SimulationThread.join();
}

void CCore::Close()
{
	NP.Close();
	UP.Close();
	CMapData::GET()->Close();

	closesocket(ListenSock);
	CloseHandle(HIOCP);
	WSACleanup();
}

void CCore::CoreAcceptThread()
{
	printf("Start Accept Thread\n");

	while (true)
	{
		SOCKADDR_IN CAddr;
		ZeroMemory(&CAddr, sizeof(SOCKADDR_IN));
		int AddrSize = sizeof(SOCKADDR_IN);
		auto CSock = WSAAccept(ListenSock, reinterpret_cast<sockaddr*>(&CAddr), &AddrSize, NULL, NULL);

		if (INVALID_SOCKET == CSock) { ErrorUtil::Err_Quit("Accept Thread!\n"); continue; }
		WORD NewID = UP.GetUsableID();

		if (NewID == -1)
		{
			printf("Client Pool Overflow!!!\n");
			closesocket(CSock);
			continue;
		}
		printf("New Client Connected ID[%d]\n", &NewID);

		UP.UserSetting(NewID, CSock);
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(CSock), HIOCP, NewID, 0);
		UP.PlayerInit(NewID);
		UP.CallRecv(NewID);
	}

}

void CCore::CoreWorkerThread()
{
	unsigned long       IO_Size;
	unsigned long long  IO_Key;
	WORD                Key;
	bool                Ret;
	WSAOVERLAPPED*      Over;

	printf("Start Worker Thread\n");
	while (true)
	{
		Ret = GetQueuedCompletionStatus(HIOCP, &IO_Size, (&IO_Key), &Over, INFINITE);
		Key = static_cast<WORD>(IO_Key);

		if (false == Ret || IO_Size == 0)
		{
			ErrorUtil::Err_Display("ERROR IN GQCS");
			UP.PlayerDisconnect(Key);
			continue;
		}

		EXOVER* Exover = reinterpret_cast<EXOVER*>(Over);

		switch (Exover->IO_Type)
		{
		case IO_TYPE::IO_RECV:
		{
			UP.PacketProcess(Key, static_cast<int>(IO_Size), Exover->IO_Buf);
			break;
		}
		case IO_TYPE::IO_SEND:
		{
			delete Exover;
			break;
		}
		case IO_TYPE::IO_EVENT:
		{
			if (NP.IsNPC(Key)) NP.EventProcess(Exover->EV_Type, Key);
			else UP.EventProcess(Exover->EV_Type, Key);

			delete Exover;
			break;
		}
		}
	}
}

void CCore::CoreEventThread()
{
	printf("Start Event Thread\n");

	NPC_EVENT E;
	while (true)
	{
		Sleep(10);
		while (false == NP.IsQueueEmpty())
		{
			if (NP.TryPopOfQueue(E))
			{
				while (E.W_Time > chrono::high_resolution_clock::now());
				EXOVER* Ex = new EXOVER;
				Ex->EV_Type = E.Type;
				Ex->IO_Type = IO_TYPE::IO_EVENT;
				PostQueuedCompletionStatus(HIOCP, 1, E.NPC_ID, &Ex->WSAOVER);
			}
		}
	}
}

void CCore::CoreSimulationThread()
{
	printf("Start Simulation Thread\n");
	NP.SimulationNPC();
}

