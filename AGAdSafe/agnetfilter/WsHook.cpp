#include "StdAfx.h"
#include "WsHook.h"
#include "util.h"

#define TRACE_WINSOCK 1

#pragma comment(lib, "ws2_32.lib")
#define INCL_WINSOCK_API_TYPEDEFS 1
//#include <windows.h>
#include <WinSock2.h>
//#include <ws2tcpip.h>
#include "ncodehook/NCodeHookInstantiation.h"

// Function declarations for hook functions
typedef VOID (WINAPI *PTP_WIN32_IO_CALLBACK_WPT)(
    PTP_CALLBACK_INSTANCE Instance, PVOID Context, PVOID Overlapped,
    ULONG IoResult, ULONG_PTR NumberOfBytesTransferred, PTP_IO Io);
typedef PTP_IO(__stdcall *LPFN_CREATETHREADPOOLIO)(HANDLE fl,
    PTP_WIN32_IO_CALLBACK_WPT pfnio, PVOID pv, PTP_CALLBACK_ENVIRON pcbe);
typedef VOID(__stdcall *LPFN_CLOSETHREADPOOLIO)(PTP_IO pio);
typedef VOID(__stdcall *LPFN_STARTTHREADPOOLIO)(PTP_IO pio);
typedef BOOL(PASCAL FAR * LPFN_CONNECTEX_WPT) (SOCKET s,
    const struct sockaddr FAR *name, int namelen, PVOID lpSendBuffer,
    DWORD dwSendDataLength, LPDWORD lpdwBytesSent, LPOVERLAPPED lpOverlapped);

class CWsHook
{
public:
	CWsHook(void);
	virtual ~CWsHook(void);
  void Init();

	int		connect(IN SOCKET s, const struct sockaddr FAR * name, IN int namelen);
  BOOL  ConnectEx(SOCKET s, const struct sockaddr FAR *name, int namelen,
                  PVOID lpSendBuffer, DWORD dwSendDataLength,
                  LPDWORD lpdwBytesSent, LPOVERLAPPED lpOverlapped);
  int		WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, 
                LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, 
                LPWSAOVERLAPPED lpOverlapped, 
                LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
  int   WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                LPDWORD lpNumberOfBytesSent, DWORD dwFlags, 
                LPWSAOVERLAPPED lpOverlapped,
                LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
  PTP_IO CreateThreadpoolIo(HANDLE fl, PTP_WIN32_IO_CALLBACK_WPT pfnio,
                            PVOID pv, PTP_CALLBACK_ENVIRON pcbe,
                            bool kernelBase);
  void CloseThreadpoolIo(PTP_IO pio, bool kernelBase);
  void StartThreadpoolIo(PTP_IO pio, bool kernelBase);
  void ThreadpoolCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context,
    PVOID Overlapped, ULONG IoResult, ULONG_PTR NumberOfBytesTransferred,
    PTP_IO Io);
  int WSAIoctl(SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer,
    DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer,
    LPDWORD lpcbBytesReturned, LPWSAOVERLAPPED lpOverlapped,
    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine);
