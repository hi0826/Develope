#pragma once
#include "../Util/ErrorUtil.h"
#include "../Object/User.h"
#include "../Object/NPC.h"
class CSendManager
{
public:
	static void SendPacket(const CUser& cl, void* ptr)
	{
		unsigned char* Packet = reinterpret_cast<unsigned char*>(ptr);
		EXOVER* Over = new EXOVER;
		memcpy(Over->IO_Buf, ptr, Packet[0]);

		Over->IO_Type = IO_SEND;
		Over->WsaBuf.buf = Over->IO_Buf;
		Over->WsaBuf.len = Packet[0];
		ZeroMemory(&Over->WSAOVER, sizeof(WSAOVERLAPPED));
		int ret = WSASend(cl.GetSock(), &Over->WsaBuf, 1, NULL, 0, &Over->WSAOVER, NULL);

		if (0 != ret)
		{
			int err_no = WSAGetLastError();
			if (WSA_IO_PENDING != err_no)
				ErrorUtil::Err_Display("Error In Send Packet");
		}
	}

	static void PutPlayerPacket(const CUser& to, CUser& from)
	{
		sc_packet_put_player Packet;
		Packet.size    = sizeof(sc_packet_put_player);
		Packet.type    = SC_PUT_PLAYER;
		Packet.id      = from.GetID();
		Packet.pos     = from.GetPOS();
		Packet.dir     = from.GetDIR();
		Packet.state   = from.GetSTATE();
		SendPacket(to, &Packet);
	}

	static void PutNPCPacket(const CUser& to,  CNPC& from)
	{
		sc_packet_put_npc Packet;

		Packet.size   = sizeof(sc_packet_put_npc);
		Packet.type   = SC_PUT_NPC;
		Packet.id     = from.GetID();
		Packet.mtype = from.GetMType();
		Packet.pos    = from.GetPOS();
		Packet.dir    = from.GetDIR();
		SendPacket(to, &Packet);
		
	}

	static void RemovePlayerPacket(const CUser& to,  CUser& from)
	{
		sc_packet_remove_player Packet;

		Packet.size = sizeof(sc_packet_remove_player);
		Packet.type = SC_REMOVE_PLAYER;
		Packet.id = from.GetID();
		SendPacket(to, &Packet);
	}

	static void RemoveNPCPacket(const CUser& to, CNPC& from)
	{  
		sc_packet_remove_npc Packet;

		Packet.size = sizeof(sc_packet_remove_npc);
		Packet.type = SC_REMOVE_PLAYER;
		Packet.id   = from.GetID();

		SendPacket(to, &Packet);
	}

	static void MovePlayerPacket(const CUser& to,  CUser& from )
	{
		sc_packet_move_player Packet;
		
		Packet.size  = sizeof(sc_packet_move_player);
		Packet.type  = SC_MOVE_PLAYER;
		Packet.id    = from.GetID();
		Packet.pos   = from.GetPOS();
		Packet.dir   = from.GetDIR();
		Packet.state = from.GetSTATE();
		SendPacket(to, &Packet);
	}

	static void MoveNPCPacket(const CUser& to,  CNPC& from)
	{
		sc_packet_move_npc Packet;

		Packet.size = sizeof(sc_packet_move_npc);
		Packet.type = SC_MOVE_NPC;
		Packet.id   = from.GetID();
		Packet.pos  = from.GetPOS();
		SendPacket(to, &Packet);
	}

	static void HeatedNPCPacket(const CUser&, CNPC& from)
	{
		
	}
	static void AttackPlayerHeatPacket(const CUser&, CNPC&from)
	{
		sc_packet_player_attack_heat Packet;
		
	}
	static void AttackPlayerNoHeatPacket(const CUser& to, CUser& from)
	{	
		sc_packet_player_attack_no_heat Packet; 

		Packet.size = sizeof(sc_packet_player_attack_no_heat);
		Packet.type = SC_PLAYER_ATTACK_NO_HEAT;
		Packet.id = from.GetID();
		SendPacket(to,&Packet);
	}

	static void LoginSuccPacket(const CUser& to)
	{
		sc_packet_logsucc Packet;
		
		Packet.size = sizeof(sc_packet_logsucc);
		Packet.type = SC_LOG_SUCC;



		SendPacket(to, &Packet);
	}

	static void LoginFailPacket(const CUser& to)
	{
		sc_packet_Logfail Packet;

		Packet.size = sizeof(sc_packet_Logfail);
		Packet.type = SC_LOG_FAIL;

		SendPacket(to, &Packet);
	}

};
