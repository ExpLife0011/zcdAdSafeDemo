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
__declspec( dllexport ) void ZcnInstallHook(void);
__declspec( dllexport ) void ZcnUnInstallHook(void);
__declspec( dllexport ) void WINAPI ZcnSetProxyEnabled(bool enableProxy);
}

void WINAPI ZcnSetProxyEnabled(bool cleared_cache) {
  shared_proxy_enabled = cleared_cache;
}


static DWORD WINAPI ZcnHookThreadProc(void* arg)
{
  ::SetPriorityClass(::GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS);

  // actually do the startup work
  global_hook = new WptHook;
  global_hook->Init();

  return 0;
}

void ZcnInstallHook(void)
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
    global_hook->Destroy();
    delete global_hook;
    global_hook = NULL;
  }

  return 0;
}

void ZcnUnInstallHook(void)
{
  static bool started = false;
  if (!started)
  {
    started = true;
    //HANDLE thread_handle = CreateThread(NULL, 0, ::ZcnUnHookThreadProc, 0, 0, NULL);
    //::Sleep(1000);
    //if (thread_handle)
    //  CloseHandle(thread_handle);

  if( global_hook) 
  {
   // global_hook->Destroy();
    delete global_hook;
    global_hook = NULL;
  }
  }
  return;
}




HHOOK global_hCallwndHook = NULL;

LRESULT ZcnCallwndHookProc (
  int nCode,       // hook code
  WPARAM wParam,  // virtual-key code =
  LPARAM lParam   // keystroke-message information
)
{
  if (nCode < 0) // do not process message
    return CallNextHookEx(global_hCallwndHook, nCode, wParam, lParam);

  if (nCode >= HC_ACTION)
  {
    LPMSG msg = (LPMSG)lParam;
    LPCWPSTRUCT cmsg = (LPCWPSTRUCT)lParam;
    if (cmsg->message == WM_HOOKEX && cmsg->lParam )
    {
      do{
      ZcnInstallHook();
      }while(false);

    }
    else if( cmsg->message == WM_HOOKEX )
    {
      do{
      ZcnUnInstallHook();

      }while(false);
    }
  }

  return ::CallNextHookEx(global_hCallwndHook, nCode, wParam, lParam);
}
int ZcnInstallCallwndHook( )
{
  DWORD id = 0;//GetCurrentProcessId();GetCurrentThreadId
  global_hCallwndHook = SetWindowsHookEx( WH_CALLWNDPROC,(HOOKPROC)ZcnCallwndHookProc,g_hInstance, id );

  return (global_hCallwndHook != NULL);
}
int ZcnUnInstallCallwndHook( )
{
  UnhookWindowsHookEx( global_hCallwndHook);
  global_hCallwndHook =NULL;
  return (global_hCallwndHook == NULL);
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
          if(WM_HOOKEX == 0) WM_HOOKEX = ::RegisterWindowMessage(GUID_HOOKMSG);
          // IE gets instrumented from the BHO so don't start the actual
          // hooking, just let the DLL load
          //if (lstrcmpi(exe, _T("iexplore.exe")))
            ZcnInstallHook();
            //ZcnInstallCallwndHook();
            //
            //LoadLibrary(path);
            ::MessageBeep(MB_OK);
        }
      }
    }
    break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      {
        
        //ZcnUnInstallHook();
        //ZcnUnInstallCallwndHook();
         ::MessageBeep(MB_OK);
      }
      break;
    case DLL_PROCESS_DETACH:
        ZcnUnInstallHook();
        break;
  }
  return ok;
}
