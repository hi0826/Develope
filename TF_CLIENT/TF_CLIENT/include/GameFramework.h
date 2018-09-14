#pragma once

//#define FRAME_BUFFER_WIDTH      1280
//#define FRAME_BUFFER_HEIGHT      800

#include "SingleTone.h"
#include "Timer.h"
#include "Player.h"
#include "Scene.h"

class CGameFramework  :public CSingleTone <CGameFramework>
{
private: //WINAPI Variable 
   HINSTANCE                        m_hInstance;
   HWND                             m_hWnd;
   unsigned int                     m_nWndClientWidth;
   unsigned int                     m_nWndClientHeight;

private: //DirectX12 Variable 
   static const UINT            m_nSwapChainBuffers = 2; // »ó¼ö 

   ID3D12Device*                m_pd3dDevice;
   IDXGIFactory4*               m_pdxgiFactory;
   IDXGISwapChain3*             m_pdxgiSwapChain;
   ID3D12Resource*              m_pd3dDepthStencilBuffer;
   ID3D12Resource*              m_ppd3dSwapChainBackBuffers[m_nSwapChainBuffers];
   ID3D12DescriptorHeap*        m_pd3dRtvDescriptorHeap;
   ID3D12DescriptorHeap*        m_pd3dDsvDescriptorHeap;
   ID3D12CommandAllocator*      m_pd3dCommandAllocator;
   ID3D12CommandQueue*          m_pd3dCommandQueue;
   ID3D12GraphicsCommandList*   m_pd3dCommandList;
   ID3D12Fence*                 m_pd3dFence;

   bool                         m_bMsaa4xEnable;
   UINT                         m_nMsaa4xQualityLevels;
   UINT                         m_nSwapChainBufferIndex;
   UINT                         m_nRtvDescriptorIncrementSize;
   UINT                         m_nDsvDescriptorIncrementSize;
   UINT64                       m_nFenceValues[m_nSwapChainBuffers];
   HANDLE                       m_hFenceEvent;

#if defined(_DEBUG)
   ID3D12Debug               *m_pd3dDebugController;
   ID3D12DebugDevice*         m_DebugDevice;
#endif

private:
   POINT                   m_ptOldCursorPos;
   _TCHAR                  m_pszFrameRate[50];

private: 
   CGameTimer                  m_GameTimer;
   static BOOL                 m_bLoop;
   static BOOL                 m_bActive;
private: 
	std::thread                 NetWorkThread;
public:
   CGameFramework();
   ~CGameFramework();

public: //API
    bool  Initialize(HINSTANCE hInstance);                         
	ATOM  MyRegisterClass();
	BOOL  MyCreateWindow();
	int   Run();
	void  Close();

public:// Direct X 12 Function
   void  CreateSwapChain();
   void  CreateDirect3DDevice();
   void  CreateRtvAndDsvDescriptorHeaps();
   void  CreateRenderTargetViews();
   void  CreateDepthStencilView();
   void  CreateCommandQueueAndList();                             
   void  OnResizeBackBuffers();
   void  WaitForGpuComplete();
                              
public:// 6 Core Function 
   void  FrameAdvance();
   void  BuildObjects();
   void  ProcessInput();
   void  Render();
   void  Update();
   void  NetworkRun();

public://IO Function
   static LRESULT CALLBACK  WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
   void                     OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
   void                     OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
   int                      OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
   std::thread              CreateNetworkThread() { return std::thread([this] { this->NetworkRun();  }); }


};
