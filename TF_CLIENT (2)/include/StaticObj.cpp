#include "stdafx.h"
#include "Shader.h"
#include "Object.h"
#include "StaticObj.h"


CStaticObj::CStaticObj() :CGameObject(1)
{
}

CStaticObj::CStaticObj(const CStaticObj & other)
{
	m_pMesh = other.m_pMesh;
	m_MD5Model = other.m_MD5Model;
}

void CStaticObj::Update(float fTimeElapsed)
{

}

void CStaticObj::Rotate(float x, float y, float z, DWORD dwDirection)
{
	//XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
	//m_xmf4x4World = Matrix4x4::Multiply(xmmtxRotate, m_xmf4x4World);

	//if (dwDirection & DIR_FORWARD) {
	//	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	//	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	//}
	//if (dwDirection & DIR_BACKWARD) {
	//	m_xmf3Right = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	//	m_xmf3Look = XMFLOAT3(0.0f, 0.0f, -1.0f);
	//}
	//if (dwDirection & DIR_RIGHT) {
	//	m_xmf3Right = XMFLOAT3(0.0f, 0.0f, -1.0f);
	//	m_xmf3Look = XMFLOAT3(1.0f, 0.0f, 0.0f);
	//}
	//if (dwDirection & DIR_LEFT) {
	//	m_xmf3Right = XMFLOAT3(0.0f, 0.0f, 1.0f);
	//	m_xmf3Look = XMFLOAT3(-1.0f, 0.0f, 0.0f);
	//}

	//if (dwDirection & DIR_FR) {
	//	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, -1.0f);
	//	m_xmf3Look = Vector3::Normalize(XMFLOAT3(1.0f, 0.0f, 1.0f));
	//}

	//if (dwDirection & DIR_FL) {
	//	m_xmf3Right = XMFLOAT3(1.0f, 0.0f, 1.0f);
	//	m_xmf3Look = Vector3::Normalize(XMFLOAT3(-1.0f, 0.0f, 1.0f));
	//}

	//if (dwDirection & DIR_BR) {
	//	m_xmf3Right = XMFLOAT3(-1.0f, 0.0f, -1.0f);
	//	m_xmf3Look = Vector3::Normalize(XMFLOAT3(1.0f, 0.0f, -1.0f));
	//}

	//if (dwDirection & DIR_BL) {
	//	m_xmf3Right = XMFLOAT3(-1.0f, 0.0f, 1.0f);
	//	m_xmf3Look = Vector3::Normalize(XMFLOAT3(-1.0f, 0.0f, -1.0f));
	//}
	//m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	//m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	//m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}



CTree::CTree(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, CMesh* Mesh, Model3D Model, void * pContext, int nMeshes)
	: CStaticObj()
{   /*pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature*/
	

	m_pMesh = Mesh;
	m_MD5Model = Model;
	SetMesh(0, *m_ppMeshes);
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource* pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("TreeMap.dds"), 0);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	tex->Release();
}

CTree::~CTree()
{

}

CFence::CFence(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList, ID3D12RootSignature * pd3dGraphicsRootSignature, CMesh * Mesh, Model3D Model, void * pContext, int nMeshes)
	: CStaticObj()
{    ///*pd3dDevice, pd3dCommandList, pd3dGraphicsRootSignature*/
	m_pMesh = Mesh;
	m_MD5Model = Model;
	SetMesh(0, m_pMesh);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	ID3D12Resource *pd3dcbResource = CreateShaderVariables(pd3dDevice, pd3dCommandList);
	CPlayerShader *pShader = new CPlayerShader();
	m_pMaterial = new CMaterial();
	CTexture *tex = new CTexture(1, RESOURCE_TEXTURE2D, 0);
	tex->LoadTextureFromFile(pd3dDevice, pd3dCommandList, _T("FenceMap.dds"), 0);
	m_pMaterial->SetTexture(tex);
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	pShader->CreateCbvAndSrvDescriptorHeaps(pd3dDevice, pd3dCommandList, 1, 1);
	pShader->CreateConstantBufferViews(pd3dDevice, pd3dCommandList, 1, pd3dcbResource, ncbElementBytes);
	pShader->CreateShaderResourceViews(pd3dDevice, pd3dCommandList, tex, 5, true);
	SetCbvGPUDescriptorHandle(pShader->GetGPUCbvDescriptorStartHandle());
	m_pMaterial->SetShader(pShader);
	tex->Release();
}

CFence::~CFence()
{

}
