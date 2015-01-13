#pragma once


//#include <atlbase.h>
//#include <atlwin.h>
#include <atlconv.h>
#include <winhttp.h>
#include "ncodehook/NCodeHookInstantiation.h"

typedef HINTERNET(WINAPI * LPFN_WINHTTPOPENW)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET(WINAPI * LPFN_WINHTTPOPENA)(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET(WINAPI * LPFN_WINHTTPCONNECTW)(HINTERNET hSession, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);
typedef HINTERNET(WINAPI * LPFN_WINHTTPCONNECTA)(HINTERNET hSession, LPCSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);

typedef HINTERNET(WINAPI * LPFN_WINHTTPOPEN)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET(WINAPI * LPFN_WINHTTPCONNECT)(HINTERNET hSession, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);

typedef HINTERNET(WINAPI * LPFN_WINHTTPOPENREQUEST)(HINTERNET hInternet, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR FAR * plpszAcceptTypes, DWORD dwFlags);
typedef BOOL(WINAPI * LPFN_WINHTTPGETPROXYFORURL)(HINTERNET hInternet, LPCWSTR lpszUrl, WINHTTP_AUTOPROXY_OPTIONS *pAutoProxyOptions, OUT WINHTTP_PROXY_INFO * pProxyInfo);


class CWinHttpHook
{
public:
  CWinHttpHook(void);
  virtual ~CWinHttpHook(void);
  void Init(void);
  void Destroy(void);
  
  HINTERNET WinHttpOpenW(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
  HINTERNET WinHttpOpenA(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);

  HINTERNET WinHttpConnectW(HINTERNET hSession, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);
  HINTERNET WinHttpConnectA(HINTERNET hSession, LPCSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);

  HINTERNET WinHttpOpen(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
  HINTERNET WinHttpConnect(HINTERNET hSession, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);

HINTERNET WinHttpOpenRequest(HINTERNET hInternet, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR FAR * plpszAcceptTypes, DWORD dwFlags);
BOOL WinHttpGetProxyForUrl(HINTERNET hInternet, LPCWSTR lpszUrl, WINHTTP_AUTOPROXY_OPTIONS *pAutoProxyOptions, OUT WINHTTP_PROXY_INFO * pProxyInfo);

private:
  bool hookOpenA;
  CRITICAL_SECTION cs;
  NCodeHookIA32* mhook;


  LPFN_WINHTTPOPENW _WinHttpOpenW;
  LPFN_WINHTTPOPENA _WinHttpOpenA;
  LPFN_WINHTTPCONNECTW _WinHttpConnectW;
  LPFN_WINHTTPCONNECTA _WinHttpConnectA;

  LPFN_WINHTTPOPEN  _WinHttpOpen;
  LPFN_WINHTTPCONNECT _WinHttpConnect;

  LPFN_WINHTTPOPENREQUEST  _WinHttpOpenRequest;
  LPFN_WINHTTPGETPROXYFORURL _WinHttpGetProxyForUrl;
};

