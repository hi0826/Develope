#pragma once
#include "../Util/SingleTone.h"
#include "NPC.h"
class CCompare
{
public:
	CCompare() {}
	bool operator() (const NPC_EVENT& L, const NPC_EVENT& R) const {
		return (L.W_Time > R.W_Time);
	}
};

class CNPCProcessor :public CSingleTone<CNPCProcessor>
{   
	typedef void(CNPCProcessor::*functionPointer)(WORD, char* packet);
	typedef priority_queue<NPC_EVENT, vector<NPC_EVENT>, CCompare> TimerSys;
private:
	std::map<char, functionPointer>    Processor;
	std::array<CNPC, NUM_OF_NPC>       NPCS;
	TimerSys                           TimerQueue;
	mutex                              Tml;
public:
	CNPCProcessor();
	~CNPCProcessor();

public:
	bool Initialize();
	void Close();

public:
	CNPC& GetNPC(WORD x) { return NPCS[x]; }
	bool  GetIsAlive(WORD id) { return NPCS[id].GetIsAlive(); }
	void  AWakeNPC(WORD id);
	void  AddTimer(WORD id, EV_TYPE type, unsigned int startTime, WORD target);
	bool  IsQueueEmpty() { return TimerQueue.empty(); }
	const NPC_EVENT& TopOfQueue() { return TimerQueue.top(); }
	void  Pop() { lock_guard<mutex> m(Tml); TimerQueue.pop(); }

/*몬스터 행동*/ 
public:
   

};

