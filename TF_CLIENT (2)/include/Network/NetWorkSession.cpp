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
	mReliableUdpThreadHandle = NULL; // 같은 패킷을 받을 때까지 계속 전송해주는 스레드 핸들입니다. 

	mReliableUdpThreadStartupEvent = NULL; //ReliableudpThreadHandle의 시작을 알리는 이벤트 
	mReliableUdpThreadDestroyEvent = NULL; // //  종료를 알리는 이벤트
	mReliableUdpThreadWakeUpEvent = NULL;  // 스레드를 깨울때 사용하는 이벤트 
	mReliableUdpWriteCompleteEvent = NULL; //  상대가 패킷을 받아 더이상 보낼 필요가 없을때 사용하는 이벤트임

	mIsReliableUdpSending = FALSE;    // 현재 보내고 있는 Reliable Data가 있는지 확인 


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

	if (mSocket) // 이미 실행된 소켓이 있는경우  실패처리 
		return FALSE;

	//변수 초기화 
	ZeroMemory(mReadBuffer, sizeof(mReadBuffer));
	//UDP를 사용할 때 데이터를 보낼 상대 정보를 저장하기 위한 변수 
	ZeroMemory(&mUdpRemoteInfo, sizeof(mUdpRemoteInfo));

	//앞으로 계속 사용하게 될 소켓 핸들 
	mSocket = NULL;

	mReliableUdpThreadHandle = NULL; // Relible UDP 메인 스레드 핸들
	mReliableUdpThreadStartupEvent = NULL; // 스레드가 시작됨을 알려주는 이벤트 
	mReliableUdpThreadDestroyEvent = NULL; // 스레드가 종료됨을 알려주는 이벤트 
	mReliableUdpThreadWakeUpEvent = NULL;
	mReliableUdpWriteCompleteEvent = NULL; // 스레드 내 한개의 데이터 전송이 완료되었을 때  발생하는 이벤트 

	mIsReliableUdpSending = FALSE;         // 현재 데이터를 전송중인지 확인하는 FLAG 변수 

	return TRUE;
}

