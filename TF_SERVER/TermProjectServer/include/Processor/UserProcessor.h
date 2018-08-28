#pragma once
#include "CProcessor.h"
class CUserProcessor :public CProcessor
{
	typedef void(CUserProcessor::*functionPointer)(WORD, char* packet);
	std::unordered_map<char, functionPointer>  Processor;

    


	typedef void(CUserProcessor::*EventfuntionPointer)(WORD);
	std::unordered_map<char, EventfuntionPointer>  EventProcessor;
public:
	CUserProcessor();
	virtual ~CUserProcessor() {};

public:
    bool Initialize();
	void Process(WORD id, int work_size, char* io_ptr);
	void EventProcess(EV_TYPE type, WORD id) { (this->*EventProcessor[type])(id); }
	void Close();

public:
	WORD  GetUsableID();
	void  UserSetting(WORD id, SOCKET sc);
	void  CallRecv(WORD id);
	void  PlayerInit(WORD id);

public:
	void  PlayerLogin(WORD id, char* packet);
	void  PlayerMove(WORD  id, char* packet);
	void  PlayerAttack(WORD id, char* packet);
	void  PlayerCollision(WORD id, char* packet);
	void  PlayerChanageCharacter(WORD id, char* packet);
	void  PlayerRespawn(WORD id);

public:
	void  PlayerDisconnect(WORD id);
	void  UpdateViewList(WORD id);

};

