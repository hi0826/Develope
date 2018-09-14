#pragma once
#include "../Include.h"
#include "../Sector/Sector.h"
class CSector;
class CObject
{
	static const int ObjectLockNum = 10;
	std::shared_mutex ObjectLock[ObjectLockNum];
private:
	OBJECT_DATA           Data; 
	BoundingOrientedBox*  OOBB;
	BYTE                 Stage;
	BOOL               IsAlive;
	high_resolution_clock::time_point  ExecuteTime;

public:
	std::mutex                Vlm;
	std::unordered_set<WORD>  VL;

public:
	CObject() {}
	virtual ~CObject() { delete OOBB; };

public:
	void  SetID(WORD id)                  { std::unique_lock<shared_mutex> lock(ObjectLock[0]); Data.ID = id; }
	void  SetHP(float  hp)                { std::unique_lock<shared_mutex> lock(ObjectLock[1]); Data.HP = hp; }
	void  SetMP(float  mp)                { std::unique_lock<shared_mutex> lock(ObjectLock[2]); Data.MP = mp; }
	void  SetAtk(float atk)               { std::unique_lock<shared_mutex> lock(ObjectLock[3]); Data.ATK = atk; }
	void  SetPos(const XMFLOAT3& pos)     { std::unique_lock<shared_mutex> lock(ObjectLock[4]); Data.POS = pos;}
	void  SetDir(WORD look)               { std::unique_lock<shared_mutex> lock(ObjectLock[5]); Data.DIR = look; }
	void  SetState(BYTE state)            { std::unique_lock<shared_mutex> lock(ObjectLock[6]); Data.STATE = state; }
	void  SetStage(const BYTE stage)      { std::unique_lock<shared_mutex> lock(ObjectLock[7]); Stage =stage; }
	void  SetIsAlive(bool check)          { std::unique_lock<shared_mutex> lock(ObjectLock[8]); IsAlive = check; }
	void  SetOOBB(const XMFLOAT3& pos)    { std::unique_lock<shared_mutex> lock(ObjectLock[9]); OOBB->Center = pos; }
public:
	 const WORD      GetID()             { std::shared_lock<shared_mutex> lock(ObjectLock[0]); return Data.ID; }
	 const float     GetHP()             { std::shared_lock<shared_mutex> lock(ObjectLock[1]); return Data.HP; }
	 const float     GetMP()             { std::shared_lock<shared_mutex> lock(ObjectLock[2]); return Data.MP; }
	 const float     GetAtk()            { std::shared_lock<shared_mutex> lock(ObjectLock[3]); return Data.ATK; }
	 const XMFLOAT3& GetPos()            { std::shared_lock<shared_mutex> lock(ObjectLock[4]); return Data.POS; }
 	 const WORD      GetDir()            { std::shared_lock<shared_mutex> lock(ObjectLock[5]); return Data.DIR; }
	 const BYTE      GetState()          { std::shared_lock<shared_mutex> lock(ObjectLock[6]); return Data.STATE; }
	 const BYTE      GetStage()          { std::shared_lock<shared_mutex> lock(ObjectLock[7]); return Stage; }
	 const bool      GetIsAlive()        { std::shared_lock<shared_mutex> lock(ObjectLock[8]); return IsAlive; }
	 const BoundingOrientedBox* GetOOBB(){ std::shared_lock<shared_mutex> lock(ObjectLock[9]); return OOBB; }

public:
	void   VlLock() { Vlm.lock(); }
	void   VlUnlock() { Vlm.unlock(); }
public:
	void   Initialize();
	void   Close();
	bool   CheckCollision( BoundingOrientedBox& other);

};

