#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <sqlext.h>
#include <conio.h>
#include <windowsx.h>
#include <mswsock.h>
#include <thread>
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <unordered_set>
#include  <unordered_map>
#include <array>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <queue>
#include <chrono>

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <tbb/tbbmalloc_proxy.h>


/*Server Lib*/
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib,"mswsock.lib") 
#pragma comment(lib,  "lua53.lib")

/*DX12 Lib*/
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

extern "C"
{
#include "Lua\lua.h"
#include "Lua\lauxlib.h"
#include "Lua\lualib.h"
}

using namespace DirectX;
using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;
using namespace std;
using namespace std; 
using namespace chrono;

#include "Define.h"
#include "protocol.h"
#include "Struct.h"
#include "Inline.h"
