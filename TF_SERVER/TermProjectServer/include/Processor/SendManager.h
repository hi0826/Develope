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

	static void LoginSuccPacket(CUser& to)
	{
		sc_packet_logsucc Packet;
		/*플레이어의 초기 정보를 넘겨준다.*/
		Packet.size      = sizeof(sc_packet_logsucc);
		Packet.type      = SC_LOG_SUCC;
		Packet.id        = to.GetID();
		Packet.pos       = to.GetPos();
		Packet.dir       = to.GetDir();
		Packet.state     = to.GetState();
		Packet.character = to.GetCurCharacter();
		Packet.stage     = to.GetStage();
		Packet.hp        = to.GetHP();
		Packet.mp        = to.GetMP();
		Packet.atk       = to.GetAtk();
		
		SendPacket(to, &Packet);
	}

	static void PlayerDamageStepPacket(CUser& to)
	{
		sc_packet_damagestep_player Packet;
		Packet.size = sizeof(sc_packet_damagestep_player);
		Packet.type = SC_DAMAGESTEP_PLAYER;
		Packet.id   = to.GetID();
		Packet.hp   = to.GetHP();
		SendPacket(to, &Packet);
	}

	static void LoginFailPacket(const CUser& to)
	{
		sc_packet_Logfail Packet;
		Packet.size = sizeof(sc_packet_Logfail);
		Packet.type = SC_LOG_FAIL;
		SendPacket(to, &Packet);
	}

	static void PutPlayerPacket(const CUser& to, CUser& from)
	{
		sc_packet_put_player Packet;
		Packet.size      = sizeof(sc_packet_put_player);
		Packet.type      = SC_PUT_PLAYER;
		Packet.id        = from.GetID();
		Packet.pos       = from.GetPos();
		Packet.dir       = from.GetDir();
		Packet.state     = from.GetState();
		Packet.character = from.GetCurCharacter();

		SendPacket(to, &Packet);
	}

	static void PutNPCPacket(const CUser& to,  CNPC& from)
	{
		sc_packet_put_npc Packet;
		Packet.size    = sizeof(sc_packet_put_npc);
		Packet.type    = SC_PUT_NPC;
		Packet.id      = from.GetID()-NPC_START;
		Packet.mtype   = from.GetMType();
		Packet.pos     = from.GetPos();
		Packet.dir     = from.GetDir();
		Packet.state   = from.GetState();
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

	static void DeadPlayerPacket(const CUser& to, CUser& from)
	{
		sc_packet_dead_player Packet;
		
		Packet.size = sizeof(sc_packet_dead_player);
		Packet.type = SC_DEAD_PLAYER;
		Packet.id   = from.GetID();
		SendPacket(to, &Packet);
	}

	static void RemoveNPCPacket(const CUser& to, CNPC& from)
	{  
		sc_packet_remove_npc Packet;

		Packet.size = sizeof(sc_packet_remove_npc);
		Packet.type = SC_REMOVE_NPC;
		Packet.id   = from.GetID() - NPC_START;

		SendPacket(to, &Packet);
	}

	static void StatePlayerPacket(const CUser& to,  CUser& from )
	{
		sc_packet_move_player Packet;
		
		Packet.size  = sizeof(sc_packet_move_player);
		Packet.type  = SC_MOVE_PLAYER;
		Packet.id    = from.GetID();
		Packet.pos   = from.GetPos();
		Packet.dir   = from.GetDir();
		Packet.state = from.GetState();
		SendPacket(to, &Packet);
	}

	static void StateNPCPacket(const CUser& to,  CNPC& from)
	{
		sc_packet_state_npc Packet;
		Packet.size    = sizeof(sc_packet_state_npc);
		Packet.type    = SC_STATE_NPC;
		Packet.id      = from.GetID() - NPC_START;
		Packet.dir     = from.GetDir();
		Packet.pos     = from.GetPos();
		Packet.state   = from.GetState();
		SendPacket(to, &Packet);
	}

	static void OtherPlayerAttackPacket (const CUser& to, CUser& from)
	{
		sc_packet_attack_player Packet;
		Packet.size = sizeof(sc_packet_attack_player);
		Packet.type = SC_ATTACK_PLAYER;
		Packet.id   = from.GetID();

		SendPacket(to, &Packet);
	}

	static void CollisionPlayerPacket(const CUser& to, CUser& from)
	{
		sc_packet_collision_player Packet; 
		Packet.size = sizeof(sc_packet_collision_player); 
		Packet.type = SC_COLLISION_PLAYER;
		Packet.id   = from.GetID();
		Packet.pos  = from.GetPos();
		SendPacket(to, &Packet);
	}
	static void ChangeCharacterPlayerPacket(const CUser& to, CUser& from)
	{
		sc_packet_change_character Packet; 
		Packet.size = sizeof(sc_packet_change_character);
		Packet.type = SC_CHAGNE_CHARACTER;
		Packet.id = from.GetID();
		Packet.num = from.GetCurCharacter();
		SendPacket(to, &Packet);
	}

	
};
