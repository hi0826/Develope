#pragma once
#include "UIShader.h"
#include "Player.h"
#include "Camera.h"
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
	ID3D12RootSignature*         m_pd3dGraphicsRootSignature;
	ID3D12Resource*              m_pd3dcbMaterials;
	ID3D12Resource*              m_pd3dcbLights;
	UIShader*				     m_UiShader;

protected:
	int                          m_nShaders;
	CShader**                    m_ppShaders;
	
	int                          m_nInstancingShaders;
	int                          m_nMaterials;
	MATERIALS*                   m_pMaterials;
	MATERIAL*                    m_pcbMappedMaterials;
	CSkyBox*                     m_pSkyBox;
	LIGHTS*                      m_pLights;
	LIGHTS*                      m_pcbMappedLights;

protected:
	CHeightMapTerrain*            m_pTerrain;
	CPlayer**                 m_pPlayer;
    CCamera*                      m_pCamera;
	//std::list<class CLayer*>      m_LayerList;

public:
	//class CLayer* CreateLayer(const string& strTag, int iZOrder = 0);
	//class CLayer* FindLayer(const string& strTag);
	//static bool   LayerSort(class CLayer * pL1, class CLayer * pL2); 

public:
	CScene();
	virtual ~CScene() = 0;

public:
	ID3D12RootSignature*  CreateGraphicsRootSignature(ID3D12Device *pd3dDevice);
	ID3D12RootSignature*  GetGraphicsRootSignature() { return(m_pd3dGraphicsRootSignature); }
	void                  SetGraphicsRootSignature(ID3D12GraphicsCommandList *pd3dCommandList) { pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature); }

public:
	void                  ReleaseObjects();
	void                  SetMaterial(int nIndex, MATERIAL *pMaterial);
	void                  UpdateMaterial(CGameObject *pObject);
	CCamera*              GenerateCamera(DWORD cameramode,CGameObject* target);
	CHeightMapTerrain*    GetTerrain() { return(m_pTerrain); }

public:
	virtual bool          Initialize() = 0;
	virtual void          BuildObjects(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) = 0;
	virtual void          ProcessInput(float fDeltaTime) = 0;
	virtual void          Update(float fDeltaTime) = 0;
	virtual void          Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL) = 0;
	virtual bool          OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual bool          OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) = 0;
	virtual void          ReleaseUploadBuffers() = 0;
	
	virtual void          CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
};