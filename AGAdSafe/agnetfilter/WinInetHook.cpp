#include "StdAfx.h"
#include "WinInetHook.h"
#include "util.h"

#include "hook_wininet.h"
#include "hook_wininet.cc"

BOOL WinInetInstallHooks(void)
{
  if(!gs_pWinInetHook)
    gs_pWinInetHook = new CWinInetHook();
  return gs_pWinInetHook!=NULL;
}

void WinInetRemoveHooks(void)
{
  if(gs_pWinInetHook)
	{
    delete gs_pWinInetHook;
    gs_pWinInetHook = NULL;
		WriteAGLog("WinInetRemoveHooks");
	}
}
