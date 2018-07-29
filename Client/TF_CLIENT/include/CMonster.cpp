#include "stdafx.h"
#include "CMonster.h"
#include "Shader.h"
#include "md5Loader.h"
CMonster::CMonster() :CMoveObject()
{
}

CMonster::~CMonster()
{
	ReleaseShaderVariables();
}

void CMonster::Update(float fDeltaTime)
{
	if (!m_bActive) return;

	if (m_pMesh != NULL)  UpdateMD5Model(m_pMesh, m_MD5Model, fDeltaTime, AnimState);
	
	if (AnimState == MOVESTATE) {
		Rotate(Direction);
		xmf3Shift = Vector3::Add(XMFLOAT3(0, 0, 0), GetLook(), fDeltaTime * 10);
		CMoveObject::Move(xmf3Shift);
	}
	else if (AnimState == ATTACKSTATE) {
		if ((m_MD5Model.animations[ATTACKSTATE].currAnimTime == 0.f)) AnimState = IDLESTATE;
	}

	m_pOOBB.Center.x = GetWPosition().x;
	m_pOOBB.Center.y = GetWPosition().y;
	m_pOOBB.Center.z = GetWPosition().z;
}

CStoneMon::CStoneMon(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes )
:CMonster()
{
	AnimState = IDLESTATE;
	xmf3Shift = XMFLOAT3(0, 0, 0);

	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, m_pMesh, L"Assets/Model/StonemonMesh.md5mesh", m_MD5Model, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/StonemonIdle.md5anim", m_MD5Model);
	LoadMD5Anim(L"Assets/Model/StonemonWalk.md5anim", m_MD5Model);
	LoadMD5Anim(L"Assets/Model/StoneMonDamaged.md5anim", m_MD5Model);

	SetMesh(0, m_pMesh);
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

	m_pOOBB = BoundingOrientedBox(GetWPosition(), XMFLOAT3(3.0, 6.0, 3.0), XMFLOAT4(0, 0, 0, 1));
}

CStoneMon::~CStoneMon()
{

}

CBeatleMon::CBeatleMon(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes )
:CMonster()
{

}

CBeatleMon::~CBeatleMon()
{

}
