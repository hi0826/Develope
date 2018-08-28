#pragma once
#include "../Include.h"
#include "../Object/Object.h"
#include "../Object/User.h"
#include "../Object/NPC.h"
#include "../Util/Timer.h"
class CTCompare
{
public:
	CTCompare() {}
	bool operator() (const NPC_EVENT& L, const NPC_EVENT& R) const {
		return (L.W_Time > R.W_Time);
	}
};

class CProcessor
{
protected:
	static std::array<CUser, MAX_USER> CLIENTS;
	static std::array<CNPC, NUM_OF_NPC - NPC_START> NPCS;
	static tbb::concurrent_priority_queue<NPC_EVENT, CTCompare> EventQueue;

public:
	CProcessor();
	virtual ~CProcessor();

public:
	void  AddEvent(WORD id, EV_TYPE type, const high_resolution_clock::time_point& time) { EventQueue.push(NPC_EVENT{ time,type,id }); };
	bool  IsQueueEmpty() { return EventQueue.empty(); }
	bool  TryPopOfQueue(NPC_EVENT& t) { return EventQueue.try_pop(t); }

public:
	bool InMySight(const XMFLOAT3& me, const XMFLOAT3& other, BYTE distance) { return  Vector3::Length(Vector3::Subtract(me, other)) <= distance; };
	bool IsNPC(WORD id) { return id >= NPC_START; }
	void AWakeNPC(WORD id);

	//public:
	//	void SimulationObejcts(); // thread ÇÔ¼ö 

};

