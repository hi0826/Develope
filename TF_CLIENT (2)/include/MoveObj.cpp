#include "stdafx.h"
#include "Shader.h"
#include "MoveObj.h"


CMoveObject::CMoveObject(int nMesh) : CGameObject(nMesh)
{

	m_AniNum = 0;
}

CMoveObject::CMoveObject(const CMoveObject& other)
{    
	m_pMesh = other.m_pMesh;
	m_MD5Model = other.m_MD5Model;
}

void CMoveObject::Update(float fTimeElapsed)
{

}

void CMoveObject::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);

		XMFLOAT3 xmf3Look = GetLook();
		if (dwDirection & DIR_FR)        xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_FL)        xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_BR)        xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_BL)        xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
																			 
		if (dwDirection & DIR_FORWARD)   xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD)  xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_RIGHT)     xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_LEFT)      xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CMoveObject::Move(const XMFLOAT3 & xmf3Shift, bool bVelocity)
{
	
	SetWPosition(Vector3::Add(GetWPosition(), xmf3Shift));
		//m_pCamera->Move(xmf3Shift);
	
}


void CMoveObject::MoveStrafe(float fDistance)
{
	XMFLOAT3	xmf3Position = Vector3::Add(GetWPosition(), GetRight(), fDistance);
	CGameObject::SetWPosition(xmf3Position);
}

void CMoveObject::MoveUp(float fDistance)
{
	XMFLOAT3	xmf3Position = Vector3::Add(GetWPosition(), GetUp(), fDistance);
	CGameObject::SetWPosition(xmf3Position);
}

void CMoveObject::MoveForward(float fDistance)
{
	XMFLOAT3	xmf3Position = Vector3::Add(GetWPosition(), GetLook(), fDistance);
	CGameObject::SetWPosition(xmf3Position);
}




CStoneMon::CStoneMon(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, CMesh* Mesh, Model3D Model, void * pContext, int nMeshes)
	: CMoveObject()
{
	//pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature
	//std::vector<ID3D12Resource*> shaderResourceViewArray1;
	//std::vector<std::wstring> texFileNameArray1;
	//LoadMD5Model(pd3dDevice, pd3dCommandList, pMesh, L"StonemonMesh0508.md5mesh", NewMD5Model1, shaderResourceViewArray1,  , 1, 1, 1);
	//CMesh*                              m_pMesh = NULL;
	//m_MD5Model1 = Mesh->NewMD5Model1;
	//LoadMD5Anim(L"StonemonIdle0508.md5anim", m_pStonModel); 
	//LoadMD5Anim(L"StonemonWalk0508.md5anim", m_pStonModel);

	m_pMesh = Mesh;
	m_MD5Model = Model;
	m_pAnimations = Model.animations;

	SetMesh(0, m_pMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	m_pMaterial = new CMaterial();
	m_pMaterial->AddRef();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("stonemon.dds"), 0);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
}

CStoneMon::~CStoneMon()
{
}

void CStoneMon::Update(float fTimeElapsed)
{
	
}
