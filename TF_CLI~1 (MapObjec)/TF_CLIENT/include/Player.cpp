#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "md5Loader.h"
#include "Input.h"
#include "../include/NetWork/NetWorkManager.h"


CPlayer::CPlayer(int nMeshes) : CMoveObject(nMeshes)
{
	hp        = 100;
	CheckTime = 0;
	PrevDirection = 127;
	ISMOVE = false; ISIDLE = true;
}

CPlayer::~CPlayer()
{
}

bool CPlayer::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature)
{   
	CMesh* tempMesh;
	CMesh* FairyMesh;
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	std::vector<ID3D12Resource*> shaderResourceViewArray2;
	std::vector<std::wstring> texFileNameArray2; //이거 왜있는겨 아무것도아닌거같은데? 
	//m_WarriorModel = tempOB[0].Model;
	LoadMD5Model(pd3dDevice, pd3dCommandList, tempMesh, L"Assets/Model/Movable/Player/Warrior/WarriorMesh.md5mesh", m_Models[0], shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	//m_WarriorModel = tempOB[0].Model;
	//m_WarriorMesh = tempOB[0].Mesh;
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorIdle.md5anim", m_Models[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorWalk.md5anim", m_Models[0]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Warrior/WarriorAttack.md5anim", m_Models[0]);
	
	LoadMD5Model(pd3dDevice, pd3dCommandList, FairyMesh, L"Assets/Model/Movable/Player/Fairy/FairyMesh.md5mesh", m_Models[1] , shaderResourceViewArray2, texFileNameArray2, 1, 1, 1);

	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyIdle.md5anim", m_Models[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyFly.md5anim", m_Models[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyAttack.md5anim", m_Models[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyDamaged.md5anim", m_Models[1]);
	LoadMD5Anim(L"Assets/Model/Movable/Player/Fairy/FairyDead.md5anim", m_Models[1]);

	//Warrior_animations = Wani;
	//tempMesh = tempOB[0].Mesh;
	//m_MD5Model.animations = Warrior_animations;
	SetMesh(0, tempMesh);
	SetMesh(1, FairyMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(2, RESOURCE_TEXTURE2D, 0); 
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Player/Warrior/warrior.dds"), 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/Movable/Player/Fairy/FairyMap.dds"), 1);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 2);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderVariables(pd3dDevice, pd3dCommandList);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);

	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());

	m_pMaterial->SetShader(pShader);
	m_OOBB = BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(3.f, 3.f, 3.f), XMFLOAT4(0, 0, 0, 1));
	return true;
}

void CPlayer::ProcessInput(float fDeltaTime)
{
	if (!m_bActive) return;
	if (m_AnimState != ATTACKSTATE)
	{
		if (KEYPRESS("MF")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_FORWARD);
		}
		if (KEYPRESS("MR")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_RIGHT);
		}
		if (KEYPRESS("MB")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_BACKWARD);
		}
		if (KEYPRESS("ML")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_LEFT);
		}
		if (KEYPRESS("MRF")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_FR);
		}
		if (KEYPRESS("MLF")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_FL);
		}
		if (KEYPRESS("MRB")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_BR);
		}
		if (KEYPRESS("MLB")) {
			ISMOVE = true; ISIDLE = false;
			SetDirection(DIR_BL);
		}
		if (KEYDOWN("F1"))
		{
			m_CurMesh = 0;
		}
		if (KEYDOWN("F2"))
		{
			m_CurMesh = 1;
		}
		if (ISMOVE)
		{
			m_AnimState = MOVESTATE;
			ISMOVE = false;

			if (PrevDirection != m_Direction)
			{
				PrevDirection = GetDirection();
				CheckTime = 0.f;
				CNetWorkManager::GET_SINGLE()->SendMovePacket(*this);
			}

			if (CheckTime < MoveTime) CheckTime += fDeltaTime;
			else
			{
				CheckTime = 0.f;
				CNetWorkManager::GET_SINGLE()->SendMovePacket(*this);
			}
		}
		else if (!ISIDLE)
		{
			m_AnimState = IDLESTATE;
			ISIDLE = true;
			CheckTime = 0.f;
			PrevDirection = 127;
			CNetWorkManager::GET_SINGLE()->SendMovePacket(*this);
			printf("%f , %f , %f\n", this->GetWPosition().x, this->GetWPosition().y, this->GetWPosition().z);
		}
	}
	if (KEYDOWN("Fire")) {
		CNetWorkManager::GET_SINGLE()->SendAttackPacket(*this);
		m_AnimState = ATTACKSTATE;
	}
}

void CPlayer::Update(float fDeltaTime)
{
	if (!m_bActive) return;

	//for (int i = 0; i < m_nMeshes; ++i) 
	if (m_nMeshes > 0)  UpdateMD5Model(m_ppMeshes[m_CurMesh], m_Models[m_CurMesh], fDeltaTime, m_AnimState);
	
	if (m_AnimState == MOVESTATE) {
		Rotate(m_Direction);
		CMoveObject::Move(Vector3::Add(XMFLOAT3(0, 0, 0), GetLook(), fDeltaTime * 10));
	}
	else if (m_AnimState == ATTACKSTATE) {
		if ((m_Models[m_CurMesh].animations[ATTACKSTATE].currAnimTime == 0.f)) m_AnimState = IDLESTATE;
	}

	m_OOBB.Center = GetWPosition();
}

void CPlayer::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera, UINT nInstances)
{
	if (!m_bActive) return;
	//if(IsInCamera(pCamera))
	if (m_pMaterial)
	{
		if (m_pMaterial->m_pShader)
		{
			m_pMaterial->m_pShader->Render(pd3dCommandList);
			m_pMaterial->m_pShader->UpdateShaderVariables(pd3dCommandList);
			UpdateShaderVariables(pd3dCommandList);
		}
		if (m_pMaterial->m_pTexture)
			m_pMaterial->m_pTexture->UpdateShaderVariables(pd3dCommandList, m_CurMesh);
	}
	
	//pd3dCommandList->SetGraphicsRootDescriptorTable(ROOT_PARAMETER_OBJECT, m_d3dCbvGPUDescriptorHandle);
	pd3dCommandList->SetGraphicsRootDescriptorTable(2, m_d3dCbvGPUDescriptorHandle);
	/*if (m_ppMeshes && (m_nMeshes > 0)) {
		for (int i = 0; i < m_nMeshes; i++)
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(pd3dCommandList, nInstances);
	}*/
	if (m_nMeshes > 0) m_ppMeshes[m_CurMesh]->Render(pd3dCommandList, nInstances);
}
