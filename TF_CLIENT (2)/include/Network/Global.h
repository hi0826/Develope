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

///*DirectX 12 header*/
//#include <d3d12.h>
//#include <dxgi1_4.h>
//#include <D3Dcompiler.h>
//#include <DirectXMath.h>
//#include <DirectXPackedVector.h>
//#include <DirectXColors.h>
//#include <DirectXCollision.h>
//#include <Mmsystem.h>
//#import "C:\\Program Files\\Common Files\\System\\ADO\\msado15.dll" rename("EOF", "EndOfFile") no_namespace

#pragma comment (lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"mswsock.lib")

///*Direct X 12 lib*/
//#pragma comment(lib, "d3dcompiler.lib")
//#pragma comment(lib, "d3d12.lib")
//#pragma comment(lib, "dxgi.lib")


/*Direct X 12 namespace*/

//using namespace std;
//using namespace DirectX;
//using namespace DirectX::PackedVector;
//
//using Microsoft::WRL::ComPtr;

enum IO_TYPE
{
	IO_ACCEPT,
	IO_READ,
	IO_WRITE
};

typedef struct _OVERLAPPED_EX
{
	OVERLAPPED	overlapped;
	IO_TYPE		IoType;
	VOID		*Object;
} OVERLAPPED_EX;

#define MAX_BUFFER_LENGTH	4096

#ifdef _SERVER_SIDE
#define MAX_QUEUE_LENGTH	50
#else
#define MAX_QUEUE_LENGTH	500
#endif

#include "Log.h"