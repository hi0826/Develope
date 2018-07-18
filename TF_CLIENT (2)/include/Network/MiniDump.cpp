#include "stdafx.h"
#include "MiniDump.h"

typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)( //callback 함수의 원형 
	HANDLE hProcess,
	DWORD dwPid,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_EXCEPTION_INFORMATION UserStreamparam,
	CONST PMINIDUMP_EXCEPTION_INFORMATION CalbackParam);

LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = NULL;

//Unhandled Exception이 발생했을 때 넘어 오는 콜백함수 
LONG WINAPI  UnHandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo)
{
	HMODULE  DllHandle = NULL;

	//window 2000 이전에는 따로 DBGHELP를 배포해서 설정해주어야합니다. 
	// 여기서 사용하는 것은 DBGHELP.dll 안에 포함된 함수들입니다. 꼭 로드 해주어야합니다. 

	DllHandle = LoadLibrary(_T("DBGHELP.DLL"));

	if (DllHandle)
	{
		//덤프를 받아 파일로 만드는 과정 

		MINIDUMPWRITEDUMP  Dump = (MINIDUMPWRITEDUMP)GetProcAddress(DllHandle, "MiniDumpWriteDump");

		if (Dump)
		{
			TCHAR    DumpPath[MAX_PATH] = { 0, };
			SYSTEMTIME SystemTime;

			//현재 시간을 가져옵니다. 
			GetLocalTime(&SystemTime);

			//현재 시간을 기준으로 로그 파일을 만듭니다. 
			_sntprintf(DumpPath, MAX_PATH, _T("%d-%d-%d %d_%d_%d.dmp"),
				SystemTime.wYear,
				SystemTime.wMonth,
				SystemTime.wDay,
				SystemTime.wHour,
				SystemTime.wMinute,
				SystemTime.wSecond);

			//파일을 만듭니다. 
			HANDLE FileHandle = CreateFile(
				DumpPath,
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			//파일 만들기에 성공했을 때 
			if (FileHandle != INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInfo;

				// MINIDump 예외 정보 저장 구조체입니다. 
				MiniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
				MiniDumpExceptionInfo.ExceptionPointers = exceptionInfo;
				MiniDumpExceptionInfo.ClientPointers = NULL;

				//현재 프로세스에 대한 덤프를 실행합니다. 
				//여기서 Dump는 위의 DBGHELP.DLL에서 불러온 것을 이용 
				BOOL Success = Dump(
					GetCurrentProcess(),
					GetCurrentProcessId(),
					FileHandle,
					MiniDumpNormal,
					&MiniDumpExceptionInfo,
					NULL,
					NULL);

				if (Success)
				{
					//성공 했을 경우 
					CloseHandle(FileHandle);
					return EXCEPTION_EXECUTE_HANDLER;
				}
			}
			CloseHandle(FileHandle);
		}
	}

	return EXCEPTION_CONTINUE_SEARCH;
}


BOOL MiniDump::Begin(VOID)
{
	SetErrorMode(SEM_FAILCRITICALERRORS);

	PreviousExceptionFilter = SetUnhandledExceptionFilter(UnHandledExceptionFilter);

	return true;
}

BOOL MiniDump::End(VOID)
{
	SetUnhandledExceptionFilter(PreviousExceptionFilter);

	return true;
}
