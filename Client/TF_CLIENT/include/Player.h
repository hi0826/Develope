#pragma once

#include "MoveObj.h"
class CPlayer : public CMoveObject
{
protected:
	BoundingOrientedBox           m_pSwordOOBB;
public:
	CPlayer();
	virtual ~CPlayer();

public:
	virtual void Update(float fDeltaTime);
	BoundingOrientedBox				GetSwordOOBB() { return m_pSwordOOBB; }
};



class CWarrior : public CPlayer
{
	const float  MoveTime   = 1.0f;
	const float  AttackTime = 0.5f;

	
private:
	float      CheckTime;
	BYTE       PrevDirection;
	bool       ISMOVE;
	bool       ISIDLE;
	int        Check = 0;
public:
	CWarrior(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList,ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes );
	virtual ~CWarrior();

public:
	virtual void ProcessInput(float fDeltaTime);

	
};



