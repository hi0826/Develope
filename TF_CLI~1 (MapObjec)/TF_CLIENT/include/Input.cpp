#include "stdafx.h"
#include "Input.h"


CInput::CInput() :m_pCreateKey(NULL)
{
}

CInput::~CInput()
{

	// m_mapKey Delete 를 해주자 
	//safe_delete_map 

	for (auto D : m_mapKey)
	{
		if (D.second)
			delete D.second;
	}
}

PKEYINFO CInput::FindKey(const string & strKey) const
{

	auto iter = m_mapKey.find(strKey);
	if (iter == m_mapKey.end()) return NULL;
	return iter->second;
}

bool CInput::Initialize(HWND hWnd)
{
	m_hwnd = hWnd;
	AddKey(VK_UP, "MF");
	AddKey(VK_DOWN, "MB");
	AddKey(VK_LEFT, "ML");
	AddKey(VK_RIGHT, "MR");
	AddKey(VK_UP, VK_RIGHT, "MRF");
	AddKey(VK_DOWN, VK_RIGHT, "MRB");
	AddKey(VK_UP, VK_LEFT, "MLF");
	AddKey(VK_LEFT, VK_DOWN, "MLB");
	AddKey("Fire", VK_SPACE);
	return true;
}

void CInput::Update(float fDeltaTime)
{
	if(GetFocus()!= NULL)
	for (auto& iter : m_mapKey)
	{
		int PushCount = 0;
		for (size_t i = 0; i < iter.second->vecKey.size(); ++i){
			if (GetAsyncKeyState(iter.second->vecKey[i]) & 0x8000) 
				++PushCount;
		}

		if (PushCount == iter.second->vecKey.size())
		{
			//눌렀고 
			if (!iter.second->bKeyDown && !iter.second->bKeyPress) iter.second->bKeyDown = true;
			else if (iter.second->bKeyDown && !iter.second->bKeyPress)
			{
				iter.second->bKeyPress = true;
				iter.second->bKeyDown  = false;
			}
		}
		else
		{
			if (iter.second->bKeyDown || iter.second->bKeyPress)
			{
				iter.second->bKeyUp = true;
				iter.second->bKeyPress = false;
				iter.second->bKeyDown = false;
			}
			else if (iter.second->bKeyUp)
				iter.second->bKeyUp = false;
		}
	}

}

bool CInput::KeyPress(const string & strKey) const
{
	PKEYINFO pInfo = FindKey(strKey);

	if (!pInfo) return false;

	return pInfo->bKeyPress;
}

bool CInput::KeyDown(const string & strKey) const
{

	PKEYINFO pInfo = FindKey(strKey);

	if (!pInfo) return false;

	return pInfo->bKeyDown;

}

bool CInput::KeyUp(const string & strKey) const
{
	PKEYINFO pInfo = FindKey(strKey);

	if (!pInfo) return false;

	return pInfo->bKeyUp;
}
