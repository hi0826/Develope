#include "NPC.h"
#include "../Util/VectorData.h"
#include "../Util/MapData.h"

CNPC::CNPC()
{
	IsAwake = false;
	Target = NONE_TARGET;
}

void CNPC::Initialize()
{
	CObject::Initialize();
}

void CNPC::Close()
{
}

void CNPC::LookAtTarget(const XMFLOAT3& otherpos)
{
	XMFLOAT3 MonsterLook = Vector3::Normalize(Vector3::Subtract(otherpos, GetPOS()));
	if (MonsterLook.x > -0.5f && MonsterLook.x < 0.5f) MonsterLook.x = 0.f; // 0.5 이하이면 다 0 으로 처리하자.
	if (MonsterLook.z > -0.5f && MonsterLook.z < 0.5f) MonsterLook.z = 0.f;
	MonsterLook = Vector3::Clamp(Vector3::ScalarProduct(MonsterLook, 10.f));
	for (auto& V : CVData::GET_SINGLE()->LookVectorMap) {
		if (Vector3::Equal(V.second, MonsterLook)) { SetDIR(V.first); }
	}
}

void CNPC::MapCollisionCheck(float fDelatTime)
{

	CMapData* ptr = CMapData::GET_SINGLE();
	BYTE DIR;
	XMFLOAT3 xmfShift;
	XMFLOAT3 S;
	/*Player <-> Forest*/
	for (int i = 0; i < ptr->MapBoundingBoxes[GetSTAGE()].size(); ++i)
	{
		auto& Collider = ptr->MapBoundingBoxes[GetSTAGE()][i];
		if (CheckCollision(Collider))
		{
			xmfShift = GetPOS();
			if (i == 0)      DIR = DIR_BACKWARD;
			else if (i == 1) DIR = DIR_FORWARD;
			else if (i == 2) DIR = DIR_RIGHT;
			else if (i == 3) DIR = DIR_LEFT;
			else if (i == 4)
			{
				if (xmfShift.z > -490)
				{
					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_BACKWARD;
			}
			else if (i == 5)
			{
				if (xmfShift.x < 435)
				{
					if (xmfShift.z < Collider.Center.z) DIR = DIR_BACKWARD;
					else DIR = DIR_FORWARD;
				}
				else DIR = DIR_RIGHT;
			}
			else if (i == 6)
			{
				if (xmfShift.z < -135)
				{
					if (xmfShift.x < Collider.Center.x)DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else  DIR = DIR_FORWARD;
			}
			else if (i == 7)
			{
				if (xmfShift.x > 195)
				{
					if (xmfShift.z < Collider.Center.z)DIR = DIR_BACKWARD;
					else  DIR = DIR_FORWARD;
				}
				else DIR = DIR_LEFT;
			}
			else if (i == 8)
			{
				if (xmfShift.z > -375)
				{
					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_BACKWARD;
			}
			else if (i == 9)
			{
				if (xmfShift.x < 315)
				{
					if (xmfShift.z < Collider.Center.z) DIR = DIR_BACKWARD;
					else DIR = DIR_FORWARD;
				}
				else DIR = DIR_RIGHT;
			}
			else if (i == 10)
			{
				if (xmfShift.z < -75)
				{
					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_FORWARD;
			}
			else if (i == 11)
			{
				if (xmfShift.x > 135)
				{
					if (xmfShift.z < Collider.Center.z) DIR = DIR_BACKWARD;
					else DIR = DIR_FORWARD;
				}
				else DIR = DIR_LEFT;
			}
			else if (i == 12)
			{
				if (xmfShift.z > -375)
				{
					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_BACKWARD;
			}
			else if (i == 13)
			{
				if (xmfShift.x < 375)
				{
					if (xmfShift.z < Collider.Center.z) DIR = DIR_BACKWARD;
					else  DIR = DIR_FORWARD;
				}
				else DIR = DIR_RIGHT;
			}
			else if (i == 14)
			{
				if (xmfShift.z < -195)
				{
					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
					else DIR = DIR_RIGHT;
				}
				else DIR = DIR_FORWARD;
			}
			else if (i == 15)
			{
				if (xmfShift.x > 255)
				{
					if (xmfShift.z < Collider.Center.z)DIR = DIR_BACKWARD;
					else  DIR = DIR_FORWARD;
				}
				else DIR = DIR_LEFT;
			}

			CollisionMove(DIR, fDelatTime);
		}
	}

}

void CNPC::CollisionMove(BYTE DIR, float fDeltaTime)
{
	XMFLOAT3 LOOK = Vector3::Normalize(CVData::GET_SINGLE()->LookVectorMap[DIR]);
	XMFLOAT3 S;
	S = Vector3::Subtract(LOOK, Vector3::ScalarProduct(CVData::GET_SINGLE()->LookVectorMap[DIR], Vector3::DotProduct(LOOK, CVData::GET_SINGLE()->LookVectorMap[DIR])));
	S = Vector3::Add(GetPOS(), Vector3::Normalize(S), fDeltaTime * Speed);
	S = Vector3::Add(GetPOS(), CVData::GET_SINGLE()->LookVectorMap[DIR], fDeltaTime * (Speed-2));
	SetPOS(S);
	SetOOBB(S);

}

void CNPC::Move(const float& fDeltaTime)
{
	SetPOS(Vector3::Add(GetPOS(), Vector3::ScalarProduct(Vector3::Normalize(CVData::GET_SINGLE()->LookVectorMap[GetDIR()]), fDeltaTime * Speed)));
	SetOOBB(GetPOS());
}
