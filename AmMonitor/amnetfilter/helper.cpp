#include "stdafx.h"
#include "helper.h"
#include "shared_mem.h"
/*-----------------------------------------------------------------------------
    See if this is the browser process we're actually interested in
    instrumenting (for browsers that have multi-process architectures)
-----------------------------------------------------------------------------*/
bool IsCorrectBrowserProcess(LPCTSTR exe)
{
  bool ok = false;


  if (!lstrcmpi(exe, _T("chrome.exe"))) {
    ok = true;
  } else if (!lstrcmpi(exe, _T("sogou.exe"))) {
    ok = true;
  } else if (!lstrcmpi(exe, _T("iexplore.exe"))) {
    ok = true;
  } else if (!lstrcmpi(exe, shared_browser_exe))
  {
  }

  return ok;
}

int GetBrowserVersion(LPCTSTR exe)
{
  return 0;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) 
{ 
    DWORD dwCurProcessId = *((DWORD*)lParam); 
    DWORD dwProcessId = 0; 
 
    GetWindowThreadProcessId(hwnd, &dwProcessId); 
    if(dwProcessId == dwCurProcessId && GetParent(hwnd) == NULL)
    { 
        *((HWND *)lParam) = hwnd;
        return FALSE; 
    } 
    return TRUE; 
} 
 
 
HWND GetMainWindow() 
{ 
    DWORD dwCurrentProcessId = GetCurrentProcessId();
    if(!EnumWindows(EnumWindowsProc, (LPARAM)&dwCurrentProcessId)) 
    {     
        return (HWND)dwCurrentProcessId; 
    } 
    return NULL; 
} 
 

bool GetExeProcessName(LPTSTR exePath,LPCTSTR modPath)
{
  bool rv = false;

  TCHAR path[_MAX_PATH]={0};

  lstrcpy(path, modPath);
  lstrcpy(exePath, modPath);
  TCHAR * token = _tcstok(path, _T("\\"));
  while (token != NULL)
  {
    if (lstrlen(token))
      lstrcpy(exePath, token);
    token = _tcstok(NULL, _T("\\"));
    rv = true;
  }
  return rv;
}
