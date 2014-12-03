#include "StdAfx.h"
#include "WinHttpHook.h"
#include "util.h"

#include <winhttp.h>
#include "ncodehook/NCodeHookInstantiation.h"

typedef HINTERNET(__stdcall * LPWINHTTPOPENW)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET(__stdcall * LPWINHTTPOPENA)(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET(__stdcall * LPWINHTTPCONNECTW)(HINTERNET hSession, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);
typedef HINTERNET(__stdcall * LPWINHTTPCONNECTA)(HINTERNET hSession, LPCSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);

typedef HINTERNET(__stdcall * LPWINHTTPOPEN)(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
typedef HINTERNET(__stdcall * LPWINHTTPCONNECT)(HINTERNET hSession, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);


class CWinHttpHook
{
public:
	CWinHttpHook(void);
	virtual ~CWinHttpHook(void);
	void InstallHooks(void);

	HINTERNET WinHttpOpenW(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
	HINTERNET WinHttpOpenA(LPCSTR lpszAgent, DWORD dwAccessType, LPCSTR lpszProxy, LPCSTR lpszProxyBypass, DWORD dwFlags);
	HINTERNET WinHttpConnectW(HINTERNET hSession, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);
	HINTERNET WinHttpConnectA(HINTERNET hSession, LPCSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);

	HINTERNET WinHttpOpen(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);
	HINTERNET WinHttpConnect(HINTERNET hSession, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved);

private:
	bool				hookOpenA;
	CRITICAL_SECTION	cs;
	NCodeHookIA32		hook;


	LPWINHTTPOPENW	_WinHttpOpenW;
	LPWINHTTPOPENA	_WinHttpOpenA;
	LPWINHTTPCONNECTW _WinHttpConnectW;
	LPWINHTTPCONNECTA _WinHttpConnectA;

	LPWINHTTPOPEN		_WinHttpOpen;
	LPWINHTTPCONNECT	_WinHttpConnect;
};


static CWinHttpHook * pHook = NULL;

BOOL WinHttpInstallHooks(void)
{
	if(!pHook)
		pHook = new CWinHttpHook();
	return pHook!=NULL;
}

void WinHttpRemoveHooks(void)
{
	if(pHook)
	{
		delete pHook;
		pHook = NULL;
		WriteAGLog("WinHttpRemoveHooks");
	}
}

HINTERNET __stdcall WinHttpOpenW_Hook(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags)
{
	//
	lpszProxy = _T("http://127.0.0.1:8888");
	dwAccessType = WINHTTP_ACCESS_TYPE_NAMED_PROXY;
	//

	HINTERNET ret = NULL;
	__try{
		if(pHook)
		{
			ret = pHook->WinHttpOpenW(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
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
		if(pHook)
		{
			ret = pHook->WinHttpOpen(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
		}
		else
		{
			WriteAGLog("WinHttpOpen_Hook pHook==NULL");
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
		if(pHook)
		{
			ret = pHook->WinHttpOpenA(lpszAgent, dwAccessType, lpszProxy, lpszProxyBypass, dwFlags);
		}
	}__except(1){}
	return ret;
}

HINTERNET __stdcall WinHttpConnectW_Hook(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
	HINTERNET ret = NULL;
	__try{
		if (pHook)
		{
			//hInternet = WinHttpOpenW(L"agsafe", NULL, NULL, 0);
			ret = pHook->WinHttpConnectW(hInternet, lpszServerName, nServerPort, dwReserved);
		}
	}__except(1){}
	return ret;
}

HINTERNET __stdcall WinHttpConnect_Hook(HINTERNET hInternet, LPCWSTR lpszServerName, INTERNET_PORT nServerPort, DWORD dwReserved)
{
	WriteAGLog("WinHttpConnect_Hook Begin");
	HINTERNET ret = NULL;
	__try{
		if (pHook)
		{
			hInternet = WinHttpOpen(L"agsafe", 0, NULL, NULL, 0);
			//::WideCharToMultiByte(CP_ACP, 0, lpszServerName, )
			//WriteAGLog()
			ret = pHook->WinHttpConnect(hInternet, lpszServerName, nServerPort, dwReserved);
		}
		else
			WriteAGLog("WinHttpConnect_Hook pHook==NULL");
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
		if (pHook)
		{
			//hInternet = WinHttpOpenA("agsafe", NULL, NULL, 0);
			ret = pHook->WinHttpConnectA(hInternet, lpszServerName, nServerPort, dwReserved);
		}
	}__except(1){}
	return ret;
}

CWinHttpHook::CWinHttpHook()
{
	WriteAGLog("CWinHttpHook");
	InitializeCriticalSection(&cs);

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
}

CWinHttpHook::~CWinHttpHook()
{
	if (pHook == this)
	{
		pHook = NULL;
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