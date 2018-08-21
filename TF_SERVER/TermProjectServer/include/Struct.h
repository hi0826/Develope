#pragma once
typedef struct _OBJECT_DATA
{
public: 
	XMFLOAT3      POS;
	WORD          DIR;
	WORD           ID;
    float          HP;
	float          MP;
	float         ATK;
	BYTE        STATE;
public:
	_OBJECT_DATA()
	{   
		POS       = XMFLOAT3(0.f, 0.f, 0.f);
		DIR       = 0;
		ID        = -1;
		HP        = 0;
		MP        = 0;
		ATK       = 0;
		STATE     = IDLESTATE;
	}

}OBJECT_DATA , *POBJECT_DATA;

typedef struct _NPC_EVENT
{
	chrono::high_resolution_clock::time_point   W_Time;
	EV_TYPE                                     Type;
	WORD                                        NPC_ID;
}NPC_EVENT, *PNPC_EVENT;

typedef struct _EXOVER
{
	WSAOVERLAPPED  WSAOVER;
	IO_TYPE        IO_Type;
	EV_TYPE        EV_Type;
	WSABUF         WsaBuf; 
	char           IO_Buf[MAX_BUFF_SIZE];
}EXOVER, *PEXOVER;
