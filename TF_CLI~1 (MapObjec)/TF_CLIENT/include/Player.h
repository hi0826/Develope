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
	std::vector<ModelAnimation> Warrior_animations;
	Model3D		 m_WarriorModel;
	Model3D		 m_FairyModel;

	Model3D		 m_Models[3];
	CMesh*		 m_WarriorMesh;
public:
	int			 m_CurMesh = 0;
	CMaterial*    m_pMaterial1;


public:
	CPlayer(int nMeshes);
	virtual ~CPlayer();

public:
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void ProcessInput(float fDeltaTime);
	virtual void Update(float fDeltaTime);
	virtual void  Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances = 1);
};


