//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//-----------------------------------------------------------------------------

#include "stdafx.h"
//#include "Timer.h"
//#include "Object.h"
#include "Player.h"
#include "Shader.h"
#include "md5Loader.h"
#include "Input.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CPlayer

CPlayer::CPlayer() : CMoveObject(1)
{
	
}

CPlayer::~CPlayer()
{
	ReleaseShaderVariables();
}



//void CPlayer::Move(DWORD dwDirection, float fDistance, bool bUpdateVelocity)
//{
//	if (dwDirection)
//	{
//		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
//
//		xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fDistance);
//
//		Move(xmf3Shift);
//	}
//}

//void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
//{
//	if (bUpdateVelocity)
//	{
//		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
//	}
//	else
//	{
//		//m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
//		
//		m_pCamera->Move(xmf3Shift);
//	}
//}

void CPlayer::Rotate(float x, float y, float z, DWORD dwDirection)
{
	//XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
	//m_xmf4x4World = Matrix4x4::Multiply(xmmtxRotate, m_xmf4x4World);

	if (dwDirection & DIR_FORWARD) {
		SetRight(XMFLOAT3(1.0f, 0.0f, 0.0f));
		SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
		//m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
		//m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	}
	if (dwDirection & DIR_BACKWARD) {
		SetRight(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		SetLook(XMFLOAT3(0.0f, 0.0f, -1.0f));
		//m_xmf3Right = XMFLOAT3(-1.0f, 0.0f, 0.0f);
		//m_xmf3Look = XMFLOAT3(0.0f, 0.0f, -1.0f);
	}
	if (dwDirection & DIR_RIGHT) {
		SetRight(XMFLOAT3(0.0f, 0.0f, -1.0f));
		SetLook(XMFLOAT3(1.0f, 0.0f, 0.0f));
		//m_xmf3Right = XMFLOAT3(0.0f, 0.0f, -1.0f);
		//m_xmf3Look = XMFLOAT3(1.0f, 0.0f, 0.0f);
	}
	if (dwDirection & DIR_LEFT) {
		SetRight(XMFLOAT3(0.0f, 0.0f, 1.0f));
		SetLook(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		//m_xmf3Right = XMFLOAT3(0.0f, 0.0f, 1.0f);
		//m_xmf3Look = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	}

	if (dwDirection & DIR_FR) {
		SetRight(XMFLOAT3(1.0f, 0.0f, -1.0f));
		SetLook(Vector3::Normalize(XMFLOAT3(1.0f, 0.0f, 1.0f)));
		//m_xmf3Right = XMFLOAT3(1.0f, 0.0f, -1.0f);
		//m_xmf3Look = Vector3::Normalize(XMFLOAT3(1.0f, 0.0f, 1.0f));
	}

	if (dwDirection & DIR_FL) {
		SetRight(XMFLOAT3(1.0f, 0.0f, 1.0f));
		SetLook(Vector3::Normalize(XMFLOAT3(-1.0f, 0.0f, 1.0f)));
		//m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 1.0f);
		//m_xmf3Look = Vector3::Normalize(XMFLOAT3(-1.0f, 0.0f, 1.0f));
	}

	if (dwDirection & DIR_BR) {
		SetRight(XMFLOAT3(-1.0f, 0.0f, -1.0f));
		SetLook(Vector3::Normalize(XMFLOAT3(1.0f, 0.0f, -1.0f)));
		//m_xmf3Right = XMFLOAT3(-1.0f, 0.0f, -1.0f);
		//m_xmf3Look = Vector3::Normalize(XMFLOAT3(1.0f, 0.0f, -1.0f));
	}

	if (dwDirection & DIR_BL) {
		SetRight(XMFLOAT3(-1.0f, 0.0f, 1.0f));
		SetLook(Vector3::Normalize(XMFLOAT3(-1.0f, 0.0f, -1.0f)));
		//m_xmf3Right = XMFLOAT3(-1.0f, 0.0f, 1.0f);
		//m_xmf3Look = Vector3::Normalize(XMFLOAT3(-1.0f, 0.0f, -1.0f));
	}

	SetLook(Vector3::Normalize(GetLook()));
	SetRight(Vector3::CrossProduct(GetUp(), GetLook(), true));
	SetUp(Vector3::CrossProduct(GetLook(), GetRight(), true));
	//m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	//m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	//m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float fTimeElapsed)
{
	if (m_pMesh != NULL) UpdateMD5Model(m_pMesh, m_MD5Model, fTimeElapsed * 1, AniNum);
	ETime = fTimeElapsed;

}

void CPlayer::Input(float fTimeElapsed)
{
	if (m_MD5Model.animations[2].currAnimTime != 0)
	{
		AniNum = 2;
	}
	else
	{
		if (battack)
		{
			AniNum = 0;
			battack = false;
		}

		
	}

	if (AniNum != 2)
	{
		if (KEYDOWN("MRF"))
		{
			imove = 2;
			AniNum = 1;

		}
		else if (KEYDOWN("MLF"))
		{
			imove = 8;
			AniNum = 1;
		}
		else if (KEYDOWN("MRB"))
		{
			imove = 4;
			AniNum = 1;
		}
		else if (KEYDOWN("MLB"))
		{
			imove = 6;
			AniNum = 1;
		}
		else if (KEYDOWN("MF"))
		{
			imove = 1;
			AniNum = 1;
		}
		else if (KEYDOWN("MR"))
		{
			imove = 3;
			AniNum = 1;
		}
		else if (KEYDOWN("MB"))
		{
			imove = 5;
			AniNum = 1;
		}
		else if (KEYDOWN("ML"))
		{
			imove = 7;
			AniNum = 1;
		}
	}
	
	
	switch (imove)
	{

	case 1:
	{
		Rotate(0.f, 0.6f, 0.f, DIR_FORWARD);
		dwDirection = DIR_FORWARD;
		xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fTimeElapsed * 10);
		break;
	}
	case 2:
	{
		Rotate(0.f, 0.6f, 0.f, DIR_FR);
		dwDirection = DIR_FR;
		xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fTimeElapsed * 10);
		break;
	}
	case 3:
	{
		Rotate(0.f, 0.6f, 0.f, DIR_RIGHT);
		dwDirection = DIR_RIGHT;
		xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fTimeElapsed * 10);
		break;
	}
	case 4:
	{
		Rotate(0.f, 0.6f, 0.f, DIR_BR);
		dwDirection = DIR_BR;
		xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fTimeElapsed * 10);
		break;
	}
	case 5:
	{
		Rotate(0.f, 0.6f, 0.f, DIR_BACKWARD);
		dwDirection = DIR_BACKWARD;
		xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fTimeElapsed * 10);
		break;
	}
	case 6:
	{
		Rotate(0.f, 0.6f, 0.f, DIR_BL);
		dwDirection = DIR_BL;
		xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fTimeElapsed * 10);
		break;
	}
	case 7:
	{
		Rotate(0.f, 0.6f, 0.f, DIR_LEFT);
		dwDirection = DIR_LEFT;
		xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fTimeElapsed * 10);
		break;
	}
	case 8:
	{
		Rotate(0.f, 0.6f, 0.f, DIR_FL);
		dwDirection = DIR_FL;
		xmf3Shift = XMFLOAT3(0, 0, 0);
		xmf3Shift = Vector3::Add(xmf3Shift, GetLook(), fTimeElapsed * 10);
		break;
	}
	}
	
	if (KEYUP("MF"))
	{
		if (imove == 1)
		{
			AniNum = 0;
			imove = 0;
		}
		else if (imove == 2)
			imove = 3;
		else if (imove == 8)
			imove = 7;
	}
	if (KEYUP("MR"))
	{
		if (imove == 3)
		{
			AniNum = 0;
			imove = 0;
		}
		else if (imove == 2)
			imove = 1;
		else if (imove == 4)
			imove = 5;
	}
	if (KEYUP("MB"))
	{
		if (imove == 5)
		{
			AniNum = 0;
			imove = 0;
		}
			
		else if (imove == 4)
			imove = 3;
		else if (imove == 6)
			imove = 7;
	}
	if (KEYUP("ML"))
	{
		if (imove == 7)
		{
			imove = 0;
			AniNum = 0;
		}
			
		else if (imove == 6)
			imove = 5;
		else if (imove == 8)
			imove = 1;
	}
	if(imove != 0 && AniNum != 2)
		Move(xmf3Shift);
	
	
	
	if (KEYDOWN("Fire"))
	{
		hp -= 10;
		AniNum = 2;
		battack = true;
		printf("Fire누름");
	}

	
}

