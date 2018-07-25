#pragma once
class CStaticObj : public CGameObject
{

	/*추가할거면 추가하셈*/

public:
	CStaticObj();
	CStaticObj(const CStaticObj& other); //복사생성자 

public:
	virtual ~CStaticObj() {}
	virtual void OnPrepareRender() {};
	virtual void Update(float fTimeElapsed);

public:
	/*void         SetPosition(const XMFLOAT3& xmf3Position) { m_xmf3Position = xmf3Position; }*/
	void            Rotate(float x, float y, float z, DWORD dwDirection);
};



/*Tree 조형물*/
class CTree : public CStaticObj
{
public:
	CTree(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CMesh* pMesh, Model3D model, void *pContext = NULL, int nMeshes = 1);
	virtual ~CTree();

};


/*Fence 조형물*/
class CFence : public CStaticObj
{
public:
	CFence(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature,
		CMesh* pMesh, Model3D model, void *pContext = NULL, int nMeshes = 1);
	virtual ~CFence();
};