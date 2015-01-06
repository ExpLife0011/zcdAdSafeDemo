// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include <Windows.h>
#include <tchar.h>
#include "shared_mem.h"
#include "wpthook.h"
extern WptHook * global_hook;

const TCHAR szApp[] = _T("DllPart.dll");

HINSTANCE g_hInstance = NULL;
//HHOOK global_hCallwndHook = NULL;
HWND global_target = NULL;
HWND global_host = NULL;

extern "C" {
__declspec( dllexport ) void ZcnInstallHook(void);
__declspec( dllexport ) void ZcnUnInstallHook(void);
__declspec( dllexport ) int ZcnInstallCallwndHook(HWND handle,HWND host);
__declspec( dllexport ) int ZcnUnInstallCallwndHook(void);
}


void ZcnInstallHook(void)
{
  //static bool started = false;
  //if (!started)
    for(int i=0;i<3;i++) ::MessageBeep(MB_ICONEXCLAMATION);
  if(!shared_has_gpu)
  {
    //started = true;
    shared_has_gpu  = true;

    // actually do the startup work
    global_hook = new WptHook;
    global_hook->Init();

  }
  return;
}

void ZcnUnInstallHook(void)
{
  //static bool started = false;
  //if (!started)
    for(int i=0;i<3;i++) ::MessageBeep(MB_ICONASTERISK);
  if(shared_has_gpu)
  {
    shared_has_gpu = false;
    //started = true;

  if( global_hook) 
  {
   // global_hook->Destroy();
    delete global_hook;
    global_hook = NULL;
  }
   /*向主线程发送 API UNHOOK 处理完毕的消息*/
   // DWORD idHost = ::GetWindowThreadProcessId(global_host,NULL);
    //PostThreadMessage(idHost,WM_AMMONITORRET,0,0);
  }
  return;
}




LRESULT CALLBACK ZcnCallwndHookProc (
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
    ::MessageBeep(MB_ICONQUESTION);
    if (cmsg->message == WM_AMMONITOR && (cmsg->lParam==1) )
    {
     // do{
        TCHAR lib_name[MAX_PATH]; 
        ::GetModuleFileName( g_hInstance, lib_name, MAX_PATH );
						
        //if( !::LoadLibrary( lib_name ) )
	        ;
        
        ZcnInstallHook();
      //}while(false);
      ::MessageBeep(MB_ICONEXCLAMATION);

    }
    else if( cmsg->message == WM_AMMONITOR  && (cmsg->lParam == 0))
    {
      //do{
      ZcnUnInstallHook();
      //::FreeLibrary( g_hInstance );
      //}while(false);
      ::MessageBeep(MB_ICONASTERISK);
    }
  }

  return ::CallNextHookEx(global_hCallwndHook, nCode, wParam, lParam);
}
int ZcnInstallCallwndHook(HWND handle,HWND host)
{

  DWORD ThreadId=0;// GetWindowThreadProcessId(handle, NULL);
  //DWORD ThreadId = GetCurrentThreadId();//0;// //GetCurrentProcessId();//
  global_target = handle;
  //global_host = host;
  global_hCallwndHook = SetWindowsHookEx( WH_CALLWNDPROC,(HOOKPROC)ZcnCallwndHookProc,g_hInstance, ThreadId );
  if( global_hCallwndHook)
  {
    DWORD err = GetLastError();
  }
  else
  {
    DWORD err = GetLastError();
    ::MessageBeep(MB_ICONASTERISK);
  }
  return (global_hCallwndHook != NULL);
}
int ZcnUnInstallCallwndHook( )
{
  //
  /*向目标线程发送消息进行API UNHOOK*/
  //DWORD idTarget = ::GetWindowThreadProcessId(global_target,NULL);
  //PostThreadMessage(idTarget,WM_AMMONITOR,(WPARAM)GetCurrentThreadId(),0);
    //showerr("WM_DISABLEAPIHOOK");
    /*等待目标进程返回WM_UNHOOKOK消息，确认可以将WH_GETMESSAGE的HOOK去掉*/

    //MSG Msg;
    //do {
    //    GetMessage(&Msg,NULL,0,0);
    //}while(Msg.message !=  WM_AMMONITORRET);

    UnhookWindowsHookEx(global_hCallwndHook);

    //PostThreadMessage(idTarget,WM_DISABLEAPIHOOKOK,(WPARAM)GetCurrentThreadId(),0);
  global_hCallwndHook =NULL;
  return (global_hCallwndHook == NULL);
}

int ZcnInstallCallwndHookMy()
{

  //DWORD ThreadId =0;// GetWindowThreadProcessId(host, NULL);
  DWORD ThreadId = GetCurrentThreadId();//0;// //GetCurrentProcessId();//
  //global_target = handle;
  //global_host = host;
  global_hCallwndHook = SetWindowsHookEx( WH_CALLWNDPROC,(HOOKPROC)ZcnCallwndHookProc,g_hInstance, ThreadId );
  if( global_hCallwndHook)
  {
    DWORD err = GetLastError();
  }
  else
  {
    DWORD err = GetLastError();
    ::MessageBeep(MB_ICONASTERISK);
  }
  return (global_hCallwndHook != NULL);
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
 
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved)
{
  BOOL ok = TRUE;
  switch (ul_reason_for_call)
  {
    case DLL_PROCESS_ATTACH:
    {
     // ::MessageBeep(MB_OK);
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

        if (!lstrcmpi(exe, szApp)) {
          ok = TRUE;
        }
        else if(/*lstrlen(shared_browser_exe) &&*/
                  /*IsCorrectBrowserProcess(exe)*/
                  true)
        {
          ok = TRUE;
          g_hInstance = (HINSTANCE)hModule;
          if(WM_AMMONITOR == 0) WM_AMMONITOR = ::RegisterWindowMessage(GUID_HOOKMSG);
          if(WM_AMMONITORRET == 0) WM_AMMONITORRET = ::RegisterWindowMessage(GUID_HOOKMSGRET);
          // IE gets instrumented from the BHO so don't start the actual
          // hooking, just let the DLL load
          //if (lstrcmpi(exe, _T("iexplore.exe")))
            //ZcnInstallHook();
          HWND hw = GetMainWindow();
            //ZcnInstallCallwndHook(hw);
            //
            //LoadLibrary(path);
            ::MessageBeep(MB_OK);
            //DisableThreadLibraryCalls(hModule);
        }
      }
    }
    break;
    case DLL_THREAD_ATTACH:
      break;
    case DLL_THREAD_DETACH:
      break;
    case DLL_PROCESS_DETACH:
        //ZcnUnInstallHook();
        //ZcnUnInstallCallwndHook();
        break;
  }
  return ok;
}



/*显示GetLastError的信息*/

void showerr(const char *m)
{
    char message[255];
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError()
     ,MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),message,255, 0);
    MessageBoxA(NULL,message,m,MB_OK);

}

