#pragma once
#include "stdafx.h"
#include "hook_winsock.h"

static CWinsockHook * gs_pWptsockHook = NULL;


BOOL WptsockInstallHooks()
{
  if(!gs_pWptsockHook)
    gs_pWptsockHook = new CWinsockHook();
  return gs_pWptsockHook!=NULL;
}

void WptsockRemoveHooks()
{
  if (gs_pWptsockHook)
	{
    delete gs_pWptsockHook;
    gs_pWptsockHook = NULL;
	}
}

#include "hook_winsock.cc"