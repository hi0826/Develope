#include "stdafx.h"
#include "NetWork.h"

TFNetWorkCore::TFNetWorkCore(VOID)
{
	WSADATA WsaData;
	WSAStartup(MAKEWORD(2, 2), &WsaData);
}

TFNetWorkCore::~TFNetWorkCore(VOID)
{
	WSACleanup();
}

BOOL TFNetWorkCore::BeginTcp(LPSTR remoteAddress, USHORT remotePort)
{
	if (!remoteAddress || remotePort <= 0) return FALSE;

	if (!mSession.Begin())
	{
		End();
		return FALSE;
	}
	if (!mSession.TCPBind())
	{
		End();
		return FALSE;
	}

	if (!CEventSelect::Begin(mSession.GetSocket()))
	{
		End();
		return FALSE;
	}

	if (!mSession.Connect(remoteAddress, remotePort))
	{
		End();
		return FALSE;
	}
	return TRUE;
}

BOOL TFNetWorkCore::End(VOID)
{
	mSession.End();
	CEventSelect::End();
	return TRUE;
}

VOID TFNetWorkCore::OnIoRead(VOID)
{
	if (mSession.ReadPacketForEventSelect())
	{
		DWORD  Protocol = 0;
		BYTE   Packet[PACKET_SIZE] = { 0, };
		DWORD  PacketLength;
		//CStreamSP Stream;
		//INT64 Serial;
		//float Pos[3];
		//float Look[3];
		//float Right[3];
		Stream->SetBuffer(Packet);
		while (mSession.GetPacket(Protocol, Packet, PacketLength))
		{
			switch (Protocol)
			{
			case SC_ROGIN_FAIL:
				break;
			case SC_ROGIN_SUCC:
			{
				PlayerInit(Packet);
				IsConnected = true;
				break;
			}
			case SC_PUT_PLAYER:
			{
				//printf("새로운놈 등판\n");
				mScene->InitOtherPlayer(Packet);
				break;
			}
			case SC_NOT_MY_VIEW:
			{
				// printf("애기왔어요!\n");
				mScene->NotMyViewPlayer(Packet);
				break;
			}
			case SC_LOG_OUT_PLAYER:
			{
				//Stream->ReadInt64(&Serial);
				//printf("LogOut Serial:: %d\n", Serial);
				mScene->LogOutPlayer(Packet);
				break;
			}
			case SC_PLAYER_MOVE:
			{
				MovePlayer(Packet);
				break;
			}
			case SC_PLAYER_MOVE_SYNC:
			{
				//printf("다른놈이 움직인다\n");
				mScene->MoveOtherPlayer(Packet);
				break;
			}
			case SC_PLAYER_ATTACK:
			{
				mScene->AttackOtherPlayer(Packet);
				break;
			}
			case SC_PLAYER_NOT_ACT:
			{
				mScene->NotActingOtherPlayer(Packet);
				// printf("아무것도 안하는놈이있어\n");
				break;
			}
			}
		}
	}
}
VOID TFNetWorkCore::OnIoConnected(VOID)
{
	//printf("Connect to Server Success\n");
}

VOID TFNetWorkCore::OnIoDisconnected(VOID)
{
	//	printf("DisConnect to Server\n");
}

VOID TFNetWorkCore::RogInReQuest(const char* id, const char* password)
{
	BYTE   Packet[MAX_BUFFER_LENGTH] = {};
	CStreamSP Stream;
	Stream->SetBuffer(Packet);
	Stream->WriteInt32(strlen(id));
	Stream->WriteBytes((BYTE*)id, strlen(id));
	Stream->WriteInt32(strlen(password));
	Stream->WriteBytes((BYTE*)password, strlen(password));
	mSession.WritePacket(CS_REQ_ROGIN, Packet, Stream->GetLength());
	mSession.WriteComplete();

	return;

}

VOID TFNetWorkCore::MovePlayerRequest(BYTE Direction)
{
	//BYTE Packet[5] = {};
	//CStreamSP Stream;
	Stream->SetBuffer(Packet);
	Stream->WriteInt32(mSerialNumber);
	Stream->WriteByte(Direction);
	mSession.WritePacket(CS_PLAYER_MOVE, Packet, Stream->GetLength());
	mSession.WriteComplete();
	//printf("보낸다!\n");
}

VOID TFNetWorkCore::AttackPlayer()
{
	BYTE Packet[2];
	//CStreamSP Stream;
	mSession.WritePacket(CS_PLAYER_ATTACK, Packet, 0);
	mSession.WriteComplete();
}

VOID TFNetWorkCore::NotActingPlayer()
{
	BYTE Packet[2];
	//CStreamSP Stream;
	mSession.WritePacket(CS_PLAYER_NOT_ACT, Packet, 0);
	mSession.WriteComplete();
}

VOID TFNetWorkCore::PlayerInit(BYTE*  Packet)
{
	//CStreamSP Stream;
	//Stream->SetBuffer(Packet);
	////XMFLOAT3 Pos;
	////XMFLOAT3 Look;
	////XMFLOAT3 Right;
	//Stream->ReadInt32(&mSerialNumber);
	//Stream->ReadFloat(&Pos.x);
	//Stream->ReadFloat(&Pos.y);
	//Stream->ReadFloat(&Pos.z);
	//Stream->ReadFloat(&Look.x);
	//Stream->ReadFloat(&Look.y);
	//Stream->ReadFloat(&Look.z);
	//Stream->ReadFloat(&Right.x);
	//Stream->ReadFloat(&Right.y);
	//Stream->ReadFloat(&Right.z);
	mScene->m_pPlayer->PlayerInit(/*Pos, Look, Right*/Packet);
}



VOID TFNetWorkCore::MovePlayer(BYTE* Packet)
{
	/*CStreamSP Stream;*/
	//Stream->SetBuffer(Packet);
	//XMFLOAT3 Pos;
	//XMFLOAT3 Look;
	//XMFLOAT3 Right;
	///*패킷 분해*/
	//Stream->ReadInt32(&mSerialNumber);
	//Stream->ReadFloat(&Pos.x);
	//Stream->ReadFloat(&Pos.y);
	//Stream->ReadFloat(&Pos.z);
	//Stream->ReadFloat(&Look.x);
	//Stream->ReadFloat(&Look.y);
	//Stream->ReadFloat(&Look.z);
	//Stream->ReadFloat(&Right.x);
	//Stream->ReadFloat(&Right.y);
	//Stream->ReadFloat(&Right.z);

	/*클라이언트에 적용*/
	mScene->m_pPlayer->MovePlayer(/*Pos, Look, Right*/Packet);

}