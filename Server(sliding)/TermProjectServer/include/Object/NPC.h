#pragma once
#include "Object.h"
class CNPC :public CObject
{
private:
	bool           IsAwake;
	bool           IsAlive;
	int            TickTime; 
	MONSTER_TYPE   MType;
	EV_TYPE        EType;
	WORD           Enemy;
	int            MoveTime; /*움직이는 시간 (한방향으로 몇초간 움직인다를 정해줘야 하니*/

public:
	CNPC() {};
	virtual ~CNPC() {};

public:
	void Initialize();
	void Close();

public:
	void  SetIsAlive(bool check)      { IsAlive = check; }
	void  SetIsAwake(bool check)      { IsAwake = check; }
	void  SetTickTime(int time)       { TickTime = time; }
	void  SetMoveTime(int time)       { MoveTime = time; }
	void  SetMType(MONSTER_TYPE type) { MType = type; }
	void  SetEType(EV_TYPE type)      { EType = type; }
	void  SetEnemy(WORD enemy)        { Enemy = enemy; }

public:
	bool  GetIsAlive() const          { return IsAlive; }
	bool  GetIsAwake() const          { return IsAwake; }
	int   GetTickTime() const         { return TickTime; }
	int   GetMoveTime() const         { return MoveTime; }
	WORD    GetEnemy() const           { return Enemy; }
	EV_TYPE GetEType() const          { return EType; }
	MONSTER_TYPE  GetMType() const    { return MType; }

};

