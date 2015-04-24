#include "StdAfx.h"
#include "hook_winhttp.h"
#include "shared_mem.h"
#include "zcdbg.h"

static CWinHttpHook * gs_pWinHttpHook = NULL;

HINTERNET WINAPI AmhHook_WinHttpOpen(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
  //
  OutputDebugLog("WinHttpOpen_Hook Begin");

  if(shared_proxy_enabled)
  {
    lpszProxy = shared_proxy_inetW;
    dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
  }
  //

  HINTERNET ret = NULL;
  __try{
    if(gs_pWinHttpHook)
    {
      ret = gs_pWinHttpHook->WinHttpOpen(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
    }
    else
    {
      OutputDebugLog("WinHttpOpen_Hook gs_pWinHttpHook==NULL");
    }
  }__except(1){
    OutputDebugLog("WinHttpOpen_Hook Failed");
  }

  OutputDebugLog("WinHttpOpen_Hook End");
  return ret;
}

HINTERNET WINAPI AmhHook_WinHttpConnect(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  OutputDebugLog("WinHttpConnect_Hook Begin");
  HINTERNET ret = NULL;
  __try{
    if (gs_pWinHttpHook)
    {
      ret = gs_pWinHttpHook->WinHttpConnect(hInternet, lpszServerName, nServerPort, dwReserved);
    }
    else
    {
      OutputDebugLog("WinHttpConnect_Hook gs_pWinHttpHook==NULL");
    }
  }__except(1){
    OutputDebugLog("WinHttpConnect_Hook Failed");
  }
  OutputDebugLog("WinHttpConnect_Hook End");
  return ret;
}

HINTERNET WINAPI AmhHook_WinHttpOpenRequest(HINTERNET hInternet, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR FAR * plpszAcceptTypes, DWORD dwFlags)
{
  OutputDebugLog("WinHttpOpenRequest_Hook Begin");
  HINTERNET ret = NULL;
  __try{
    if (gs_pWinHttpHook)
    {
      ret = gs_pWinHttpHook->WinHttpOpenRequest(hInternet, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, plpszAcceptTypes, dwFlags);
    }
    else
    {
      OutputDebugLog("WinHttpOpenRequest_Hook gs_pWinHttpHook==NULL");
    }
  }__except(1){
    OutputDebugLog("WinHttpConnect_Hook Failed");
  }
  OutputDebugLog("WinHttpOpenRequest_Hook Begin");
  return ret;
}

BOOL WINAPI  AmhHook_WinHttpGetProxyForUrl(HINTERNET hInternet, LPCWSTR lpszUrl, WINHTTP_AUTOPROXY_OPTIONS *pAutoProxyOptions, OUT WINHTTP_PROXY_INFO * pProxyInfo)
{
  OutputDebugLog("WinHttpGetProxyForUrl_Hook Begin");
  BOOL ret = FALSE;
  __try{
    if (gs_pWinHttpHook)
    {
      ret = gs_pWinHttpHook->WinHttpGetProxyForUrl(hInternet, lpszUrl, pAutoProxyOptions, pProxyInfo);
    }
    else
    {
      OutputDebugLog("WinHttpGetProxyForUrl_Hook gs_pWinHttpHook==NULL");
    }
  }__except(1){
    OutputDebugLog("WinHttpConnect_Hook Failed");
  }
  OutputDebugLog("WinHttpGetProxyForUrl_Hook End");
  return ret;
}

CWinHttpHook::CWinHttpHook():
_WinHttpOpen(NULL),_WinHttpConnect(NULL),_WinHttpOpenRequest(NULL),_WinHttpGetProxyForUrl(NULL)
{
  OutputDebugLog("CWinHttpHook");
  InitializeCriticalSection(&cs);

}

void CWinHttpHook::Init()
{
  if (gs_pWinHttpHook || mhook)
    return;

  gs_pWinHttpHook = this;
  mhook =  new NCodeHookIA32();

  //
  _WinHttpOpen =  mhook->createHookByName("winhttp.dll", "WinHttpOpen",  AmhHook_WinHttpOpen);
  _WinHttpConnect =  mhook->createHookByName("winhttp.dll", "WinHttpConnect",  AmhHook_WinHttpConnect);
  _WinHttpOpenRequest =  mhook->createHookByName("winhttp.dll", "WinHttpOpenRequest",  AmhHook_WinHttpOpenRequest);
  _WinHttpGetProxyForUrl =  mhook->createHookByName("winhttp.dll", "WinHttpGetProxyForUrl",  AmhHook_WinHttpGetProxyForUrl);
  if ((_WinHttpOpen==NULL) || (_WinHttpConnect == NULL) || (_WinHttpOpenRequest==NULL) || (_WinHttpGetProxyForUrl==NULL))
  {
    OutputDebugLog("_WinHttpOpen==NULL or _WinHttpConnect==NULL or _WinHttpOpenRequest==NULL or _WinHttpGetProxyForUrl==NULL");
  }
}

void CWinHttpHook::Destroy()
{
  if( mhook)
  {
    delete mhook;
    mhook = NULL;
  }


  _WinHttpOpen = NULL;
  _WinHttpConnect = NULL;

  _WinHttpOpenRequest = NULL;
  _WinHttpGetProxyForUrl = NULL;
}

CWinHttpHook::~CWinHttpHook()
{
  if (gs_pWinHttpHook == this)
  {
    gs_pWinHttpHook = NULL;
    Destroy();
  }

  DeleteCriticalSection(&cs);
}

HINTERNET CWinHttpHook::WinHttpOpenW(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
  HINTERNET ret = NULL;

  if(_WinHttpOpenW)
  {
    ret = _WinHttpOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
  }

  return ret;
}

HINTERNET CWinHttpHook::WinHttpOpen(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
  HINTERNET ret = NULL;

  if(_WinHttpOpen)
  {
    OutputDebugLog("CWinHttpHook::WinHttpOpen() -> _WinHttpOpen!=NULL");
    ret = _WinHttpOpen(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
  }

  return ret;
}

HINTERNET CWinHttpHook::WinHttpOpenA(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags)
{
  HINTERNET ret = NULL;

  if (_WinHttpOpenA)
  {
    ret = _WinHttpOpenA(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
  }

  return ret;
}

HINTERNET CWinHttpHook::WinHttpConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  HINTERNET ret = NULL;

  if(_WinHttpConnectW)
  {
    ret = _WinHttpConnectW(hInternet, lpszServerName, nServerPort, dwReserved);
  }

  return ret;
}

HINTERNET CWinHttpHook::WinHttpConnect(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  HINTERNET ret = NULL;

  if(_WinHttpConnect)
  {
    ret = _WinHttpConnect(hInternet, lpszServerName, nServerPort, dwReserved);
  }

  return ret;
}

HINTERNET CWinHttpHook::WinHttpConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  HINTERNET ret = NULL;

  if(_WinHttpConnectA)
  {
    ret = _WinHttpConnectA(hInternet, lpszServerName, nServerPort, dwReserved);
  }

  return ret;
}

HINTERNET CWinHttpHook::WinHttpOpenRequest(HINTERNET hInternet, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR FAR * plpszAcceptTypes, DWORD dwFlags)
{
  HINTERNET ret = NULL;

  if (_WinHttpOpenRequest)
  {
    ret = _WinHttpOpenRequest(hInternet, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, plpszAcceptTypes, dwFlags);
  }

  return ret;
}

BOOL CWinHttpHook::WinHttpGetProxyForUrl(HINTERNET hInternet, LPCWSTR lpszUrl, WINHTTP_AUTOPROXY_OPTIONS *pAutoProxyOptions, OUT WINHTTP_PROXY_INFO * pProxyInfo)
{
  BOOL ret = FALSE;

  if(_WinHttpGetProxyForUrl)
  {
    ret = _WinHttpGetProxyForUrl(hInternet, lpszUrl, pAutoProxyOptions, pProxyInfo);
  }

  return ret;
}
