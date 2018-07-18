#include "stdafx.h"
#include "CriticalSection.h"
#include "MultiThreadSync.h"
#include "CircularQueue.h"
#include "NetworkSession.h"

DWORD WINAPI ReliableUdpThreadCallback(LPVOID arg)
{
	CNetWorkSession* Owner = (CNetWorkSession*)arg;
	Owner->ReliableUdpThreadCallback();
	return 0; 
}

CNetWorkSession::CNetWorkSession(VOID)
{
	ZeroMemory(&mAcceptOverlapped, sizeof(mAcceptOverlapped));
	ZeroMemory(&mReadOVerlapped, sizeof(mReadOVerlapped));
	ZeroMemory(&mWriteOverlapped, sizeof(mWriteOverlapped));

	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	mSocket = NULL;
	mReliableUdpThreadHandle = NULL; // ���� ��Ŷ�� ���� ������ ��� �������ִ� ������ �ڵ��Դϴ�. 

	mReliableUdpThreadStartupEvent = NULL; //ReliableudpThreadHandle�� ������ �˸��� �̺�Ʈ 
	mReliableUdpThreadDestroyEvent = NULL; // //  ���Ḧ �˸��� �̺�Ʈ
	mReliableUdpThreadWakeUpEvent = NULL;  // �����带 ���ﶧ ����ϴ� �̺�Ʈ 
	mReliableUdpWriteCompleteEvent = NULL; //  ��밡 ��Ŷ�� �޾� ���̻� ���� �ʿ䰡 ������ ����ϴ� �̺�Ʈ��

	mIsReliableUdpSending = FALSE;    // ���� ������ �ִ� Reliable Data�� �ִ��� Ȯ�� 


	mAcceptOverlapped.IoType = IO_ACCEPT;
	mReadOVerlapped.IoType = IO_READ;
	mWriteOverlapped.IoType = IO_WRITE;

	mAcceptOverlapped.Object = this;
	mReadOVerlapped.Object = this;
	mWriteOverlapped.Object = this;
}

CNetWorkSession::~CNetWorkSession(VOID)
{

}

BOOL CNetWorkSession::Begin(VOID)
{
	CThreadSync Sync;

	if (mSocket) // �̹� ����� ������ �ִ°��  ����ó�� 
		return FALSE;

	//���� �ʱ�ȭ 
	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	//UDP�� ����� �� �����͸� ���� ��� ������ �����ϱ� ���� ���� 
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	//������ ��� ����ϰ� �� ���� �ڵ� 
	mSocket = NULL;

	mReliableUdpThreadHandle = NULL; // Relible UDP ���� ������ �ڵ�
	mReliableUdpThreadStartupEvent = NULL; // �����尡 ���۵��� �˷��ִ� �̺�Ʈ 
	mReliableUdpThreadDestroyEvent = NULL; // �����尡 ������� �˷��ִ� �̺�Ʈ 
	mReliableUdpThreadWakeUpEvent = NULL;
	mReliableUdpWriteCompleteEvent = NULL; // ������ �� �Ѱ��� ������ ������ �Ϸ�Ǿ��� ��  �߻��ϴ� �̺�Ʈ 

	mIsReliableUdpSending = FALSE;         // ���� �����͸� ���������� Ȯ���ϴ� FLAG ���� 

	return TRUE;
}

BOOL CNetWorkSession::End(VOID)
{
	CThreadSync Sync;

	if (!mSocket)
		return FALSE;

	shutdown(mSocket, SD_BOTH);  // �ش� ������ READ/WRITE�� ����ϴ�.
	closesocket(mSocket);        // �ش� ������ �ݽ��ϴ�. 

	mSocket = NULL;


	if (mReliableUdpThreadHandle) //ReliableUDPThread�� �������� ��� 
	{
		SetEvent(mReliableUdpThreadDestroyEvent); //���� �̺�Ʈ�� ȣ���ϰ� 

		WaitForSingleObject(mReliableUdpThreadHandle, INFINITE); //���ᰡ �ɶ����� ��ٷ��� 

		CloseHandle(mReliableUdpThreadHandle); // ���ᰡ �Ǹ� �ڵ��� �ݾ��ݴϴ�. 
	}

	//���� �̺�Ʈ���� ��� ���� �����ݴϴ�. 
	if (mReliableUdpThreadDestroyEvent)
		CloseHandle(mReliableUdpThreadDestroyEvent);

	if (mReliableUdpThreadStartupEvent)
		CloseHandle(mReliableUdpThreadStartupEvent);

	if (mReliableUdpThreadWakeUpEvent)
		CloseHandle(mReliableUdpThreadWakeUpEvent);

	if (mReliableUdpWriteCompleteEvent)
		CloseHandle(mReliableUdpWriteCompleteEvent);

	mReliableWriteQueue.End();
	return TRUE;

}

