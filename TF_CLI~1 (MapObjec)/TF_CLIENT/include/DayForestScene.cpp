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
	Processor[SC_ATTACK_PLAYER]       = &CScene::PlayerAttack;


	Processor[SC_PUT_NPC]             = &DayForestScene::PutNPC;
	Processor[SC_HEATED_NPC]          = &DayForestScene::HeatedNPC;
	Processor[SC_REMOVE_NPC]          = &DayForestScene::RemoveNPC;
	Processor[SC_MOVE_NPC]            = &DayForestScene::MoveNPC;

	
}

DayForestScene::~DayForestScene()
{
	if (m_UiShader) m_UiShader->ReleaseObjects();
	SAFE_DELETE(m_UiShader);
	SAFE_DELETE(m_pMap);
	Safe_Delete_VecList(m_vMonsters);
	Safe_Delete_VecList(m_StaticObjects);
}

bool DayForestScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{   
	CScene::Initialize(pd3dDevice, pd3dCommandList);

	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/Trees3x3_3.MD5MESH", "Tree");
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/WarriorMesh.MD5MESH", "Warrior");
	CreateStaticObProtoType(pd3dDevice, pd3dCommandList, L"Assets/Model/FairyMesh0724.MD5MESH", "Fairy");
    ReadMap();

	CreateStaticObjectFromMapFile(pd3dDevice, pd3dCommandList);
	CreateAniProtoType(pd3dDevice, pd3dCommandList, "Warrior");

	m_pMap = new CPlaneMap(m_MapWidth,m_MapHeight);
	m_pMap->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);

	//for (int i = 0; i < m_vMonsters.capacity(); ++i) {
	//	CStoneMon* i_Monster = new CStoneMon();
	//	i_Monster->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
	//	m_vMonsters.push_back(i_Monster);
	//	m_vMonsters[i]->SetActive(false);
	//}
	
	if (m_pPlayerVector.size() == 0) { 
		for (int i = 0; i < MAX_USER; ++i) {
			CPlayer* Player = new CPlayer();
			INSTANCEOB tempOB[2];
			tempOB[0] = FindStaticObProtoType("Warrior");
			tempOB[1] = FindStaticObProtoType("Fairy");
			vector<ModelAnimation> Warrior_Ani = FindAniProtoType("Warrior");
			//vector<ModelAnimation> Warrior_animations = FindAniProtoType("Warrior");
			Player->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, tempOB, Warrior_Ani);
			m_pPlayerVector.push_back(Player);
			m_pPlayerVector[i]->SetActive(false);
		}
	}

	BuildLightsAndMaterials();
	m_pCamera = GenerateCamera(THIRD_PERSON_CAMERA, m_pPlayer);
	m_pCamera->SetTimeLag(0.f);
	m_pCamera->SetOffset(XMFLOAT3(0.0f, 50.0f, -40.0f));
	m_pCamera->GenerateProjectionMatrix(1.0f, 5000.0f, ASPECT_RATIO, 60.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	m_UiShader = new UIShader();
	m_UiShader->CreateGraphicsRootSignature(pd3dDevice);
	m_UiShader->CreateShader(pd3dDevice, 1);
	m_UiShader->BuildObjects(pd3dDevice, pd3dCommandList);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	return true; 
}

void DayForestScene::ProcessInput(float fDeltaTime)
{   
	if(m_pPlayer) m_pPlayer->ProcessInput(fDeltaTime);
}

