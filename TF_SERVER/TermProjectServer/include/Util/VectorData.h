#pragma once
#include "../Include.h"
#include "SingleTone.h"

class CVData :public CSingleTone<CVData>
{
public:
	std::unordered_map<BYTE, XMFLOAT3> LookVectorMap;
	std::unordered_map<BYTE, XMFLOAT3> RightVectorMap;
	std::unordered_map<BYTE, BYTE>     RandomDirMap;
public:
	bool Initialize()
	{
		LookVectorMap[DIR_FORWARD] = XMFLOAT3(0.f, 0.f, 1.f);
		LookVectorMap[DIR_BACKWARD] = XMFLOAT3(0.f, 0.f, -1.f);
		LookVectorMap[DIR_RIGHT] = XMFLOAT3(1.f, 0.f, 0.f);
		LookVectorMap[DIR_LEFT] = XMFLOAT3(-1.f, 0.f, 0.f);
		LookVectorMap[DIR_FR] = XMFLOAT3(1.f, 0.f, 1.f);
		LookVectorMap[DIR_FL] = XMFLOAT3(-1.f, 0.f, 1.f);
		LookVectorMap[DIR_BR] = XMFLOAT3(1.f, 0.f, -1.f);
		LookVectorMap[DIR_BL] = XMFLOAT3(-1.f, 0.f, -1.f);

		RandomDirMap[0] = DIR_FORWARD;
		RandomDirMap[1] = DIR_BACKWARD;
		RandomDirMap[2] = DIR_RIGHT;
		RandomDirMap[3] = DIR_LEFT;
		RandomDirMap[4] = DIR_FR;
		RandomDirMap[5] = DIR_FL;
		RandomDirMap[6] = DIR_BR;
		RandomDirMap[7] = DIR_BL;

		RightVectorMap[DIR_FORWARD] = XMFLOAT3(0.f, 0.f, 1.f);
		RightVectorMap[DIR_BACKWARD] = XMFLOAT3(0.f, 0.f, -1.f);
		RightVectorMap[DIR_RIGHT] = XMFLOAT3(1.f, 0.f, 0.f);
		RightVectorMap[DIR_LEFT] = XMFLOAT3(-1.f, 0.f, 0.f);
		RightVectorMap[DIR_FR] = XMFLOAT3(1.f, 0.f, 1.f);
		RightVectorMap[DIR_FL] = XMFLOAT3(-1.f, 0.f, 1.f);
		RightVectorMap[DIR_BR] = XMFLOAT3(1.f, 0.f, -1.f);
		RightVectorMap[DIR_BL] = XMFLOAT3(-1.f, 0.f, -1.f);
	return true;
	}
};