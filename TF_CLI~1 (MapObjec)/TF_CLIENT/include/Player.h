#pragma once

#include "MoveObj.h"
class CPlayer : public CMoveObject
{
	const float  MoveTime = 1.0f;
	const float  AttackTime = 0.5f;

	float        CheckTime;
	BYTE         PrevDirection;
	bool         ISMOVE;
	bool         ISIDLE;
	int          Check = 0;

public:
	CPlayer();
	virtual ~CPlayer();

public:
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void ProcessInput(float fDeltaTime);
	virtual void Update(float fDeltaTime);
};


