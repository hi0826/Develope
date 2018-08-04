#pragma once
#include "MoveObj.h"

class CStoneMon :public CMoveObject
{
public:
	CStoneMon();
	virtual ~CStoneMon();

public:
<<<<<<< HEAD
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CMesh* mesh, Model3D model);
=======
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
>>>>>>> 1b444bfd5c9d5d477b5c55d7bd8f583a66b25add
	virtual void Update(float fDeltaTime);
};


class CBeatleMon :public CMoveObject
{
public:
	CBeatleMon();
 	virtual ~CBeatleMon();

public:	
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature);
	virtual void Update(float fDeltaTime);
};
