// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once
#include "../include/Network/Global.h"
#include <chrono>
#include <typeinfo>
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#pragma comment(lib,"winmm.lib")
//#pragma comment(lib,"CORE.lib")
using namespace std;

/*Direct X 12 Header*/
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
/*#pragma comment(lib, "DirectXTex.lib") */

using namespace DirectX;
using namespace DirectX::PackedVector;
using Microsoft::WRL::ComPtr;

/*User Header*/

#include "../include/Network/CriticalSection.h"
#include "../include/Network/MultiThreadSync.h"
#include "../include/Network/MemoryPool.h"
#include "../include/Network/CircularQueue.h"
#include "../include/Network/NetWorkSession.h"
			
#include "../include/Network/PacketSession.h"
#include "../include/Network/EventSelect.h"
#include "../include/Network/ClientSession.h"
#include "../include/Network/Stream.h"
#include "../include/Network/NetWorkSession.h"

#include "Protocol.h"
#include "Define.h"
#include "inline.h"
#include "Enum.h"

/*Extern Function*/
extern UINT	gnCbvSrvDescriptorIncrementSize;
extern ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource **ppd3dUploadBuffer = NULL);
extern ID3D12Resource *CreateTextureResourceFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, ID3D12Resource **ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);





