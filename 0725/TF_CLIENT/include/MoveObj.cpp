#include "stdafx.h"
#include "MoveObj.h"
#include "Shader.h"


CMoveObject::CMoveObject(int nMesh) : CGameObject(nMesh)
{
}

CMoveObject::CMoveObject(const CMoveObject& other)
{    
	m_pMesh = other.m_pMesh;
	m_MD5Model = other.m_MD5Model;
}

void CMoveObject::Update(float fTimeElapsed)
{

}

void CMoveObject::Move(WORD dwDirection, float fDistance, bool bUpdateVelocity)
{
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);

		XMFLOAT3 xmf3Look = GetLook();
		if (dwDirection & DIR_FR)        xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_FL)        xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_BR)        xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_BL)        xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
																			 
		if (dwDirection & DIR_FORWARD)   xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD)  xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_RIGHT)     xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		if (dwDirection & DIR_LEFT)      xmf3Shift = Vector3::Add(xmf3Shift, xmf3Look, fDistance);
		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CMoveObject::Move(const XMFLOAT3 & xmf3Shift, bool bVelocity)
{   
	SetWPosition(Vector3::Add(GetWPosition(), xmf3Shift));
}

void CMoveObject::Rotate(WORD direction)
{
	if (direction & DIR_FORWARD)
	{
		SetRight(XMFLOAT3(1.0f, 0.0f, 0.0f));
		SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
	}
	else if (direction & DIR_BACKWARD)
	{
		SetRight(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		SetLook(XMFLOAT3(0.0f, 0.0f, -1.0f));
	}
	else if (direction & DIR_RIGHT)
	{
		SetRight(XMFLOAT3(0.0f, 0.0f, -1.0f));
		SetLook(XMFLOAT3(1.0f, 0.0f, 0.0f));
	}
	else if (direction & DIR_LEFT)
	{
		SetRight(XMFLOAT3(0.0f, 0.0f, 1.0f));
		SetLook(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	}
	else if (direction & DIR_FR)
	{
		SetRight(XMFLOAT3(1.0f, 0.0f, -1.0f));
		SetLook(XMFLOAT3(1.0f, 0.0f, 1.0f));
	}
	else if (direction & DIR_FL)
	{
		SetRight(XMFLOAT3(1.0f, 0.0f, 1.0f));
		SetLook(XMFLOAT3(-1.0f, 0.0f, 1.0f));
	}
	else if (direction & DIR_BR)
	{
		SetRight(XMFLOAT3(-1.0f, 0.0f, -1.0f));
		SetLook(XMFLOAT3(1.0f, 0.0f, -1.0f));
	}
	else if (direction & DIR_BL) {
		SetRight(XMFLOAT3(-1.0f, 0.0f, 1.0f));
		SetLook(XMFLOAT3(-1.0f, 0.0f, -1.0f));
	}

	SetDirection(direction);
	SetLook(Vector3::Normalize(GetLook()));
	SetRight(Vector3::CrossProduct(GetUp(), GetLook(), true));
	SetUp(Vector3::CrossProduct(GetLook(), GetRight(), true));
}

void CMoveObject::MoveStrafe(float fDistance)
{
	XMFLOAT3	xmf3Position = Vector3::Add(GetWPosition(), GetRight(), fDistance);
	CGameObject::SetWPosition(xmf3Position);
}

void CMoveObject::MoveUp(float fDistance)
{
	XMFLOAT3	xmf3Position = Vector3::Add(GetWPosition(), GetUp(), fDistance);
	CGameObject::SetWPosition(xmf3Position);
}

void CMoveObject::MoveForward(float fDistance)
{
	XMFLOAT3	xmf3Position = Vector3::Add(GetWPosition(), GetLook(), fDistance);
	CGameObject::SetWPosition(xmf3Position);
}