BOOL CNetWorkSession::TCPBind(VOID)
{
	CThreadSync Sync;

	//mSocket�� NULL�� �ƴ� ��� �̹� ������ �����Ǿ� Bind �Ǿ��ִ� �����̹Ƿ� �ٽ� TCPBIND �Լ� �� 
	// �������� �ʽ��ϴ�. 
	if (mSocket)
		return FALSE;

	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (mSocket == INVALID_SOCKET)
	return FALSE;

	BOOL NoDelay =TRUE;
	setsockopt(mSocket,IPPROTO_TCP,TCP_NODELAY,(const char FAR*)&NoDelay,sizeof(NoDelay));
	return TRUE;
}

BOOL CNetWorkSession::UDPBind(USHORT port)
{
	CThreadSync sync;

	if (mSocket)
		return FALSE;

	SOCKADDR_IN RemoteAddressInfo;

	RemoteAddressInfo.sin_family = AF_INET;
	//������ ��Ʈ�� �ޱ����ؼ� ����
	RemoteAddressInfo.sin_port = htons(port); //������Ʈ ���� 
	RemoteAddressInfo.sin_addr.s_addr = htonl(INADDR_ANY);

	mSocket = WSASocket(AF_INET, SOCK_DGRAM, IPPROTO_UDP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (mSocket == INVALID_SOCKET)
		return FALSE;

	//bind
	if (::bind(mSocket, (struct sockaddr*)&RemoteAddressInfo, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		End();
		return FALSE;
	}

	//ReliableudpThread���� ����� �ڵ� �̺�Ʈ�� �����մϴ�. 
	// mReliableUdpThreadDestroyEvent
	//ReliableudpThread �����Ҷ� �߻���Ű�� �̺�Ʈ 
	mReliableUdpThreadDestroyEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (mReliableUdpThreadDestroyEvent == NULL) {
		End();
		return FALSE;
	}
	//

	// mReliableUdpThreadStartupEvent
	mReliableUdpThreadStartupEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (mReliableUdpThreadStartupEvent == NULL) {
		End();
		return FALSE;
	}
	//

	// mReliableUdpThreadWakeUpEvent
	mReliableUdpThreadWakeUpEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (mReliableUdpThreadWakeUpEvent == NULL) {
		End();
		return FALSE;
	}
	//

	// mReliableUdpWriteCompleteEvent
	mReliableUdpWriteCompleteEvent = CreateEvent(0, FALSE, FALSE, 0);
	if (mReliableUdpWriteCompleteEvent == NULL) {
		End();
		return FALSE;
	}
	//

	if (!mReliableWriteQueue.Begin()) {
		End();
		return FALSE;
	}

	DWORD ReliableUdpThreadID = 0;
	mReliableUdpThreadHandle = CreateThread(NULL, 0, ::ReliableUdpThreadCallback, this, 0, &ReliableUdpThreadID);
	WaitForSingleObject(mReliableUdpThreadStartupEvent, INFINITE);

	return TRUE;
}

BOOL CNetWorkSession::Listen(USHORT port, INT backLog)
{
	CThreadSync  Sync;
	if (port <= 0 || backLog <= 0) //backlog�� ���� SOMAXCONN 
		return FALSE;

	if (!mSocket)
		return FALSE;

	// ���� �ּ� ����ü �ۼ�
	SOCKADDR_IN ListenSocketInfo;

	ListenSocketInfo.sin_family = AF_INET;
	ListenSocketInfo.sin_port = htons(port);
	ListenSocketInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (::bind(mSocket, (struct sockaddr*)&ListenSocketInfo, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		End();
		return FALSE;
	}
	if (listen(mSocket, backLog) == SOCKET_ERROR)
	{
		End();
		return FALSE;
	}

	LINGER Linger;
	Linger.l_onoff = 1;
	Linger.l_linger = 0;

	if (setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&Linger, sizeof(LINGER)) == SOCKET_ERROR)
	{
		End();
		return FALSE;
	}

	return TRUE;

}

BOOL CNetWorkSession::Connect(LPSTR address, USHORT port)
// ���⼭�� TCHAR ���·� �ּҸ� �����ʽ��ϴ�. 
{

	CThreadSync Sync;
	if (!address || port <= 0)
		return FALSE;
	if (!mSocket)
		return FALSE;

	SOCKADDR_IN RemoteAddressInfo; //������ �ּ��� ������ �����ϴ� ����ü 

	RemoteAddressInfo.sin_family = AF_INET;
	RemoteAddressInfo.sin_port = htons(port);
	RemoteAddressInfo.sin_addr.S_un.S_addr = inet_addr(address);

	if (WSAConnect(mSocket, (LPSOCKADDR)&RemoteAddressInfo, sizeof(SOCKADDR_IN), NULL, NULL, NULL, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();
			return FALSE;
		}
	}

	return TRUE;
}

SOCKET CNetWorkSession::GetSocket(VOID)
{
	CThreadSync Sync; 
	return mSocket;
}

BOOL CNetWorkSession::Accept(SOCKET listenSocket)
{
	CThreadSync Sync;

	if (!listenSocket)
		return FALSE;

	if (mSocket)
		return FALSE;

	mSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0,
		WSA_FLAG_OVERLAPPED);

	if (mSocket == INVALID_SOCKET)
	{
		End();
		return FALSE;
	}
	BOOL NoDelay = TRUE;
	setsockopt(mSocket,IPPROTO_TCP,TCP_NODELAY,(const char FAR*)&NoDelay,sizeof(NoDelay));

	if (!AcceptEx(listenSocket,
		mSocket,
		mReadBuffer,
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		NULL,
		&mAcceptOverlapped.overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
		{
			End();
			return FALSE;
		}

	}

	return TRUE;
}

BOOL CNetWorkSession::InitializeReadForIocp(VOID)// TCP�� ���� Recv ȣ�� 
{
	CThreadSync Sync;

	if (!mSocket)
		return FALSE;

	WSABUF wsaBuf;           //WSABUF buf,len���� �����Ǿ� �ֽ��ϴ�.
	DWORD ReadBytes = 0;
	DWORD ReadFlag = 0;

	wsaBuf.buf = (CHAR*)mReadBuffer; // ����� ����� ReadBuffer�� �����͸� �Ѱ��ݴϴ�.
	wsaBuf.len = MAX_BUFFER_LENGTH;  // �ִ� ������ ���� 

	//WSARecvȣ�� 
	INT ReturnValue = WSARecv(mSocket, &wsaBuf,
		1, // ���� ������ �����մϴ�. WSABUF�� �̿��� ���� ���۸� ����� ��� 1�̻��� ���ڸ� �Է� 
		&ReadBytes, &ReadFlag, &mReadOVerlapped.overlapped, NULL);

	//SOCKET_ERRO ������ WSA_IO_PENDING �̰ų� WSAEWOULDBLOCK�̸� ���������� �����մϴ�. 
	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();
		return FALSE;
	}

	return TRUE;

	//�뷫������ WSARecv ó���� ACCept�ް� ȣ������ִ� �� ����. 
}

BOOL CNetWorkSession::InitializeReadFromIocp(VOID) // UDP�� ���� Recv ȣ�� 
{
	CThreadSync Sync;

	if (!mSocket) return FALSE;

	WSABUF WsaBuf;     //����� WSABUF 
	DWORD ReadBytes = 0;
	DWORD ReadFlag = 0;

	INT RemoteAddressInfoSize = sizeof(mUdpRemoteInfo);
	WsaBuf.buf = (CHAR*)mReadBuffer;
	WsaBuf.len = MAX_BUFFER_LENGTH;

	INT ReturnValue = WSARecvFrom(mSocket, &WsaBuf, 1, &ReadBytes, &ReadFlag,
		(SOCKADDR*)&mUdpRemoteInfo, &RemoteAddressInfoSize, &mReadOVerlapped.overlapped, NULL);

	if (SOCKET_ERROR == ReturnValue && WSAGetLastError() != WSA_IO_INCOMPLETE &&
		WSAGetLastError() != WSAEWOULDBLOCK) {
		End();
		return FALSE;
	}

	return TRUE;
}

BOOL CNetWorkSession::ReadForIocp(BYTE * data, DWORD & dataLength)
{
	CThreadSync Sync;

	if (!mSocket)
		return FALSE;
	if (!data || dataLength <= 0)
		return FALSE;

	CopyMemory(data, mReadBuffer, dataLength);
	//WSARecv�� ���ؼ� �޾ƿ� �����Ͱ� ����ִ� mReadBuffer���� �����͸� �����մϴ�.

	return TRUE;
}

BOOL CNetWorkSession::ReadFromForIocp(LPSTR remoteAddress, USHORT & remotePort, BYTE * data, DWORD & dataLength)
{
	CThreadSync Sync;

	if (!mSocket) return FALSE;

	if (!data || dataLength <= 0) return FALSE;

	CopyMemory(data, mReadBuffer, dataLength); // ���� �����͸� data�� �־��ݴϴ�. 
	//WSARecvFrom���� �����͸� �ޱ����ؼ� ����ߴ�  mReadBuffer�� �����͸� ���� 

	strcpy(remoteAddress, inet_ntoa(mUdpRemoteInfo.sin_addr)); //�����͸� ���� �ּ� Ȯ�� 
	remotePort = ntohs(mUdpRemoteInfo.sin_port); // �����͸� ���� ��Ʈ Ȯ�� 

	USHORT Ack = 0;
	CopyMemory(&Ack, mReadBuffer, sizeof(USHORT));

	if (Ack == 9999) { //���� 2����Ʈ�� 9999�� ��� �����͸� �� ���� ���� ��Ŷ���� �ν� 
		return FALSE;
	}
	else {
		Ack = 9999; // ���ο� �������ϰ�� 
		WriteTo2(remoteAddress, remotePort, (BYTE*)&Ack, sizeof(USHORT));
		//�����͸� ���� �ּҷ� Ack 2����Ʈ�� ���� 
	}

	return TRUE;
}

BOOL CNetWorkSession::ReadForEventSelect(BYTE * data, DWORD & dataLength)
{
	CThreadSync Sync;

	if (!mSocket)
		return FALSE;
	if (!data)
		return FALSE;

	WSABUF wsaBuf; //WSABUF �� ��� 
	DWORD ReadBytes = 0; //IOCP������ ������� ������ EventSelect������ ��� 
	DWORD ReadFlag = 0;

	wsaBuf.buf = (CHAR*)mReadBuffer;
	wsaBuf.len = MAX_BUFFER_LENGTH;

	//EventSelect ����� �̿��� ���� WSARecv�� IOCP�� ���� �װͰ��� �ٸ��ϴ�. 
	INT ReturnValue = WSARecv(mSocket,
		&wsaBuf, 1, &ReadBytes, &ReadFlag, &mReadOVerlapped.overlapped, NULL);

	//SOCKET_ERRO ������ WSA_IO_PENDING �̰ų� WSAEWOULDBLOCK�̸� ���������� �����մϴ�. 
	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();
		return FALSE;
	}


	//���� ������ �������ݴϴ�. 
	CopyMemory(data, mReadBuffer, ReadBytes);
	dataLength = ReadBytes;
	return TRUE;

}

