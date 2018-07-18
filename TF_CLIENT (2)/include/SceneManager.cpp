/*여기다 추가하고싶은 Scene 클래스를 추가한다.*/
#include "stdafx.h"
#include "SceneManager.h"
#include "DayForestScene.h"



CSceneManager::CSceneManager()
{
}


CSceneManager::~CSceneManager()
{
	m_pScene->ReleaseObjects();
}

bool CSceneManager::Initialize()
{
	// 기본적으로 1개의 Scene을 만들어야함.. 
	// 그게 여기서 DayForestScene이고 
	// SC_CURRENT 는 현재씬
	// SC_NEXT는  다음 SCene
	CreateScene<DayForestScene>(SC_CURRENT);
	return true;
}

void CSceneManager::ProcessInput(float fDeltaTime)
{
	m_pScene->ProcessInput(fDeltaTime);
}

void CSceneManager::Update(float fDeltaTime)
{
	m_pScene->Update(fDeltaTime);
}

void CSceneManager::BuildObjects(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	m_pScene->BuildObjects(pd3dDevice, pd3dCommandList);
}

void CSceneManager::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	m_pScene->Render(pd3dCommandList);
}

void CSceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}

void CSceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
}

void CSceneManager::ReleaseUploadBuffers()
{
	m_pScene->ReleaseUploadBuffers();
}
