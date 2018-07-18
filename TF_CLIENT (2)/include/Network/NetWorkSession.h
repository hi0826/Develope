#pragma once

class CNetWorkSession : public CMultiThreadSync<CNetWorkSession>
{
private: 
	OVERLAPPED_EX   mAcceptOverlapped;
	OVERLAPPED_EX   mReadOVerlapped;
	OVERLAPPED_EX   mWriteOverlapped;

	BYTE            mReadBuffer[MAX_BUFFER_LENGTH];
	SOCKADDR_IN     mUdpRemoteInfo;
	 
	SOCKET          mSocket;

	HANDLE			mReliableUdpThreadHandle;  
	HANDLE			mReliableUdpThreadStartupEvent;
	HANDLE			mReliableUdpThreadDestroyEvent;
	HANDLE			mReliableUdpThreadWakeUpEvent;
	HANDLE			mReliableUdpWriteCompleteEvent;

	CCircularQueue	mReliableWriteQueue;

	BOOL			mIsReliableUdpSending;

public:
	CNetWorkSession(VOID);
	virtual ~CNetWorkSession(VOID);

public:
	BOOL            Begin(VOID);
	BOOL            End(VOID);

	BOOL            TCPBind(VOID);
	BOOL            UDPBind(USHORT port); 
	BOOL            Listen(USHORT port, INT backLog); 
	BOOL            Accept(SOCKET listenSocket);
	BOOL            Connect(LPSTR address, USHORT port);

	SOCKET          GetSocket(VOID);


	BOOL            InitializeReadForIocp(VOID); //TCP ->WSARecv 호출 
	BOOL            InitializeReadFromIocp(VOID); //UDP ->WSARecvFrom 호출 

	BOOL            ReadForIocp(BYTE* data, DWORD& dataLength); //TCP 데이터 읽어들이기 
	BOOL            ReadFromForIocp(LPSTR remoteAddress, USHORT& remotePort, BYTE* data, DWORD& dataLength); // UDP 데이터 읽어드리기 

	BOOL            ReadForEventSelect(BYTE* data, DWORD& dataLength); //CLIENT의  EventSelect 
	BOOL            ReadFromForEventSelect(LPSTR remoteAddress, USHORT& remotePort, BYTE* data, DWORD& dataLength);

	BOOL            Write(BYTE* data, DWORD dataLength); // TCP Send호출
	BOOL            WriteTo(LPCSTR remoteAddress, USHORT remotePort, BYTE* data, DWORD dataLength); 
	// WriteTO는 실질적으로 WSASend를 호출시켜주는 것이아님 UDP데아터는 ReliableUDPThread안에서 WriteTo2함수를 이용하여 실제로 전송됨  
    // WriteTo 함수는 보내야할 데이터를 큐에 넣어주는 역활과 새로운 데이터가 들어왔을때 Thread를 Wakeup해주는 역할을 하게 된다. 
	BOOL            WriteTo2(LPSTR remoteAddress, USHORT remotePort, BYTE* data, DWORD dataLength);

	VOID            ReliableUdpThreadCallback(VOID); //ReliableUDP를 위한 콜백함수 

	BOOL            GetLocalIP(WCHAR* pIP);
	BOOL            GetLocalPort(VOID);


	
};

