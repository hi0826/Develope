#pragma once
#include "Object.h"
class CMoveObject : public CGameObject
{
protected:
	bool						       moveState = true;
	bool                               AttackState = false;
	int                                AniNum = 0;
	int                                m_AniNum;
	std::vector<ModelAnimation>        m_pAnimations;

public:
	CMoveObject(int nMesh = 1);
	CMoveObject(const CMoveObject& other);

public:
	virtual ~CMoveObject() {}
	virtual void Update(float fTimeElapsed);
	//virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	//virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL) { CGameObject::Render(pd3dCommandList, pCamera); };
	//virtual void Animate(float fTimeElapsed);
public:
	void                              Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity);
	void                              Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);



	void                              MoveStrafe(float fDistance = 1.0f);
	void                              MoveUp(float fDistance = 1.0f);
	void                              MoveForward(float fDistance = 1.0f);
	//void                              SetPosition(const XMFLOAT3& xmf3Position) { m_xmf3Position = xmf3Position; }
};


/*************************************************************/
class CStoneMon : public CMoveObject
{	
public:
	CStoneMon(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		CMesh* pMesh, Model3D model, void *pContext = NULL, int nMeshes = 1);
	virtual ~CStoneMon();
	virtual void Update(float fTimeElapsed);
};