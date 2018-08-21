#include "NetWorkManager.h"
#include "ErrorUtil.h"
#include "../SceneManager.h"


CNetWorkManager::CNetWorkManager()
{  
	WSASendBuf.buf = SendBuffer;
	WSASendBuf.len = MAX_PACKET_SIZE;
	WSARecvBuf.buf = RecvBuffer;
	WSARecvBuf.len = MAX_PACKET_SIZE;
	InPacketSize = 0;
	SavedPacketSize = 0;
	Ret = 0;
	CSock = NULL;
	WSAData  wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
}

CNetWorkManager::~CNetWorkManager()
{

}

bool CNetWorkManager::Initialize()
{   
	char IP[30];
	//cout << "IP를 입력해 주세요!(LoopBack Address == -1)" << endl;
	//cin >> IP;
	//if (!strcmp(IP, "-1"))
		strcpy(IP, "192.168.102.35");
	CreateSocketNEvent(IP);
	/*ID 입력~*/
	return true;
}

void CNetWorkManager::CreateSocketNEvent(char* IP)
{   

	CSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, WSA_FLAG_OVERLAPPED);
	if (CSock == INVALID_SOCKET) { ErrorUtil::Err_Quit("Create Socket Error"); }

	SOCKADDR_IN ServerAddr;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(DEFAULT_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr(IP);

	if (WSAConnect(CSock, (SOCKADDR*)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
		ErrorUtil::Err_Quit("WSAConnect ERROR!");
	}

	hEvent = WSACreateEvent();
	if (WSAEventSelect(CSock, hEvent, FD_READ || FD_CLOSE) == SOCKET_ERROR){
		ErrorUtil::Err_Quit("Create EventSelct ERROR!");
	}
}

void CNetWorkManager::Close()
{
	WSACloseEvent(hEvent);
	if (CSock != NULL) closesocket(CSock);
	WSACleanup();
}

bool CNetWorkManager::Run()
{   
	Ret = WSAWaitForMultipleEvents(1, &hEvent, FALSE, WSA_INFINITE, FALSE);

	if ((Ret == WSA_WAIT_FAILED) || (Ret == WSA_WAIT_TIMEOUT)) return true; 
	else
	{
		WSAEnumNetworkEvents(CSock, hEvent, &NetEvent);
		if (NetEvent.lNetworkEvents & FD_READ) 
		{
			if (NetEvent.iErrorCode[FD_READ_BIT] != 0) {
				ErrorUtil::Err_Display("ERROR!");
				return  true; 
			}

			DWORD IO_Byte, IO_Flag = 0; 
			int Ret = WSARecv(CSock, &WSARecvBuf, 1, &IO_Byte, &IO_Flag, NULL, NULL);
			if (Ret) { ErrorUtil::Err_Display("RECV ERROR"); }

			BYTE* ptr = reinterpret_cast<BYTE*>(RecvBuffer);

			while (0 != IO_Byte)
			{
				if (0 == InPacketSize) InPacketSize = ptr[0];
				if (IO_Byte + SavedPacketSize >= InPacketSize) {
					memcpy(PacketBuffer + SavedPacketSize, ptr, InPacketSize - SavedPacketSize);
					/*처리 ProcessPacket 부분*/
					CSceneManager::GET_SINGLE()->PacketProcess(PacketBuffer);
					ptr += InPacketSize - SavedPacketSize;
					IO_Byte -= InPacketSize - SavedPacketSize;
					InPacketSize    = 0;
					SavedPacketSize = 0;
				}
				else
				{
					memcpy(PacketBuffer + SavedPacketSize, ptr, IO_Byte);
					SavedPacketSize += IO_Byte;
					IO_Byte = 0;
				}
			}
		}
		if (NetEvent.lNetworkEvents & FD_CLOSE){
			ErrorUtil::Err_Display("FD_CLOSE!");
			return false;
		}
	}
	return true;
}

void CNetWorkManager::SendPacket(void * ptr)
{
	char *Packet = reinterpret_cast<char *>(ptr);
	memcpy(SendBuffer, Packet, Packet[0]);
	WSASendBuf.buf = SendBuffer;
	WSASendBuf.len = SendBuffer[0];
	DWORD iobyte;
	WSASend(CSock, &WSASendBuf, 1, &iobyte, 0, NULL, NULL);
}

void CNetWorkManager::SendMovePacket(CPlayer& player)
{  
	cs_packet_move Packet; 
	Packet.size  = sizeof(cs_packet_move);
	Packet.type  = CS_MOVE;
	Packet.id    = SERVER_ID;
	Packet.pos   = player.GetWPosition();
	Packet.dir   = player.GetDirection();
	Packet.state = player.GetAnimState();
	SendPacket(&Packet);
}


void CNetWorkManager::SendAttackPacket(CPlayer & player)
{
	cs_packet_attack Packet;
	Packet.size = sizeof(cs_packet_attack);
	Packet.type = CS_ATTACK;
	Packet.id = SERVER_ID;

	SendPacket(&Packet);
}

void CNetWorkManager::SendAccurCollisionPacket(CPlayer & player)
{
	cs_packet_collision Packet; 
	Packet.size = sizeof(cs_packet_collision);
	Packet.type = CS_COLLISION;
	Packet.id   = SERVER_ID;
	Packet.pos  = player.GetWPosition();

	SendPacket(&Packet);
}

void CNetWorkManager::SendChangeCharacterPacket(CPlayer & player)
{
	cs_packet_change_character Packet; 
	Packet.size = sizeof(cs_packet_change_character);
	Packet.type = CS_CHANGE_CHARACTER;
	Packet.id   = SERVER_ID;
	Packet.num  = player.GetCurMeshNum();
	
	SendPacket(&Packet);
}
