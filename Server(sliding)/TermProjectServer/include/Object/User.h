#pragma once
#include "Object.h"
class CUser :public CObject
{
	int                 PacketSize;
	int                 PrevPacketSize;
	char                Packet[MAX_PACKET_SIZE];
	SOCKET              mSock;
	EXOVER              Exover;
	bool                IsConnected;
	char                Name[MAX_STR_SIZE];
	std::shared_mutex   NM;
	std::mutex          Vlm;

public:
	std::unordered_set<WORD>  VL;
public:
	CUser() {};
	virtual ~CUser() {};

public:
	void  SetSock(SOCKET sock) { mSock = sock; }
	void  SetIsConnected(bool check) { IsConnected = check; }
	void  SetName(char* name)  { std::unique_lock<shared_mutex> lock(NM); strncpy(Name, name, sizeof(name)); }
									
public:								
	SOCKET       GetSock() const    { return mSock; }
	const bool   GetIsConnected()   { return IsConnected; }
    const char*  GetName()          { std::shared_lock<shared_mutex> lock(NM); return Name; }

public:
	void   VlLock()   { Vlm.lock(); }
	void   VlUnlock() { Vlm.unlock(); }
	
public:
	 void  Initialize();
	 void  Close();
	 void  CallRecv();
	 char* Process(int data_size, char* io_ptr);
};

