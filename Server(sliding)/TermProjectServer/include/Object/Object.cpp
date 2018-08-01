#include "Object.h"


void CObject::Initialize()
{
}

void CObject::Close()
{
	Data.ID = -1;
	Data.HP = 0;
	Data.EXP = 0;
	Data.ATK = 0;
	Data.LEV = 0;
	Data.POS = XMFLOAT3(0.f, 0.f, 0.f);
	Data.DIR = DIR_FORWARD;
}

bool CObject::CeckCollision(const BoundingOrientedBox& other)
{
	if (OOBB.Contains(other) == INTERSECTS) return true;
	//printf("¶ì¿ä¿Ë\n");
	return false;
}

