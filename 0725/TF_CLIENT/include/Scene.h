#pragma once
#include "UIShader.h"
#include "Player.h"
#include "Camera.h"
#include "StaticObj.h"
struct LIGHT
{
	XMFLOAT4            m_xmf4Ambient;
	XMFLOAT4            m_xmf4Diffuse;
	XMFLOAT4            m_xmf4Specular;
	XMFLOAT3            m_xmf3Position;
	float               m_fFalloff;
	XMFLOAT3            m_xmf3Direction;
	float               m_fTheta; //cos(m_fTheta)
	XMFLOAT3            m_xmf3Attenuation;
	float               m_fPhi; //cos(m_fPhi)
	bool                m_bEnable;
	int                 m_nType;
	float               m_fRange;
	float               padding;
};

struct LIGHTS
{
	LIGHT               m_pLights[MAX_LIGHTS];
	XMFLOAT4            m_xmf4GlobalAmbient;
};

struct MATERIAL
{
	XMFLOAT4            m_xmf4Ambient;
	XMFLOAT4            m_xmf4Diffuse;
	XMFLOAT4            m_xmf4Specular; //(r,g,b,a=power)
	XMFLOAT4            m_xmf4Emissive;
};

struct MATERIALS
{
	MATERIAL            m_pReflections[MAX_MATERIALS];
};

class CCamera;
class CShader;

class CScene
{
protected:
	static std::vector<CPlayer*> m_pPlayerVector;
	static CPlayer*              m_pPlayer;
	static BOOL                  FirstTime;
	static WORD                  PLAYERID;
	UIShader*				     m_UiShader;

protected:
	ID3D12RootSignature* m_pd3dGraphicsRootSignature;
	ID3D12Resource*      m_pd3dcbMaterials;
	ID3D12Resource*      m_pd3dcbLights;

protected:
	int                  m_nShaders;
	int                  m_nInstancingShaders;
	int                  m_nMaterials;
	CShader**            m_ppShaders;
	MATERIALS*           m_pMaterials;
	MATERIAL*            m_pcbMappedMaterials;
	CSkyBox*             m_pSkyBox;
	LIGHTS*              m_pLights;
	LIGHTS*              m_pcbMappedLights;

protected:
	CHeightMapTerrain * m_pTerrain;
	CCamera*            m_pCamera;
	CPlaneMap*			m_pMap;

	int					width, height;
	int**				map;


	CMesh*				TreeMesh;
	Model3D				TreeModel;
	int					m_nTrees = 0;
	CGameObject**		m_pTrees;
	XMFLOAT3*			m_PosTree;

public:
	CScene();
	virtual ~CScene() = 0;

public:
	ID3D12RootSignature*  CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature*  GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	void                  SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

public:
	bool               Initialize();
	void               ReleaseObjects();
	void               SetMaterial(int nIndex, MATERIAL *pMaterial);
	void               UpdateMaterial(CGameObject *pObject);
	CCamera*           GenerateCamera(DWORD cameramode, CGameObject* target);
	CHeightMapTerrain* GetTerrain() { return(m_pTerrain); }

public:
	virtual void BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) = 0;
	virtual void ProcessInput(float fDeltaTime) = 0;
	virtual void Update(float fDeltaTime) = 0;
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL) = 0;
	virtual bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void PacketProcess(char* packet) {};
	virtual void BuildLightsAndMaterials() = 0;
	virtual void ReleaseUploadBuffers() = 0;

public:
	void PutPlayer(char* packet);
	void RemovePlayer(char* packet);
	void PlayerMove(char* packet);
	void PlayerAttackStart(char* packet);
	void PlayerAttackStop(char* packet);
	void PlayerDie(char* packet);


};