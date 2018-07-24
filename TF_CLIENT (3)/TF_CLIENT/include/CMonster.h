#pragma once
#include "MoveObj.h"
class CMonster :public CMoveObject
{
public:
	CMonster();
	virtual ~CMonster();
public: 
	virtual void Update(float fDeltaTime);
};


class CStoneMon :public CMonster
{
public:
	CStoneMon(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes =1);
	virtual ~CStoneMon();
};


class CBeatleMon :public CMonster
{
public:
	CBeatleMon(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes =1);
 	virtual ~CBeatleMon();
};
