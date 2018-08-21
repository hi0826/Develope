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
	static priority_queue<NPC_EVENT, vector<NPC_EVENT>, CTCompare> EventQueue;

protected:
	std::shared_mutex  EMl;
	CGameTimer FrameTimer;

public:
	CProcessor();
	virtual ~CProcessor();

public:
	void  AddEvent(WORD id, EV_TYPE type, high_resolution_clock::time_point time);
	void  AddUserEvent(WORD id, EV_TYPE type, high_resolution_clock::time_point time);
	bool  IsQueueEmpty() { return EventQueue.empty(); }
	const NPC_EVENT& TopOfQueue() { std::shared_lock<shared_mutex> lock(EMl); return EventQueue.top(); }
	void  Pop() { std::unique_lock<shared_mutex> lock(EMl); EventQueue.pop(); }

public:
	bool InMySight(const XMFLOAT3& me, const XMFLOAT3& other, BYTE distance);
	bool IsNPC(WORD id) { return  id >= NPC_START; }
	void AWakeNPC(WORD id);

public:
	void SimulationObejcts(); // thread ÇÔ¼ö 

};

