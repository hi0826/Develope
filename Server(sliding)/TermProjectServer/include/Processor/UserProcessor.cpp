#include "UserProcessor.h"
#include "NPCProcessor.h"
#include "SendManager.h"
#include "../DB/DB.h"


CUserProcessor::CUserProcessor()
{
	Processor[CS_LOGIN] = &CUserProcessor::PlayerLogin;
	Processor[CS_MOVE] = &CUserProcessor::PlayerMove;
	Processor[CS_ATTACK] = &CUserProcessor::PlayerAttack;
}

bool CUserProcessor::Initialize()
{
	for (int i = 0; i < MAX_USER; ++i) {
		CLIENTS[i].Initialize();
		CLIENTS[i].SetPOS(XMFLOAT3(150.f, 0.f, 100.f));
		CLIENTS[i].SetDIR(DIR_FORWARD);
		CLIENTS[i].SetSTATE(IDLESTATE);
	}
	return true;
}

void CUserProcessor::Close()
{
	for (int i = 0; i < MAX_USER; ++i) {
		CLIENTS[i].Close();
	}
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

}

void CUserProcessor::CallRecv(WORD id)
{
	if (id >= 0 && id < MAX_USER)
		CLIENTS[id].CallRecv();
}

void CUserProcessor::Test(WORD id)
{
	CSendManager::PutPlayerPacket(CLIENTS[id], CLIENTS[id]);

	for (WORD i = 0; i < MAX_USER; ++i)
	{
		if (CLIENTS[i].GetIsConnected())
		{
			if (i == id) continue;
			if (!CProcessor::InMySight(CLIENTS[id], CLIENTS[i], VIEW_RADIUS)) continue;
			CLIENTS[i].VlLock();
			CLIENTS[i].VL.insert(id);
			CLIENTS[i].VlUnlock();
			CSendManager::PutPlayerPacket(CLIENTS[i], CLIENTS[id]);
		}
	}

	for (WORD i = 0; i < MAX_USER; ++i)
	{
		if (CLIENTS[i].GetIsConnected())
			if (i != id) {
				if (!CProcessor::InMySight(CLIENTS[i], CLIENTS[id], VIEW_RADIUS)) continue;
				CLIENTS[id].VlLock();
				CLIENTS[id].VL.insert(i);
				CLIENTS[id].VlUnlock();
				CSendManager::PutPlayerPacket(CLIENTS[id], CLIENTS[i]);
			}
	}

	//나의 주위에 있는 NPC정보를 전송 
	for (WORD i = NPC_START; i < NUM_OF_NPC; ++i)
	{
		//주위에 있는 녀석들을 깨워서 움직이게 해줘야 한다. 
		if (!NPCS[i - NPC_START].GetIsAlive()) continue;
		if (!CProcessor::InMySight(CLIENTS[id], NPCS[i - NPC_START], VIEW_RADIUS)) continue;
		CProcessor::AWakeNPC(i - NPC_START);
		CLIENTS[id].VlLock();
		CLIENTS[id].VL.insert(i);
		CLIENTS[id].VlUnlock();
		CSendManager::PutNPCPacket(CLIENTS[id], NPCS[i - NPC_START]);
	}

}

void CUserProcessor::Process(WORD id, int work_size, char * io_ptr)
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
		if (id != i)
			if (!strcmp(CLIENTS[i].GetName(), Str)) {
				CSendManager::LoginFailPacket(CLIENTS[id]);
				return;
			}
	}

	/*DB Querry*/
	bool Success;
	OBJECT_DATA Data = CDB::GET_SINGLE()->GetUserData(id, CLIENTS[id], Str, &Success);
	/*DB Success*/

	if (Success)
	{
		CLIENTS[id].SetName(Str);
		CLIENTS[id].SetIsConnected(true);
		CLIENTS[id].SetID(id);
		CLIENTS[id].SetLEV(Data.LEV);
		CLIENTS[id].SetEXP(Data.EXP);
		CLIENTS[id].SetHP(Data.HP);
		CLIENTS[id].SetMP(Data.MP);
		CLIENTS[id].SetATK(Data.ATK);
		CLIENTS[id].SetPOS(Data.POS);
		CLIENTS[id].SetDIR(Data.DIR);
		CLIENTS[id].SetSTATE(Data.STATE);
		/*Login Success*/
		CSendManager::LoginSuccPacket(CLIENTS[id]);
		// 자신 제외 내 접속을 모든플레이어에게 알림.. 
		for (WORD i = 0; i < MAX_USER; ++i)
		{
			if (CLIENTS[i].GetIsConnected()) {
				if (i == id) continue;
				if (!CProcessor::InMySight(CLIENTS[id], CLIENTS[i], VIEW_RADIUS)) continue;
				CSendManager::PutPlayerPacket(CLIENTS[i], CLIENTS[id]);
				CLIENTS[i].VlLock();
				CLIENTS[i].VL.insert(id);
				CLIENTS[i].VlUnlock();
			}
		}

		//나에게  다른 플레이어 정보 전송 
		for (WORD i = 0; i < MAX_USER; ++i)
		{
			if (CLIENTS[i].GetIsConnected())
				if (i != id) {
					if (!CProcessor::InMySight(CLIENTS[i], CLIENTS[id], VIEW_RADIUS)) continue;
					CLIENTS[id].VlLock();
					CLIENTS[id].VL.insert(i);
					CLIENTS[id].VlUnlock();
					CSendManager::PutPlayerPacket(CLIENTS[id], CLIENTS[i]);
				}
		}

		//나의 주위에 있는 NPC정보를 전송 
		//for (WORD i = NPC_START; i < NUM_OF_NPC; ++i)
		//{
		//	//주위에 있는 녀석들을 깨워서 움직이게 해줘야 한다. 
		//	if (!NPCM->GetIsAlive(i)) continue;
		//		if (!InMySight(CLIENTS[id], NPCM->GetNPC(i-NPC_START), VIEW_RADIUS)) continue;
		//	CNPCProcessor::GET_SINGLE()->AWakeNPC(i);
		//	CLIENTS[id].VlLock();
		//	CLIENTS[id].VL.insert(i);
		//	CLIENTS[id].VlUnlock();
		//	CSendManager::PutNPCPacket(CLIENTS[id], NPCM->GetNPC(i-NPC_START));
		//}
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
	printf("dasdasd\n");
	CLIENTS[id].SetPOS(Packet->pos);
	CLIENTS[id].SetDIR(Packet->dir);
	CLIENTS[id].SetSTATE(Packet->state);
	UpdateViewList(id);
}

