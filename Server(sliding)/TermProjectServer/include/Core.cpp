#include "Core.h"
#include "Util/MapData.h"
#include "Util\ErrorUtil.h"
#include "DB\DB.h"

void CCore::InitNetwork()
{
	wcout.imbue(locale("Korean"));

	/*Window Startup*/
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);

	/*Create IOCP HANDLE*/
	HIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

	/*Create Socket*/
	ListenSock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	/*Bind N Listen*/
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
	/*Bind & Listen*/
	InitNetwork();

	/*Processor Initialize*/
	if (!UP.Initialize()) {
		std::cout << "Processor Initialize Failed..." << std::endl;
	}
	/*NPCManager Initialize*/
	if (!NP.Initialize()){
		std::cout << "NPC Manager Initialize Failed..." << std::endl;
	}
	  
	if (!CMapData::GET_SINGLE()->Initialize()) {
		std::cout << "MapData Read failed!\n" << std::endl;
	}
}

void CCore::Run()
{
	std::vector<thread> WorkerThread;
	std::thread AcceptThread;
	std::thread EventThread;

	std::cout << "WORKER THREAD  VECTOR SIZE:: ";
	std::cin >> WorkerSize;

	AcceptThread = CreateAcceptThread();
	for (int i = 0; i < WorkerSize; ++i)
		WorkerThread.push_back(CreateWorkerThread());
	EventThread = CreateEventThread();


	AcceptThread.join();
	for (auto& th : WorkerThread) th.join();
	EventThread.join();

}

void CCore::Close()
{   
	/*CUserProcessor::GET_SINGLE()->Close();
	CUserProcessor::DESTROY_SINGLE();

	CNPCProcessor::GET_SINGLE()->Close();
	CNPCProcessor::DESTROY_SINGLE();*/

	NP.Close();
	UP.Close();

	closesocket(ListenSock);
	CloseHandle(HIOCP);
	WSACleanup();
}

void CCore::CoreAcceptThread()
{
	/*CUserProcessor* P     = CUserProcessor::GET_SINGLE();
	CNPCUserProcessor* NP = CNPCUserProcessor::GET_SINGLE();*/

	printf("Start Accept Thread\n");

	while (true)
	{
		SOCKADDR_IN CAddr;
		ZeroMemory(&CAddr, sizeof(SOCKADDR_IN));
	    int AddrSize = sizeof(SOCKADDR_IN);
		auto CSock = WSAAccept(ListenSock, reinterpret_cast<sockaddr*>(&CAddr), &AddrSize, NULL, NULL);

		if (INVALID_SOCKET == CSock) { ErrorUtil::Err_Quit("Accept Thread!\n"); continue;}
		WORD NewID = UP.GetUsableID();

		if (NewID == -1) 
		{
			printf("Client Pool Overflow!!!\n");
			closesocket(CSock);
			continue;
		}

		printf("New Client Connected ID[%s]\n", &NewID);

		UP.UserSetting(NewID, CSock);
		CreateIoCompletionPort(reinterpret_cast<HANDLE>(CSock), HIOCP, NewID, 0);
		/*Test¿ë*/
		UP.Test(NewID);

	    UP.CallRecv(NewID);

	}

}

void CCore::CoreWorkerThread()
{
	/*CUserProcessor* P = CUserProcessor::GET_SINGLE();
	CNPCProcessor* NP = CNPCProcessor::GET_SINGLE();*/
	unsigned long                  IO_Size;
	unsigned long long             IO_Key;
	WORD                           Key;
	bool                           Ret;
	WSAOVERLAPPED*                 Over;

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

		/*Send / Recv Ã³¸®*/
		EXOVER* Exover = reinterpret_cast<EXOVER*>(Over);
		switch (Exover->IO_Type)
		{
		case IO_TYPE::IO_RECV:{
			UP.Process(Key, static_cast<int>(IO_Size), Exover->IO_Buf);
			break;
		}
		case IO_TYPE::IO_SEND:{
			delete Exover;
			break;
		}
		case IO_TYPE::IO_EVENT:{
			NP.Process(Exover->EV_Type, Key, Exover->Event_Target);
			delete Exover;
			break;
		}
		}
	}
	return;
}

void CCore::CoreEventThread()
{   
	printf("Start Event Thread\n");
	while (true)
	{   
		Sleep(10);
		while (!NP.IsQueueEmpty())
		{
			if (NP.TopOfQueue().W_Time >= GetTickCount()) break;
			NPC_EVENT E = NP.TopOfQueue();
			NP.Pop();
			EXOVER* Ex = new EXOVER;
			Ex->Event_Target = E.Target;
			Ex->EV_Type = E.Type;
			Ex->IO_Type = IO_EVENT;
			PostQueuedCompletionStatus(HIOCP, 1, E.NPC_ID, &Ex->WSAOVER);
		}
	}
	return;
}



