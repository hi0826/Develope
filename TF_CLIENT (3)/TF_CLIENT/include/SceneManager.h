#pragma once
#include "SingleTone.h"
class CScene;
class CSceneManager : public CSingleTone<CSceneManager>
{
private:
	class CScene*      m_pScene;
	class CScene*      m_pNextScene;

public:
	CSceneManager();
	virtual ~CSceneManager();

public: 
	template<typename T>
	T*    CreateScene(SCENE_CREATE sc)
	{
		T* pScene = new T(); 
		if (!pScene->Initialize())
		{
			SAFE_DELETE(pScene);
			return NULL; 
		}
		switch (sc)
		{
		case SC_CURRENT:
			//SAFE_DELETE(m_pScene);
			m_pScene = pScene;
			break;
		case SC_NEXT:
			SAFE_DELETE(m_pNextScene);
			m_pNextScene = pScene;
			break;
		}
		return pScene;
	}
	class CScene* GetCurScene() const { return m_pScene; }
public:
	bool  Initialize();
	void  ProcessInput(float fDeltaTime);
	void  Update(float fDeltaTime);
	void  BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	void  Render(ID3D12GraphicsCommandList* pd3dCommandList);
	void  OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void  OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void  ReleaseUploadBuffers();
	void  PacketProcess(char* packet);
};

