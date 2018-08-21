#include "CProcessor.h"
#include "../Util/MapData.h"
#include "SendManager.h"
#include "../Util/Timer.h"
#include "../Util/VectorData.h"
std::array<CUser, MAX_USER> CProcessor::CLIENTS;
std::array<CNPC, NUM_OF_NPC - NPC_START> CProcessor::NPCS;
std::priority_queue<NPC_EVENT, vector<NPC_EVENT>, CTCompare>  CProcessor::EventQueue;
CProcessor::CProcessor()
{
	FrameTimer.Start();
}

CProcessor::~CProcessor()
{
	FrameTimer.Stop();
}

void CProcessor::AddEvent(WORD id, EV_TYPE type, high_resolution_clock::time_point time)
{
	NPCS[id].SetEType(type);
	WORD ID = id + NPC_START;
	std::unique_lock<shared_mutex> lock(EMl);
	EventQueue.push(NPC_EVENT{ time,type,ID });
}

void CProcessor::AddUserEvent(WORD id, EV_TYPE type, high_resolution_clock::time_point time)
{
	std::unique_lock<shared_mutex> lock(EMl);
	EventQueue.push(NPC_EVENT{ time,type,id });
}

bool CProcessor::InMySight(const XMFLOAT3& me, const XMFLOAT3& other, BYTE distance)
{
	return  Vector3::Length(Vector3::Subtract(me, other)) <= distance;
}

void CProcessor::AWakeNPC(WORD id)
{
	if (NPCS[id].GetIsAwake()) return;
	NPCS[id].SetIsAwake(true);
	AddEvent(id, NPC_MOVE, chrono::high_resolution_clock::now() + 3s);
}

void CProcessor::SimulationObejcts()
{
	FrameTimer.Tick(60.f);
	float fDeltaTime = FrameTimer.GetTimeElapsed();
	
	for (auto& NPC : NPCS)
	{    
		if (!NPC.GetIsAlive()) continue;
		if (!NPC.GetIsAwake()) continue;
		if (NPC.GetSTATE() != MOVESTATE) continue;
		BYTE Target = NPC.GetTarget();
		std::unordered_set<WORD> OldNPCViewList;

		NPC.VlLock();
		OldNPCViewList = NPC.VL;
		NPC.VlUnlock();

		if (Target != NONE_TARGET) NPC.LookAtTarget(CLIENTS[Target].GetPOS());
		NPC.Move(fDeltaTime);
		NPC.MapCollisionCheck(fDeltaTime);

		std::unordered_set<WORD> NewNPCViewList;
		for (auto& CL : CLIENTS){
			if (!CL.GetIsConnected()) continue;
			if (CL.GetSTAGE() != NPC.GetSTAGE()) continue;
			if (CProcessor::InMySight(CL.GetPOS(), NPC.GetPOS(), VIEW_RADIUS)) NewNPCViewList.insert(CL.GetID());
		}

		for (auto ID : NewNPCViewList) {
			CLIENTS[ID].VlLock();
			if (0 == CLIENTS[ID].VL.count(NPC.GetID())) {
				CLIENTS[ID].VL.insert(NPC.GetID());
				CLIENTS[ID].VlUnlock();
				CSendManager::PutNPCPacket(CLIENTS[ID], NPC);
			}
			else CLIENTS[ID].VlUnlock();
		}

		for (auto ID : OldNPCViewList) // 시야에서 지워준다. 
		{
			if (0 == NewNPCViewList.count(ID))
			{
				CLIENTS[ID].VlLock();
				if (0 != CLIENTS[ID].VL.count(NPC.GetID())) {
					CLIENTS[ID].VL.erase(NPC.GetID());
					CLIENTS[ID].VlUnlock();
					CSendManager::RemoveNPCPacket(CLIENTS[ID], NPC);
				}else CLIENTS[ID].VlUnlock();
			}
		}

		NPC.VlLock();
		NPC.VL = NewNPCViewList;
		NPC.VlUnlock();
	}
	FrameTimer.GetFrameRate();
}



