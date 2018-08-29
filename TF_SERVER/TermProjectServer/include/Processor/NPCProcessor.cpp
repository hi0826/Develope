#include "NPCProcessor.h"
#include "SendManager.h"
#include "../Util/MapData.h"
#include "../Util/VectorData.h"
#include "../Util/Astar.h"

CNPCProcessor::CNPCProcessor()
{
	srand((unsigned)time(NULL));

	Processor[NPC_IDLE] = &CNPCProcessor::IdleNPC;
	Processor[NPC_MOVE] = &CNPCProcessor::MoveNPC;
	Processor[NPC_ATTACK] = &CNPCProcessor::AttackNPC;
	Processor[NPC_DEAD] = &CNPCProcessor::DeadNPC;
}

bool CNPCProcessor::Initialize()
{

	srand(GetTickCount());
	WORD Index = 0;
	for (int i = NPC_START; i < NUM_OF_NPC; ++i)
	{
		Index = i - NPC_START;
		NPCS[Index].Initialize();
		NPCS[Index].SetID(i);
		NPCS[Index].SetDir(DIR_FORWARD);
		NPCS[Index].SetState(IDLESTATE);
		NPCS[Index].SetHP(100.f);
		NPCS[Index].SetIsAwake(false);
		NPCS[Index].SetMType(MONSTER_TYPE::STONEMON);
		NPCS[Index].SetTarget(NONE_TARGET);
		NPCS[Index].VL.clear();
		AddEvent(Index, NPC_MOVE, high_resolution_clock::now());
	}

	Index = 0;
	for (int y = 0; y < CMapData::GET()->Height; ++y)
	{
		for (int x = 0; x < CMapData::GET()->Width; ++x)
		{
			if (CMapData::GET()->Stage1[y][x] == READ_DATA::SMON)
			{
				/*StoneMon*/
				for (int i = 0; i < 3; ++i) {
					int randNum = rand() % 30 - 15;
					XMFLOAT3 InitPos(x*30.f + randNum, 0, -30 * y + randNum);
					NPCS[Index].SetPos(InitPos);
					NPCS[Index].SetOOBB(InitPos);
					NPCS[Index].SetStage(STAGE::STAGE1);
					NPCS[Index].SetMType(MONSTER_TYPE::STONEMON);
					NPCS[Index].SetHP(80.f);
					NPCS[Index].SetAtk(3.f);
					NPCS[Index].SetSpeed(5);
					NPCS[Index].SetIsAlive(true);
					Index++;
				}
			}
			else if (CMapData::GET()->Stage1[y][x] == READ_DATA::BMON)
			{
				/*BeatleMon*/
				for (int i = 0; i < 3; ++i) {
					int randNum = rand() % 30 - 15;
					XMFLOAT3 InitPos(x*30.f + randNum, 0, -30 * y + randNum);
					NPCS[Index].SetPos(InitPos);
					NPCS[Index].SetOOBB(InitPos);
					NPCS[Index].SetStage(STAGE::STAGE1);
					NPCS[Index].SetMType(MONSTER_TYPE::BEATLEMON);
					NPCS[Index].SetHP(100.f);
					NPCS[Index].SetAtk(5);
					NPCS[Index].SetSpeed(7);
					NPCS[Index].SetIsAlive(true);
					Index++;
				}
			}
			else if (CMapData::GET()->Stage1[y][x] == READ_DATA::SMONBOSS)
			{
				/*BeatleMon*/
				XMFLOAT3 InitPos(x*30.f, 0, -30 * y);
				NPCS[Index].SetPos(InitPos);
				NPCS[Index].SetOOBB(InitPos);
				NPCS[Index].SetStage(STAGE::STAGE1);
				NPCS[Index].SetMType(MONSTER_TYPE::STONEBOSS);
				NPCS[Index].SetHP(120.f);
				NPCS[Index].SetAtk(9);
				NPCS[Index].SetSpeed(10);
				NPCS[Index].SetIsAlive(true);
				Index++;
			}
			else if (CMapData::GET()->Stage1[y][x] == READ_DATA::BMONBOSS)
			{
				/*BeatleMon*/
				XMFLOAT3 InitPos(x*30.f, 0, -30 * y);
				NPCS[Index].SetPos(InitPos);
				NPCS[Index].SetOOBB(InitPos);
				NPCS[Index].SetStage(STAGE::STAGE1);
				NPCS[Index].SetMType(MONSTER_TYPE::BEATLEBOSS);
				NPCS[Index].SetHP(150.f);
				NPCS[Index].SetAtk(13);
				NPCS[Index].SetSpeed(10);
				NPCS[Index].SetIsAlive(true);
				Index++;
			}
		}
	}


	return true;
}

