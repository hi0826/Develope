#pragma once
#include "Object.h"
class CMoveObject : public CGameObject
{
	static const int LOCKNUM     = 2;
protected:
	BYTE                         m_AnimState;
	WORD                         m_Direction;
	std::shared_mutex            MM[LOCKNUM];
	Model3D                      m_MD5Model;
	
	unsigned                     hp;

public:
	CMoveObject(int nMesh = 1);
	virtual ~CMoveObject() {}

public:
	void      Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void      MoveStrafe(float fDistance = 1.0f);
	void      MoveUp(float fDistance = 1.0f);
	void      MoveForward(float fDistance = 1.0f);
	void      Rotate(WORD direction);

public:
	void  SetAnimState(BYTE state)     { std::unique_lock<shared_mutex> lock(MM[0]); m_AnimState =  state; }
	void  SetDirection(WORD direction) { std::unique_lock<shared_mutex> lock(MM[1]); m_Direction = direction; }
	void  SetHp(unsigned HP) { hp = HP; }

public:
	const WORD  GetDirection() { std::shared_lock<shared_mutex> lock(MM[0]); return m_Direction; }
	const BYTE  GetAnimState() { std::shared_lock<shared_mutex> lock(MM[1]); return m_AnimState; }
	unsigned    GetHp()        { return hp; }

};
