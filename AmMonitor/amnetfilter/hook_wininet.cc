#include "StdAfx.h"
#include "hook_wininet.h"
#include "shared_mem.h"
#include "zcdbg.h"

static CWinInetHook * gs_pWinInetHook = NULL;

HINTERNET WINAPI AmhHook_InternetOpenW(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
  OutputDebugLog("InternetOpenW_Hook Begin");
  if(shared_proxy_enabled)
  {
    lpszProxy = shared_proxy_inetW;
    dwAccessType = INTERNET_OPEN_TYPE_PROXY;
  }

  HINTERNET ret = NULL;
  __try{
    if(gs_pWinInetHook)
    {
      ret = gs_pWinInetHook->InternetOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
    }
    else
    {
      OutputDebugLog("InternetOpenW_Hook gs_pWinInetHook == NULL");
    }
  }__except(1){
    OutputDebugLog("InternetOpenW_Hook Failed");
  }
  OutputDebugLog("InternetOpenW_Hook End");
  return ret;
}

HINTERNET WINAPI AmhHook_InternetOpenA(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags)
{
  //
  OutputDebugLog("InternetOpenA_Hook Begin");
  if(shared_proxy_enabled)
  {
    lpszProxy = shared_proxy_inetA;
    dwAccessType = INTERNET_OPEN_TYPE_PROXY;
  }
  //
  HINTERNET ret = NULL;
  __try{
    if(gs_pWinInetHook)
    {
      ret = gs_pWinInetHook->InternetOpenA(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
    }
    else
    {
      OutputDebugLog("InternetOpenA_Hook gs_pWinInetHook == NULL");
    }
  }__except(1){
    OutputDebugLog("InternetOpenA_Hook Failed");
  }
  OutputDebugLog("InternetOpenA_Hook End");
  return ret;
}
HINTERNET WINAPI AmhHook_InternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{
  OutputDebugLog("InternetConnectW_Hook Begin");
  HINTERNET ret = NULL;
  __try{
    if(gs_pWinInetHook)
    {
      hInternet = InternetOpenW(L"safe", 0, NULL, NULL, 0);
      ret = gs_pWinInetHook->InternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
    }
    else
    {
      OutputDebugLog("InternetConnectW_Hook gs_pWinInetHook == NULL");
    }
  }__except(1){
  OutputDebugLog("InternetConnectW_Hook Failed");
  }
  OutputDebugLog("InternetConnectW_Hook End");
  return ret;
}

HINTERNET WINAPI AmhHook_InternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{
  OutputDebugLog("InternetConnectA_Hook Begin");
  HINTERNET ret = NULL;
  __try{
    if(gs_pWinInetHook)
    {
      hInternet = InternetOpenA("safe", 0, NULL, NULL, 0);
      ret = gs_pWinInetHook->InternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
    }
    else
    {
      OutputDebugLog("InternetConnectA_Hook gs_pWinInetHook == NULL");
    }
  }__except(1){
    OutputDebugLog("InternetConnectA_Hook Failed");
  }
  OutputDebugLog("InternetConnectA_Hook End");
  return ret;
}


CWinInetHook::CWinInetHook(void):
hookOpenA(true),hook_(0)
{
  OutputDebugLog("CWinInetHook");
  InitializeCriticalSection(&cs);

}

CWinInetHook::~CWinInetHook(void)
{
  if(gs_pWinInetHook == this)
  {
    gs_pWinInetHook = NULL;
    this->Destroy();
  }
  DeleteCriticalSection(&cs);
  OutputDebugLog("~CWinInetHook");
}

void CWinInetHook::Destroy(void)
{
  OutputDebugLog("CWinInetHook::Destroy");
  if( hook_)
  {
    delete hook_;
    hook_ = NULL;
  }

  _InternetOpenW = NULL;
  _InternetOpenA = NULL;
  _InternetConnectW = NULL;
  _InternetConnectA = NULL;

}

void CWinInetHook::Init(void)
{

  if (gs_pWinInetHook || hook_)
    return;

  gs_pWinInetHook = this;
  hook_ =  new NCodeHookIA32();

 _InternetOpenW = hook_->createHookByName("wininet.dll", "InternetOpenW", AmhHook_InternetOpenW);
 _InternetOpenA = hook_->createHookByName("wininet.dll", "InternetOpenA", AmhHook_InternetOpenA);

 _InternetConnectW = hook_->createHookByName("wininet.dll", "InternetConnectW", AmhHook_InternetConnectW);
 _InternetConnectA = hook_->createHookByName("wininet.dll", "InternetConnectA", AmhHook_InternetConnectA);

}

HINTERNET CWinInetHook::InternetOpenW(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
  HINTERNET ret = NULL;

  if ( _InternetOpenW )
  {
    ret = _InternetOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
  }

 return ret;
}

HINTERNET CWinInetHook::InternetOpenA(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags)
{
  HINTERNET ret = NULL;

  if (_InternetOpenA)
  {
    ret = _InternetOpenA(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
  }

  return ret;
}

HINTERNET CWinInetHook::InternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{
  HINTERNET ret = NULL;

  if( _InternetConnectW )
  {
    ret = _InternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
  }

  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
HINTERNET CWinInetHook::InternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{
  HINTERNET ret = NULL;

  if( _InternetConnectA )
  {
    ret = _InternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
  }

  return ret;
}
