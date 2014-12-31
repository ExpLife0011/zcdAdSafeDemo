#include "StdAfx.h"
#include "WinsockHook.h"
#include "util.h"

#include "hook_ws2.h"
static CWs2Hook * gs_pWinsockHook = NULL;


BOOL WinsockInstallHooks()
{
  if(!gs_pWinsockHook)
    gs_pWinsockHook = new CWs2Hook();
  return gs_pWinsockHook!=NULL;
}

void WinsockRemoveHooks()
{
  if (gs_pWinsockHook)
	{
    delete gs_pWinsockHook;
    gs_pWinsockHook = NULL;
	}
}

#include "hook_ws2.cc"
