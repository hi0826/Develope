#include "UserProcessor.h"
#include "NPCProcessor.h"
#include "SendManager.h"
#include "../Util/VectorData.h"
#include "../DB/DB.h"


CUserProcessor::CUserProcessor()
{
	Processor[CS_LOGIN] = &CUserProcessor::PlayerLogin;
	Processor[CS_MOVE] = &CUserProcessor::PlayerMove;
	Processor[CS_ATTACK] = &CUserProcessor::PlayerAttack;
	Processor[CS_COLLISION] = &CUserProcessor::PlayerCollision;
	Processor[CS_CHANGE_CHARACTER] = &CUserProcessor::PlayerChanageCharacter;
	EventProcessor[PLAYER_RESPAWN] = &CUserProcessor::PlayerRespawn;
}

bool CUserProcessor::Initialize()
{
	XMFLOAT3  InitPos = XMFLOAT3(60.f, 0.f, -60.f);

	for (int i = 0; i < MAX_USER; ++i)
	{
		CLIENTS[i].Initialize();
		CLIENTS[i].SetPos(InitPos);
		CLIENTS[i].SetDir(DIR_BACKWARD);
		CLIENTS[i].SetState(IDLESTATE);
		CLIENTS[i].SetStage(STAGE::STAGE1);
		CLIENTS[i].SetCurCharacter(CHARACTER::WARRIOR);
		CLIENTS[i].SetIsAlive(false);
		CLIENTS[i].SetHP(500.f);
		CLIENTS[i].SetMP(500.f);
		CLIENTS[i].SetAtk(10.f);
		CLIENTS[i].SetOOBB(InitPos);
	}
	return true;
}

void CUserProcessor::Close()
{
	for (int i = 0; i < MAX_USER; ++i) CLIENTS[i].Close();
}

WORD CUserProcessor::GetUsableID()
{
	for (WORD i = 0; i < MAX_USER; ++i)
		if (!CLIENTS[i].GetIsConnected()) return i;
	return -1;
}

void CUserProcessor::UserSetting(WORD id, SOCKET sock)
{
	CLIENTS[id].SetID(id);
	CLIENTS[id].SetSock(sock);
	CLIENTS[id].SetIsConnected(true);
	CLIENTS[id].SetIsAlive(true);
}

void CUserProcessor::CallRecv(WORD id)
{
	if (id >= 0 && id < MAX_USER) CLIENTS[id].CallRecv();
}

void CUserProcessor::PlayerInit(WORD id)
{
	/*나에게 패킷*/
	CSendManager::LoginSuccPacket(CLIENTS[id]);

	/* 내주위의 있는 사람에게 패킷*/
	for (WORD i = 0; i < MAX_USER; ++i)
	{
		if (i == id) continue;
		if (!CLIENTS[i].GetIsConnected()) continue;
		if (CLIENTS[i].GetStage() != CLIENTS[id].GetStage()) continue;
		if (!CLIENTS[i].GetIsAlive()) continue;
		if (!CProcessor::InMySight(CLIENTS[id].GetPos(), CLIENTS[i].GetPos(), VIEW_RADIUS)) continue;
		CLIENTS[i].VlLock();
		CLIENTS[i].VL.insert(id);
		CLIENTS[i].VlUnlock();
		CSendManager::PutPlayerPacket(CLIENTS[i], CLIENTS[id]);
	}

	/*나에게 주변 사람 패킷*/
	for (WORD i = 0; i < MAX_USER; ++i) {

		if (i == id) continue;
		if (!CLIENTS[i].GetIsConnected()) continue;
		if (CLIENTS[id].GetStage() != CLIENTS[i].GetStage()) continue;
		if (!CProcessor::InMySight(CLIENTS[i].GetPos(), CLIENTS[id].GetPos(), VIEW_RADIUS)) continue;
		CLIENTS[id].VlLock();
		CLIENTS[id].VL.insert(i);
		CLIENTS[id].VlUnlock();
		CSendManager::PutPlayerPacket(CLIENTS[id], CLIENTS[i]);

	}

	//나의 주위에 있는 NPC정보를 전송 
	for (WORD i = NPC_START; i < NUM_OF_NPC; ++i)
	{
		if (!NPCS[i - NPC_START].GetIsAlive()) continue;
		if (CLIENTS[id].GetStage() != NPCS[i - NPC_START].GetStage()) continue;
		if (!CProcessor::InMySight(CLIENTS[id].GetPos(), NPCS[i - NPC_START].GetPos(), VIEW_RADIUS)) continue;
		CProcessor::AWakeNPC(i - NPC_START);
		CLIENTS[id].VlLock();
		CLIENTS[id].VL.insert(i);
		CLIENTS[id].VlUnlock();
		CSendManager::PutNPCPacket(CLIENTS[id], NPCS[i - NPC_START]);
	}

}

