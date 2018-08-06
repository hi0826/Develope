#include "stdafx.h"
#include "CMonster.h"
#include "Shader.h"
#include "md5Loader.h"

CStoneMon::CStoneMon()

{
	

}

CStoneMon::~CStoneMon()
{

}

bool CStoneMon::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	CMesh* TempMesh;
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempMesh, L"Assets/Model/StoneMonMesh_0805.md5mesh", m_MD5Model, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/StonemonIdle.md5anim", m_MD5Model);
	LoadMD5Anim(L"Assets/Model/StonemonWalk.md5anim", m_MD5Model);
	LoadMD5Anim(L"Assets/Model/StoneMonDamaged.md5anim", m_MD5Model);

	SetMesh(0, TempMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();

	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Stonemon.dds"), 0);

	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	m_AnimState = IDLESTATE;
	return true;
}

void CStoneMon::Update(float fDeltaTime)
{
	if (!m_bActive) return;
	for (int i = 0; i < m_nMeshes;++i) {
	if (m_ppMeshes[i] != NULL)  UpdateMD5Model(m_ppMeshes[i], m_MD5Model, fDeltaTime, m_AnimState);
	}

	if (m_AnimState == MOVESTATE) {
		Rotate(m_Direction);
		CMoveObject::Move(Vector3::Add(XMFLOAT3(0, 0, 0), GetLook(), fDeltaTime * 10));
	}
	else if (m_AnimState == ATTACKSTATE) {
		if ((m_MD5Model.animations[ATTACKSTATE].currAnimTime == 0.f)) m_AnimState = IDLESTATE;
	}

	m_OOBB.Center = GetWPosition();
}

CBeatleMon::CBeatleMon()
{

}

CBeatleMon::~CBeatleMon()
{

}

bool CBeatleMon::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{
	return true;
}

void CBeatleMon::Update(float fDeltaTime)
{
}
