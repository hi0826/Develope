#include "stdafx.h"
#include "MoveObj.h"
#include "Shader.h"


CMoveObject::CMoveObject(int nMesh) : CGameObject(nMesh)
{
}

void CMoveObject::Move(const XMFLOAT3 & xmf3Shift, bool bVelocity)
{
	SetWPosition(Vector3::Add(GetWPosition(), xmf3Shift));
}

void CMoveObject::Rotate(WORD direction)
{
	if (direction & DIR_FORWARD)       SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
	else if (direction & DIR_BACKWARD) SetLook(XMFLOAT3(0.0f, 0.0f, -1.0f));
	else if (direction & DIR_RIGHT)    SetLook(XMFLOAT3(1.0f, 0.0f, 0.0f));
	else if (direction & DIR_LEFT)     SetLook(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	else if (direction & DIR_FR)       SetLook(XMFLOAT3(1.0f, 0.0f, 1.0f));
	else if (direction & DIR_FL)       SetLook(XMFLOAT3(-1.0f, 0.0f, 1.0f));
	else if (direction & DIR_BR)       SetLook(XMFLOAT3(1.0f, 0.0f, -1.0f));
	else if (direction & DIR_BL)       SetLook(XMFLOAT3(-1.0f, 0.0f, -1.0f));

	SetDirection(direction);
	SetLook(Vector3::Normalize(GetLook()));
	SetRight(Vector3::CrossProduct(GetUp(), GetLook(), true));

	///printf("X:%f,Y:%f,Z:%f\n", GetLook().x, GetLook().y, GetLook().z);
}

void CMoveObject::MoveStrafe(float fDistance)
{
	CGameObject::SetWPosition(Vector3::Add(GetWPosition(), GetRight(), fDistance));
}

void CMoveObject::MoveUp(float fDistance)
{
	CGameObject::SetWPosition(Vector3::Add(GetWPosition(), GetUp(), fDistance));
}

void CMoveObject::MoveForward(float fDistance)
{
	//XMFLOAT3	xmf3Position = Vector3::Add(GetWPosition(), GetLook(), fDistance);
	CGameObject::SetWPosition(Vector3::Add(GetWPosition(), GetLook(), fDistance));
}