//CCamera *CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
//{
//	CCamera *pNewCamera = NULL;
//	switch (nNewCameraMode)
//	{
//	case FIRST_PERSON_CAMERA:
//		pNewCamera = new CFirstPersonCamera(m_pCamera);
//		break;
//	case THIRD_PERSON_CAMERA:
//		pNewCamera = new CThirdPersonCamera(m_pCamera);
//		break;
//	case SPACESHIP_CAMERA:
//		pNewCamera = new CSpaceShipCamera(m_pCamera);
//		break;
//	}
//	
//	if (pNewCamera)
//	{
//		pNewCamera->SetMode(nNewCameraMode);
//		pNewCamera->SetPlayer(this);
//	}
//
//	if (m_pCamera) delete m_pCamera;
//
//	return(pNewCamera);
//}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CAirplanePlayer

CWarrior::CWarrior(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes) : CPlayer()
{
	//m_pCamera = ChangeCamera(THIRD_PERSON_CAMERA, 0.0f);
	//if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

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
}

//CCamera *Warrior::ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed)
//{
//	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
//	if (nCurrentCameraMode == nNewCameraMode) return(m_pCamera);
//	switch (nNewCameraMode)
//	{
//	case FIRST_PERSON_CAMERA:
//		SetFriction(200.0f);
//		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
//		SetMaxVelocityXZ(125.0f);
//		SetMaxVelocityY(400.0f);
//		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
//		m_pCamera->SetTimeLag(0.0f);
//		m_pCamera->SetOffset(XMFLOAT3(0.0f, 200.0f, 0.0f));
//		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
//		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
//		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
//		break;
//	case THIRD_PERSON_CAMERA:
//		SetFriction(250.0f);
//		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
//		SetMaxVelocityXZ(125.0f);
//		SetMaxVelocityY(400.0f);
//		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
//		m_pCamera->SetTimeLag(0.25f);
//		// 쿼터뷰 카메라
//		m_pCamera->SetOffset(XMFLOAT3(0.0f, 30.0f, -30.0f));
//		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
//		m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
//		m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
//		break;
//	default:
//		break;
//	}
//	Update(fTimeElapsed);
//
//	return(m_pCamera);
//}


