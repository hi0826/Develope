#include "stdafx.h"
#include "DayForestScene.h"
#include "Player.h"
#include "CMonster.h"
#include "Input.h"

DayForestScene::DayForestScene()
{
	Processor[SC_PUT_PLAYER]          = &CScene::PutPlayer;
	Processor[SC_REMOVE_PLAYER]       = &CScene::RemovePlayer;
	Processor[SC_MOVE_PLAYER]         = &CScene::PlayerMove;
	Processor[SC_PLAYER_ATTACK_START] = &CScene::PlayerAttackStart;
	Processor[SC_PLAYER_ATTACK_STOP]  = &CScene::PlayerAttackStop;
	Processor[SC_PLAYER_DIE]          = &CScene::PlayerDie;


	Processor[SC_PUT_NPC]             = &DayForestScene::PutNPC;
	Processor[SC_REMOVE_NPC]          = &DayForestScene::RemoveNPC;
	Processor[SC_MOVE_NPC]            = &DayForestScene::MoveNPC;

	
}

DayForestScene::~DayForestScene()
{
	if (m_UiShader)
		m_UiShader->ReleaseObjects();

	if (m_UiShader)
		delete m_UiShader;
}

void DayForestScene::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(pd3dDevice);

	// Map

	ReadMap();
	CalculatingGameObject();

	m_PosTree = new XMFLOAT3[m_nTrees];
	SetTreePos();

	m_pMap = new CPlaneMap(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, width, height, 513, 513);

	//XMFLOAT3 xmf3Scale(1.0f, 1.0f, 1.0f);
	//XMFLOAT4 xmf4Color(0.0f, 0.5f, 0.0f, 0.0f);

	/*Terrain*/
	//m_pTerrain = new CHeightMapTerrain(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,
	//	_T("Assets/Image/Terrain/Stage01.raw"), 513, 513, 513, 513, xmf3Scale, xmf4Color);

	/*SkyBox*/
	//m_pSkyBox = new CSkyBox(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	m_pTrees = new CGameObject*[m_nTrees];
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1;
	LoadMD5Model(pd3dDevice, pd3dCommandList, TreeMesh, L"Assets/Model/Trees.md5mesh", TreeModel, shaderResourceViewArray1, texFileNameArray1, 0.15, 0.15, 0.15);

	for (int i = 0; i < m_nTrees; ++i) {

		m_pTrees[i] = new CTree(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, TreeMesh, TreeModel);
		m_pTrees[i]->SetWPosition(XMFLOAT3(m_PosTree[i].x * 20, 10, m_PosTree[i].z * 20));
		m_pTrees[i]->SetScale(1, 1, 1);
		m_pTrees[i]->Rotate(0, 0, 0);
	}

	if (m_pPlayerVector.size() == 0) { 
		for (int i = 0; i < MAX_USER; ++i) {
			m_pPlayerVector.push_back(new CWarrior(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature,1));
			m_pPlayerVector[i]->SetActive(false);
		}
	}
	if (!m_pPlayer) {
		m_pPlayer = new CWarrior(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 1);
		m_pPlayer->SetActive(false);

	}


	for (int i = 0; i < 2; ++i) {
		STONEMONS[i] = new CStoneMon(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, 1);
		STONEMONS[i]->SetWPosition(10.f*i, 0.f, 0.f);
		STONEMONS[i]->Rotate(DIR_BACKWARD);
		STONEMONS[i]->SetAnimState(IDLESTATE);
	}

	BuildLightsAndMaterials();

	m_pCamera = GenerateCamera(THIRD_PERSON_CAMERA, m_pPlayer);
	m_pCamera->SetTimeLag(0.f);
	m_pCamera->SetOffset(XMFLOAT3(0.0f, 30.0f, -40.0f));
	m_pCamera->GenerateProjectionMatrix(1.0f, 5000.0f, ASPECT_RATIO, 60.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_UiShader = new UIShader(m_pPlayer);
	m_UiShader->CreateGraphicsRootSignature(pd3dDevice);
	m_UiShader->CreateShader(pd3dDevice, 1);
	m_UiShader->BuildObjects(pd3dDevice, pd3dCommandList);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void DayForestScene::ProcessInput(float fDeltaTime)
{   
	if(m_pPlayer) m_pPlayer->ProcessInput(fDeltaTime);
}

void DayForestScene::Update(float fDeltaTime)
{

	for (int i = 0; i < MAX_USER; ++i) {
		if(m_pPlayerVector[i])
		m_pPlayerVector[i]->Update(fDeltaTime);
	}
	if (m_pPlayer) m_pPlayer->Update(fDeltaTime);
	
	for (int i = 0; i < 2; ++i) {
		if(STONEMONS[i])
		STONEMONS[i]->Update(fDeltaTime);
	}

	m_pCamera->Update(fDeltaTime);
	m_pCamera->RegenerateViewMatrix();

	XMFLOAT3 PlayerLook;
	XMFLOAT3 PlayerPosition;
	XMFLOAT3 xmf3Shift = XMFLOAT3(0.f, 0.f, 0.f);
	for (int i = 0; i < 2; ++i) {
		ContainmentType containType = STONEMONS[i]->GetOOBB().Contains(m_pPlayer->GetOOBB());
		
		switch (containType)
		{
		case DISJOINT:
			break;
		case INTERSECTS:
			// 1 왼 2 오른 3 위 4 아래 5 산
			printf("충돌");
			//printf("OOBB\tx: %.2f, y: %.2f, z: %.2f \n", m_pPlayer->m_pOOBB.Center.x, m_pPlayer->m_pOOBB.Center.y, m_pPlayer->m_pOOBB.Center.z);
			PlayerLook = m_pPlayer->GetLook();
			PlayerLook = Vector3::ScalarProduct(PlayerLook, -1.0);

			PlayerPosition = m_pPlayer->GetWPosition();
			xmf3Shift = Vector3::Add(xmf3Shift, PlayerLook, 3.0f);
			m_pPlayer->Move(xmf3Shift, true);
			PlayerPosition = m_pPlayer->GetWPosition();
			break;
		}
	}
	
	if (m_pPlayer->GetAnimState() == ATTACKSTATE)
	{
		for (int i = 0; i < 2; ++i) {
			ContainmentType containType = STONEMONS[i]->GetOOBB().Contains(m_pPlayer->GetSwordOOBB());

			switch (containType)
			{
			case DISJOINT:
				break;
			case INTERSECTS:
				// 1 왼 2 오른 3 위 4 아래 5 산
				printf("공격");
				STONEMONS[i]->SetAnimState(ATTACKSTATE);	
				break;
			}
		}
	}
	
}

void DayForestScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);
	m_pCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pCamera->UpdateShaderVariables(pd3dCommandList);
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
	::memcpy(m_pcbMappedMaterials, m_pMaterials, sizeof(MATERIALS));



	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbMaterialsGpuVirtualAddress = m_pd3dcbMaterials->GetGPUVirtualAddress();
	pd3dCommandList->SetGraphicsRootConstantBufferView(3, d3dcbMaterialsGpuVirtualAddress); //Materials
	//m_pSkyBox->Render(pd3dCommandList, pCamera);
	//m_pTerrain->Render(pd3dCommandList, m_pCamera);

	if(m_pMap) 
		m_pMap->Render(pd3dCommandList, m_pCamera);
	for (int i = 0; i < m_nTrees; ++i) {
		m_pTrees[i]->Render(pd3dCommandList, m_pCamera);
	}

	for (int i = 0; i < MAX_USER; ++i) {
		if (m_pPlayerVector[i])
		m_pPlayerVector[i]->Render(pd3dCommandList, m_pCamera);
	}

	if (m_pPlayer) m_pPlayer->Render(pd3dCommandList, m_pCamera);

	
	
	for (int i = 0; i < 2; ++i) {
		if (STONEMONS[i])
			STONEMONS[i]->Render(pd3dCommandList, m_pCamera);
	}

	m_UiShader->Render(pd3dCommandList, m_pCamera);
}