BOOL CNetWorkSession::ReadFromForEventSelect(LPSTR remoteAddress, USHORT & remotePort, BYTE * data, DWORD & dataLength)
{
	CThreadSync Sync;

	if (!mSocket) return FALSE;
	if (!data)    return FALSE;
	if (!mSocket) return FALSE;

	WSABUF WsaBuf;
	DWORD  ReadBytes = 0;
	DWORD  ReadFlag = 0;

	INT RemoteAddressInfoSize = sizeof(mUdpRemoteInfo); // �����͸� ������ �ּ��� ���� 

	WsaBuf.buf = (CHAR*)mReadBuffer;
	WsaBuf.len = MAX_BUFFER_LENGTH;

	INT ReturnValue = WSARecvFrom(mSocket, &WsaBuf, 1, &ReadBytes, &ReadFlag, (SOCKADDR*)&mUdpRemoteInfo,
		&RemoteAddressInfoSize, &mReadOVerlapped.overlapped, NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
		End();
		return FALSE;
	}

	//IOCP�� �޸� �ٷ� ����� �����⶧���� ó���� ���ֵ��� �Ѵ� 
	CopyMemory(data, mReadBuffer, ReadBytes);
	dataLength = ReadBytes;

	//�ּ� ������ �����մϴ�.
	strcpy(remoteAddress, inet_ntoa(mUdpRemoteInfo.sin_addr));
	remotePort = ntohs(mUdpRemoteInfo.sin_port);

	//IOCP�ʰ� ���� ReliableUDP �ڵ��Դϴ�. 
	USHORT Ack = 0;
	CopyMemory(&Ack, mReadBuffer, sizeof(USHORT));

	// 9999�� ���� ó���� ���ֵ��� ���� 
	if (Ack == 9999)
	{
		//t��밡 �����͸� �޾Ҵٰ� ������ ���� ��쿡�� WriteComplete �̺�Ʈ�� �߻����� �ݴϴ�. 
		SetEvent(mReliableUdpWriteCompleteEvent);
		return FALSE;
	}
	else
	{
		Ack = 9999;
		WriteTo2(remoteAddress, remotePort, (BYTE*)&Ack, sizeof(USHORT));
	}

	return TRUE;
}

