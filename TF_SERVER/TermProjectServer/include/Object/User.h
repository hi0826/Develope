#pragma once
#include "Object.h"
class CUser :public CObject
{
	static const BYTE   UserLockNum = 2; 

	int                PacketSize;
	int                PrevPacketSize;
	char               Packet[MAX_PACKET_SIZE];
	SOCKET             mSock;
	EXOVER             Exover;
	bool               IsConnected;
	char               Name[MAX_STR_SIZE];
	BYTE               CurCharacter;
	std::shared_mutex  UserLock[UserLockNum];

public:
	CUser() {};
	virtual ~CUser() {};

public:
	void SetSock(SOCKET sock) { mSock = sock; }
	void SetIsConnected(bool check) { IsConnected = check; }
	void SetName(char* name)             { std::unique_lock<shared_mutex> lock(UserLock[0]); strncpy(Name, name, sizeof(name)); }
	void SetCurCharacter(const BYTE cur) { std::unique_lock<shared_mutex> lock(UserLock[1]); CurCharacter = cur; }

public:								
	SOCKET      GetSock() const   { return mSock; }
	const bool  GetIsConnected()  { return IsConnected; }
    const char* GetName()         { std::shared_lock<shared_mutex> lock(UserLock[0]); return Name; }
	const BYTE  GetCurCharacter() { std::shared_lock<shared_mutex> lock(UserLock[1]); return CurCharacter; }

public:
	 void  Initialize();
	 void  Close();
	 void  CallRecv();
	 char* Process(int data_size, char* io_ptr);
};

