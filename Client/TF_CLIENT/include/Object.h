//------------------------------------------------------- ----------------------
// File: Object.h
//-----------------------------------------------------------------------------

#pragma once
#include "Ref.h"
#include "UploadBuffer.h"
#include "Mesh.h"
#include "Camera.h"
#include "md5Loader.h"

#define RESOURCE_TEXTURE2D        0x01
#define RESOURCE_TEXTURE2D_ARRAY  0x02   
#define RESOURCE_TEXTURE2DARRAY   0x03
#define RESOURCE_TEXTURE_CUBE     0x04
#define RESOURCE_BUFFER           0x05

class CShader;

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4   m_xmf4x4World;
	UINT         m_nMaterial;
};

struct SRVROOTARGUMENTINFO
{
	UINT                             m_nRootParameterIndex = 0;
	D3D12_GPU_DESCRIPTOR_HANDLE      m_d3dSrvGpuDescriptorHandle;
};

class CTexture :public CRef
{
public:
	CTexture(int nTextureResources = 1, UINT nResourceType = RESOURCE_TEXTURE2D, int nSamplers = 0);
	virtual ~CTexture();

private:
	int                                        m_nTextures = 0;
	int                                        m_nSamplers = 0;
	UINT                                       m_nTextureType = RESOURCE_TEXTURE2D;
	ID3D12Resource**                           m_ppd3dTextures = NULL;
	ID3D12Resource**                           m_ppd3dTextureUploadBuffers;
	SRVROOTARGUMENTINFO*                       m_pRootArgumentInfos = NULL;
	D3D12_GPU_DESCRIPTOR_HANDLE*               m_pd3dSamplerGpuDescriptorHandles = NULL;

public:
	int GetTextureCount() { return(m_nTextures); }
	ID3D12Resource* GetTexture(int nIndex) { return(m_ppd3dTextures[nIndex]); }
	UINT GetTextureType() { return(m_nTextureType); }

public:
	void SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dsrvGpuDescriptorHandle);
	void SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE d3dSamplerGpuDescriptorHandle);

public:
	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void UpdateShaderVariable(ID3D12GraphicsCommandList *pd3dCommandList, int nIndex);
	void ReleaseShaderVariables();
	void LoadTextureFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, UINT nIndex);
	void ReleaseUploadBuffers();
	SRVROOTARGUMENTINFO& GetArgumentInfos(UINT nIndex)
	{
		return m_pRootArgumentInfos[nIndex];
	}
};

class CMaterial :public CRef
{
public:
	CMaterial();
	virtual ~CMaterial();

public:
	XMFLOAT4                        m_xmf4Albedo;
	UINT                            m_nReflection;
	CTexture*                       m_pTexture;
	CShader*                        m_pShader;

public:
	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }
	void SetTexture(CTexture *pTexture);
	void SetShader(CShader *pShader);

	void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	void ReleaseShaderVariables();
	void ReleaseUploadBuffers();
};

class CGameObject :public CRef
{
protected:
	D3D12_GPU_DESCRIPTOR_HANDLE   m_d3dCbvGPUDescriptorHandle;
	ID3D12Resource*               m_pd3dcbGameObject;
	CB_GAMEOBJECT_INFO*           m_pcbMappedGameObject;
	bool                          m_bActive; 
	class CScene*                 m_pScene;

protected:
	int                           m_nMeshes;
	CMesh*                        m_pMesh;
	CMesh**                       m_ppMeshes;
	CMaterial*                    m_pMaterial;
	Model3D                       m_MD5Model;
	std::shared_mutex             PosLock;    


protected: /*Component*/
	XMFLOAT4X4                    m_xmf4x4World;
	BoundingOrientedBox           m_pOOBB;
	 
public:
	void            SetScene(class CScene* pScene) { m_pScene = pScene; }
	class CScene*   GetScene() const { return m_pScene; }
public:
	CGameObject(int nMeshes = 1);
	virtual ~CGameObject();


public:  
	virtual void                    Initialize();
	virtual ID3D12Resource*         CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void                    ReleaseShaderVariables();
	virtual void                    UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void                    Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances = 1);
	virtual void                    Update(float fDeltaTime);
	virtual void                    ProcessInput(float fDeltaTime) {};
	virtual void                    ReleaseUploadBuffers();


public: //Get
	D3D12_GPU_DESCRIPTOR_HANDLE     GetCbvGPUDescriptorHandle() const { return(m_d3dCbvGPUDescriptorHandle); }
	XMFLOAT4X4                      GetWMatrix()   const { return m_xmf4x4World; }
	CMaterial*                      GetMaterial()  const { return m_pMaterial; }
	XMFLOAT3                        GetWPosition()       { std::shared_lock<shared_mutex> lock(PosLock); return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43)); }
	XMFLOAT3                        GetLook()      const { return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33))); }
	XMFLOAT3                        GetUp()        const { return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23))); }
	XMFLOAT3                        GetRight()     const { return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13))); }
	bool                            GetActive()    const { return m_bActive; }
	bool                            IsInCamera(CCamera* pCamera = NULL);
	BoundingOrientedBox				GetOOBB() { return m_pOOBB; }