void CNPCProcessor::Close()
{
	FrameTimer.Stop();
}

void CNPCProcessor::SimulationNPC()
{
	FrameTimer.Start();
	float fDeltaTime;
	XMFLOAT3 NewPos;
	while (true)
	{
		FrameTimer.Tick(0.f);
		fDeltaTime = FrameTimer.GetTimeElapsed();
		for (auto& NPC : NPCS)
		{
			if (!NPC.GetIsAlive()) continue;
			if (!NPC.GetIsAwake()) continue;
			if (MOVESTATE != NPC.GetState()) continue;

			if (NPC.GetTarget() != NONE_TARGET) NPC.LookAtTarget(CLIENTS[NPC.GetTarget()].GetPos());

			NewPos = Vector3::Add(NPC.GetPos(), Vector3::ScalarProduct(Vector3::Normalize(CVData::GET()->LookVectorMap[NPC.GetDir()]),
				fDeltaTime * NPC.GetSpeed()));

			NPC.SetPos(NewPos);
		
		}
	}
}


void CNPCProcessor::IdleNPC(WORD id)
{
	WORD Index = id - NPC_START;
	if (!NPCS[Index].GetIsAlive() || !NPCS[Index].GetIsAwake()) return;
	NPCS[Index].SetState(IDLESTATE);
	BroadCastingNPCState(Index);
	AddEvent(Index, NPC_MOVE, high_resolution_clock::now() + 3s);
}

void CNPCProcessor::MoveNPC(WORD id)
{
	// 무브상태는 2가지 경우가 있다. 추적과 자율이동 
	WORD  Index = id - NPC_START;
	if (!NPCS[Index].GetIsAlive() || !NPCS[Index].GetIsAwake()) return;
	WORD Target = NPCS[Index].GetTarget();
	NPCS[Index].SetState(MOVESTATE);
	NPCS[Index].SetDir(CVData::GET()->RandomDirMap[rand() % 8]);

	std::unordered_set<WORD> CurNPCViewList;
	for (auto& CL : CLIENTS) { // NPC의 ViewList 업데이트
		if (!CL.GetIsConnected()) continue;
		if (CL.GetStage() != NPCS[Index].GetStage()) continue;
		if (!CProcessor::InMySight(NPCS[Index].GetPos(), CL.GetPos(), VIEW_RADIUS)) continue;
		CurNPCViewList.insert(CL.GetID());
	}

	NPCS[Index].VlLock();
	NPCS[Index].VL = CurNPCViewList;
	NPCS[Index].VlUnlock();

	for (auto& ID : CurNPCViewList) CSendManager::StateNPCPacket(CLIENTS[ID], NPCS[Index]);

	if (!CurNPCViewList.empty()) {

		if (Target != NONE_TARGET) // 타겟이 있을경우 
		{
			if (CProcessor::InMySight(NPCS[Index].GetPos(), CLIENTS[Target].GetPos(), ATTACK_RADIUS)) {
				if (NPCS[Index].GetEType() != NPC_ATTACK)
					AddEvent(Index, NPC_ATTACK, chrono::high_resolution_clock::now() + 10ms);
			}
			else {
				AddEvent(Index, NPC_MOVE, chrono::high_resolution_clock::now() + 500ms);
			}
		}
		else
		{
			switch (rand() % 2) {
			case 0: AddEvent(Index, NPC_MOVE, chrono::high_resolution_clock::now() + 500ms); break;
			case 1:	AddEvent(Index, NPC_IDLE, chrono::high_resolution_clock::now() + 10ms);  break;
			}
		}
	}
	else
	{
		NPCS[Index].SetState(IDLESTATE);
		NPCS[Index].SetIsAwake(false);
		NPCS[Index].SetTarget(NONE_TARGET);
	}
}

