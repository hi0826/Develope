/*여기다 추가하고싶은 Scene 클래스를 추가한다.*/
#include "stdafx.h"
#include "SceneManager.h"
#include "DayForestScene.h"
#include "StartScene.h"



CSceneManager::CSceneManager()
{
}


CSceneManager::~CSceneManager()
{
	m_pScene->ReleaseObjects();
	delete m_pScene;
}

bool CSceneManager::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	CreateScene<StartScene>(SC_CURRENT, pd3dDevice, pd3dCommandList);
	CreateScene<DayForestScene>(SC_NEXT, pd3dDevice, pd3dCommandList);

	soundMgr = new SoundMgr();
	soundMgr->sound_bgm();

	return true;
}

void CSceneManager::ProcessInput(float fDeltaTime)
{
	if (!nextState)
		m_pScene->ProcessInput(fDeltaTime);
	else
		m_pNextScene->ProcessInput(fDeltaTime);
}

void CSceneManager::Update(float fDeltaTime)
{
	if (!nextState)
		m_pScene->Update(fDeltaTime);
	else
		m_pNextScene->Update(fDeltaTime);
}

void CSceneManager::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	if (!nextState)
		m_pScene->Render(pd3dCommandList);
	else
		m_pNextScene->Render(pd3dCommandList);
}

void CSceneManager::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (!nextState)
	{
		if (m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam)) {
			soundMgr->soundBasePlay();
			nextState = true;
		}
	}
}

void CSceneManager::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	if (nextState)
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	else
		m_pNextScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
}

void CSceneManager::ReleaseUploadBuffers()
{
	if (!nextState)
		m_pScene->ReleaseUploadBuffers();
	else
		m_pNextScene->ReleaseUploadBuffers();
}

void CSceneManager::PacketProcess(char * packet)
{   
	m_pNextScene->PacketProcess(packet);
	//m_pScene->PacketProcess(packet);
}