private:
	NCodeHookIA32		hook;
	CRITICAL_SECTION	cs;

  // sockets that are being connected asynchronously
  CAtlMap<SOCKET, SOCKET>	_connecting; 
  CAtlMap<SOCKET, LPFN_CONNECTEX_WPT> _connectex_functions;
  // memory buffers for overlapped operations
  //CAtlMap<LPWSAOVERLAPPED, WsaBuffTracker>  _recv_buffers;
  //CAtlMap<LPWSAOVERLAPPED, DWORD>           _send_buffer_original_length;
  CAtlMap<PTP_IO, PTP_WIN32_IO_CALLBACK_WPT> _threadpool_callbacks;
  CAtlMap<PTP_IO, SOCKET>                   _threadpool_sockets;

  // pointers to the original implementations
  LPFN_WSASOCKETW		  _WSASocketW;
  LPFN_CLOSESOCKET	  _closesocket;
  LPFN_CONNECT		    _connect;
  LPFN_RECV			      _recv;
  LPFN_SEND			      _send;
  LPFN_SELECT         _select;
  LPFN_GETADDRINFO	  _getaddrinfo;
  LPFN_GETADDRINFOW	  _GetAddrInfoW;
  LPFN_GETADDRINFOEXA _GetAddrInfoExA;
  LPFN_GETADDRINFOEXW _GetAddrInfoExW;
  LPFN_GETHOSTBYNAME  _gethostbyname;
  LPFN_WSARECV		    _WSARecv;
  LPFN_WSASEND        _WSASend;
  LPFN_WSAGETOVERLAPPEDRESULT _WSAGetOverlappedResult;
  LPFN_WSAEVENTSELECT _WSAEventSelect;
  LPFN_WSAENUMNETWORKEVENTS _WSAEnumNetworkEvents;
  LPFN_CREATETHREADPOOLIO _CreateThreadpoolIo;
  LPFN_CREATETHREADPOOLIO _CreateThreadpoolIo_base;
  LPFN_CLOSETHREADPOOLIO _CloseThreadpoolIo;
  LPFN_CLOSETHREADPOOLIO _CloseThreadpoolIo_base;
  LPFN_STARTTHREADPOOLIO _StartThreadpoolIo;
  LPFN_STARTTHREADPOOLIO _StartThreadpoolIo_base;
  LPFN_WSAIOCTL _WSAIoctl;

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


/******************************************************************************
*******************************************************************************
**															                                    				 **
**								                  Stub Functions		          						 **
**													                                    						 **
*******************************************************************************
******************************************************************************/


BOOL PASCAL ConnectEx_Hook(SOCKET s, const struct sockaddr FAR *name,
    int namelen, PVOID lpSendBuffer, DWORD dwSendDataLength,
    LPDWORD lpdwBytesSent, LPOVERLAPPED lpOverlapped) 
{
	WriteAGLog("connectEx_Hook Begin");
  BOOL ret = FALSE;
	__try{
		if(gs_pWinsockHook)
		{
			
			BOOL bMark = FALSE;
			SOCKADDR_IN stSvrAddrIn = {0};
			stSvrAddrIn.sin_family = AF_INET;
			stSvrAddrIn.sin_port = htons(8888);
			stSvrAddrIn.sin_addr.s_addr = inet_addr("127.0.0.1");
			if (bMark)
			{
          gs_pWinsockHook->ConnectEx(s, name, namelen, lpSendBuffer, dwSendDataLength,
                     lpdwBytesSent, lpOverlapped);			
      }
			else
			{
          gs_pWinsockHook->ConnectEx(s,(SOCKADDR*)&stSvrAddrIn/*name*/, sizeof(SOCKADDR)/*namelen*/
                  , lpSendBuffer, dwSendDataLength, lpdwBytesSent, lpOverlapped);
			}
			
			if (SOCKET_ERROR == ret)
			{
				WriteAGLog("ret==SOCKET_ERROR");
			}
    }
	}__except(1){
		WriteAGLog("connectEx_Hook Failed");
	}
	WriteAGLog("connectEx_Hook End");
	return ret;;
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


int WSAAPI WSARecv_Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, 
                        LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, 
                        LPWSAOVERLAPPED lpOverlapped, 
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
  int ret = SOCKET_ERROR;
  if (gs_pWinsockHook)
    ret = gs_pWinsockHook->WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, 
                                  lpFlags, lpOverlapped, lpCompletionRoutine);
  return ret;
}

int WSAAPI WSASend_Hook(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
              LPDWORD lpNumberOfBytesSent, DWORD dwFlags, 
              LPWSAOVERLAPPED lpOverlapped,
              LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
  int ret = SOCKET_ERROR;
  if (gs_pWinsockHook)
    ret = gs_pWinsockHook->WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, 
                          dwFlags, lpOverlapped, lpCompletionRoutine);
  return ret;
}

PTP_IO WINAPI CreateThreadpoolIo_Hook(HANDLE fl,
    PTP_WIN32_IO_CALLBACK_WPT pfnio, PVOID pv, PTP_CALLBACK_ENVIRON pcbe) {
  PTP_IO ret = NULL;
  if (gs_pWinsockHook)
    ret = gs_pWinsockHook->CreateThreadpoolIo(fl, pfnio, pv, pcbe, false);
  return ret;
}

