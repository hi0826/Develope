#pragma once
#include "CProcessor.h"
#include "../Include.h"
#include "../Util/Timer.h"
class CNPCProcessor :public CProcessor
{   
	typedef void(CNPCProcessor::*functionPointer)(WORD);
	std::unordered_map<BYTE, functionPointer>  Processor;

private:
	CGameTimer FrameTimer;

public:
	CNPCProcessor();
	virtual ~CNPCProcessor() {};

public:
	bool Initialize();
	void Close();
	
	void EventProcess(EV_TYPE type, WORD id) { (this->*Processor[type])(id); }
	void SimulationNPC();

private:
	void  IdleNPC(WORD id);
	void  MoveNPC(WORD id);
	void  AttackNPC(WORD id);
	void  DeadNPC(WORD id);

public:
	void BroadCastingNPCState(WORD index);
};

