#pragma once
class TFNetWorkCore :public CEventSelect
{
public:
	TFNetWorkCore(VOID);
	~TFNetWorkCore(VOID);
private:
	CPacketSession                 mSession;


	BYTE Packet[5] = {};
	//XMFLOAT3 Pos;
	//XMFLOAT3 Look;
	//XMFLOAT3 Right;
	CStreamSP Stream;

public:
	BOOL               BeginTcp(LPSTR remoteAddress, USHORT remotePort);
	BOOL               End(VOID);

public: //CEventSelect virtual 함수 구현 
	virtual VOID       OnIoRead(VOID);
	virtual VOID       OnIoConnected(VOID);
	virtual VOID       OnIoDisconnected(VOID);

public: /*Write함수*/
	VOID               RogInReQuest(const char* id, const char* password);
	VOID               MovePlayerRequest(BYTE direction);
	VOID               AttackPlayer();
	VOID               NotActingPlayer();


public:/*Read 함수*/
	VOID              PlayerInit(BYTE* Packet);
	VOID              MovePlayer(BYTE* Packet);

};