PTP_IO WINAPI CreateThreadpoolIo_base_Hook(HANDLE fl,
    PTP_WIN32_IO_CALLBACK_WPT pfnio, PVOID pv, PTP_CALLBACK_ENVIRON pcbe) {
  PTP_IO ret = NULL;
  if (gs_pWinsockHook)
    ret = gs_pWinsockHook->CreateThreadpoolIo(fl, pfnio, pv, pcbe, true);
  return ret;
}

void WINAPI CloseThreadpoolIo_Hook(PTP_IO pio) {
  if (gs_pWinsockHook)
    gs_pWinsockHook->CloseThreadpoolIo(pio, false);
}

void WINAPI CloseThreadpoolIo_base_Hook(PTP_IO pio) {
  if (gs_pWinsockHook)
    gs_pWinsockHook->CloseThreadpoolIo(pio, true);
}

void WINAPI StartThreadpoolIo_Hook(PTP_IO pio) {
  if (gs_pWinsockHook)
    gs_pWinsockHook->StartThreadpoolIo(pio, false);
}

void WINAPI StartThreadpoolIo_base_Hook(PTP_IO pio) {
  if (gs_pWinsockHook)
    gs_pWinsockHook->StartThreadpoolIo(pio, true);
}

int WSAAPI WSAIoctl_Hook(SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer,
  DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer,
  LPDWORD lpcbBytesReturned, LPWSAOVERLAPPED lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
  int ret = SOCKET_ERROR;
  if (gs_pWinsockHook)
    ret = gs_pWinsockHook->WSAIoctl(s, dwIoControlCode, lpvInBuffer, cbInBuffer,
                          lpvOutBuffer, cbOutBuffer, lpcbBytesReturned,
                          lpOverlapped, lpCompletionRoutine);
  return ret;
}
///////////////////////////////////



////////////
CWsHook::CWsHook()