BOOL CNetWorkSession::End(VOID)
{
	CThreadSync Sync;

	if (!mSocket)
		return FALSE;

	shutdown(mSocket, SD_BOTH);  // 해당 소켓의 READ/WRITE를 멈춥니다.
	closesocket(mSocket);        // 해당 소켓을 닫습니다. 

	mSocket = NULL;


	if (mReliableUdpThreadHandle) //ReliableUDPThread가 실행중일 경우 
	{
		SetEvent(mReliableUdpThreadDestroyEvent); //종료 이벤트를 호출하고 

		WaitForSingleObject(mReliableUdpThreadHandle, INFINITE); //종료가 될때까지 기다려줌 

		CloseHandle(mReliableUdpThreadHandle); // 종료가 되면 핸들을 닫아줍니다. 
	}

	//관련 이벤트들을 모두 종료 시켜줍니다. 
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

	//mSocket이 NULL이 아닐 경우 이미 소켓이 생성되어 Bind 되어있는 상태이므로 다시 TCPBIND 함수 를 
	// 실행하지 않습니다. 
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
	//지정된 포트를 받기위해서 지정
	RemoteAddressInfo.sin_port = htons(port); //서버포트 지정 
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

	//ReliableudpThread에서 사용할 핸들 이벤트를 생성합니다. 
	// mReliableUdpThreadDestroyEvent
	//ReliableudpThread 종료할때 발생시키는 이벤트 
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
	if (port <= 0 || backLog <= 0) //backlog는 보통 SOMAXCONN 
		return FALSE;

	if (!mSocket)
		return FALSE;

	// 서버 주소 구조체 작성
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
// 여기서는 TCHAR 형태로 주소를 받지않습니다. 
{

	CThreadSync Sync;
	if (!address || port <= 0)
		return FALSE;
	if (!mSocket)
		return FALSE;

	SOCKADDR_IN RemoteAddressInfo; //접속할 주소의 정보를 저장하는 구조체 

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

BOOL CNetWorkSession::InitializeReadForIocp(VOID)// TCP를 위한 Recv 호출 
{
	CThreadSync Sync;

	if (!mSocket)
		return FALSE;

	WSABUF wsaBuf;           //WSABUF buf,len으로 구성되어 있습니다.
	DWORD ReadBytes = 0;
	DWORD ReadFlag = 0;

	wsaBuf.buf = (CHAR*)mReadBuffer; // 멤버로 선언된 ReadBuffer의 포인터를 넘겨줍니다.
	wsaBuf.len = MAX_BUFFER_LENGTH;  // 최대 버퍼의 길이 

	//WSARecv호출 
	INT ReturnValue = WSARecv(mSocket, &wsaBuf,
		1, // 버퍼 개수를 지정합니다. WSABUF를 이용한 원형 버퍼를 사용할 경우 1이상의 숫자를 입력 
		&ReadBytes, &ReadFlag, &mReadOVerlapped.overlapped, NULL);

	//SOCKET_ERRO 이지만 WSA_IO_PENDING 이거나 WSAEWOULDBLOCK이면 정상적으로 진행합니다. 
	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();
		return FALSE;
	}

	return TRUE;

	//대략적으로 WSARecv 처음에 ACCept받고 호출시켜주는 문 같다. 
}

BOOL CNetWorkSession::InitializeReadFromIocp(VOID) // UDP를 위한 Recv 호출 
{
	CThreadSync Sync;

	if (!mSocket) return FALSE;

	WSABUF WsaBuf;     //사용할 WSABUF 
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
	//WSARecv를 통해서 받아온 데이터가 들어있는 mReadBuffer에서 데이터를 복사합니다.

	return TRUE;
}

BOOL CNetWorkSession::ReadFromForIocp(LPSTR remoteAddress, USHORT & remotePort, BYTE * data, DWORD & dataLength)
{
	CThreadSync Sync;

	if (!mSocket) return FALSE;

	if (!data || dataLength <= 0) return FALSE;

	CopyMemory(data, mReadBuffer, dataLength); // 받은 데이터를 data에 넣어줍니다. 
	//WSARecvFrom에서 데이터를 받기위해서 등록했던  mReadBuffer의 데이터를 복사 

	strcpy(remoteAddress, inet_ntoa(mUdpRemoteInfo.sin_addr)); //데이터를 보낸 주소 확인 
	remotePort = ntohs(mUdpRemoteInfo.sin_port); // 데이터를 보낸 포트 확인 

	USHORT Ack = 0;
	CopyMemory(&Ack, mReadBuffer, sizeof(USHORT));

	if (Ack == 9999) { //앞의 2바이트가 9999일 경우 데이터를 잘 받은 응답 패킷으로 인식 
		return FALSE;
	}
	else {
		Ack = 9999; // 새로운 데이터일경우 
		WriteTo2(remoteAddress, remotePort, (BYTE*)&Ack, sizeof(USHORT));
		//데이터를 보낸 주소로 Ack 2바이트를 전송 
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

	WSABUF wsaBuf; //WSABUF 를 사용 
	DWORD ReadBytes = 0; //IOCP에서는 사용하지 않지만 EventSelect에서는 사용 
	DWORD ReadFlag = 0;

	wsaBuf.buf = (CHAR*)mReadBuffer;
	wsaBuf.len = MAX_BUFFER_LENGTH;

	//EventSelect 방식을 이용할 때의 WSARecv는 IOCP일 떄의 그것과는 다릅니다. 
	INT ReturnValue = WSARecv(mSocket,
		&wsaBuf, 1, &ReadBytes, &ReadFlag, &mReadOVerlapped.overlapped, NULL);

	//SOCKET_ERRO 이지만 WSA_IO_PENDING 이거나 WSAEWOULDBLOCK이면 정상적으로 진행합니다. 
	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK)
	{
		End();
		return FALSE;
	}


	//받은 내용을 복사해줍니다. 
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

	INT RemoteAddressInfoSize = sizeof(mUdpRemoteInfo); // 데이터를 보내온 주소의 길이 

	WsaBuf.buf = (CHAR*)mReadBuffer;
	WsaBuf.len = MAX_BUFFER_LENGTH;

	INT ReturnValue = WSARecvFrom(mSocket, &WsaBuf, 1, &ReadBytes, &ReadFlag, (SOCKADDR*)&mUdpRemoteInfo,
		&RemoteAddressInfoSize, &mReadOVerlapped.overlapped, NULL);

	if (ReturnValue == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING && WSAGetLastError() != WSAEWOULDBLOCK) {
		End();
		return FALSE;
	}

	//IOCP와 달리 바로 결과가 나오기때문에 처리를 해주도록 한다 
	CopyMemory(data, mReadBuffer, ReadBytes);
	dataLength = ReadBytes;

	//주소 정보를 복사합니다.
	strcpy(remoteAddress, inet_ntoa(mUdpRemoteInfo.sin_addr));
	remotePort = ntohs(mUdpRemoteInfo.sin_port);

	//IOCP쪽과 같은 ReliableUDP 코드입니다. 
	USHORT Ack = 0;
	CopyMemory(&Ack, mReadBuffer, sizeof(USHORT));

	// 9999에 대한 처리를 해주도록 하자 
	if (Ack == 9999)
	{
		//t상대가 데이터를 받았다고 응답을 보낸 경우에는 WriteComplete 이벤트를 발생시켜 줍니다. 
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

	//큐에 데이터를 입력합니다. 
	if (!mReliableWriteQueue.Push(this, data, dataLength))
	  return FALSE;

	// 만약 현재 보내는 데이터가 없을 경우 
	  if (!mIsReliableUdpSending) {
		  //보내는 데이터가 있다고 플래그를 바꾸어주고 ReliableThread를 꺠웁니다. 
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

	//데이터를 보낼 주소를 입력 
	RemoteAdrressInfo.sin_family = AF_INET;
	RemoteAdrressInfo.sin_addr.S_un.S_addr = inet_addr(remoteAddress);
	RemoteAdrressInfo.sin_port = htons(remotePort);

	//WSASendTo함수를 이용해서 데이터를 전송합니다. 
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
	DWORD EventID = 0; // 시작 종료를 체크하는 이벤트ID값
	//ReliableUDPThread의 시작과 종료를 담당하는 이벤트 
	HANDLE ThreadEvents[2] = { mReliableUdpThreadDestroyEvent,mReliableUdpThreadWakeUpEvent };

	CHAR RemoteAddress[32] = { 0, }; // 데이터를 보낼 주소 
	USHORT RemotePort = 0;
	BYTE Data[MAX_BUFFER_LENGTH] = { 0, };
	DWORD DataLength = 0;
	VOID* object = NULL;

	while (TRUE)
	{
		SetEvent(mReliableUdpThreadStartupEvent);
		//ReliableUDPThread가 시작됐다는 것을 알리는 이벤트 udpBind의 wait을 끝내줌 
		//어떤 이벤트가 발생하기전까지 무한대로 대기합니다. 

		//어떤 이벤트가 발생하기 전까지 무한대로 대기합니다. 
		EventID = WaitForMultipleObjects(2, ThreadEvents, FALSE, INFINITE);
		switch (EventID)
		{
		case WAIT_OBJECT_0:
			return;

		case WAIT_OBJECT_0 + 1:
		NEXT_DATA:

			//Write일 경우 1개의 보낼 데이터를 pop을 해주고
			if (mReliableWriteQueue.Pop(&object, Data, DataLength, RemoteAddress,
				RemotePort))
			{
				//데이터가 있을 경우 
				//실제 Write를 해주고 WaitrforsingleObject를 해줍니다. 
				//받았을 떄 그 setEvent를 해주면 풀립니다. 
			RETRY:
				//실제로 데이터를 전송하는 함수 
				if (!WriteTo2(RemoteAddress, RemotePort, Data, DataLength))
					return;

				//응답이 오기를 0.01초 동안 기다립니다. 
				DWORD Result = WaitForSingleObject(mReliableUdpWriteCompleteEvent, 10);
				//받았을 경우 다음데이터 처리 
				if (Result == WAIT_OBJECT_0)
					goto NEXT_DATA;
				else
					goto RETRY;
			}
			else
				//WriteTo함수에 현재 보내는 데이터가 있는지없는지를 확인하기위한 FLAG변수입니다. 
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
	
	//HOST NAME 얻기이이 
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
