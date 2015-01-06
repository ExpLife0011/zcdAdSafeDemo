#include "StdAfx.h"
#include "hook_winhttp.h"
#include "util.h"

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

HINTERNET __stdcall WinHttpOpen_Hook(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
  //
  WriteAGLog("WinHttpOpen_Hook Begin");
  lpszProxy = _T("http://127.0.0.1:8888");
  dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
  //

  HINTERNET ret = NULL;
  __try{
    if(gs_pWinHttpHook)
    {
      ret = gs_pWinHttpHook->WinHttpOpen(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
    }
    else
    {
      WriteAGLog("WinHttpOpen_Hook gs_pWinHttpHook==NULL");
    }
  }__except(1){
    WriteAGLog("WinHttpOpen_Hook Failed");
  }

  WriteAGLog("WinHttpOpen_Hook End");
  return ret;
}

HINTERNET __stdcall WinHttpOpenA_Hook(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags)
{
  //
  lpszProxy = "http://127.0.0.1:8888";
  dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
  //

  HINTERNET ret = NULL;
  __try{
    if(gs_pWinHttpHook)
    {
      ret = gs_pWinHttpHook->WinHttpOpenA(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
    }
  }__except(1){}
  return ret;
}

HINTERNET __stdcall WinHttpConnectW_Hook(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  HINTERNET ret = NULL;
  __try{
    if (gs_pWinHttpHook)
    {
      //hInternet = WinHttpOpenW(L"agsafe", NULL, NULL, 0);
      ret = gs_pWinHttpHook->WinHttpConnectW(hInternet, lpszServerName, nServerPort, dwReserved);
    }
  }__except(1){}
  return ret;
}

HINTERNET __stdcall WinHttpConnect_Hook(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  WriteAGLog("WinHttpConnect_Hook Begin");
  HINTERNET ret = NULL;
  __try{
    if (gs_pWinHttpHook)
    {
      //hInternet = WinHttpOpen(L"agsafe", 0, NULL, NULL, 0);
      USES_CONVERSION;
      WriteAGLog(W2CA(lpszServerName));
      char buf[10];
      itoa(nServerPort, buf, 10);
      WriteAGLog(buf);
      ret = gs_pWinHttpHook->WinHttpConnect(hInternet, lpszServerName, nServerPort, dwReserved);
    }
    else
      WriteAGLog("WinHttpConnect_Hook gs_pWinHttpHook==NULL");
  }__except(1){
    WriteAGLog("WinHttpConnect_Hook Failed");
  }
  WriteAGLog("WinHttpConnect_Hook End");
  return ret;
}

HINTERNET __stdcall WinHttpConnectA_Hook(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  HINTERNET ret = NULL;
  __try{
    if (gs_pWinHttpHook)
    {
      //hInternet = WinHttpOpenA("agsafe", NULL, NULL, 0);
      ret = gs_pWinHttpHook->WinHttpConnectA(hInternet, lpszServerName, nServerPort, dwReserved);
    }
  }__except(1){}
  return ret;
}

HINTERNET __stdcall WinHttpOpenRequest_Hook(HINTERNET hInternet, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR FAR * plpszAcceptTypes, DWORD dwFlags)
{
  WriteAGLog("WinHttpOpenRequest_Hook Begin");
  HINTERNET ret = NULL;
  __try{
    if (gs_pWinHttpHook)
    {
      USES_CONVERSION;
      WriteAGLog(W2CA(lpszObjectName));
      ret = gs_pWinHttpHook->WinHttpOpenRequest(hInternet, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, plpszAcceptTypes, dwFlags);
    }
  }__except(1){}
  WriteAGLog("WinHttpOpenRequest_Hook Begin");
  return ret;
}

BOOL __stdcall WinHttpGetProxyForUrl_Hook(HINTERNET hInternet, LPCWSTR lpszUrl, WINHTTP_AUTOPROXY_OPTIONS *pAutoProxyOptions, OUT WINHTTP_PROXY_INFO * pProxyInfo)
{
  WriteAGLog("WinHttpGetProxyForUrl_Hook Begin");
  BOOL ret = FALSE;
  __try{
    if (gs_pWinHttpHook)
    {
      USES_CONVERSION;
      WriteAGLog(W2CA(lpszUrl));
      ret = gs_pWinHttpHook->WinHttpGetProxyForUrl(hInternet, lpszUrl, pAutoProxyOptions, pProxyInfo);
    }
  }__except(1){}
  WriteAGLog("WinHttpGetProxyForUrl_Hook End");
  return ret;
}

CWinHttpHook::CWinHttpHook():
_WinHttpOpen(NULL),_WinHttpConnect(NULL),_WinHttpOpenRequest(NULL),_WinHttpGetProxyForUrl(NULL)
{
  WriteAGLog("CWinHttpHook");
  InitializeCriticalSection(&cs);

}

void CWinHttpHook::Init()
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

CWinHttpHook::~CWinHttpHook()
{
  if (gs_pWinHttpHook == this)
  {
    gs_pWinHttpHook = NULL;
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
    WriteAGLog("CWinHttpHook::WinHttpOpen() -> _WinHttpOpen!=NULL");
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
    ret = _WinHttpConnectW(hInternet, lpszServerName, nServerPort, dwReserved);

  return ret;
}

HINTERNET CWinHttpHook::WinHttpConnect(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  HINTERNET ret = NULL;

  if(_WinHttpConnect)
    ret = _WinHttpConnect(hInternet, lpszServerName, nServerPort, dwReserved);

  return ret;
}

HINTERNET CWinHttpHook::WinHttpConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
  HINTERNET ret = NULL;

  if(_WinHttpConnectA)
    ret = _WinHttpConnectA(hInternet, lpszServerName, nServerPort, dwReserved);

  return ret;
}

HINTERNET CWinHttpHook::WinHttpOpenRequest(HINTERNET hInternet, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR FAR * plpszAcceptTypes, DWORD dwFlags)
{
  HINTERNET ret = NULL;

  if (_WinHttpOpenRequest)
    ret = _WinHttpOpenRequest(hInternet, lpszVerb, lpszObjectName, lpszVersion, lpszReferrer, plpszAcceptTypes, dwFlags);

  return ret;
}

BOOL CWinHttpHook::WinHttpGetProxyForUrl(HINTERNET hInternet, LPCWSTR lpszUrl, WINHTTP_AUTOPROXY_OPTIONS *pAutoProxyOptions, OUT WINHTTP_PROXY_INFO * pProxyInfo)
{
  BOOL ret = FALSE;

  if(_WinHttpGetProxyForUrl)
    ret = _WinHttpGetProxyForUrl(hInternet, lpszUrl, pAutoProxyOptions, pProxyInfo);

  return ret;
}