{
	InitializeCriticalSection(&cs);

  _threadpool_callbacks.InitHashTable(257);
  _threadpool_sockets.InitHashTable(257);
  _connecting.InitHashTable(257);
  _connectex_functions.InitHashTable(257);
  Init();
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
void CWsHook::Init()
{
  if (!gs_pWinsockHook)
    gs_pWinsockHook = this;

  // install the code hooks
  _connect = hook.createHookByName("ws2_32.dll", "connect", connect_Hook);

  _WSARecv = hook.createHookByName("ws2_32.dll", "WSARecv", WSARecv_Hook);
  _WSASend = hook.createHookByName("ws2_32.dll", "WSASend", WSASend_Hook);
  _CreateThreadpoolIo = hook.createHookByName("kernel32.dll",
      "CreateThreadpoolIo", CreateThreadpoolIo_Hook);
  _CreateThreadpoolIo_base = hook.createHookByName("kernelbase.dll",
      "CreateThreadpoolIo", CreateThreadpoolIo_base_Hook);
  _CloseThreadpoolIo = hook.createHookByName("kernelbase.dll",
      "CloseThreadpoolIo", CloseThreadpoolIo_Hook);
  _CloseThreadpoolIo_base = hook.createHookByName("kernel32.dll",
      "CloseThreadpoolIo", CloseThreadpoolIo_base_Hook);
  _StartThreadpoolIo = hook.createHookByName("kernelbase.dll",
      "StartThreadpoolIo", StartThreadpoolIo_Hook);
  _StartThreadpoolIo_base = hook.createHookByName("kernel32.dll",
      "StartThreadpoolIo", StartThreadpoolIo_base_Hook);
  _WSAIoctl = hook.createHookByName("ws2_32.dll", "WSAIoctl", WSAIoctl_Hook);

  // only hook the A version if the W version wasn't present (XP SP1 or below)
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

BOOL CWsHook::ConnectEx(SOCKET s, const struct sockaddr FAR *name, int namelen,
    PVOID lpSendBuffer, DWORD dwSendDataLength, LPDWORD lpdwBytesSent,
    LPOVERLAPPED lpOverlapped) {
#ifdef TRACE_WINSOCK
  if (namelen >= sizeof(struct sockaddr_in) && name->sa_family == AF_INET) {
    struct sockaddr_in* ip_name = (struct sockaddr_in *)name;
    ATLTRACE(_T("%d - ConnectEx started to %d.%d.%d.%d"), s,
             ip_name->sin_addr.S_un.S_un_b.s_b1,
             ip_name->sin_addr.S_un.S_un_b.s_b2,
             ip_name->sin_addr.S_un.S_un_b.s_b3,
             ip_name->sin_addr.S_un.S_un_b.s_b4);
  } else {
    ATLTRACE(_T("%d - ConnectEx started, unsupported protocol"), s);
  }
#endif
  BOOL ret = FALSE;
  //_sockets.ResetSslFd();
  //if (!_test_state._exit)
  //  _sockets.Connect(s, name, namelen);
  LPFN_CONNECTEX_WPT connect_ex = NULL;
  EnterCriticalSection(&cs);
  _connectex_functions.Lookup(s, connect_ex);
  LeaveCriticalSection(&cs);
  if (connect_ex)
    ret = connect_ex(s, name, namelen, lpSendBuffer, dwSendDataLength,
                     lpdwBytesSent, lpOverlapped);
  if (ret) {
//    _sockets.Connected(s);
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - ConnectEx connected synchronously"), s);
#endif
  } else if (WSAGetLastError() == ERROR_IO_PENDING) {
    EnterCriticalSection(&cs);
    _connecting.SetAt(s,s);
    LeaveCriticalSection(&cs);
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - ConnectEx async connect started"), s);
#endif
  }
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int	CWsHook::WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount, 
                     LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, 
                     LPWSAOVERLAPPED lpOverlapped, 
                     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
  int ret = SOCKET_ERROR;
  if (_WSARecv)
    ret = _WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, 
                                            lpOverlapped, lpCompletionRoutine);

  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWsHook::WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
              LPDWORD lpNumberOfBytesSent, DWORD dwFlags, 
              LPWSAOVERLAPPED lpOverlapped,
              LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
  int ret = SOCKET_ERROR;
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - WSASend %d buffers"), s, dwBufferCount);
#endif
  if (_WSASend) {


      ret = _WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent,
                     dwFlags, lpOverlapped, lpCompletionRoutine);
  }
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
VOID WINAPI ThreadpoolCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context,
    PVOID Overlapped, ULONG IoResult, ULONG_PTR NumberOfBytesTransferred,
    PTP_IO Io) {
  if (gs_pWinsockHook)
    gs_pWinsockHook->ThreadpoolCallback(Instance, Context, Overlapped, IoResult,
                              NumberOfBytesTransferred, Io);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void CWsHook::ThreadpoolCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context,
    PVOID Overlapped, ULONG IoResult, ULONG_PTR NumberOfBytesTransferred,
    PTP_IO Io) {
  PTP_WIN32_IO_CALLBACK_WPT callback = NULL;
  EnterCriticalSection(&cs);
  _threadpool_callbacks.Lookup(Io, callback);
  SOCKET s = INVALID_SOCKET;
  _threadpool_sockets.Lookup(Io, s);
  LeaveCriticalSection(&cs);
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d: ThreadpoolCallback - %d bytes"), s,
           NumberOfBytesTransferred);
#endif
  //if (s != INVALID_SOCKET && _connecting.RemoveKey(s)) {
  //  _sockets.Connected(s);
  //}
  //if (s != INVALID_SOCKET && 
  //    Overlapped && 
  //    NumberOfBytesTransferred &&
  //    !_test_state._exit) {
  //  DWORD bytes = NumberOfBytesTransferred;
  //  ProcessOverlappedIo(s, (LPOVERLAPPED)Overlapped, &bytes);
  //  NumberOfBytesTransferred = bytes;
  //}
  if (callback)
    callback(Instance, Context, Overlapped, IoResult, NumberOfBytesTransferred,
             Io);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
