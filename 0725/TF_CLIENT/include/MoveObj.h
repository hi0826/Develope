#pragma once
#include "Object.h"
class CMoveObject : public CGameObject
{
	static const int LOCKNUM     = 2;
protected:
	XMFLOAT3                     xmf3Shift;
	BYTE                         AnimState;
	WORD                         Direction;
	std::shared_mutex            MM[LOCKNUM];
	unsigned                     hp;

public:
	CMoveObject(int nMesh = 1);
	CMoveObject(const CMoveObject& other);

public:
	virtual ~CMoveObject() {}
	virtual void Update(float fTimeElapsed);
public:
	void  Move(WORD dwDirection, float fDistance, bool bUpdateVelocity);
	void  Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void  MoveStrafe(float fDistance = 1.0f);
	void  MoveUp(float fDistance = 1.0f);
	void  MoveForward(float fDistance = 1.0f);
	void  Rotate(WORD direction);
	unsigned  GetHp() { return hp; }
	void  SetHp(unsigned HP) { hp = HP; }

public:
	void  SetAnimState(BYTE state)     { std::unique_lock<shared_mutex> lock(MM[0]); AnimState =  state; }
	void  SetDirection(WORD direction) { std::unique_lock<shared_mutex> lock(MM[1]); Direction = direction; }
public:
	const WORD  GetDirection() { std::shared_lock<shared_mutex> lock(MM[0]); return Direction; }
	const BYTE  GetAnimState() { std::shared_lock<shared_mutex> lock(MM[1]); return AnimState; }

};
