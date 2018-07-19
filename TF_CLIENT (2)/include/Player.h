#pragma once

#define DIR_FORWARD				0x01
#define DIR_BACKWARD			0x02
#define DIR_LEFT				0x04
#define DIR_RIGHT				0x08
#define DIR_UP					0x10
#define DIR_DOWN				0x20

#include "MoveObj.h"
#include "Camera.h"

class CPlayer : public CMoveObject
{	
public:
	CPlayer();
	virtual ~CPlayer();
	DWORD dwDirection;
	XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);
	//bool bMove = false;
	//bool bStop = false;
	int imove = 0;
	bool battack = false;
	int hp = 100;
	float ETime = 0;
public: //Get
	//CCamera*                          GetCamera() const { return(m_pCamera); }

public: //Virtual

	virtual void                      Input(float fTimeElapsed);
	virtual void                      Update(float fTimeElapsed);
	
	//virtual CCamera*                  ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }
	//virtual void                      Animate(float fTimeElapsed);
	//CCamera*                          OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);
	//void Move(ULONG nDirection, float fDistance, bool bVelocity = false);
	//void Move(const XMFLOAT3& xmf3Shift, bool bVelocity = false);
	//void Move(float fxOffset = 0.0f, float fyOffset = 0.0f, float fzOffset = 0.0f);
	void Rotate(float x, float y, float z, DWORD dwDirection);

};

class CWarrior : public CPlayer
{
public:
	CWarrior(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, ID3D12RootSignature *pd3dGraphicsRootSignature, int nMeshes = 1);
	virtual ~CWarrior();
	//virtual CCamera *ChangeCamera(DWORD nNewCameraMode, float fTimeElapsed);
};


