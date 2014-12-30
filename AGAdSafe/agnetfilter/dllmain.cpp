// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

#include <Windows.h>
#include <tchar.h>
//#include <Imm.h>
#include <atlstr.h>
//#pragma comment(lib, "Imm32.lib")
#include "InputMoniter.h"

#include "util.h"
#include "shared_mem.h"


HINSTANCE g_hInstance = NULL;
int g_bNetHooked = 0;// START button subclassed?


void installGeHookDll();
void uninstallGeHookDll();
BOOL InstallCallwndHook();
BOOL UninstallCallwndHook();
BOOL InstallKbHook();
BOOL UninstallKbHook();

BOOL InstallFilter()
{
  BOOL ret = FALSE;
  //InstallCallwndHook();
  InstallKbHook();
  return ret;
}

BOOL UninstallFilter()
{ 
  BOOL ret= FALSE;
	//::SendMessageTimeout(HWND_BROADCAST,WM_HOOKEX,0,0, SMTO_BLOCK, 1500, 0);
  //UninstallCallwndHook();
  UninstallKbHook();
  return ret;
}

/////////////////////
LRESULT CALLBACK KbHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{

    TCHAR host_name[MAX_PATH]; 
  ::GetModuleFileName( NULL, host_name, MAX_PATH );
  LPTSTR lpName = ::PathFindFileNameW(host_name);
  CString IEName =_T("iexplore.exe");
  bool isIE = (0 == IEName.CompareNoCase(lpName));
  ::OutputDebugString(lpName);

	if (nCode < 0)
	{
		return CallNextHookEx(g_hKbHook, nCode, wParam, lParam);
	}
	if (nCode != HC_ACTION)
	{
		return CallNextHookEx(g_hKbHook, nCode, wParam, lParam);
	}

	//::SendMessageTimeout(HWND_BROADCAST,WM_HOOKEX,0,1, SMTO_BLOCK, 1500, 0);
  installGeHookDll();

	return CallNextHookEx(g_hKbHook, nCode, wParam, lParam);
}

BOOL InstallKbHook()
{
  WriteAGLog("EnableKeyboardCapture");
  if(g_hKbHook == NULL)
  {
  g_hKbHook=SetWindowsHookExW(WH_KEYBOARD, (HOOKPROC)KbHookProc, g_hInstance, 0);
  if (g_hKbHook == NULL)
  {
	  WriteAGLog("SetWindowsHookEx FALSE");
	  return FALSE;
  }
  WriteAGLog("SetWindowsHookEx TRUE");
  }
  return TRUE;
}
// 导出函数：解除键盘锁定
BOOL UninstallKbHook()
{
	WriteAGLog("DisableKeyboardCapture");
	BOOL bOK = UnhookWindowsHookEx(g_hKbHook);

	g_hKbHook = NULL;
	WriteAGLog("UnhookWindowsHookEx TRUE");
	return bOK;
}

// end

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
  TCHAR szPath[_MAX_PATH+1]={0};
	if (ul_reason_for_call ==  DLL_PROCESS_ATTACH)
  {

    g_hInstance = (HINSTANCE)hModule;

    //EnableKeyboardCapture();
    DisableThreadLibraryCalls(hModule); 
    //::GetModuleFileName(hModule,szPath,_MAX_PATH);
    //::LoadLibrary(szPath);
    ////::FreeLibraryAndExitThread(hModule,0);
    //// Safely remove hook
    ////::UnhookWindowsHookEx( g_hKbHook );
    //::GetModuleFileName(NULL,szPath,_MAX_PATH);
    //::OutputDebugString(szPath);

		if( WM_HOOKEX==NULL )
			WM_HOOKEX = ::RegisterWindowMessage( GUID_HOOKMSG);			
    }
  else if( DLL_THREAD_ATTACH ==ul_reason_for_call)
  {
  }
  else if( DLL_THREAD_DETACH ==ul_reason_for_call)
  {
  }
  else if( DLL_PROCESS_DETACH ==ul_reason_for_call)
  {
  }

	return TRUE;
}

//#define _USE_WINHTTP_
//#define _USE_WININET_
//#define _USE_WINSOCK_
#define _USE_WPTSOCK_

#ifdef _USE_WINHTTP_
#include "WinHttpHook.h"
#elif defined(_USE_WININET_)
#include "WinInetHook.h"
#elif defined(_USE_WINSOCK_)
#include "WinsockHook.h"
#elif defined(_USE_WPTSOCK_)
#include "WptsockHook.h"
#else
#include "WptsockHook.h"
#include "WinHttpHook.h"
#include "WinInetHook.h"
#endif

BOOL g_bWinHttpApiHook = FALSE;
BOOL g_bWinInetApiHook = FALSE;
BOOL g_bWinsockApiHook = FALSE;
BOOL g_bWptsockApiHook = FALSE;

