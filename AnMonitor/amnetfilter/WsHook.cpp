#include "StdAfx.h"
#include "WsHook.h"

#include "hook_ws2.h"
static CWs2Hook * gs_pWs2Hook = NULL;


BOOL WsInstallHooks()
{
  if(!gs_pWs2Hook)
  {
    gs_pWs2Hook = new CWs2Hook();
    gs_pWs2Hook->Init();
  }
  return gs_pWs2Hook!=NULL;
}

void WsRemoveHooks()
{
  if (gs_pWs2Hook)
	{
    delete gs_pWs2Hook;
    gs_pWs2Hook = NULL;
	}
}

//#include "hook_ws2.cc"
