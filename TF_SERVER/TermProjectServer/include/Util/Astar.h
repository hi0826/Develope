#pragma once
#include "../Include.h"
#include "../Object/Object.h"

struct NODE
{
	XMFLOAT3 Pos;
	float  F; //총 비용
	float  G; //시작점 부터 현재 위치까지의 비용
	float  H; //현재 위치에서 최종 목적지 까지의 비용
	
	NODE() 
	{
		Pos = XMFLOAT3(0.f, 0.f, 0.f);
		F = -9999;
		G = -9999;
		H = -9999;
	}
	NODE(const XMFLOAT3& pos, float f, float g, float h)
	{
		Pos = pos;
		F   = f; 
		G   = g;
		H   = h;
	}

};

class CNCompare {
public:
	CNCompare() {};
	bool operator() (const NODE& L, const NODE& R) const {
		return (L.F > R.F);
	}
};

class CAstar
{
public:
   static void FindPath(CObject& me, CObject& other);
   const NODE& FindNODE(CObject& me);
};

