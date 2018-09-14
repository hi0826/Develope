#include "stdafx.h"
#include "StartScene.h"

StartScene::StartScene()
{
}

StartScene::~StartScene()
{
	if (m_StartUiShader) m_StartUiShader->ReleaseObjects();
	SAFE_DELETE(m_StartUiShader);
}

bool StartScene::Initialize(ID3D12Device * pd3dDevice, ID3D12GraphicsCommandList * pd3dCommandList)
{
	CScene::Initialize(pd3dDevice, pd3dCommandList);

	m_pStartCamera = GenerateCamera(THIRD_PERSON_CAMERA, m_pPlayer);
	m_pStartCamera->SetTimeLag(0.f);
	m_pStartCamera->SetOffset(XMFLOAT3(0.0f, 50.0f, -40.0f));
	m_pStartCamera->GenerateProjectionMatrix(1.0f, 5000.0f, ASPECT_RATIO, 60.0f);
	m_pStartCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
	m_pStartCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);

	UIShader* tempUI = new UIShader();
	tempUI->CreateGraphicsRootSignature(pd3dDevice);
	tempUI->CreateStartShader(pd3dDevice, 1);
	tempUI->StartScene(pd3dDevice, pd3dCommandList);

	m_StartUiShader = tempUI;

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	return true;
}

void StartScene::Render(ID3D12GraphicsCommandList * pd3dCommandList, CCamera * pCamera)
{
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature);

	m_pStartCamera->SetViewportsAndScissorRects(pd3dCommandList);
	m_pStartCamera->UpdateShaderVariables(pd3dCommandList);


	m_StartUiShader->RenderStartScene(pd3dCommandList, m_pStartCamera);
}

bool StartScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	//cout << LOWORD(lParam) << ", " << HIWORD(lParam) << endl;
	//
	//if (LOWORD(lParam) < 690 && LOWORD(lParam) > 380) {
	//	if (HIWORD(lParam) < 695 && HIWORD(lParam) > 600) {
	//		return true;
	//	}
	//}
	//
	//else if (LOWORD(lParam) < 690 && LOWORD(lParam) > 380) {
	//	if (HIWORD(lParam) < 590 && HIWORD(lParam) > 500) {
	//		exit(0);
	//	}
	//}
	//return false;
	//cout << LOWORD(lParam) << ", " << HIWORD(lParam) << endl;

	if (LOWORD(lParam) < 690 && LOWORD(lParam) > 380) {
		if (HIWORD(lParam) < 695 && HIWORD(lParam) > 600) {
			exit(0);
		}

		if (HIWORD(lParam) < 590 && HIWORD(lParam) > 500) {
			return true;
		}

	}

	return false;
}

