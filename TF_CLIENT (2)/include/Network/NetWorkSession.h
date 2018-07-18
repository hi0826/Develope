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


	BOOL            InitializeReadForIocp(VOID); //TCP ->WSARecv ȣ�� 
	BOOL            InitializeReadFromIocp(VOID); //UDP ->WSARecvFrom ȣ�� 

	BOOL            ReadForIocp(BYTE* data, DWORD& dataLength); //TCP ������ �о���̱� 
	BOOL            ReadFromForIocp(LPSTR remoteAddress, USHORT& remotePort, BYTE* data, DWORD& dataLength); // UDP ������ �о�帮�� 

	BOOL            ReadForEventSelect(BYTE* data, DWORD& dataLength); //CLIENT��  EventSelect 
	BOOL            ReadFromForEventSelect(LPSTR remoteAddress, USHORT& remotePort, BYTE* data, DWORD& dataLength);

	BOOL            Write(BYTE* data, DWORD dataLength); // TCP Sendȣ��
	BOOL            WriteTo(LPCSTR remoteAddress, USHORT remotePort, BYTE* data, DWORD dataLength); 
	// WriteTO�� ���������� WSASend�� ȣ������ִ� ���̾ƴ� UDP�����ʹ� ReliableUDPThread�ȿ��� WriteTo2�Լ��� �̿��Ͽ� ������ ���۵�  
    // WriteTo �Լ��� �������� �����͸� ť�� �־��ִ� ��Ȱ�� ���ο� �����Ͱ� �������� Thread�� Wakeup���ִ� ������ �ϰ� �ȴ�. 
	BOOL            WriteTo2(LPSTR remoteAddress, USHORT remotePort, BYTE* data, DWORD dataLength);

	VOID            ReliableUdpThreadCallback(VOID); //ReliableUDP�� ���� �ݹ��Լ� 

	BOOL            GetLocalIP(WCHAR* pIP);
	BOOL            GetLocalPort(VOID);


	
};

