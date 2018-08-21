#include "Object.h"



void CObject::Initialize()
{
	OOBB = new BoundingOrientedBox(XMFLOAT3(0.f, 0.f, 0.f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0, 0, 0, 1));
}

void CObject::Close()
{
	Data.ID  = -1;
	Data.HP  = 0;
	Data.ATK = 0;
	Data.POS = XMFLOAT3(0.f, 0.f, 0.f);
	Data.DIR = DIR_FORWARD;
	OOBB->Center = XMFLOAT3(0.f, 0.f, 0.f);
}

bool CObject::CheckCollision( BoundingOrientedBox& other)
{   
	//ContainmentType Type = GetOOBB()->Contains(other);
	//if (Type == INTERSECTS  || Type == CONTAINS) {  return true; }
	// return false;
	 return GetOOBB()->Intersects(other);
}

