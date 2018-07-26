#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "md5Loader.h"
#include "Input.h"
#include "../include/NetWork/NetWorkManager.h"


CPlayer::CPlayer() : CMoveObject()
{
	hp = 100;

}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
}

void CPlayer::Update(float fDeltaTime)
{
	if (!m_bActive) return;
	if (m_pMesh != NULL)  UpdateMD5Model(m_pMesh, m_MD5Model, fDeltaTime, AnimState);

	if (AnimState == MOVESTATE) {
		Rotate(Direction);
		xmf3Shift = Vector3::Add(XMFLOAT3(0, 0, 0), GetLook(), fDeltaTime * 100);
		CMoveObject::Move(xmf3Shift);
	}
	else if (AnimState == ATTACKSTATE) {
		if ((m_MD5Model.animations[ATTACKSTATE].currAnimTime == 0.f)) AnimState = IDLESTATE;
	}

}


CWarrior::CWarrior(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,int nMeshes ) 
:CPlayer()
{
	AnimState = IDLESTATE;
	xmf3Shift = XMFLOAT3(0, 0, 0);
	CheckTime = 0;
    PrevDirection = 127;
    ISMOVE = false; ISIDLE = true;

	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1; //이거 왜있는겨 아무것도아닌거같은데? 
	LoadMD5Model(pd3dDevice, pd3dCommandList, m_pMesh, L"Assets/Model/WarriorMesh.md5mesh", m_MD5Model, shaderResourceViewArray1, texFileNameArray1, 1, 1, 1);
	LoadMD5Anim(L"Assets/Model/WarriorIdle.md5anim", m_MD5Model);
	LoadMD5Anim(L"Assets/Model/WarriorWalk.md5anim", m_MD5Model);
	LoadMD5Anim(L"Assets/Model/WarriorAttack.md5anim", m_MD5Model);
	SetMesh(0, m_pMesh);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();

	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("Assets/Model/warrior.dds"), 0);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
}

CWarrior::~CWarrior()
{
	delete m_pMesh;
}


void CWarrior::ProcessInput(float fDeltaTime)
{  
	if (!m_bActive) return;
	if (AnimState != ATTACKSTATE)
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

		if (ISMOVE)
		{
			AnimState = MOVESTATE; 
			ISMOVE = false;
			
			if (PrevDirection != Direction)
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
			AnimState = IDLESTATE; 
			ISIDLE = true; 
			CheckTime = 0.f;
			PrevDirection = 127;
			CNetWorkManager::GET_SINGLE()->SendMovePacket(*this);
		}
	}
	if (KEYPRESS("Fire")) {
		AnimState = ATTACKSTATE;
	}
}