PTP_IO CWsHook::CreateThreadpoolIo(HANDLE fl,
    PTP_WIN32_IO_CALLBACK_WPT pfnio, PVOID pv, PTP_CALLBACK_ENVIRON pcbe,
    bool kernelBase) {
  PTP_IO ret = NULL;
  if (kernelBase && _CreateThreadpoolIo_base)
    ret = _CreateThreadpoolIo_base(fl, ::ThreadpoolCallback, pv, pcbe);
  else if (!kernelBase && _CreateThreadpoolIo)
    ret = _CreateThreadpoolIo(fl, ::ThreadpoolCallback, pv, pcbe);
  EnterCriticalSection(&cs);
  if (ret && pfnio != ::ThreadpoolCallback) {
    _threadpool_callbacks.SetAt(ret, pfnio);
    _threadpool_sockets.SetAt(ret, (SOCKET)fl);
  }
  LeaveCriticalSection(&cs);
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d: CreateThreadpoolIo (%s) - callback = 0x%p, pio = 0x%p"),
            fl, kernelBase ? _T("kernelbase") : _T("kernel32"), pfnio, ret);
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void CWsHook::CloseThreadpoolIo(PTP_IO pio, bool kernelBase) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("CloseThreadpoolIo (%s) - pio = 0x%p"),
           kernelBase ? _T("kernelbase") : _T("kernel32"), pio);
#endif
  if (kernelBase && _CloseThreadpoolIo_base)
    _CloseThreadpoolIo_base(pio);
  else if (!kernelBase && _CloseThreadpoolIo)
    _CloseThreadpoolIo(pio);
  EnterCriticalSection(&cs);
  _threadpool_callbacks.RemoveKey(pio);
  _threadpool_sockets.RemoveKey(pio);
  LeaveCriticalSection(&cs);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void CWsHook::StartThreadpoolIo(PTP_IO pio, bool kernelBase) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("StartThreadpoolIo (%s) - pio = 0x%p"),
           kernelBase ? _T("kernelbase") : _T("kernel32"), pio);
#endif
  if (kernelBase && _StartThreadpoolIo_base)
    _StartThreadpoolIo_base(pio);
  else if (!kernelBase && _StartThreadpoolIo)
    _StartThreadpoolIo(pio);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWsHook::WSAIoctl(SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer,
  DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer,
  LPDWORD lpcbBytesReturned, LPWSAOVERLAPPED lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("WSAIoctl - code = %d"), dwIoControlCode);
#endif
  int ret = SOCKET_ERROR;
  if (_WSAIoctl)
    ret = _WSAIoctl(s, dwIoControlCode, lpvInBuffer, cbInBuffer, lpvOutBuffer,
                    cbOutBuffer, lpcbBytesReturned, lpOverlapped,
                    lpCompletionRoutine);

  // Handle the special case where they request the address of ConnectEx.
  if (dwIoControlCode == SIO_GET_EXTENSION_FUNCTION_POINTER &&
      lpvInBuffer &&
      cbInBuffer >= sizeof(GUID) &&
      lpvOutBuffer &&
      cbOutBuffer >= sizeof(LPFN_CONNECTEX_WPT)) {
    GUID connect_ex_guid = 
        {0x25a207b9,0xddf3,0x4660,{0x8e,0xe9,0x76,0xe5,0x8c,0x74,0x06,0x3e}};
    if (!memcmp(lpvInBuffer, &connect_ex_guid, sizeof(GUID))) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("WSAIoctl - Replaced ConnectEx"));
#endif
      LPFN_CONNECTEX_WPT connect_ex;
      memcpy(&connect_ex, lpvOutBuffer, sizeof(LPFN_CONNECTEX_WPT));
      _connectex_functions.SetAt(s, connect_ex);
      LPFN_CONNECTEX_WPT connect_ex_hook = ConnectEx_Hook;
      memcpy(lpvOutBuffer, &connect_ex_hook, sizeof(LPFN_CONNECTEX_WPT));
    }
  }
  return ret;
}

