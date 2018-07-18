#pragma once
#include "stdafx.h"

class CScene;
class CGameObject;

class CLayer
{
private:
	CScene*     m_pScene;
	string      m_strTag;
	int         m_iZOrder;
	std::list<class CGameObject*> m_ObjList;

public:
	CLayer();
	~CLayer();

public:
	void       SetTag(const string& strTag) { m_strTag = strTag; }
	void       SetZOrder(int iZOrder) { m_iZOrder = iZOrder; }
	void       SetScene(class CScene* pScene) { m_pScene = pScene; }

public:
	string    GetTag() const { return m_strTag; }
	int       GetZOrder() const { return m_iZOrder; }
	CScene*   GetScene() const { return m_pScene; }

public:
	void AddObject(class CGameObject* pObj);

public:
	void         ProcessInput();
	void         Update(float fDeltaTime);
	void         Render(ID3D12GraphicsCommandList *pd3dCommandList);

};

