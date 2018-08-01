#include "CProcessor.h"
#include "../Object/User.h"
#include "../Object/NPC.h"
std::array<CUser, MAX_USER> CProcessor::CLIENTS;
std::array<CNPC, NUM_OF_NPC> CProcessor::NPCS;
std::unordered_map<WORD, XMFLOAT3> CProcessor::DIRMAP;
CProcessor::CProcessor()
{
	if (DIRMAP.size() == 0) {

		DIRMAP[DIR_FORWARD]  = XMFLOAT3(0.f, 0.f, 1.f);
		DIRMAP[DIR_BACKWARD] = XMFLOAT3(0.f,0.f, -1.f);
		DIRMAP[DIR_RIGHT]    = XMFLOAT3(1.f, 0.f, 0.f);
		DIRMAP[DIR_LEFT]     = XMFLOAT3(-1.f, 0.f,0.f);
		DIRMAP[DIR_FR]       = XMFLOAT3(1.f, 0.f, 1.f);
		DIRMAP[DIR_FL]       = XMFLOAT3(-1.f, 0.f,1.f);
		DIRMAP[DIR_BR]       = XMFLOAT3(1.f, 0.f, -1.f);
		DIRMAP[DIR_BL]       = XMFLOAT3(-1.f, 0.f, -1.f);
	}



}

CProcessor::~CProcessor()
{
	if (DIRMAP.size() > 0) { DIRMAP.clear();}
}

void CProcessor::AddTimer(WORD id, EV_TYPE type, unsigned int startTime, WORD target)
{
	NPCS[id].SetEType(type);
	NPCS[id].SetTickTime(startTime);
	std::unique_lock<shared_mutex> lock(Tml);
	TimerQueue.push(NPC_EVENT{ startTime,type,id,target });
}

bool CProcessor::InMySight(CObject & me, CObject & other, BYTE distance)
{
	return Vector3::Length(Vector3::Subtract(me.GetPOS(), other.GetPOS())) <= distance;
}

void CProcessor::AWakeNPC(WORD id)
{
	if (NPCS[id].GetIsAwake()) return;
	NPCS[id].SetIsAwake(true);

	switch (NPCS[id].GetMType())
	{
	case MONSTER_TYPE::STONEMON:  
		AddTimer(id, NPC_MOVE, GetTickCount()+10); break;

	case MONSTER_TYPE::BEATLEMON:
		AddTimer(id, NPC_MOVE, GetTickCount()+10); break;
	}
}
