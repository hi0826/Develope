#pragma once
#include "../stdafx.h"
#include "../Player.h"
#include "../SingleTone.h"

class CNetWorkManager :public CSingleTone<CNetWorkManager>
{
private:
	SOCKET   CSock;
	WSAEVENT hEvent;
	WSANETWORKEVENTS NetEvent;
	WSABUF   WSASendBuf;
	WSABUF   WSARecvBuf;
	char     SendBuffer  [MAX_PACKET_SIZE];
	char     RecvBuffer  [MAX_PACKET_SIZE];
	char     PacketBuffer[MAX_PACKET_SIZE];
	DWORD    InPacketSize;
	int      SavedPacketSize;
	int      Ret;
    
private:
	WORD    SERVER_ID;
public:
	CNetWorkManager();
	virtual ~CNetWorkManager();

public:
	bool Initialize();
	void CreateSocketNEvent(char* IP);
	void Close();
	bool Run();

public:
	void SetNetworkID(WORD id) { SERVER_ID = id; }
	WORD GetNetworkID() const { return SERVER_ID; }

public:
	void SendPacket(void* ptr);
	void SendMovePacket(CPlayer& player);
	void SendAttackPacket(CPlayer& player);
};

