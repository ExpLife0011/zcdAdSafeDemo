
#include "stdafx.h"
#include "installhook.h"
#include "shared_mem.h"
#include "util.h"
#include "wpthook.h"

BOOL g_bNetHooked = false;
HOOK g_hCallwndHook = NULL;
//-------------------------------------------------------------
// global variables (unshared!)
//

//-------------------------------------------------------------
// HookProc
// Notice:
// - executed by the instance of "HookInjEx.dll" mapped into "explorer.exe";
//
// When called from InjectDll:
//    -  sublasses the start button;
//    -  removes the hook, but the DLL stays in the remote process
//    though, because we increased its reference count via LoadLibray
//    (this way we disturb the target process as litle as possible);
//
// When called from UnmapDll:
//    -  restores the old window procedure for the start button;
//    - reduces the reference count of the DLL (via FreeLibrary);
//    -  removes the hook, so the DLL is unmapped;
//
//    Also note, that the DLL isn't unmapped immediately after the
//    call to UnhookWindowsHookEx, but in the near future
//    (right after finishing with the current message).
//    Actually it's obvious why: windows can NOT unmap the
//    DLL in the middle of processing a meesage, because the code
//    in the hook procedure is still required.
//    That's why we can change the order LoadLibrary/FreeLibrary &
//    UnhookWindowsHookEx are called.
//
//    FreeLibrary, in contrast, unmapps the DLL imeditaley if the
//    reference count reaches zero.
//

LRESULT AmhCallwndHookProc (
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

      installGeHookDll();
      }while(false);

    }
    else if( cmsg->message == WM_HOOKEX )
    {
      do{
      uninstallGeHookDll();

      }while(false);
    }
  }

  return ::CallNextHookEx(g_hCallwndHook, nCode, wParam, lParam);
}

//-------------------------------------------------------------
// InjectDll
// Notice:
//  - injects "HookInjEx.dll" into "explorer.exe" (via SetWindowsHookEx);
//  - subclasses the START button (see HookProc for more details);
//
//    Parameters: - hWnd = START button handle
//
//    Return value:  1 - success;
//            0 - failure;
//
int InstallCallwndHook( )
{
  // Hook "explorer.exe"
  UINT id = GetCurrentProcessId();
  g_hCallwndHook = SetWindowsHookEx( WH_CALLWNDPROC,(HOOKPROC)AmhCallwndHookProc,g_hInstance, id );

  if( g_hCallwndHook==NULL )
    return 0;

  return (g_hCallwndHook != NULL);
}


//-------------------------------------------------------------
// UnmapDll
// Notice:
//  - restores the old window procedure for the START button;
//  - unmapps the DLL from the remote process
//    (see HookProc for more details);
//
//    Return value:  1 - success;
//            0 - failure;
//
int UninstallCallwndHook( )
{
  //::SendMessageTimeout(HWND_BROADCAST,WM_HOOKEX,0,0, SMTO_BLOCK, 1500, 0);

  UnhookWindowsHookEx( g_hCallwndHook);
  g_hCallwndHook =NULL;

  return (g_hCallwndHook == NULL);
}