BOOL CNetWorkSession::Write(BYTE * data, DWORD dataLength)
{
	CThreadSync Sync;

	if (!mSocket) return  FALSE;
	if (!data || dataLength <= 0) return FALSE;

	WSABUF WsaBuf;
	DWORD  WriteBytes = 0;
	DWORD  WriteFlag = 0;

	WsaBuf.buf = (CHAR*)data;
	WsaBuf.len = dataLength;

	INT	ReturnValue = WSASend(mSocket, &WsaBuf, 1, &WriteBytes,
		WriteFlag, &mWriteOverlapped.overlapped, NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
		End();
		return FALSE;
	}

	return TRUE;
}

BOOL CNetWorkSession::WriteTo(LPCSTR remoteAddress, USHORT remotePort, BYTE * data, DWORD dataLength)
{
	CThreadSync Sync;

	if (!mSocket) return FALSE;

	if (!remoteAddress || remotePort <= 0 || !data || dataLength <= 0) return FALSE;

	//ť�� �����͸� �Է��մϴ�. 
	if (!mReliableWriteQueue.Push(this, data, dataLength))
	  return FALSE;

	// ���� ���� ������ �����Ͱ� ���� ��� 
	  if (!mIsReliableUdpSending) {
		  //������ �����Ͱ� �ִٰ� �÷��׸� �ٲپ��ְ� ReliableThread�� �ƿ�ϴ�. 
		  mIsReliableUdpSending = TRUE;
		  SetEvent(mReliableUdpThreadWakeUpEvent);
	 }
	  return TRUE;
}

