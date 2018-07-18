#include "stdafx.h"
#include "Iocp.h"


DWORD WINAPI WorkerThreadCallback(LPVOID arg)
{
	CIocp* Owner = (CIocp*)arg;
	Owner->WorkerThreadCallback();
	
	return 0;
}

CIocp::CIocp(VOID)
{ // 생성자
	mIocpHandle          = NULL; //IOCP를 관리하는 핸들
	mWorkerThreadCount   = 0;    // 사용할 WorkerThread의 개수 

	mStartupEventHandle  = NULL; // 시작을 관리하는 이벤트 
}


CIocp::~CIocp(VOID)
{
}

BOOL CIocp::Begin(VOID)
{
	mIocpHandle          = NULL; //mIocpHandle 값의 초기화

	//서버 시스템의 정보를 가져옵니다. 
	// 가져온 시스템 정보에서 CPU 정보만을 사용할 것입니다. 
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	//CPU의 개수의 2배만큼 워커 스레드를 만든다. 
	mWorkerThreadCount = SystemInfo.dwNumberOfProcessors * 2;
	//Iocp 핸들을 생성 
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	//생성이 실패했다면 종료됩니다. 
	if (!mIocpHandle) return FALSE;

	//시작을 관리히는 이벤트도 생성합니다. 
	mStartupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
	if (mStartupEventHandle == NULL)
	{
		End();
		return FALSE;
	}
	 

	//CPU 개수의 2배만큼 WorekerThread를 생성해서 mWorkerThreadVector 
	for (DWORD i = 0; i < 4; ++i)
	{
		HANDLE WorkerThread = CreateThread(NULL, 0, ::WorkerThreadCallback, this, 0, NULL);
		mWorkerThreadVector.push_back(WorkerThread);
		//하나의 스레드마다 생성이 완료될때 까지 대기합니다.
		WaitForSingleObject(mStartupEventHandle, INFINITE);
	}

	return TRUE;
}

BOOL CIocp::End(VOID)
{
	//IOCP를 이용해서 각 WorkerThread마다 종료신호를 내보님 
	for (DWORD i = 0; i < mWorkerThreadVector.size(); ++i)
		PostQueuedCompletionStatus(mIocpHandle, 0, 0, NULL); //IOCP에 강제적으로 신호를 보내는 함수

	for (DWORD i = 0; i < mWorkerThreadVector.size(); ++i)
	{   
		//한 WorkerThread가 완전히 종료될 때 까지 대기시켜줍니다. 
		WaitForSingleObject(mWorkerThreadVector[i], INFINITE);
		CloseHandle(mWorkerThreadVector[i]);
	}

	//IOCP 핸들도 종료합니다. 
	if (mIocpHandle) CloseHandle(mIocpHandle);

	//WorkerThread를 관리하는 벡터를 지워줍니다. 
	mWorkerThreadVector.clear();
   
	if (mStartupEventHandle) CloseHandle(mStartupEventHandle);
	
	return TRUE;
}

BOOL CIocp::RegisterSocketToIocp(SOCKET socket, ULONG_PTR completionKey)
{// IOCP에  ClientSocket을 등록하는 역할을 하는 함수 
	if (!socket || !completionKey)
		return FALSE;

	mIocpHandle = CreateIoCompletionPort((HANDLE)socket, mIocpHandle, completionKey, 0);
	if (!mIocpHandle)return FALSE;
	
	return TRUE;
}

VOID CIocp::WorkerThreadCallback(VOID)
{
	//성공,실패 유무를 판단하는 변수 
	BOOL  Successed                   = FALSE; 
	DWORD NumberOfByteTransfered      = 0;    // 몇바이트의 IO가 발생했는지 확인
	VOID* CompletionKey               = NULL; // 키 값
	OVERLAPPED* Overlapped            = NULL; // Overlapped 값 
	OVERLAPPED_EX* OverlappedEx       = NULL; //
	VOID* Object                      = NULL; // 소켓 핸들에 매치되는 개체 포인터 값 


	while (TRUE)
	{
		//우선 WorkerThread가 실행됬다는 것을 알려줘야겠지 
		SetEvent(mStartupEventHandle);

		//PostQueuedCompletionStatus 나 IOCP에 등록된 소켓 및 파일의 Read Write관련 함수의 상태를 받아오는 함수 
		Successed = GetQueuedCompletionStatus(mIocpHandle,              // IOCP 핸들
			                                  &NumberOfByteTransfered,  // IO에 이용된 크기
			                                  (LPDWORD)& CompletionKey, // 키 값
			                                  &Overlapped,              // 넘겨온 OVerlapped 구조체  Read Write 등에 사용된 Overlapped 값 
			                                  INFINITE);                // 기다리는 시간 
	
	   // 키가 NULL 일 경우 종료합니다. 이것은 POSTQueuedCompletionStatus의 
	   // 키를 NULL로 넣어준 것으로 동작합니다. 
	   // 또는 코딩의 문제로 데이터가 잘못넘어올 경우 걸러주는 역할 합니다. 
		if (!CompletionKey)
			return ;

		//Overlapped 포인터를 받아 OverlappedEx의 포인터를 확인합니다. 
        
		OverlappedEx                 = (OVERLAPPED_EX*)Overlapped; // 캐스팅해서 넣어주면 확인이 가능합니다 대신 OVERLAPPEDEX의 첫번재 자료형은 무조건 OVERLAPPED 
		// 이 부분은 CNetWorkSession에서 WSARecv, WSASend 등을 할때 넘겨준 OVERLAPPED_EX를 그대로 받습니다. 
		// CNetworkSession의 Begin 함수를 보시면 OverlappedEx-> Object 에 this를 입력 해주어 어느 개체인지 확일 할 수 있게 한것을 볼 수 있다. 
		Object                       = OverlappedEx->Object;

		//GetQueuedCompletionStatus 의 결과가 실패거나 성공이지만 IO 사용된 크기가 0일 경우에는  
		if (!Successed || (Successed && !NumberOfByteTransfered))
		{
			if (OverlappedEx->IoType == IO_ACCEPT)
				OnIoConnected(Object);
			else
				OnIoDisconnected(Object);

			//다시 루프를 초기화합니다.
			continue;
		}

		//그리고 AcceptEx에는 IO_TYPE를 IO_ACCEPT로 설정한 OverlappedEx를 WSARecv에는 IO_TYPE를 IO_READ로 설정한 것을 WSASend에는 IO_TYPE을 IO_WRITE로 설정한 
		//OverlappedEx를 넣어주는 것도 볼 수 있습니다.
		switch (OverlappedEx->IoType)
		{
		case IO_READ:
			OnIoRead(Object, NumberOfByteTransfered);
			break;

		case IO_WRITE:
			OnIoWrite(Object, NumberOfByteTransfered);
			break;
		}

	}




}
