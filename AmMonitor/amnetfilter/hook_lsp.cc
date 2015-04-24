#include "StdAfx.h"
#include "hook_lsp.h"
#include "zcdbg.h"

static CWinHttpHook * gs_pWinHttpHook = NULL;

HINTERNET __stdcall WinHttpOpenW_Hook(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
  //
  lpszProxy = _T("http://127.0.0.1:8888");
  dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
  //

  HINTERNET ret = NULL;
  __try{
    if(gs_pWinHttpHook)
    {
      ret = gs_pWinHttpHook->WinHttpOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
    }
  }__except(1){}
  return ret;
}


CMSWSockHook::CMSWSockHook():
_WinHttpOpen(NULL),_WinHttpConnect(NULL),_WinHttpOpenRequest(NULL),_WinHttpGetProxyForUrl(NULL)
{
  WriteAGLog("CWinHttpHook");
  InitializeCriticalSection(&cs);

}

void CMSWSockHook::Init()
{
  //

  //_WinHttpOpenW = hook.createHookByName("winhttp.dll", "WinHttpOpenW", WinHttpOpenW_Hook);
  //_WinHttpOpenA = hook.createHookByName("winhttp.dll", "WinHttpOpenA", WinHttpOpenA_Hook);
  //if(_WinHttpOpenW==NULL && _WinHttpOpenA==NULL)
  //{
    //::MessageBox(0, _T("apihooked"), _T("CWinHttpHook"), MB_OK);
  //}

  //_WinHttpConnectW = hook.createHookByName("winhttp.dll", "WinHttpConnectW", WinHttpConnectW_Hook);
  //_WinHttpConnectA = hook.createHookByName("winhttp.dll", "WinHttpConnectA", WinHttpConnectA_Hook);

  _WinHttpOpen = hook.createHookByName("winhttp.dll", "WinHttpOpen", WinHttpOpen_Hook);
  _WinHttpConnect = hook.createHookByName("winhttp.dll", "WinHttpConnect", WinHttpConnect_Hook);
  _WinHttpOpenRequest = hook.createHookByName("winhttp.dll", "WinHttpOpenRequest", WinHttpOpenRequest_Hook);
  _WinHttpGetProxyForUrl = hook.createHookByName("winhttp.dll", "WinHttpGetProxyForUrl", WinHttpGetProxyForUrl_Hook);
  if ((_WinHttpOpen==NULL) || (_WinHttpConnect == NULL) || (_WinHttpOpenRequest==NULL) || (_WinHttpGetProxyForUrl==NULL))
  {
    WriteAGLog("_WinHttpOpen==NULL or _WinHttpConnect==NULL or _WinHttpOpenRequest==NULL or _WinHttpGetProxyForUrl==NULL");
  }
}
void CMSWSockHook::Destroy()
{}
CMSWSockHook::~CWinHttpHook()
{
  if (gs_pWinHttpHook == this)
  {
    gs_pWinHttpHook = NULL;
  }

  DeleteCriticalSection(&cs);
}

HINTERNET CMSWSockHook::WinHttpOpenW(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
  HINTERNET ret = NULL;

  if(_WinHttpOpenW)
  {
    ret = _WinHttpOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
  }

  return ret;
}
