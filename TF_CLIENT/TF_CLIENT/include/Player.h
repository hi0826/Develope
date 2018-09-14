#pragma once

#include "MoveObj.h"

class CPlayer : public CMoveObject
{
	const float  MoveTime     = 1.0f;
	const float  AttackTime   = 0.5f;
	const BYTE   CharacterNum = 2; 

private:
	//Model3D		 m_pModels[3];
	BYTE         PrevDirection;
	BYTE	     CurMeshNum;
	float        CheckTime;
	bool         ISMOVE;
	bool         ISIDLE;
	int          Speed;
	std::shared_mutex MeshNumLock;

public:
	CPlayer(int nMeshes);
	virtual ~CPlayer();

public:
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void ProcessInput(float fDeltaTime);
	virtual void Update(float fDeltaTime);
	virtual void  Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances = 1);

public: 
	const BYTE GetCurMeshNum()       { std::shared_lock<shared_mutex> lock(MeshNumLock); return CurMeshNum; }
	void SetCurMeshNum(BYTE curMesh) { std::unique_lock<shared_mutex> lock(MeshNumLock); CurMeshNum = curMesh; }
	
	void SetSpeed(int speed) { Speed = speed; }
	int  GetSpeed() { return Speed;  }

};


