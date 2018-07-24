#pragma once

/*Direct X Define*/
#define FRAME_BUFFER_WIDTH			1024
#define FRAME_BUFFER_HEIGHT			760

#define MAX_LIGHTS					8 
#define MAX_MATERIALS				64 

#define POINT_LIGHT					1
#define SPOT_LIGHT					2
#define DIRECTIONAL_LIGHT			3

#define ROOT_PARAMETER_CAMERA		1
#define ROOT_PARAMETER_OBJECT		2
#define ROOT_PARAMETER_PLAYER		0
#define ROOT_PARAMETER_MATERIAL		3
#define ROOT_PARAMETER_LIGHT		4
#define ROOT_PARAMETER_TEXTURE		5

#define _WITH_CB_WORLD_MATRIX_DESCRIPTOR_TABLE
#pragma warning(disable:4996)
#define START_CONSOLE() {AllocConsole();  freopen("CONOUT$", "w", stdout); freopen("CONIN$", "r", stdin);}
#define STOP_CONSOLE()  {FreeConsole();}

#define RANDOM_COLOR	XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))
#define EPSILON				1.0e-10f

#define SAFE_DELETE(p) if(p)  {delete p; p =NULL;}
#define SAFE_DELETE_ARRAY(p) if(p) {delete[] p ; p = NULL;}
#define SAFE_RELEASE(p) if(p) { p->Release();  p =NULL;}



/*Network Define*/
#define MAX_USER         3
#define DEFAULT_PORT     9000
#define MAX_PACKET_SIZE  512
#define MAX_STR_SIZE     50
     
/*#define NPC_START        1000      
#define NUM_OF_NPC       6000*/    


//키매니저 
//이제 이 매크로를 ProcessInput 에다 가져다 쓰면댄당게 
#define KEYDOWN(Key)  CInput::GET_SINGLE()->KeyDown(Key)
#define KEYPRESS(Key) CInput::GET_SINGLE()->KeyPress(Key)
#define KEYUP(Key)    CInput::GET_SINGLE()->KeyUp(Key)

template<typename T>
void Safe_Delete_VecList(T& p)
{
	for (auto& iter : p)
		SAFE_DELETE(iter);
	p.clear();
}


template<typename T>
void Safe_Release_VecList(T& p)
{
	for (auto& iter : p)
		SAFE_RELEASE(iter);
	p.clear();
}

template<typename T>
void Safe_Delete_Map(T& p)
{
	for (auto& iter : p)
		SAFE_DELETE(iter->second);
	p.clear();
}

template<typename T>
void Safe_Release_Map(T& p)
{
	for (auto& iter : p)
		SAFE_RELEASE(iter->second);
	p.clear();
}


