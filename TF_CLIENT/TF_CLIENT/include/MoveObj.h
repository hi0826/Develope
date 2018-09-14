#pragma once
#include "Object.h"
class CMoveObject : public CGameObject
{
	static const int PlayerLockNum     = 6;
	std::shared_mutex MM[PlayerLockNum];
protected:
	BYTE     m_AnimState;
	WORD     m_Direction;
	BYTE     m_Stage;
	Model3D* m_pModels;
	float    m_hp;
	float    m_mp;
	float    m_atk;
public:
	CMoveObject(int nMesh = 1);
	virtual ~CMoveObject() { if (m_nMeshes > 1) delete[] m_pModels; else delete m_pModels; }

public:
	void      Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void      MoveStrafe(float fDistance = 1.0f);
	void      MoveUp(float fDistance = 1.0f);
	void      MoveForward(float fDistance = 1.0f);
	void      Rotate(WORD direction);

public:
	void  SetAnimState(BYTE state)     { std::unique_lock<shared_mutex> lock(MM[0]); m_AnimState =  state; }
	void  SetDirection(WORD direction) { std::unique_lock<shared_mutex> lock(MM[1]); m_Direction = direction; }
	void  SetStage(const BYTE stage)   { std::unique_lock<shared_mutex> lock(MM[2]); m_Stage = stage; }
	void  SetHp(const float& HP)    { std::unique_lock<shared_mutex> lock(MM[3]); m_hp = HP; }
	void  SetMp(const float& MP)    { std::unique_lock<shared_mutex> lock(MM[4]); m_mp = MP; }
	void  SetAtk(const float& Atk)  { std::unique_lock<shared_mutex> lock(MM[5]); m_atk = Atk; }

public:
	const WORD  GetDirection() { std::shared_lock<shared_mutex> lock(MM[0]); return m_Direction; }
	const BYTE  GetAnimState() { std::shared_lock<shared_mutex> lock(MM[1]); return m_AnimState; }
	const BYTE  GetStage()     { std::shared_lock<shared_mutex> lock(MM[2]); return m_Stage; }
	float       GetHp()        { std::shared_lock<shared_mutex> lock(MM[3]); return m_hp; }
	float       GetMp()        { std::shared_lock<shared_mutex> lock(MM[4]); return m_hp; }
	float       GetAtk()       { std::shared_lock<shared_mutex> lock(MM[5]); return m_atk; }
	Model3D* GetCurModel() { return m_pModels; }
};
