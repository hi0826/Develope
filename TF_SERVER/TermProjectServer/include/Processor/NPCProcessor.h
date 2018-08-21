#pragma once
#include "CProcessor.h"
#include "../Include.h"

class CNPCProcessor :public CProcessor
{   
	typedef void(CNPCProcessor::*functionPointer)(WORD);
	std::unordered_map<char, functionPointer>  Processor;
public:
	CNPCProcessor();
	virtual ~CNPCProcessor() {};

public:
	bool Initialize();
	void Close();
	void Process(EV_TYPE type, WORD id) { (this->*Processor[type])(id); }

public:
	void  IdleNPC(WORD id);
	void  MoveNPC(WORD id);
	void  AttackNPC(WORD id);
	void  DeadNPC(WORD id);

public:
	void BroadCastingNPCState(WORD index);

};

