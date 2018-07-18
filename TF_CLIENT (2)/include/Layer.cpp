#include "stdafx.h"
#include "Layer.h"
#include "Object.h"


CLayer::CLayer() : m_iZOrder(0),m_strTag(""),m_pScene(NULL)
{

}


CLayer::~CLayer()
{
}

void CLayer::AddObject(CGameObject * pObj)
{
	pObj->SetScene(m_pScene);
	pObj->SetLayer(this);
	pObj->AddRef();
	m_ObjList.push_back(pObj);
}

void CLayer::ProcessInput()
{
	for (auto& obj : m_ObjList){
		obj->ProcessInput();
	}
}

void CLayer::Update(float fDeltaTime)
{
	for (auto& obj : m_ObjList) {
		obj->Update(fDeltaTime);
	}
}

void CLayer::Render(ID3D12GraphicsCommandList * pd3dCommandList)
{

	for (auto& obj : m_ObjList) {
		//obj->Render(pd3dCommandList);
	}
}
