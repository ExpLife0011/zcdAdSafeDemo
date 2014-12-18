#pragma once
#include "stdafx.h"
#include "hook_winsock.h"

static CWsHook * gs_pWptsockHook = NULL;


BOOL WptsockInstallHooks()
{
  if(!gs_pWptsockHook)
    gs_pWptsockHook = new CWsHook();
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