bool DayForestScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

bool DayForestScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return true;
}

void DayForestScene::ReleaseUploadBuffers()
{
	m_pPlayer->ReleaseUploadBuffers();

	for (int i = 0; i < 2; ++i)
	{
		if (STONEMONS[i])
			STONEMONS[i]->ReleaseUploadBuffers();
	}

	if (m_pPlayerVector.size() > 0)
		for (int i = 0; i < MAX_USER; ++i) m_pPlayerVector[i]->ReleaseUploadBuffers();

	for (int i = 0; i < m_nShaders; i++) m_ppShaders[i]->ReleaseUploadBuffers();
	if (m_pTerrain) m_pTerrain->ReleaseUploadBuffers();
	if (m_pSkyBox) m_pSkyBox->ReleaseUploadBuffers();
	for (int i = 0; i < m_nTrees; ++i) if (m_pTrees[i]) m_pTrees[i]->ReleaseUploadBuffers();
	if (m_pMap)	m_pMap->ReleaseUploadBuffers();
}

void DayForestScene::BuildLightsAndMaterials()
{

	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_pLights->m_pLights[0].m_xmf4Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[0].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Diffuse = XMFLOAT4(0.35f, 0.35f, 0.35f, 1.0f);
	m_pLights->m_pLights[1].m_xmf4Specular = XMFLOAT4(0.58f, 0.58f, 0.58f, 0.0f);
	m_pLights->m_pLights[1].m_xmf3Position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_xmf3Direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_xmf4Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Diffuse = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	m_pLights->m_pLights[2].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_xmf3Direction = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	m_pLights->m_pLights[3].m_bEnable = true;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_xmf4Ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Diffuse = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_pLights->m_pLights[3].m_xmf4Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[3].m_xmf3Position = XMFLOAT3(-150.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[3].m_xmf3Direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));

	m_pLights->m_pLights[4].m_bEnable = true;
	m_pLights->m_pLights[4].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[4].m_xmf4Ambient = XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Diffuse = XMFLOAT4(0.75f, 0.75f, 0.75f, 1.0f);
	m_pLights->m_pLights[4].m_xmf4Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[4].m_xmf3Direction = XMFLOAT3(1.0f, 1.0f, 1.0f);

	m_pMaterials = new MATERIALS;
	::ZeroMemory(m_pMaterials, sizeof(MATERIALS));


	/*조명 머테리얼*/
	m_pMaterials->m_pReflections[0] = { XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[1] = { XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 10.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[2] = { XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 15.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[3] = { XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[4] = { XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 25.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[5] = { XMFLOAT4(0.0f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 30.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[6] = { XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(0.5f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 35.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_pMaterials->m_pReflections[7] = { XMFLOAT4(1.0f, 0.5f, 1.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 40.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f) };
	m_nMaterials = 8;
}




void DayForestScene::PutNPC(char * packet)
{
}

void DayForestScene::MoveNPC(char * packet)
{
}

void DayForestScene::RemoveNPC(char * packet)
{

}


void DayForestScene::ReadMap()
{
	ifstream in("Assets/Map/Map.geg");

	if (in.is_open()) {
		in >> width;
		//cout << width << endl;
		in >> height;
		//cout << height << endl;
		map = new int*[height];

		for (int i = 0; i < height; ++i) {
			map[i] = new int[width];
		}
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				in >> map[i][j];
				//cout << map[i][j];
			}
			//cout << endl;
		}
	}
}

void DayForestScene::CalculatingGameObject()
{
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (map[y][x] == 1)
				++m_nTrees;
		}
	}
	cout << m_nTrees << endl;
}

void DayForestScene::SetTreePos()
{
	int index = 0;
	for (int y = 0; y < height; ++y) {
		for (int x = 0; x < width; ++x) {
			if (map[y][x] == 1)
				m_PosTree[index++] = XMFLOAT3(x, 10, y);
		}
	}
}


