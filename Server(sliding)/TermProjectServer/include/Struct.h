#pragma once
typedef struct _OBJECT_DATA
{
public: 
	XMFLOAT3      POS;
	WORD          DIR;
	WORD           ID;
    unsigned       HP;
	unsigned       MP;
	unsigned      EXP;
	unsigned char LEV; /* ~255 ±îÁö*/
	unsigned      ATK;
	BYTE        STATE;
public:
	_OBJECT_DATA()
	{   
		POS       = XMFLOAT3(0.f, 0.f, 0.f);
		DIR       = 0;
		ID        = -1;
		HP        = 0;
		MP        = 0;
		EXP       = 0;
		LEV       = 0;
		ATK       = 0;
		STATE     = IDLESTATE;
	}

}OBJECT_DATA , *POBJECT_DATA;

typedef struct _NPC_EVENT
{
	unsigned     W_Time;
	EV_TYPE      Type;
	WORD         NPC_ID;
	WORD         Target;
}NPC_EVENT, *PNPC_EVENT;

typedef struct _EXOVER
{
	WSAOVERLAPPED  WSAOVER;
	IO_TYPE        IO_Type;
	EV_TYPE        EV_Type;
	int            Event_Target;
	WSABUF         WsaBuf; 
	char           IO_Buf[MAX_BUFF_SIZE];
}EXOVER, *PEXOVER;
