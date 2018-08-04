#include "stdafx.h"

#include "GameFramework.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{   

	if (!CGameFramework::GET_SINGLE()->Initialize(hInstance))
	{
		CGameFramework::DESTROY_SINGLE();
		return 0;
	}
	CGameFramework::GET_SINGLE()->Run();
	CGameFramework::GET_SINGLE()->Close();
	CGameFramework::DESTROY_SINGLE();
	return 0;

}


