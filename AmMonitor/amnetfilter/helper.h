#pragma once
#include <Windows.h>


bool IsCorrectBrowserProcess(LPCTSTR exe);
HWND GetMainWindow() ;

bool GetExeProcessName(LPTSTR exePath,LPCTSTR modPath);
int GetBrowserVersion(LPCTSTR exe);

