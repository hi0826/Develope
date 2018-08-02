#include "NPCProcessor.h"
#include "SendManager.h"

bool CNPCProcessor::Initialize()
{
	for (int i = NPC_START; i < NUM_OF_NPC; ++i)
	{
		NPCS[i].Initialize();
		NPCS[i].SetID(i + NPC_START);
		NPCS[i].SetIsAlive(true);
		NPCS[i].SetIsAwake(false);
		NPCS[i].SetTickTime(0);
		NPCS[i].SetEnemy(-1);
	}
	return true;
}

void CNPCProcessor::Close()
{

}

void CNPCProcessor::MoveNPC(WORD id, WORD target)
{
	if (NPCS[id].GetEType() != NPC_MOVE || !NPCS[id].GetIsAlive()) return;

	unordered_set<WORD> OldNPCViewList;
	//for (WORD i = 0; i < MAX_USER; ++i) {
	//	if (CLM->GETC(i).GetIsConnected())
	//		if (CSendUtil::InMySight(NPCS[i], CLM->GETC(i), VIEW_RADIUS))
	//			OldNPCViewList.insert(i);
	//}

	WORD Direction;
	switch (rand() % NUM_OF_DIRECTION)
	{
	case 0: NPCS[id].SetDIR(DIR_FORWARD);  Direction = DIR_FORWARD; break;
	case 1: NPCS[id].SetDIR(DIR_BACKWARD); Direction = DIR_BACKWARD;break;
	case 2: NPCS[id].SetDIR(DIR_LEFT);  Direction = DIR_LEFT; break;
	case 3: NPCS[id].SetDIR(DIR_RIGHT); Direction = DIR_RIGHT;break;
	case 4: NPCS[id].SetDIR(DIR_FR);    Direction = DIR_FR;break;
	case 5: NPCS[id].SetDIR(DIR_FL);    Direction = DIR_FL; break;
	case 6: NPCS[id].SetDIR(DIR_BR);    Direction = DIR_BR; break;
	case 7: NPCS[id].SetDIR(DIR_BL);    Direction = DIR_BL; break;
	default: break;
	}

	//XMFLOAT3 xmf3Shift = Vector3::ScalarProduct(DIRMAP[Direction],);
	//Vector3::Add(XMFLOAT3(0.f, 0.f, 0.f), DIRECTION, 10);
	//XMFLOAT3 Pos =
	//	NPCS[id].SetPOS(Vector3::Add(NPCS[id].GetPOS(), xmf3Shift));

	unordered_set<int> NewNPCViewList;

	//for (WORD i = 0; i < MAX_USER; ++i) {
	//
	//	if (CLM->GETC(i).GetIsConnected())
	//		if (CSendUtil::InMySight(CLM->GETC(i), NPCS[id], VIEW_RADIUS))
	//			NewNPCViewList.insert(i);
	//}
	//
	//
	//for (auto ID : NewNPCViewList) {
	//	CLM->GETC(ID).VlLock();
	//	if (0 == CLM->GETC(ID).VL.count(id))
	//	{
	//		CLM->GETC(ID).VL.insert(id);
	//		CLM->GETC(ID).VlUnlock();
	//		CSendManager::PutNPCPacket(CLM->GETC(ID), NPCS[id]);
	//	}
	//	else
	//	{
	//		CLM->GETC(ID).VlUnlock();
	//		CSendManager::MoveNPCPacket(CLM->GETC(ID), NPCS[id]);
	//	}
	//}
	//
	//for (auto ID : OldNPCViewList) {
	//	if (0 == NewNPCViewList.count(ID))
	//	{
	//		CLM->GETC(ID).VlLock();
	//		if (0 != CLM->GETC(ID).VL.count(id))
	//		{
	//			CLM->GETC(ID).VL.erase(id);
	//			CLM->GETC(ID).VlUnlock();
	//			CSendManager::RemoveNPCPacket(CLM->GETC(ID), NPCS[id]);
	//		}
	//		else CLM->GETC(ID).VlUnlock();
	//	}
	//}
	//
	//if (!NewNPCViewList.empty())
	//{
	//
	//}


}

void CNPCProcessor::AttackNPC(WORD id, WORD target)
{

}

void CNPCProcessor::TraceNPC(WORD id, WORD target)
{

}
