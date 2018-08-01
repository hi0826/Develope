#pragma once
#include "../Util/SingleTone.h"
#include "CProcessor.h"
class CUserProcessor :public CProcessor
{
	typedef void(CUserProcessor::*functionPointer)(WORD, char* packet);
	std::unordered_map<char, functionPointer>  Processor;
	class CNPCProcessor* NPCM;

public:
	CUserProcessor();
	virtual ~CUserProcessor() {};

public:
    bool Initialize();
	void Process(WORD id, int work_size, char* io_ptr);
	void Close();

public:
	WORD  GetUsableID();
	void  UserSetting(WORD id, SOCKET sc);
	void  CallRecv(WORD id);
	void  Test(WORD id);

public:
	void  PlayerLogin(WORD id, char* packet);
	void  PlayerMove(WORD  id, char* packet);
	void  PlayerAttack(WORD id, char* packet);

public:
	void  PlayerDisconnect(WORD id);
	void  UpdateViewList(WORD id);


};

