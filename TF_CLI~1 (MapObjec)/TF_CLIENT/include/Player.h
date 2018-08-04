#pragma once

#include "MoveObj.h"
<<<<<<< HEAD
=======

>>>>>>> 1b444bfd5c9d5d477b5c55d7bd8f583a66b25add
class CPlayer : public CMoveObject
{
	const float  MoveTime = 1.0f;
	const float  AttackTime = 0.5f;

	float        CheckTime;
	BYTE         PrevDirection;
	bool         ISMOVE;
	bool         ISIDLE;
	int          Check = 0;
<<<<<<< HEAD
=======
	std::vector<ModelAnimation> Warrior_animations;
	Model3D		 m_WarriorModel;
	CMesh*		 m_WarriorMesh;

>>>>>>> 1b444bfd5c9d5d477b5c55d7bd8f583a66b25add

public:
	CPlayer();
	virtual ~CPlayer();

public:
<<<<<<< HEAD
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
=======
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, INSTANCEOB *tempOB, vector<ModelAnimation> Wani);
>>>>>>> 1b444bfd5c9d5d477b5c55d7bd8f583a66b25add
	virtual void ProcessInput(float fDeltaTime);
	virtual void Update(float fDeltaTime);
};


