#pragma once
#include "../Include.h"
#include "../Object/User.h"
#include "../Object/NPC.h"

class CCompare
{
public:
	CCompare() {}
	bool operator() (const NPC_EVENT& L, const NPC_EVENT& R) const {
		return (L.W_Time > R.W_Time);
	}
};

class CProcessor 
{
	typedef priority_queue<NPC_EVENT, vector<NPC_EVENT>, CCompare> TimerSys;
protected:
	static std::array<CUser,MAX_USER> CLIENTS;
	static std::array<CNPC, NUM_OF_NPC> NPCS;
	static std::unordered_map<WORD, XMFLOAT3> DIRMAP;

protected:
	TimerSys           TimerQueue;
	std::shared_mutex  Tml;

public:
	CProcessor();
	virtual ~CProcessor();

public:
	void  AddTimer(WORD id, EV_TYPE type, unsigned int startTime, WORD target = -1);
	bool  IsQueueEmpty() { return TimerQueue.empty(); }
	const NPC_EVENT& TopOfQueue() { std::shared_lock<shared_mutex> lock(Tml); return TimerQueue.top(); }
	void  Pop() { std::unique_lock<shared_mutex> lock(Tml); TimerQueue.pop(); }


public:
	bool IsNPC(WORD id){return  id >= NPC_START;}
	bool InMySight(CObject& me, CObject& other, BYTE distance);

public:
	void  AWakeNPC(WORD id);

};

