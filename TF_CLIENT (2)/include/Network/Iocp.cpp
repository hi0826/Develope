#include "stdafx.h"
#include "Iocp.h"


DWORD WINAPI WorkerThreadCallback(LPVOID arg)
{
	CIocp* Owner = (CIocp*)arg;
	Owner->WorkerThreadCallback();
	
	return 0;
}

CIocp::CIocp(VOID)
{ // ������
	mIocpHandle          = NULL; //IOCP�� �����ϴ� �ڵ�
	mWorkerThreadCount   = 0;    // ����� WorkerThread�� ���� 

	mStartupEventHandle  = NULL; // ������ �����ϴ� �̺�Ʈ 
}


CIocp::~CIocp(VOID)
{
}

BOOL CIocp::Begin(VOID)
{
	mIocpHandle          = NULL; //mIocpHandle ���� �ʱ�ȭ

	//���� �ý����� ������ �����ɴϴ�. 
	// ������ �ý��� �������� CPU �������� ����� ���Դϴ�. 
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	//CPU�� ������ 2�踸ŭ ��Ŀ �����带 �����. 
	mWorkerThreadCount = SystemInfo.dwNumberOfProcessors * 2;
	//Iocp �ڵ��� ���� 
	mIocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	//������ �����ߴٸ� ����˴ϴ�. 
	if (!mIocpHandle) return FALSE;

	//������ �������� �̺�Ʈ�� �����մϴ�. 
	mStartupEventHandle = CreateEvent(0, FALSE, FALSE, 0);
	if (mStartupEventHandle == NULL)
	{
		End();
		return FALSE;
	}
	 

	//CPU ������ 2�踸ŭ WorekerThread�� �����ؼ� mWorkerThreadVector 
	for (DWORD i = 0; i < 4; ++i)
	{
		HANDLE WorkerThread = CreateThread(NULL, 0, ::WorkerThreadCallback, this, 0, NULL);
		mWorkerThreadVector.push_back(WorkerThread);
		//�ϳ��� �����帶�� ������ �Ϸ�ɶ� ���� ����մϴ�.
		WaitForSingleObject(mStartupEventHandle, INFINITE);
	}

	return TRUE;
}

BOOL CIocp::End(VOID)
{
	//IOCP�� �̿��ؼ� �� WorkerThread���� �����ȣ�� ������ 
	for (DWORD i = 0; i < mWorkerThreadVector.size(); ++i)
		PostQueuedCompletionStatus(mIocpHandle, 0, 0, NULL); //IOCP�� ���������� ��ȣ�� ������ �Լ�

	for (DWORD i = 0; i < mWorkerThreadVector.size(); ++i)
	{   
		//�� WorkerThread�� ������ ����� �� ���� �������ݴϴ�. 
		WaitForSingleObject(mWorkerThreadVector[i], INFINITE);
		CloseHandle(mWorkerThreadVector[i]);
	}

	//IOCP �ڵ鵵 �����մϴ�. 
	if (mIocpHandle) CloseHandle(mIocpHandle);

	//WorkerThread�� �����ϴ� ���͸� �����ݴϴ�. 
	mWorkerThreadVector.clear();
   
	if (mStartupEventHandle) CloseHandle(mStartupEventHandle);
	
	return TRUE;
}

BOOL CIocp::RegisterSocketToIocp(SOCKET socket, ULONG_PTR completionKey)
{// IOCP��  ClientSocket�� ����ϴ� ������ �ϴ� �Լ� 
	if (!socket || !completionKey)
		return FALSE;

	mIocpHandle = CreateIoCompletionPort((HANDLE)socket, mIocpHandle, completionKey, 0);
	if (!mIocpHandle)return FALSE;
	
	return TRUE;
}

VOID CIocp::WorkerThreadCallback(VOID)
{
	//����,���� ������ �Ǵ��ϴ� ���� 
	BOOL  Successed                   = FALSE; 
	DWORD NumberOfByteTransfered      = 0;    // �����Ʈ�� IO�� �߻��ߴ��� Ȯ��
	VOID* CompletionKey               = NULL; // Ű ��
	OVERLAPPED* Overlapped            = NULL; // Overlapped �� 
	OVERLAPPED_EX* OverlappedEx       = NULL; //
	VOID* Object                      = NULL; // ���� �ڵ鿡 ��ġ�Ǵ� ��ü ������ �� 


	while (TRUE)
	{
		//�켱 WorkerThread�� ������ٴ� ���� �˷���߰��� 
		SetEvent(mStartupEventHandle);

		//PostQueuedCompletionStatus �� IOCP�� ��ϵ� ���� �� ������ Read Write���� �Լ��� ���¸� �޾ƿ��� �Լ� 
		Successed = GetQueuedCompletionStatus(mIocpHandle,              // IOCP �ڵ�
			                                  &NumberOfByteTransfered,  // IO�� �̿�� ũ��
			                                  (LPDWORD)& CompletionKey, // Ű ��
			                                  &Overlapped,              // �Ѱܿ� OVerlapped ����ü  Read Write � ���� Overlapped �� 
			                                  INFINITE);                // ��ٸ��� �ð� 
	
	   // Ű�� NULL �� ��� �����մϴ�. �̰��� POSTQueuedCompletionStatus�� 
	   // Ű�� NULL�� �־��� ������ �����մϴ�. 
	   // �Ǵ� �ڵ��� ������ �����Ͱ� �߸��Ѿ�� ��� �ɷ��ִ� ���� �մϴ�. 
		if (!CompletionKey)
			return ;

		//Overlapped �����͸� �޾� OverlappedEx�� �����͸� Ȯ���մϴ�. 
        
		OverlappedEx                 = (OVERLAPPED_EX*)Overlapped; // ĳ�����ؼ� �־��ָ� Ȯ���� �����մϴ� ��� OVERLAPPEDEX�� ù���� �ڷ����� ������ OVERLAPPED 
		// �� �κ��� CNetWorkSession���� WSARecv, WSASend ���� �Ҷ� �Ѱ��� OVERLAPPED_EX�� �״�� �޽��ϴ�. 
		// CNetworkSession�� Begin �Լ��� ���ø� OverlappedEx-> Object �� this�� �Է� ���־� ��� ��ü���� Ȯ�� �� �� �ְ� �Ѱ��� �� �� �ִ�. 
		Object                       = OverlappedEx->Object;

		//GetQueuedCompletionStatus �� ����� ���аų� ���������� IO ���� ũ�Ⱑ 0�� ��쿡��  
		if (!Successed || (Successed && !NumberOfByteTransfered))
		{
			if (OverlappedEx->IoType == IO_ACCEPT)
				OnIoConnected(Object);
			else
				OnIoDisconnected(Object);

			//�ٽ� ������ �ʱ�ȭ�մϴ�.
			continue;
		}

		//�׸��� AcceptEx���� IO_TYPE�� IO_ACCEPT�� ������ OverlappedEx�� WSARecv���� IO_TYPE�� IO_READ�� ������ ���� WSASend���� IO_TYPE�� IO_WRITE�� ������ 
		//OverlappedEx�� �־��ִ� �͵� �� �� �ֽ��ϴ�.
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