void installGeHookDll()
{
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
#elif defined(_USE_WINSOCK_)
	if (g_bWinsockApiHook==FALSE)
	{
		WriteAGLog("WinsockInstallHooks");
		g_bWinsockApiHook = WinsockInstallHooks();
	}
#elif defined(_USE_WPTSOCK_)
	if (g_bWptsockApiHook==FALSE)
	{
		WriteAGLog("WptsockInstallHooks");
		g_bWptsockApiHook = WptsockInstallHooks();
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
  if (g_bWptsockApiHook==FALSE)
	{
		WriteAGLog("WptsockInstallHooks");
		g_bWptsockApiHook = WptsockInstallHooks();
	}
#endif
}

void uninstallGeHookDll()
{
#ifdef _USE_WINHTTP_
	WinHttpRemoveHooks();
#elif defined(_USE_WININET_)
	WinInetRemoveHooks();
#elif defined(_USE_WINSOCK_)
	WinsockRemoveHooks();
  //g_bWinsockApiHook = FALSE;
#elif defined(_USE_WPTSOCK_)
  WptsockRemoveHooks();
#else
	WinHttpRemoveHooks();
	WinInetRemoveHooks();
	WptsockRemoveHooks();
#endif
}


//-------------------------------------------------------------
// global variables (unshared!)
//

//-------------------------------------------------------------
// HookProc
// Notice:
// - executed by the instance of "HookInjEx.dll" mapped into "explorer.exe";
//
// When called from InjectDll:
//	  -	sublasses the start button;
//	  -	removes the hook, but the DLL stays in the remote process
//		though, because we increased its reference count via LoadLibray
//		(this way we disturb the target process as litle as possible);
//
// When called from UnmapDll:
//	  -	restores the old window procedure for the start button;
//	  - reduces the reference count of the DLL (via FreeLibrary);
//	  -	removes the hook, so the DLL is unmapped;
//
//		Also note, that the DLL isn't unmapped immediately after the
//		call to UnhookWindowsHookEx, but in the near future
//		(right after finishing with the current message).
//		Actually it's obvious why: windows can NOT unmap the 
//		DLL in the middle of processing a meesage, because the code
//		in the hook procedure is still required. 
//		That's why we can change the order LoadLibrary/FreeLibrary &
//		UnhookWindowsHookEx are called.
//
//		FreeLibrary, in contrast, unmapps the DLL imeditaley if the 
//		reference count reaches zero.
//

LRESULT CallwndHookProc (
  int nCode,       // hook code
  WPARAM wParam,  // virtual-key code = 
  LPARAM lParam   // keystroke-message information
)
{
  if (nCode < 0) // do not process message
    return CallNextHookEx(g_hCallwndHook, nCode, wParam, lParam);

  if (nCode >= HC_ACTION)
  {
    LPMSG msg = (LPMSG)lParam;
    LPCWPSTRUCT cmsg = (LPCWPSTRUCT)lParam;
    if (cmsg->message == WM_HOOKEX && cmsg->lParam ) 
    {
      do{
      //::UnhookWindowsHookEx( g_hCallwndHook );
      //if( g_bSubclassed ) break;;		// already subclassed?

      // Let's increase the reference count of the DLL (via LoadLibrary),
      // so it's NOT unmapped once the hook is removed;
      TCHAR lib_name[MAX_PATH]; 
      ::GetModuleFileName( g_hInstance, lib_name, MAX_PATH );
      if( !::LoadLibrary( lib_name ) )  break;

      installGeHookDll();
      g_bNetHooked = true;
      }while(false);

    }
    else if( cmsg->message == WM_HOOKEX ) 
    {
      do{
      //::UnhookWindowsHookEx( g_hCallwndHook );

      // Failed to restore old window procedure? => Don't unmap the
      // DLL either. Why? Because then "explorer.exe" would call our
      // "unmapped" NewProc and  crash!!
      uninstallGeHookDll();

      ::FreeLibrary( g_hInstance );

      g_bNetHooked = false;
      }while(false);
    }
  }

  return ::CallNextHookEx(g_hCallwndHook, nCode, wParam, lParam);
}




//-------------------------------------------------------------
// InjectDll
// Notice: 
//	- injects "HookInjEx.dll" into "explorer.exe" (via SetWindowsHookEx);
//	- subclasses the START button (see HookProc for more details);
//
//		Parameters: - hWnd = START button handle
//
//		Return value:	1 - success;
//						0 - failure;
//
int InstallCallwndHook( )
{	
	// Hook "explorer.exe"
	g_hCallwndHook = SetWindowsHookEx( WH_CALLWNDPROC,(HOOKPROC)CallwndHookProc,
								g_hInstance, 0 );
	if( g_hCallwndHook==NULL )
		return 0;
	
	// By the time SendMessage returns, 
	// the START button has already been subclassed
	//::SendMessageTimeout(HWND_BROADCAST,WM_HOOKEX,0,1, SMTO_BLOCK, 1500, 0);

	return g_bSubclassed;
}


//-------------------------------------------------------------
// UnmapDll
// Notice: 
//	- restores the old window procedure for the START button;
//	- unmapps the DLL from the remote process
//	  (see HookProc for more details);
//
//		Return value:	1 - success;
//						0 - failure;
//
int UninstallCallwndHook( )
{	
	//::SendMessageTimeout(HWND_BROADCAST,WM_HOOKEX,0,0, SMTO_BLOCK, 1500, 0);

  UnhookWindowsHookEx( g_hCallwndHook);
	g_hCallwndHook =NULL;	

	return (g_bSubclassed == NULL);
}