void DayForestScene::Update(float fDeltaTime)
{
	//XMFLOAT3 ModifiedPos;
	//for (auto& Player : m_pPlayerVector)
	//{   
	//	if (!Player->GetActive()) continue; 
	//	for (auto& Player2 : m_pPlayerVector)
	//	{
	//		if (!Player2->GetActive()) continue;
	//		if (Player2 == Player) continue;
	//	if (Player->CeckCollision(Player2->GetOOBB())){
	//		
	//		printf("충돌한다 시발 \n");
	//		ModifiedPos = Vector3::Add(Player->GetWPosition(), DIRMAP[Player->GetDirection()], -2.5);
	//		Player->SetWPosition(ModifiedPos);
	//		Player->SetOOBB(ModifiedPos);
	//		break;
	//	}
	//	}
	//}
	//if (m_pPlayerVector.size() != 0) {
	//	cout << m_pPlayerVector.size() << endl;
	//}
	
	XMFLOAT3 PlayerPos;
	XMFLOAT3 PlayerLook;
	XMFLOAT3 PlayerPosition;
	XMFLOAT3 xmf3Shift = XMFLOAT3(0.f, 0.f, 0.f);
	for (int i = 0; i < m_vMonsters.size(); ++i) {
		//for (int j = 0; j < m_pPlayerVector.size(); ++j)
		{
			ContainmentType containType = m_vMonsters[i]->GetOOBB().Contains(m_pPlayerVector[0]->GetOOBB());
	
			switch (containType)
			{
			case DISJOINT:
				break;
			case INTERSECTS:
				printf("충돌");
	
				PlayerLook = m_pPlayerVector[0]->GetLook();
				PlayerPos = Vector3::Normalize(Vector3::Subtract(m_vMonsters[i]->GetWPosition(), m_pPlayerVector[0]->GetWPosition()));
				
				if (Vector3::CrossProduct(PlayerPos, m_pPlayerVector[0]->GetLook()).y > 0)
				{
					XMFLOAT3 RightVector = m_pPlayerVector[0]->GetRight();
					RightVector.x = RightVector.x * 3;
					RightVector.y = RightVector.y * 3;
					RightVector.z = RightVector.z * 3;
					PlayerLook = Vector3::Add(m_pPlayerVector[0]->GetLook(), RightVector);
				
				}
				else
				{
					XMFLOAT3 RightVector = m_pPlayerVector[0]->GetRight();
					RightVector.x = RightVector.x * 3;
					RightVector.y = RightVector.y * 3;
					RightVector.z = RightVector.z * 3;
					PlayerLook = Vector3::Subtract(m_pPlayerVector[0]->GetLook(), RightVector);
				
				}
				
				PlayerLook = Vector3::Normalize(PlayerLook);
				
				PlayerPosition = m_pPlayerVector[0]->GetWPosition();
				
				
				xmf3Shift = Vector3::Add(xmf3Shift, PlayerLook, fDeltaTime * 10);
				m_pPlayerVector[0]->Move(xmf3Shift, true);
				PlayerPosition = m_pPlayerVector[0]->GetWPosition();
				break;
			}
		
		}
	}
	

	for (int i = 0; i < MAX_USER; ++i) {
		if(m_pPlayerVector[i])
		m_pPlayerVector[i]->Update(fDeltaTime);
	}
	for (auto i : m_vMonsters) 
		i->Update(fDeltaTime);

	m_pCamera->Update(fDeltaTime);
	m_pCamera->RegenerateViewMatrix();
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
	
	if(m_pMap) m_pMap->Render(pd3dCommandList, m_pCamera);
	
	for (auto i : m_StaticObjects)
		i->Render(pd3dCommandList, m_pCamera);
	
	for (auto i : m_vMonsters)
		i->Render(pd3dCommandList, m_pCamera);

	for (int i = 0; i < MAX_USER; ++i) {
		if (m_pPlayerVector[i])
		m_pPlayerVector[i]->Render(pd3dCommandList, m_pCamera);
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
	if (m_pPlayerVector.size() > 0) for (int i = 0; i < MAX_USER; ++i) m_pPlayerVector[i]->ReleaseUploadBuffers();

	for (auto i : m_StaticObjects) i->ReleaseUploadBuffers();
	for (auto i : m_vMonsters) i->ReleaseUploadBuffers();

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
	sc_packet_put_npc*  Packet = (sc_packet_put_npc*)packet; 
	WORD ID = Packet->id;
	m_vMonsters[ID]->Rotate(Packet->dir);
	m_vMonsters[ID]->SetDirection(Packet->dir);
	m_vMonsters[ID]->SetWPosition(Packet->pos);
	m_vMonsters[ID]->SetAnimState(Packet->state);
	m_vMonsters[ID]->SetActive(true);
}

void DayForestScene::MoveNPC(char * packet)
{
}

void DayForestScene::HeatedNPC(char * packet)
{
	sc_packet_heated_npc* Packet = (sc_packet_heated_npc*)packet;
	
	//printf("ID::: %d\n", Packet->id);
	m_vMonsters[Packet->id]->SetAnimState(ATTACKSTATE);
}

void DayForestScene::RemoveNPC(char * packet)
{
	sc_packet_remove_npc* Packet = (sc_packet_remove_npc*)packet; 
	m_vMonsters[Packet->id]->SetActive(false);
}


void DayForestScene::ReadMap()
{
	ifstream in("Assets/Map/0802.geg");

	if (in.is_open()) {
		in >> m_MapWidth;
		in >> m_MapHeight;
		m_MapFile = new int*[m_MapHeight];
		for (int i = 0; i < m_MapHeight; ++i) {
			m_MapFile[i] = new int[m_MapWidth];
		}

		for (int i = 0; i < m_MapWidth; ++i) {
			for (int j = 0; j < m_MapWidth; ++j) {
				in >> m_MapFile[i][j];
			}
		}
	}
}

void DayForestScene::CreateStaticObjectFromMapFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList)
{
	int index = 0;
	for (int y = 0; y < m_MapHeight; ++y) {
		for (int x = 0; x < m_MapWidth; ++x) {
			switch(m_MapFile[y][x])
			{
			
			case READ_DATA::TREE: {
				CTree* i_Tree = new CTree();
				INSTANCEOB tempOB = FindStaticObProtoType("Tree");
				i_Tree->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, tempOB.Mesh);
				i_Tree->SetWPosition(x*30.f, 0, y*-30);
				m_StaticObjects.push_back(i_Tree);
				break;
			}

			case READ_DATA::Monster: {
				CStoneMon* i_Monster = new CStoneMon();
				i_Monster->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature);
				i_Monster->SetWPosition(x*30.f, 0, y*-30.f);
				//i_Monster->SetActive(true);
				m_vMonsters.push_back(i_Monster);
				m_vMonsters[index++]->SetActive(true);
			}
			default: break;
			}
		/*	if (m_MapFile[y][x] == READ_DATA::TREE)
				m_PosTree[index++] = XMFLOAT3(x, 0, y);
			for (int i = 0; i < m_StaticObjects.capacity(); ++i) {
				CTree* i_Tree = new CTree();
				i_Tree->Initialize(pd3dDevice, pd3dCommandList, m_pd3dGraphicsRootSignature, TreeMesh);
				i_Tree->SetWPosition(XMFLOAT3((m_PosTree[i].x) * 20, 0, (m_PosTree[i].z) * -20));
				m_StaticObjects.push_back(i_Tree);
			}*/
		}
	}
}