public: //Set
	void                           SetLook(const XMFLOAT3& look)   { m_xmf4x4World._31 = look.x, m_xmf4x4World._32 = look.y, m_xmf4x4World._33 = look.z; }
	void                           SetRight(const XMFLOAT3& right) { m_xmf4x4World._11 = right.x, m_xmf4x4World._12 = right.y, m_xmf4x4World._13 = right.z; }
	void                           SetUp(const XMFLOAT3& up)       { m_xmf4x4World._21 = up.x, m_xmf4x4World._22 = up.y, m_xmf4x4World._23 = up.z; }
	void                           SetWPosition(float x, float y, float z) {m_xmf4x4World._41 = x; m_xmf4x4World._42 = y; m_xmf4x4World._43 = z; }
	void                           SetWPosition(const XMFLOAT3& pos) { std::unique_lock<shared_mutex> lock(PosLock); m_xmf4x4World._41 = pos.x; m_xmf4x4World._42 = pos.y; m_xmf4x4World._43 = pos.z; }
	void                           SetScale(float x, float y, float z) { XMMATRIX mtxScale = XMMatrixScaling(x, y, z); m_xmf4x4World = Matrix4x4::Multiply(mtxScale, m_xmf4x4World); }
	void                           SetMesh(int nIndex, CMesh *pMesh);
	void                           SetShader(CShader *pShader);
	void                           SetMaterial(CMaterial *pMaterial);
	void                           SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE d3dCbvGPUDescriptorHandle) { m_d3dCbvGPUDescriptorHandle = d3dCbvGPUDescriptorHandle; }
	void                           SetCbvGPUDescriptorHandlePtr(UINT64 nCbvGPUDescriptorHandlePtr) { m_d3dCbvGPUDescriptorHandle.ptr = nCbvGPUDescriptorHandlePtr; }
	void                           SetActive(bool check) { m_bActive = check; }
public: 
	void                           Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);
	void                           Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void                           Rotate(XMFLOAT4 *pxmf4Quaternion);
};


/*******Terrain, SkyBox, Billboard***************/
class CHeightMapTerrain : public CGameObject
{
public:
	CHeightMapTerrain(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		LPCTSTR pFileName, int nWidth, int nLength, int nBlockWidth, int nBlockLength, XMFLOAT3& xmf3Scale, XMFLOAT4& xmf4Color);
	virtual ~CHeightMapTerrain();

private:
	CHeightMapImage*        m_pHeightMapImage;
	XMFLOAT3                m_xmf3Scale;
	int                     m_nWidth;
	int                     m_nLength;

public:
	float    GetHeight(float x, float z, bool bReverseQuad = false) { return(m_pHeightMapImage->GetHeight(x, z, bReverseQuad) * m_xmf3Scale.y); } //World
	XMFLOAT3 GetNormal(float x, float z) { return(m_pHeightMapImage->GetHeightMapNormal(int(x / m_xmf3Scale.x), int(z / m_xmf3Scale.z))); }
	int      GetHeightMapWidth() { return(m_pHeightMapImage->GetHeightMapWidth()); }
	int      GetHeightMapLength() { return(m_pHeightMapImage->GetHeightMapLength()); }
	float    GetWidth() { return(m_nWidth * m_xmf3Scale.x); }
	float    GetLength() { return(m_nLength * m_xmf3Scale.z); }
};

class CSkyBox : public CGameObject
{
public:
	CSkyBox(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual ~CSkyBox();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera = NULL);
};

class BillboardObject : public CGameObject
{
public:
	BillboardObject(int nMeshes = 1) {}
	virtual ~BillboardObject() {}

private:
	XMFLOAT3            m_xmf3RotationAxis;

public:
	void         SetLookAt(XMFLOAT3& xmf3Target);
	virtual void Update(float fTimeElapsed, CCamera *pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera, UINT nInstances = 1);
};

class CPlaneMap : public CGameObject
{
public:
	CPlaneMap(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		int nWidth, int nLength, int nBlockWidth, int nBlockLength);
	virtual ~CPlaneMap();

private:
	int m_nWidth, m_nLength;
	XMFLOAT3 Normal;
	XMFLOAT3 m_xmf3Scale;

public:
	int GetWidth() { return m_nWidth; }
	int GetLength() { return m_nLength; }
	XMFLOAT3 GetNormal() { return Normal; }
};