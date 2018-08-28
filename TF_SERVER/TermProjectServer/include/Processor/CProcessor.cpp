#include "CProcessor.h"
#include "../Util/MapData.h"
#include "SendManager.h"
#include "../Util/Timer.h"
#include "../Util/VectorData.h"
std::array<CUser, MAX_USER> CProcessor::CLIENTS;
std::array<CNPC, NUM_OF_NPC - NPC_START> CProcessor::NPCS;
tbb::concurrent_priority_queue<NPC_EVENT, CTCompare> CProcessor::EventQueue;
CProcessor::CProcessor()
{

}

CProcessor::~CProcessor()
{

}

void CProcessor::AWakeNPC(WORD id)
{
	if (NPCS[id].GetIsAwake()) return;
	NPCS[id].SetIsAwake(true);
	AddEvent(id, NPC_MOVE, chrono::high_resolution_clock::now() + 3s);
}

//void CProcessor::SimulationObejcts()
//{
//
//
//	for (auto& NPC : NPCS)
//	{
//		if (!NPC.GetIsAlive()) continue;
//		if (!NPC.GetIsAwake()) continue;
//		if (NPC.GetState() != MOVESTATE) continue;
//
//		BYTE Target = NPC.GetTarget();
//		std::unordered_set<WORD> OldNPCViewList;
//
//		NPC.VlLock();
//		OldNPCViewList = NPC.VL;
//		NPC.VlUnlock();
//
//		if (Target != NONE_TARGET) NPC.LookAtTarget(CLIENTS[Target].GetPos());
//
//		NPC.Move(fDeltaTime);
//		NPC.MapCollisionCheck(fDeltaTime);
//
//		std::unordered_set<WORD> NewNPCViewList;
//		for (auto& CL : CLIENTS) {
//			if (!CL.GetIsConnected()) continue;
//			if (CL.GetStage() != NPC.GetStage()) continue;
//			if (CProcessor::InMySight(CL.GetPos(), NPC.GetPos(), VIEW_RADIUS)) NewNPCViewList.insert(CL.GetID());
//		}
//
//		for (auto ID : NewNPCViewList) {
//			CLIENTS[ID].VlLock();
//			if (0 == CLIENTS[ID].VL.count(NPC.GetID())) {
//				CLIENTS[ID].VL.insert(NPC.GetID());
//				CLIENTS[ID].VlUnlock();
//				CSendManager::PutNPCPacket(CLIENTS[ID], NPC);
//			}
//			else CLIENTS[ID].VlUnlock();
//		}
//
//		for (auto ID : OldNPCViewList) // 시야에서 지워준다. 
//		{
//			if (0 == NewNPCViewList.count(ID))
//			{
//				CLIENTS[ID].VlLock();
//				if (0 != CLIENTS[ID].VL.count(NPC.GetID())) {
//					CLIENTS[ID].VL.erase(NPC.GetID());
//					CLIENTS[ID].VlUnlock();
//					CSendManager::RemoveNPCPacket(CLIENTS[ID], NPC);
//				}
//				else CLIENTS[ID].VlUnlock();
//			}
//		}
//
//		NPC.VlLock();
//		NPC.VL = NewNPCViewList;
//		NPC.VlUnlock();
//	}
//
//
//	for (auto& NPC : NPCS)
//	{
//	
//		//CollisionCheck 
//
//
//		if (MOVESTATE != NPC.GetState()) continue; 
//
//		//Move
//
//
//	}
//
//
//}



