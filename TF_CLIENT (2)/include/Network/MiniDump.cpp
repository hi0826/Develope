#include "stdafx.h"
#include "MiniDump.h"

typedef BOOL(WINAPI* MINIDUMPWRITEDUMP)( //callback �Լ��� ���� 
	HANDLE hProcess,
	DWORD dwPid,
	HANDLE hFile,
	MINIDUMP_TYPE DumpType,
	CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
	CONST PMINIDUMP_EXCEPTION_INFORMATION UserStreamparam,
	CONST PMINIDUMP_EXCEPTION_INFORMATION CalbackParam);

LPTOP_LEVEL_EXCEPTION_FILTER PreviousExceptionFilter = NULL;

//Unhandled Exception�� �߻����� �� �Ѿ� ���� �ݹ��Լ� 
LONG WINAPI  UnHandledExceptionFilter(struct _EXCEPTION_POINTERS* exceptionInfo)
{
	HMODULE  DllHandle = NULL;

	//window 2000 �������� ���� DBGHELP�� �����ؼ� �������־���մϴ�. 
	// ���⼭ ����ϴ� ���� DBGHELP.dll �ȿ� ���Ե� �Լ����Դϴ�. �� �ε� ���־���մϴ�. 

	DllHandle = LoadLibrary(_T("DBGHELP.DLL"));

	if (DllHandle)
	{
		//������ �޾� ���Ϸ� ����� ���� 

		MINIDUMPWRITEDUMP  Dump = (MINIDUMPWRITEDUMP)GetProcAddress(DllHandle, "MiniDumpWriteDump");

		if (Dump)
		{
			TCHAR    DumpPath[MAX_PATH] = { 0, };
			SYSTEMTIME SystemTime;

			//���� �ð��� �����ɴϴ�. 
			GetLocalTime(&SystemTime);

			//���� �ð��� �������� �α� ������ ����ϴ�. 
			_sntprintf(DumpPath, MAX_PATH, _T("%d-%d-%d %d_%d_%d.dmp"),
				SystemTime.wYear,
				SystemTime.wMonth,
				SystemTime.wDay,
				SystemTime.wHour,
				SystemTime.wMinute,
				SystemTime.wSecond);

			//������ ����ϴ�. 
			HANDLE FileHandle = CreateFile(
				DumpPath,
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				NULL, CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				NULL);

			//���� ����⿡ �������� �� 
			if (FileHandle != INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInfo;

				// MINIDump ���� ���� ���� ����ü�Դϴ�. 
				MiniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
				MiniDumpExceptionInfo.ExceptionPointers = exceptionInfo;
				MiniDumpExceptionInfo.ClientPointers = NULL;

				//���� ���μ����� ���� ������ �����մϴ�. 
				//���⼭ Dump�� ���� DBGHELP.DLL���� �ҷ��� ���� �̿� 
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
					//���� ���� ��� 
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
