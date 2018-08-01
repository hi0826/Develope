#pragma once
#include "../Include.h"
#include "../Sector/Sector.h"
class CSector;
class CObject
{
	static const int MN = 10;
private:
	OBJECT_DATA    Data; 
	XMFLOAT3       LookVector;
    CSector*       MySector; 
	BoundingOrientedBox OOBB;
	std::shared_mutex M[MN];

public:
	CObject() {}
	virtual ~CObject() {};
public:
	void     SetID(WORD id)              { std::unique_lock<shared_mutex> lock(M[0]); Data.ID = id; }
	void     SetLEV(unsigned char lev)   { std::unique_lock<shared_mutex> lock(M[1]); Data.LEV = lev; }
	void     SetEXP(unsigned exp)        { std::unique_lock<shared_mutex> lock(M[2]); Data.EXP = exp; }
	void     SetHP(unsigned  hp)         { std::unique_lock<shared_mutex> lock(M[3]); Data.HP= hp; }
	void     SetMP(unsigned  mp)         { std::unique_lock<shared_mutex> lock(M[4]); Data.MP = mp; }
	void     SetATK(unsigned atk)        { std::unique_lock<shared_mutex> lock(M[5]); Data.ATK = atk; }
	void     SetPOS(const XMFLOAT3& pos) { std::unique_lock<shared_mutex> lock(M[6]); Data.POS = pos; OOBB.Center = pos;}
	void     SetDIR(WORD look)           { std::unique_lock<shared_mutex> lock(M[7]); Data.DIR = look; }
	void     SetSTATE(BYTE state)        { std::unique_lock<shared_mutex> lock(M[8]); Data.STATE = state; }
	void     SetMySector(CSector* sector){ std::unique_lock<shared_mutex> lock(M[9]); MySector = sector; }
public:
	 const WORD          GetID()        {std::shared_lock<shared_mutex> lock(M[0]); return Data.ID; }
     const unsigned char GetLEV()       {std::shared_lock<shared_mutex> lock(M[1]); return Data.LEV; }
	 const unsigned      GetEXP()       {std::shared_lock<shared_mutex> lock(M[2]); return Data.EXP; }
	 const unsigned      GetHP()        {std::shared_lock<shared_mutex> lock(M[3]); return Data.HP; }
	 const unsigned      GetMP()        {std::shared_lock<shared_mutex> lock(M[4]); return Data.MP; }
	 const unsigned      GetATK()       {std::shared_lock<shared_mutex> lock(M[5]); return Data.ATK; }
	 const XMFLOAT3&     GetPOS()       {std::shared_lock<shared_mutex> lock(M[6]); return Data.POS; }
	 const WORD          GetDIR()       {std::shared_lock<shared_mutex> lock(M[7]); return Data.DIR; }
	 const BYTE          GetSTATE()     {std::shared_lock<shared_mutex> lock(M[8]); return Data.STATE; }
	 const CSector*      GetMySector()  {std::shared_lock<shared_mutex> lock(M[9]); return MySector; }

public:
	void   Initialize();
	void   Close();
	bool   CeckCollision(const BoundingOrientedBox& other);

};

