// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include <Windows.h>
#include <tchar.h>
#include <atlstr.h>
#include "InputMoniter.h"
#include "wpthook.h"

#include "util.h"
#include "shared_mem.h"
#include "installhook.h"
#include "helper.h"

#define HOSTAPP  _T("wptdriver.exe")

HINSTANCE g_hInstance = NULL;
extern WptHook * global_hook;

extern "C" {
__declspec( dllexport ) void __stdcall ZcnInstallHook(void);
__declspec( dllexport ) void __stdcall ZcnUnInstallHook(void);
}

static DWORD WINAPI ZcnHookThreadProc(void* arg)
{
  ::SetPriorityClass(::GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

  // actually do the startup work
  global_hook = new WptHook;
  global_hook->Init();

  return 0;
}

void WINAPI ZcnInstallHook(void)
{
  static bool started = false;
  if (!started)
  {
    started = true;
    HANDLE thread_handle = CreateThread(NULL, 0, ::ZcnHookThreadProc, 0, 0, NULL);
    if (thread_handle)
      CloseHandle(thread_handle);
  }
  return;
}

static DWORD WINAPI ZcnUnHookThreadProc(void* arg)
{
  ::SetPriorityClass(::GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

  // actually do the startup work
  if( global_hook) 
  {
    delete global_hook;
    global_hook = NULL;
  }

  return 0;
}

void WINAPI ZcnUnInstallHook(void)
{
  static bool started = false;
  if (!started)
  {
    started = true;
    HANDLE thread_handle = CreateThread(NULL, 0, ::ZcnUnHookThreadProc, 0, 0, NULL);
    if (thread_handle)
      CloseHandle(thread_handle);
  }
  return;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
  BOOL ok = TRUE;
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
    {
      ::MessageBeep(MB_OK);
    // This is called VERY early in a process - only use kernel32.dll
    // functions.
      ok = FALSE; // Don't load by default, only if we are actively testing
      TCHAR path[MAX_PATH]={0};
      if (GetModuleFileName(NULL, path, _countof(path)))
      {
        TCHAR exe[MAX_PATH];
        lstrcpy(exe, path);
        TCHAR * token = _tcstok(path, _T("\\"));
        while (token != NULL)
        {
          if (lstrlen(token))
            lstrcpy(exe, token);
          token = _tcstok(NULL, _T("\\"));
        }

        if (!lstrcmpi(exe, HOSTAPP)) {
          ok = TRUE;
        }
        else if(/*lstrlen(shared_browser_exe) &&*/
                  /*IsCorrectBrowserProcess(exe)*/
                  true)
        {
          ok = TRUE;
          g_hInstance = (HINSTANCE)hModule;

          // IE gets instrumented from the BHO so don't start the actual
          // hooking, just let the DLL load
          //if (lstrcmpi(exe, _T("iexplore.exe")))
            ZcnInstallHook();
            //
            ::MessageBeep(MB_OK);
        }
      }
    }
    break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
      {
        ZcnUnInstallHook();
      }
      break;
  }
  return ok;
}
