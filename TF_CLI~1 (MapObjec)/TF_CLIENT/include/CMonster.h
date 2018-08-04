#pragma once
#include "MoveObj.h"

class CStoneMon :public CMoveObject
{
public:
	CStoneMon();
	virtual ~CStoneMon();

public:
	virtual bool Initialize(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, CMesh* mesh, Model3D model);
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
