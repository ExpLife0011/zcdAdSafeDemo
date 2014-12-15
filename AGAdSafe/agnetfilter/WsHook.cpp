#include "StdAfx.h"
#include "WsHook.h"
#include "util.h"

#pragma comment(lib, "ws2_32.lib")
#define INCL_WINSOCK_API_TYPEDEFS 1
//#include <windows.h>
#include <WinSock2.h>
//#include <ws2tcpip.h>
#include "ncodehook/NCodeHookInstantiation.h"

class CWsHook
{
public:
	CWsHook(void);
	virtual ~CWsHook(void);

	int		connect(IN SOCKET s, const struct sockaddr FAR * name, IN int namelen);

private:
	NCodeHookIA32		hook;
	CRITICAL_SECTION	cs;

	LPFN_CONNECT	_connect;

};

static CWsHook * gs_pWinsockHook = NULL;


BOOL WinsockInstallHooks()
{
  if(!gs_pWinsockHook)
    gs_pWinsockHook = new CWsHook();
  return gs_pWinsockHook!=NULL;
}

void WinsockRemoveHooks()
{
  if (gs_pWinsockHook)
	{
    delete gs_pWinsockHook;
    gs_pWinsockHook = NULL;
	}
}

int WSAAPI connect_Hook(IN SOCKET s, const struct sockaddr FAR * name, IN int namelen)
{
	WriteAGLog("connect_Hook Begin");
	int ret = SOCKET_ERROR;
	__try{
		if(gs_pWinsockHook)
		{
			WriteAGLog("...");
			BOOL bMark = FALSE;
			if (name!=NULL)
			{
				sockaddr_in sin;
				memcpy(&sin, &name, sizeof(sin));

				WriteAGLog("");
				if(sin.sin_port==htons(9222))
				{
					bMark = TRUE;
				}
			}
			SOCKADDR_IN stSvrAddrIn = {0};
			stSvrAddrIn.sin_family = AF_INET;
			stSvrAddrIn.sin_port = htons(8888);
			stSvrAddrIn.sin_addr.s_addr = inet_addr("127.0.0.1");
			if (bMark)
			{
				
				ret = gs_pWinsockHook->connect(s, name, namelen);
			}
			else
			{
				//ret = gs_pWinsockHook->connect(s, name, namelen);
				//unsigned long ul = 0;
				//ioctlsocket (s, FIONBIO, (unsigned long*)&ul);
				ret = gs_pWinsockHook->connect(s, (SOCKADDR*)&stSvrAddrIn/*name*/, sizeof(SOCKADDR)/*namelen*/);
			}
			
			if (SOCKET_ERROR == ret)
			{
				//DWORD dw = GetLastError();
				//LPVOID lpMsgBuf;
// 				FormatMessageA(
// 					FORMAT_MESSAGE_ALLOCATE_BUFFER |
// 					FORMAT_MESSAGE_FROM_SYSTEM |
// 					FORMAT_MESSAGE_IGNORE_INSERTS,
// 					NULL,
// 					GetLastError(),
// 					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
// 					(LPSTR) &lpMsgBuf,
// 					0,
// 					NULL
// 					);
				//WriteAGLog((LPCSTR)lpMsgBuf);
				WriteAGLog("ret==SOCKET_ERROR");
			}
		}
	}__except(1){
		WriteAGLog("connect_Hook Failed");
	}
	WriteAGLog("connect_Hook End");
	return ret;
}

CWsHook::CWsHook()
{
	InitializeCriticalSection(&cs);

	_connect = hook.createHookByName("ws2_32.dll", "connect", connect_Hook);

}

CWsHook::~CWsHook()
{
  if (gs_pWinsockHook == this)
	{
    gs_pWinsockHook = NULL;
    //hook.removeHook(_connect);
    _connect = NULL;
	}

	DeleteCriticalSection(&cs);
}

int CWsHook::connect(IN SOCKET s, const struct sockaddr FAR * name, IN int namelen)
{
	int ret = SOCKET_ERROR;

	if (_connect)
	{
		ret = _connect(s, name, namelen);
	}

	return ret;
}
