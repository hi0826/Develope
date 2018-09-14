#include "NPC.h"
#include "../Util/VectorData.h"


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
	XMFLOAT3 MonsterLook = Vector3::Normalize(Vector3::Subtract(otherpos, GetPos()));
	if (MonsterLook.x > -0.5f && MonsterLook.x < 0.5f) MonsterLook.x = 0.f; // 0.5 이하이면 다 0 으로 처리하자.
	if (MonsterLook.z > -0.5f && MonsterLook.z < 0.5f) MonsterLook.z = 0.f;
	MonsterLook = Vector3::Clamp(Vector3::ScalarProduct(MonsterLook, 10.f));

	auto NewDIR = std::find_if(CVData::GET()->LookVectorMap.begin(), CVData::GET()->LookVectorMap.end(), 
		[&](const std::pair<BYTE,XMFLOAT3>& a) {return Vector3::Equal(MonsterLook,a.second);  });
	SetDir(NewDIR->first);
}

void CNPC::ColliderCheck()
{

}

//void CNPC::MapCollisionCheck(float fDelatTime)
//{
//
//	CMapData* ptr = CMapData::GET();
//	BYTE DIR;
//	XMFLOAT3 xmfShift;
//	XMFLOAT3 S;
//	/*Player <-> Forest*/
//	for (int i = 0; i < ptr->MapBoundingBoxes[GetStage()].size(); ++i)
//	{
//		auto& Collider = ptr->MapBoundingBoxes[GetStage()][i];
//		if (CheckCollision(Collider))
//		{
//			xmfShift = GetPos();
//			if (i == 0)      DIR = DIR_BACKWARD;
//			else if (i == 1) DIR = DIR_FORWARD;
//			else if (i == 2) DIR = DIR_RIGHT;
//			else if (i == 3) DIR = DIR_LEFT;
//			else if (i == 4)
//			{
//				if (xmfShift.z > -490)
//				{
//					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
//					else DIR = DIR_RIGHT;
//				}
//				else DIR = DIR_BACKWARD;
//			}
//			else if (i == 5)
//			{
//				if (xmfShift.x < 435)
//				{
//					if (xmfShift.z < Collider.Center.z) DIR = DIR_BACKWARD;
//					else DIR = DIR_FORWARD;
//				}
//				else DIR = DIR_RIGHT;
//			}
//			else if (i == 6)
//			{
//				if (xmfShift.z < -135)
//				{
//					if (xmfShift.x < Collider.Center.x)DIR = DIR_LEFT;
//					else DIR = DIR_RIGHT;
//				}
//				else  DIR = DIR_FORWARD;
//			}
//			else if (i == 7)
//			{
//				if (xmfShift.x > 195)
//				{
//					if (xmfShift.z < Collider.Center.z)DIR = DIR_BACKWARD;
//					else  DIR = DIR_FORWARD;
//				}
//				else DIR = DIR_LEFT;
//			}
//			else if (i == 8)
//			{
//				if (xmfShift.z > -375)
//				{
//					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
//					else DIR = DIR_RIGHT;
//				}
//				else DIR = DIR_BACKWARD;
//			}
//			else if (i == 9)
//			{
//				if (xmfShift.x < 315)
//				{
//					if (xmfShift.z < Collider.Center.z) DIR = DIR_BACKWARD;
//					else DIR = DIR_FORWARD;
//				}
//				else DIR = DIR_RIGHT;
//			}
//			else if (i == 10)
//			{
//				if (xmfShift.z < -75)
//				{
//					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
//					else DIR = DIR_RIGHT;
//				}
//				else DIR = DIR_FORWARD;
//			}
//			else if (i == 11)
//			{
//				if (xmfShift.x > 135)
//				{
//					if (xmfShift.z < Collider.Center.z) DIR = DIR_BACKWARD;
//					else DIR = DIR_FORWARD;
//				}
//				else DIR = DIR_LEFT;
//			}
//			else if (i == 12)
//			{
//				if (xmfShift.z > -375)
//				{
//					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
//					else DIR = DIR_RIGHT;
//				}
//				else DIR = DIR_BACKWARD;
//			}
//			else if (i == 13)
//			{
//				if (xmfShift.x < 375)
//				{
//					if (xmfShift.z < Collider.Center.z) DIR = DIR_BACKWARD;
//					else  DIR = DIR_FORWARD;
//				}
//				else DIR = DIR_RIGHT;
//			}
//			else if (i == 14)
//			{
//				if (xmfShift.z < -195)
//				{
//					if (xmfShift.x < Collider.Center.x) DIR = DIR_LEFT;
//					else DIR = DIR_RIGHT;
//				}
//				else DIR = DIR_FORWARD;
//			}
//			else if (i == 15)
//			{
//				if (xmfShift.x > 255)
//				{
//					if (xmfShift.z < Collider.Center.z)DIR = DIR_BACKWARD;
//					else  DIR = DIR_FORWARD;
//				}
//				else DIR = DIR_LEFT;
//			}
//
//			CollisionMove(DIR, fDelatTime);
//		}
//	}
//
//}

void CNPC::CollisionMove(BYTE DIR, float fDeltaTime)
{
	XMFLOAT3 LOOK = Vector3::Normalize(CVData::GET()->LookVectorMap[DIR]);
	XMFLOAT3 S;
	S = Vector3::Subtract(LOOK, Vector3::ScalarProduct(CVData::GET()->LookVectorMap[DIR], Vector3::DotProduct(LOOK, CVData::GET()->LookVectorMap[DIR])));
	S = Vector3::Add(GetPos(), Vector3::Normalize(S), fDeltaTime * Speed);
	S = Vector3::Add(GetPos(), CVData::GET()->LookVectorMap[DIR], fDeltaTime * (Speed-2));
	SetPos(S);
	SetOOBB(S);
}
