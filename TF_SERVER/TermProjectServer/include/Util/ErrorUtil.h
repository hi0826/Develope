#pragma once
#include "../Include.h"
class ErrorUtil
{
public:
	static void Err_Quit(const char* msg);
	static void ErrDisplay(const char* msg, int err_no);
	static void Err_Display(const char* msg);
};

