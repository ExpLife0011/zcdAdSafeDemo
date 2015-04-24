#include "StdAfx.h"
#include "WinHttpHook.h"
#include "util.h"

#include "hook_winhttp.h"

#include "hook_winhttp.cc"

BOOL WinHttpInstallHooks(void)
{
  if(!gs_pWinHttpHook)
    gs_pWinHttpHook = new CWinHttpHook();
  return gs_pWinHttpHook!=NULL;
}

void WinHttpRemoveHooks(void)
{
  if(gs_pWinHttpHook)
	{
    delete gs_pWinHttpHook;
    gs_pWinHttpHook = NULL;
		WriteAGLog("WinHttpRemoveHooks");
	}
}