void CUserProcessor::PacketProcess(WORD id, int work_size, char * io_ptr)
{
	char*Packet = CLIENTS[id].Process(work_size, io_ptr);
	if (Packet == NULL) return;
	(this->*Processor[Packet[1]])(id, Packet);
	CLIENTS[id].CallRecv();
}

void CUserProcessor::PlayerLogin(WORD id, char* packet)
{
	cs_packet_login* Packet = (cs_packet_login*)packet;
	char Str[30] = { 0, };
	int StrSize = WideCharToMultiByte(CP_ACP, 0, Packet->name, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, Packet->name, -1, Str, StrSize, 0, 0);

	/*Login Check*/
	for (int i = 0; i < MAX_USER; ++i)
	{
		if (id == i) continue;
		if (!strcmp(CLIENTS[i].GetName(), Str))
		{
			CSendManager::LoginFailPacket(CLIENTS[id]);
			return;
		}
	}

	/*DB Querry*/
	bool Success;
	OBJECT_DATA Data = CDB::GET()->GetUserData(id, CLIENTS[id], Str, &Success);
	/*DB Success*/

	if (Success)
	{
		CLIENTS[id].SetName(Str);
		CLIENTS[id].SetIsConnected(true);
		CLIENTS[id].SetID(id);
		CLIENTS[id].SetHP(Data.HP);
		CLIENTS[id].SetMP(Data.MP);
		CLIENTS[id].SetAtk(Data.ATK);
		CLIENTS[id].SetPos(Data.POS);
		CLIENTS[id].SetDir(Data.DIR);
		CLIENTS[id].SetState(Data.STATE);
		/*Login Success*/
		CSendManager::LoginSuccPacket(CLIENTS[id]);
		PlayerInit(id);
	}
	else
	{
		CSendManager::LoginFailPacket(CLIENTS[id]);
		PlayerDisconnect(id);
	}
}

void CUserProcessor::PlayerMove(WORD id, char * packet)
{  

	cs_packet_move* Packet = (cs_packet_move*)packet;
	CLIENTS[id].SetDir(Packet->dir);
	CLIENTS[id].SetState(Packet->state);
	CLIENTS[id].SetOOBB(Packet->pos);
	CLIENTS[id].SetPos(Packet->pos);
	UpdateViewList(id);
}