void CNPCProcessor::AttackNPC(WORD id)
{
	WORD Index = id - NPC_START;
	WORD Target = NPCS[Index].GetTarget();
	if (!NPCS[Index].GetIsAlive() || !NPCS[Index].GetIsAwake()) return;
	if (NPCS[Index].GetEType() != NPC_ATTACK) return;
	if (Target == NONE_TARGET) return;
	if (!CLIENTS[Target].GetIsConnected()) { NPCS[Index].SetTarget(NONE_TARGET); return; }

	NPCS[Index].SetState(ATTACKSTATE);
	NPCS[Index].LookAtTarget(CLIENTS[Target].GetPos());
	BroadCastingNPCState(Index);
	float  NewHP = CLIENTS[Target].GetHP() - NPCS[Index].GetAtk();
	CLIENTS[Target].SetHP(NewHP);

	if (CLIENTS[Target].GetHP() <= 0)
	{
		CLIENTS[Target].SetIsAlive(false);
		CSendManager::DeadPlayerPacket(CLIENTS[Target], CLIENTS[Target]);
		CLIENTS[Target].VlLock();
		std::unordered_set<WORD> CurPlayerViewList = CLIENTS[Target].VL;
		CLIENTS[Target].VlUnlock();
		for (auto& ID : CurPlayerViewList)
		{
			if (IsNPC(ID)) continue;
			CLIENTS[ID].VlLock();
			if (CLIENTS[ID].VL.count(Target) != 0)
			{
				CLIENTS[ID].VL.erase(Target);
				CLIENTS[ID].VlUnlock();
				CSendManager::DeadPlayerPacket(CLIENTS[ID], CLIENTS[Target]);
			}
			else CLIENTS[ID].VlUnlock();
		}
		NPCS[Index].SetTarget(NONE_TARGET);
		NPCS[Index].SetState(IDLESTATE);
		NPCS[Index].SetEType(NPC_IDLE);
		unordered_set<WORD> CurNPCViewList;
		NPCS[Index].VlLock();
		CurNPCViewList = NPCS[Index].VL;
		NPCS[Index].VlUnlock();
		for (auto& ID : CurNPCViewList) {
			CSendManager::StateNPCPacket(CLIENTS[ID], NPCS[Index]);
		}
		AddEvent(Index, NPC_MOVE, high_resolution_clock::now() + 3s);
		//AddUserEvent(Target, PLAYER_RESPAWN, high_resolution_clock::now() + 5s);
	}
	else
	{
		CSendManager::PlayerDamageStepPacket(CLIENTS[Target]);
		if (CProcessor::InMySight(NPCS[Index].GetPos(), CLIENTS[Target].GetPos(), ATTACK_RADIUS)) {
			AddEvent(Index, NPC_ATTACK, high_resolution_clock::now() + 1s);
		}
		else AddEvent(Index, NPC_MOVE, high_resolution_clock::now() + 10ms);
	}
}

void CNPCProcessor::DeadNPC(WORD id)
{
	/*죽는 상태는 무조건 실행되어야함*/
	WORD Index = id - NPC_START;
	if (!NPCS[Index].GetIsAlive() || !NPCS[Index].GetIsAwake()) return;
	for (auto& CL : CLIENTS) {
		if (!CL.GetIsConnected()) continue;
		if (CL.GetStage() != NPCS[Index].GetStage()) continue;
		if (!CProcessor::InMySight(NPCS[Index].GetPos(), CL.GetPos(), VIEW_RADIUS)) continue;
		CSendManager::RemoveNPCPacket(CL, NPCS[Index]);
	}
	NPCS[Index].SetIsAlive(false);
	NPCS[Index].SetIsAwake(false);
}

void CNPCProcessor::BroadCastingNPCState(WORD index)
{
	for (auto& CL : CLIENTS)
	{
		if (!CL.GetIsConnected()) continue;  // 알지?
		if (CL.GetStage() != NPCS[index].GetStage()) continue; // 같은 스테이지 내에 있는가?
		if (!CProcessor::InMySight(NPCS[index].GetPos(), CL.GetPos(), VIEW_RADIUS)) continue;
		CSendManager::StateNPCPacket(CL, NPCS[index]);
	}
}

