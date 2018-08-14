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
	if (direction & DIR_FORWARD){
		SetRight(XMFLOAT3(1.0f, 0.0f, 0.0f));
		SetLook(XMFLOAT3(0.0f, 0.0f, 1.0f));
	}
	else if (direction & DIR_BACKWARD){
		SetRight(XMFLOAT3(-1.0f, 0.0f, 0.0f));
		SetLook(XMFLOAT3(0.0f, 0.0f, -1.0f));
	}
	else if (direction & DIR_RIGHT){
		SetRight(XMFLOAT3(0.0f, 0.0f, -1.0f));
		SetLook(XMFLOAT3(1.0f, 0.0f, 0.0f));
	}
	else if (direction & DIR_LEFT){
		SetRight(XMFLOAT3(0.0f, 0.0f, 1.0f));
		SetLook(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	}
	else if (direction & DIR_FR){
		SetRight(XMFLOAT3(1.0f, 0.0f, -1.0f));
		SetLook(XMFLOAT3(1.0f, 0.0f, 1.0f));
	}
	else if (direction & DIR_FL){
		SetRight(XMFLOAT3(1.0f, 0.0f, 1.0f));
		SetLook(XMFLOAT3(-1.0f, 0.0f, 1.0f));
	}
	else if (direction & DIR_BR){
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