void CUserProcessor::PlayerAttack(WORD id, char * packet)
{
	cs_packet_attack* Packet = (cs_packet_attack*)packet;
	CLIENTS[id].SetState(ATTACKSTATE);
	BoundingOrientedBox AttackOOBB(Vector3::Add(CLIENTS[id].GetPos(), CVData::GET()->LookVectorMap[CLIENTS[id].GetDir()], 10.f), XMFLOAT3(5.5f, 5.5f, 5.5f), XMFLOAT4(0, 0, 0, 1));
	std::unordered_set<WORD> CVL;
	std::unordered_map<WORD, BOOL> HeatedNPCList;
	unsigned CurNPCHP = 0;


	CLIENTS[id].VlLock();
	CVL = CLIENTS[id].VL;
	CLIENTS[id].VlUnlock();

	for (auto ID : CVL)
	{
		if (IsNPC(ID))
		{
			if (NPCS[ID - NPC_START].CheckCollision(AttackOOBB)) {
				CurNPCHP = NPCS[ID - NPC_START].GetHP() - CLIENTS[id].GetAtk();
				NPCS[ID - NPC_START].SetHP(CurNPCHP);
				if (CurNPCHP <= 0)
				{
					HeatedNPCList.insert(std::make_pair(ID, true));
					NPCS[ID - NPC_START].SetState(DEADSTATE);
				}
				else
				{
					HeatedNPCList.insert(std::make_pair(ID, false));
					NPCS[ID - NPC_START].SetState(HEATSTATE);
				}
			}
			continue;
		}
		CSendManager::OtherPlayerAttackPacket(CLIENTS[ID], CLIENTS[id]);
	}

	CVL.insert(id); // 나 자신에게도 보내줘야한다. 

	WORD Index;
	for (auto& NPCID : HeatedNPCList)
	{
		for (auto& ID : CVL)
		{
			if (IsNPC(ID)) continue;
			Index = NPCID.first - NPC_START;

			if (NPCID.second) //죽었다!
			{
				AddEvent(Index, NPC_DEAD, high_resolution_clock::now() + 1s);
			}
			else
			{
				NPCS[Index].SetState(HEATSTATE);
				if (NPCS[Index].GetTarget() == NONE_TARGET)
				{
					NPCS[Index].SetTarget(id);
					if (NPCS[Index].GetEType() != NPC_ATTACK) {
						AddEvent(Index, NPC_ATTACK, high_resolution_clock::now() + 1s);
					}
				}
			}
			CSendManager::StateNPCPacket(CLIENTS[ID], NPCS[Index]);
		}
	}
}

void CUserProcessor::PlayerCollision(WORD id, char * packet)
{
	cs_packet_collision* Packet = (cs_packet_collision*)packet;
	CLIENTS[id].SetPos(Packet->pos);

	unordered_set<WORD> PlayerViewList;
	CLIENTS[id].VlLock();
	PlayerViewList = CLIENTS[id].VL;
	CLIENTS[id].VlUnlock();

	for (auto& ID : PlayerViewList)
	{
		if (CProcessor::IsNPC(ID)) continue;
		CSendManager::CollisionPlayerPacket(CLIENTS[ID], CLIENTS[id]);
	}
}

void CUserProcessor::PlayerChanageCharacter(WORD id, char * packet)
{
	cs_packet_change_character* Packet = (cs_packet_change_character*)packet;
	CLIENTS[Packet->id].SetCurCharacter(Packet->num);

	unordered_set<WORD> PlayerViewList;
	CLIENTS[id].VlLock();
	PlayerViewList = CLIENTS[id].VL;
	CLIENTS[id].VlUnlock();

	for (auto& ID : PlayerViewList)
	{
		if (CProcessor::IsNPC(ID)) continue;
		CSendManager::ChangeCharacterPlayerPacket(CLIENTS[ID], CLIENTS[id]);
	}
}

void CUserProcessor::PlayerDisconnect(WORD id)
{
	//CDB::GET()->SaveUserData(CLIENTS[id]);

	CLIENTS[id].VlLock();
	std::unordered_set<WORD> VLC = CLIENTS[id].VL;
	CLIENTS[id].VL.clear();
	CLIENTS[id].VlUnlock();

	for (auto ID : VLC)
	{
		if (CProcessor::IsNPC(ID)) continue;
		CLIENTS[ID].VlLock();
		if (CLIENTS[ID].GetIsConnected())
		{
			if (0 != CLIENTS[ID].VL.count(id))
			{
				CLIENTS[ID].VL.erase(id);
				CLIENTS[ID].VlUnlock();
				CSendManager::RemovePlayerPacket(CLIENTS[ID], CLIENTS[id]);
			}
		}
		else CLIENTS[ID].VlUnlock();
	}
	CLIENTS[id].Close();

}