BOOL CNetWorkSession::WriteTo2(LPSTR remoteAddress, USHORT remotePort, BYTE * data, DWORD dataLength)
{
	CThreadSync Sync;

	if (!mSocket) return FALSE;

	if (!remoteAddress || remotePort <= 0 || !data || dataLength <= 0) return FALSE;

	WSABUF WsaBuf;
	DWORD WriteBytes = 0;
	DWORD WriteFlag = 0;

	SOCKADDR_IN RemoteAdrressInfo;
	INT RemoteAddressInfoSize = sizeof(RemoteAdrressInfo);

	WsaBuf.buf = (CHAR*)data;
	WsaBuf.len = dataLength;

	//�����͸� ���� �ּҸ� �Է� 
	RemoteAdrressInfo.sin_family = AF_INET;
	RemoteAdrressInfo.sin_addr.S_un.S_addr = inet_addr(remoteAddress);
	RemoteAdrressInfo.sin_port = htons(remotePort);

	//WSASendTo�Լ��� �̿��ؼ� �����͸� �����մϴ�. 
	INT ReturnValue = WSASendTo(mSocket, &WsaBuf, 1, &WriteBytes, WriteFlag,
		(SOCKADDR*)&RemoteAdrressInfo, RemoteAddressInfoSize, &mWriteOverlapped.overlapped, NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
		End();
		return FALSE;
	}

	return TRUE;
}

