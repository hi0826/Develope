#pragma once
#include "Object.h"
class CNPC :public CObject
{
	static const int  NPCLockNum = 4;

private:
	bool              IsAwake;
	MONSTER_TYPE      MType;
	EV_TYPE           EType;
	WORD              Target;
	BYTE              Speed;
	std::shared_mutex NPCLock[NPCLockNum];

public:
	CNPC();
	virtual ~CNPC() {};

public:
	void Initialize();
	void Close();

public:
	void  SetIsAwake(bool check)      { std::unique_lock<shared_mutex> lock(NPCLock[0]); IsAwake = check; }
	void  SetTarget(WORD target)      { std::unique_lock<shared_mutex> lock(NPCLock[1]); Target = target; }
	void  SetEType(EV_TYPE type)      { std::unique_lock<shared_mutex> lock(NPCLock[2]); EType = type; }
	void  SetMType(MONSTER_TYPE type) { std::unique_lock<shared_mutex> lock(NPCLock[3]); MType = type; }
	void  SetSpeed(BYTE speed)        { Speed = speed; }

public:	
	const bool          GetIsAwake()  { std::shared_lock<shared_mutex> lock(NPCLock[0]); return IsAwake; }
	const WORD          GetTarget()   { std::shared_lock<shared_mutex> lock(NPCLock[1]); return Target; }
	const EV_TYPE       GetEType()    { std::shared_lock<shared_mutex> lock(NPCLock[2]); return EType; }
	const MONSTER_TYPE  GetMType()    { std::shared_lock<shared_mutex> lock(NPCLock[3]); return MType; }
	const BYTE GetSpeed()             { return Speed; }

public: 
	void LookAtTarget(const XMFLOAT3& otherpos);
	void ColliderCheck();
	void CollisionMove(BYTE DIR, float fDeltaTime);
};

