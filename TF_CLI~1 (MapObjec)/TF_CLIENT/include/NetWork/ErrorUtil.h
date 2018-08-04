#pragma once
#include "../stdafx.h"
class ErrorUtil
{
public:
	static void Err_Quit(char* msg);
	static void ErrDisplay(const char* msg, int err_no);
	static void Err_Display(const char* msg);
};