VOID CNetWorkSession::ReliableUdpThreadCallback(VOID)
{
	DWORD EventID = 0; // ���� ���Ḧ üũ�ϴ� �̺�ƮID��
	//ReliableUDPThread�� ���۰� ���Ḧ ����ϴ� �̺�Ʈ 
	HANDLE ThreadEvents[2] = { mReliableUdpThreadDestroyEvent,mReliableUdpThreadWakeUpEvent };

	CHAR RemoteAddress[32] = { 0, }; // �����͸� ���� �ּ� 
	USHORT RemotePort = 0;
	BYTE Data[MAX_BUFFER_LENGTH] = { 0, };
	DWORD DataLength = 0;
	VOID* object = NULL;

	while (TRUE)
	{
		SetEvent(mReliableUdpThreadStartupEvent);
		//ReliableUDPThread�� ���۵ƴٴ� ���� �˸��� �̺�Ʈ udpBind�� wait�� ������ 
		//� �̺�Ʈ�� �߻��ϱ������� ���Ѵ�� ����մϴ�. 

		//� �̺�Ʈ�� �߻��ϱ� ������ ���Ѵ�� ����մϴ�. 
		EventID = WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE);
		switch (EventID)
		{
		case WAIT_OBJECT_0:
			return;

		case WAIT_OBJECT_0 + 1:
		NEXT_DATA:

			//Write�� ��� 1���� ���� �����͸� pop�� ���ְ�
			if (mReliableWriteQueue.Pop(&object, Data, DataLength, RemoteAddress,
				RemotePort))
			{
				//�����Ͱ� ���� ��� 
				//���� Write�� ���ְ� WaitrforsingleObject�� ���ݴϴ�. 
				//�޾��� �� �� setEvent�� ���ָ� Ǯ���ϴ�. 
			RETRY:
				//������ �����͸� �����ϴ� �Լ� 
				if (!WriteTo2(RemoteAddress, RemotePort, Data, DataLength))
					return;

				//������ ���⸦ 0.01�� ���� ��ٸ��ϴ�. 
				DWORD Result = WaitForSingleObject(mReliableUdpWriteCompleteEvent, 10);
				//�޾��� ��� ���������� ó�� 
				if (Result == WAIT_OBJECT_0)
					goto NEXT_DATA;
				else
					goto RETRY;
			}
			else
				//WriteTo�Լ��� ���� ������ �����Ͱ� �ִ����������� Ȯ���ϱ����� FLAG�����Դϴ�. 
				mIsReliableUdpSending = FALSE;
			break;
		}
	}
}

BOOL CNetWorkSession::GetLocalIP(WCHAR * pIP)
{
	CThreadSync Sync; 
	if (!mSocket) return FALSE;


	CHAR Name[256]          = { 0, };
	
	//HOST NAME ������� 
	gethostname(Name, sizeof(Name));

	PHOSTENT host = gethostbyname(Name);
	if (host)
	{
		if (MultiByteToWideChar(CP_ACP, 0, inet_ntoa(*(struct in_addr*)*host->h_addr_list), -1, pIP, 32) > 0)
			return TRUE;
	}

	return FALSE;
}

BOOL CNetWorkSession::GetLocalPort(VOID)
{
	CThreadSync Sync;

	if (!mSocket)
		return 0;

	SOCKADDR_IN Addr;
	ZeroMemory(&Addr, sizeof(Addr));

	INT AddrLength = sizeof(Addr);
	if (getsockname(mSocket, (sockaddr*)&Addr, &AddrLength) != SOCKET_ERROR)
		return ntohs(Addr.sin_port);

	return 0;
}