void CUserProcessor::PlayerAttack(WORD id, char * packet)
{  
	//printf("asdas\n");
	cs_packet_attack* Packet = (cs_packet_attack*)packet;
	CLIENTS[id].SetPOS(Packet->pos);
	CLIENTS[id].SetDIR(Packet->dir);
	CLIENTS[id].SetSTATE(ATTACKSTATE);
	XMFLOAT3  AttackDis = Vector3::Add(CLIENTS[id].GetPOS(), DIRMAP[Packet->dir], 10);
	BoundingOrientedBox AttackOOBB(AttackDis, XMFLOAT3(5.f, 5.f, 5.f), XMFLOAT4(0, 0, 0, 1));

	for (auto i : CLIENTS[id].VL) 
	{
		//if (NPCS[i - NPC_START].CeckCollision(AttackOOBB))
		//{
		//	unsigned npchp = NPCS[i - NPC_START].GetHP() - CLIENTS[id].GetATK();
		//	NPCS[i - NPC_START].SetHP(npchp);
		//	for (auto ID : CLIENTS[id].VL) 
		//	{
		//		if (IsNPC(ID)) continue;
		//		CSendManager::AttackPlayerHeatPacket(CLIENTS[ID], NPCS[i - NPC_START]);
		//	}
		//	if (NPCS[i - NPC_START].GetHP() > 0) AddTimer(i, NPC_TRACE, GetTickCount() + 1000, id);
		//	else AddTimer(i, NPC_RESPAWN, GetTickCount() + 1000);
		//	break;
		//}
		//else
		//{ 
			//for (auto ID : CLIENTS[id].VL) {
				//if (IsNPC(i)) continue;
		        printf("%d\n", id);
		        printf("   %d\n", i);
				CSendManager::AttackPlayerNoHeatPacket(CLIENTS[i],CLIENTS[id]);
			//}
		//}
	}
}

void CUserProcessor::PlayerDisconnect(WORD id)
{
	CDB::GET_SINGLE()->SaveUserData(CLIENTS[id]);

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
		if (CLIENTS[i].GetIsConnected())
			if (CProcessor::InMySight(CLIENTS[id], CLIENTS[i], VIEW_RADIUS))
				NewViewList.insert(i);
	}
	/*Add NPC*/
	//for (WORD i = NPC_START; i < NUM_OF_NPC; ++i) {
	//	if (NPCM->GetIsAlive(i - NPC_START))
	//		if (InMySight(CLIENTS[id], NPCM->GetNPC(i - NPC_START), VIEW_RADIUS))
	//			NewViewList.insert(i);
	//}



	/*New ViewList Update*/
	for (auto ID : NewViewList)
	{
		CLIENTS[id].VlLock();
		if (0 == CLIENTS[id].VL.count(ID))
		{
			CLIENTS[id].VL.insert(ID);
			CLIENTS[id].VlUnlock();
			//if (IsNPC(ID)) // NPC 이면
			//{
			//	NPCM->AWakeNPC(ID - NPC_START);
			//	CSendManager::PutNPCPacket(CLIENTS[id], NPCM->GetNPC(ID - NPC_START));
			//	continue;
			//}
			CSendManager::PutPlayerPacket(CLIENTS[id], CLIENTS[ID]);

			CLIENTS[ID].VlLock();
			if (0 == CLIENTS[ID].VL.count(id)) {
				CLIENTS[ID].VL.insert(id);
				CLIENTS[ID].VlUnlock();
				CSendManager::PutPlayerPacket(CLIENTS[ID], CLIENTS[id]);
			}
			else {
				CLIENTS[ID].VlUnlock();
				CSendManager::MovePlayerPacket(CLIENTS[ID], CLIENTS[id]);
			}
		}
		else
		{
			CLIENTS[id].VlUnlock();
			//if (IsNPC(ID)){
			//	CSendManager::MoveNPCPacket(CLIENTS[id], NPCM->GetNPC(ID - NPC_START));
			//	continue;
			//}

			CLIENTS[ID].VlLock();
			if (0 == CLIENTS[ID].VL.count(id)) {
				CLIENTS[ID].VL.insert(id);
				CLIENTS[ID].VlUnlock();
				CSendManager::PutPlayerPacket(CLIENTS[ID], CLIENTS[id]);
			}
			else {
				CLIENTS[ID].VlUnlock();
				CSendManager::MovePlayerPacket(CLIENTS[ID], CLIENTS[id]);
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
			//if (IsNPC(ID)){
			//	CSendManager::RemoveNPCPacket(CLIENTS[id], NPCM->GetNPC(ID - NPC_START));
			//	continue;
			//}

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



