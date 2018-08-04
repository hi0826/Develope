
#pragma once
#include <iostream>
#include <stdlib.h>
#include <tchar.h>
#include <math.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mmsystem.h>
#include <mswsock.h>


#include <mswsock.h>
#include <windows.h>
#include <dbghelp.h>
#include <assert.h>
#include <time.h>
#include <math.h>
#include <fstream>
#include <algorithm>
#include <memory.h>

#include <wrl.h>
#include <shellapi.h>


#include <vector>
#include <queue>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <utility>
#include <chrono>
#include <typeinfo>
#include <thread>
#include <mutex>
#include <shared_mutex>

#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"mswsock.lib")
#pragma comment (lib,"ws2_32.lib")
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#pragma comment(lib,"winmm.lib")
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
#include "Define.h"
#include "inline.h"
#include "Enum.h"
#include "Protocol.h"

/*Extern Function*/
extern UINT	gnCbvSrvDescriptorIncrementSize;
extern ID3D12Resource *CreateBufferResource(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, void *pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType = D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource **ppd3dUploadBuffer = NULL);
extern ID3D12Resource *CreateTextureResourceFromFile(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList, const wchar_t *pszFileName, ID3D12Resource **ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);





