#pragma once
#include "../Util/SingleTone.h"
#include "CProcessor.h"


class CNPCProcessor :public CProcessor
{   
	typedef void(CNPCProcessor::*functionPointer)(WORD, WORD);
	std::unordered_map<char, functionPointer>    Processor;
public:
	CNPCProcessor() {};
	virtual ~CNPCProcessor() {};

public:
	bool Initialize();
	void Close();
	void  Process(EV_TYPE type, WORD id, WORD target = -1) { (this->*Processor[type])(id, target); }

public:
	void  MoveNPC(WORD id,   WORD target);
	void  AttackNPC(WORD id, WORD target);
	void  TraceNPC(WORD id,  WORD target);


};