void CUserProcessor::UpdateViewList(WORD id)
{
	/*NewViewList*/
	std::unordered_set<WORD> NewViewList;
	for (WORD i = 0; i < MAX_USER; ++i)
	{
		if (i == id) continue;
		if (!CLIENTS[i].GetIsConnected()) continue;
		if (!CLIENTS[i].GetIsAlive()) continue;
		if (CLIENTS[id].GetStage() != CLIENTS[i].GetStage()) continue;
		if (CProcessor::InMySight(CLIENTS[id].GetPos(), CLIENTS[i].GetPos(), VIEW_RADIUS))
			NewViewList.insert(i);

	}

	/*Add NPC*/
	for (WORD i = NPC_START; i < NUM_OF_NPC; ++i) {
		if (!NPCS[i - NPC_START].GetIsAlive()) continue;
		if (CLIENTS[id].GetStage() != NPCS[i - NPC_START].GetStage()) continue;
		if (InMySight(CLIENTS[id].GetPos(), NPCS[i - NPC_START].GetPos(), VIEW_RADIUS))
			NewViewList.insert(i);

	}

	/*New ViewList Update*/
	for (auto ID : NewViewList)
	{
		CLIENTS[id].VlLock();
		if (0 == CLIENTS[id].VL.count(ID))
		{
			CLIENTS[id].VL.insert(ID);
			CLIENTS[id].VlUnlock();
			if (IsNPC(ID)) // NPC 이면
			{
				AWakeNPC(ID - NPC_START);
				CSendManager::PutNPCPacket(CLIENTS[id], NPCS[ID - NPC_START]);
				continue;
			}
			CSendManager::PutPlayerPacket(CLIENTS[id], CLIENTS[ID]);

			CLIENTS[ID].VlLock();
			if (0 == CLIENTS[ID].VL.count(id)) {
				CLIENTS[ID].VL.insert(id);
				CLIENTS[ID].VlUnlock();
				CSendManager::PutPlayerPacket(CLIENTS[ID], CLIENTS[id]);
			}
			else {
				CLIENTS[ID].VlUnlock();
				CSendManager::StatePlayerPacket(CLIENTS[ID], CLIENTS[id]);
			}
		}
		else
		{
			CLIENTS[id].VlUnlock();
			if (IsNPC(ID))
			{
				CSendManager::StateNPCPacket(CLIENTS[id], NPCS[ID - NPC_START]);
				continue;
			}
			CLIENTS[ID].VlLock();
			if (0 == CLIENTS[ID].VL.count(id))
			{
				CLIENTS[ID].VL.insert(id);
				CLIENTS[ID].VlUnlock();
				CSendManager::PutPlayerPacket(CLIENTS[ID], CLIENTS[id]);
			}
			else
			{
				CLIENTS[ID].VlUnlock();
				CSendManager::StatePlayerPacket(CLIENTS[ID], CLIENTS[id]);
			}
		}
	}

	/*Old ViewList Update*/
	CLIENTS[id].VlLock();
	unordered_set<WORD> OldViewList = CLIENTS[id].VL;
	CLIENTS[id].VlUnlock();

	for (auto ID : OldViewList)
	{
		if (0 == NewViewList.count(ID))
		{
			CLIENTS[id].VlLock();
			CLIENTS[id].VL.erase(ID);
			CLIENTS[id].VlUnlock();
			if (IsNPC(ID)) {
				CSendManager::RemoveNPCPacket(CLIENTS[id], NPCS[ID - NPC_START]);
				continue;
			}
			CSendManager::RemovePlayerPacket(CLIENTS[id], CLIENTS[ID]);
			CLIENTS[ID].VlLock();
			if (0 != CLIENTS[ID].VL.count(id)) {
				CLIENTS[ID].VL.erase(id);
				CLIENTS[ID].VlUnlock();
				CSendManager::RemovePlayerPacket(CLIENTS[ID], CLIENTS[id]);
			}
			else CLIENTS[ID].VlUnlock();
		}
	}
}

void CUserProcessor::PlayerRespawn(WORD id)
{
	CLIENTS[id].SetHP(500.f);
	CLIENTS[id].SetMP(500.f);
	CLIENTS[id].SetIsAlive(true);
	CUserProcessor::PlayerInit(id);
}
