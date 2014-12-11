#include "StdAfx.h"
#include "WinInetHook.h"
#include <WinInet.h>
#include "util.h"


#include <WinInet.h>
#include "ncodehook/NCodeHookInstantiation.h"

typedef HINTERNET(__stdcall * LPINTERNETOPNW)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET(__stdcall * LPINTERNETOPNA)(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);
typedef INTERNET_STATUS_CALLBACK(__stdcall * LPINTERNETSETSTATUSCALLBACK)(HINTERNET hInternet, INTERNET_STATUS_CALLBACK lpfnInternetCallback);
typedef HINTERNET(__stdcall * LPINTERNETCONNECTW)(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
typedef HINTERNET(__stdcall * LPINTERNETCONNECTA)(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
typedef HINTERNET(__stdcall * LPHTTPOPENREQUESTW)(HINTERNET hConnect, LPCWSTR lpszVerb, LPCWSTR lpszObjectName, LPCWSTR lpszVersion, LPCWSTR lpszReferrer, LPCWSTR FAR * lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);
typedef HINTERNET(__stdcall * LPHTTPOPENREQUESTA)(HINTERNET hConnect, LPCSTR lpszVerb, LPCSTR lpszObjectName, LPCSTR lpszVersion, LPCSTR lpszReferrer, LPCSTR FAR * lplpszAcceptTypes, DWORD dwFlags, DWORD_PTR dwContext);

class CWinInetHook
{
public:
	CWinInetHook(void);
	virtual ~CWinInetHook(void);
	void InstallHooks(void);

	HINTERNET	InternetOpenW(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
	HINTERNET	InternetOpenA(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);
	BOOL		InternetCloseHandle(HINTERNET hInternet);
	HINTERNET	InternetConnectW(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);
	HINTERNET	InternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext);


private:
	CRITICAL_SECTION	cs;
	bool				hookOpenA;
	NCodeHookIA32		hook;

	LPINTERNETOPNW	_InternetOpenW;
	LPINTERNETOPNA	_InternetOpenA;
	LPINTERNETCONNECTW			_InternetConnectW;
	LPINTERNETCONNECTA			_InternetConnectA;
};
////////////////////////

static CWinInetHook * gs_pWinInetHook = NULL;

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

HINTERNET __stdcall InternetOpenW_Hook(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
	WriteAGLog("InternetOpenW_Hook Begin");
	lpszProxy = _T("http://127.0.0.1:8888");
	dwAccessType = INTERNET_OPEN_TYPE_PROXY;

	HINTERNET ret = NULL;
	__try{
    if(gs_pWinInetHook)
		{
      ret = gs_pWinInetHook->InternetOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
		}
		else
		{
			WriteAGLog("InternetOpenW_Hook gs_pWinInetHook == NULL");
		}
	}__except(1){
		WriteAGLog("InternetOpenW_Hook Failed");
	}
	WriteAGLog("InternetOpenW_Hook End");
	return ret;
}

HINTERNET __stdcall InternetOpenA_Hook(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags)
{
	//
	WriteAGLog("InternetOpenA_Hook Begin");
	lpszProxy = "http://127.0.0.1:8888";
	dwAccessType = INTERNET_OPEN_TYPE_PROXY;
	//
	HINTERNET ret = NULL;
	__try{
    if(gs_pWinInetHook)
      ret = gs_pWinInetHook->InternetOpenA(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
		else
		{
			WriteAGLog("InternetOpenA_Hook gs_pWinInetHook == NULL");
		}
	}__except(1){
		WriteAGLog("InternetOpenA_Hook Failed");
	}
	WriteAGLog("InternetOpenA_Hook End");
	return ret;
}
HINTERNET __stdcall InternetConnectW_Hook(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, LPCWSTR lpszUserName, LPCWSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{
	WriteAGLog("InternetConnectW_Hook Begin");
	HINTERNET ret = NULL;
	__try{
    if(gs_pWinInetHook)
		{
			hInternet = InternetOpenW(L"safe", 0, NULL, NULL, 0);
      ret = gs_pWinInetHook->InternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
		}
	}__except(1){
		WriteAGLog("InternetConnectW_Hook Failed");
	}
	WriteAGLog("InternetConnectW_Hook End");
	return ret;
}

HINTERNET __stdcall InternetConnectA_Hook(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{
	WriteAGLog("InternetConnectA_Hook Begin");
	HINTERNET ret = NULL;
	__try{
    if(gs_pWinInetHook)
		{
			hInternet = InternetOpenA("safe", 0, NULL, NULL, 0);
      ret = gs_pWinInetHook->InternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);
		}
	}__except(1){
		WriteAGLog("InternetConnectW_Hook Failed");
	}
	WriteAGLog("InternetConnectA_Hook End");
	return ret;
}


CWinInetHook::CWinInetHook(void):
hookOpenA(true)
{
	WriteAGLog("CWinInetHook");
	InitializeCriticalSection(&cs);

	//
	//parents.InitHashTable(257);

	_InternetOpenW = hook.createHookByName("wininet.dll", "InternetOpenW", InternetOpenW_Hook);
	_InternetOpenA = hook.createHookByName("wininet.dll", "InternetOpenA", InternetOpenA_Hook);
	if (_InternetOpenW==NULL && _InternetOpenA==NULL)
	{
		::MessageBox(0, _T("apihooked"), _T("CWinInetHook"), MB_OK);
	}

	_InternetConnectW = hook.createHookByName("wininet.dll", "InternetConnectW", InternetConnectW_Hook);
	_InternetConnectA = hook.createHookByName("wininet.dll", "InternetConnectA", InternetConnectA_Hook);
}

CWinInetHook::~CWinInetHook(void)
{
  if(gs_pWinInetHook == this)
    gs_pWinInetHook = NULL;

	DeleteCriticalSection(&cs);
	WriteAGLog("~CWinInetHook");
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
		ret = _InternetConnectW(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);


	return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
HINTERNET CWinInetHook::InternetConnectA(HINTERNET hInternet, LPCSTR lpszServerName, INTERNET_PORT nServerPort, LPCSTR lpszUserName, LPCSTR lpszPassword, DWORD dwService, DWORD dwFlags, DWORD_PTR dwContext)
{
	HINTERNET ret = NULL;

	if( _InternetConnectA )
		ret = _InternetConnectA(hInternet, lpszServerName, nServerPort, lpszUserName, lpszPassword, dwService, dwFlags, dwContext);

	return ret;
}