void DayForestScene::CreateMovableObjectFromMapFile(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandLIst)
{

}

void DayForestScene::CreateStaticObProtoType(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const std::wstring filePath, const std::string strTag)
{
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1;
	INSTANCEOB  TempOBJ;
	LoadMD5Model(pd3dDevice, pd3dCommandList, TempOBJ.Mesh, filePath, TempOBJ.Model, shaderResourceViewArray1, texFileNameArray1, 0.3, 0.3, 0.3);
	m_mProtoType.insert(std::make_pair(strTag, TempOBJ));	
}

void DayForestScene::CreateAniProtoType(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, const std::string strTag)
{
	std::vector<ID3D12Resource*> shaderResourceViewArray1;
	std::vector<std::wstring> texFileNameArray1;
	//Model3D m_MD5Model;
	INSTANCEOB TempOBJ = FindStaticObProtoType("Warrior");

	LoadMD5Anim(L"Assets/Model/WarriorIdle.md5anim", TempOBJ.Model);
	LoadMD5Anim(L"Assets/Model/WarriorWalk.md5anim", TempOBJ.Model);
	LoadMD5Anim(L"Assets/Model/WarriorAttack.md5anim", TempOBJ.Model);
	//std::vector<ModelAnimation> Warrior_animations = m_MD5Model.animations;
	m_mAniProtoType.insert(std::make_pair(strTag, TempOBJ.Model.animations));
}

INSTANCEOB DayForestScene::FindStaticObProtoType(const std::string & strKey)
{
	auto Iter = m_mProtoType.find(strKey);
	/*if (Iter == m_mProtoType.end()) return NULL;*/
	return Iter->second;
}

vector<ModelAnimation> DayForestScene::FindAniProtoType(const std::string & strKey)
{
	auto Iter = m_mAniProtoType.find(strKey);
	/*if (Iter == m_mProtoType.end()) return NULL;*/
	return Iter->second;
}


