/******************************************************************************
Copyright (c) 2010, Google Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name of the <ORGANIZATION> nor the names of its contributors
    may be used to endorse or promote products derived from this software
    without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

// WsHook.cpp - Code for intercepting winsock API calls

#include "StdAfx.h"
#include "hook_winsock.h"
#include "shared_mem.h"
#include "zcdbg.h"
//#include "request.h"
//#include "track_dns.h"
//#include "track_sockets.h"
//#include "test_state.h"

//#define ENABLE_WPTRACK 1
#define WPTRACK_BEGIN if(false){
#define WPTRACK_END  }

static CWinsockHook * g_pWinsockHook = NULL;

#define TRACE_WINSOCK 1
/******************************************************************************
*******************************************************************************
**                                                                           **
**                                  Stub Functions                           **
**                                                                           **
*******************************************************************************
******************************************************************************/

int WSAAPI AmhHook_connect(IN SOCKET s, const struct sockaddr FAR * name,
 IN int namelen)
{

  OutputDebugLog("AmhHook_connect Begin");
  int ret = SOCKET_ERROR;
  __try{
    if(g_pWinsockHook)
    {
      OutputDebugLog("...");
      bool bUseOriginalFuncData = false;
      if (name!=NULL)
      {
        sockaddr_in sin;
        memcpy(&sin, &name, sizeof(sin));

        if(sin.sin_port==htons(9222))
        {
          bUseOriginalFuncData = true;
        }
      }

      if(!shared_proxy_enabled) bUseOriginalFuncData = true;

      if (bUseOriginalFuncData)
      {
        ret = g_pWinsockHook->connect(s, name, namelen);
      }
      else
      {
        SOCKADDR_IN stSvrAddr = {0};
        stSvrAddr.sin_family = AF_INET;
        stSvrAddr.sin_port = htons(shared_proxy_port);
        stSvrAddr.sin_addr.s_addr = inet_addr(shared_proxy_ip);

        ret = g_pWinsockHook->connect(s, (SOCKADDR*)&stSvrAddr/*name*/, sizeof(SOCKADDR)/*namelen*/);
      }

      if (SOCKET_ERROR == ret)
      {
          OutputDebugLog("ret==SOCKET_ERROR");
      }
    }
  }__except(1){
      OutputDebugLog("AmhHook_connect Failed");
  }
    OutputDebugLog("AmhHook_connect End");
  return ret;
}

BOOL PASCAL AmhHook_ConnectEx(SOCKET s, const struct sockaddr FAR *name,
    int namelen, PVOID lpSendBuffer, DWORD dwSendDataLength,
    LPDWORD lpdwBytesSent, LPOVERLAPPED lpOverlapped)
{

  OutputDebugLog("AmhHook_ConnectEx Begin");
  BOOL ret = FALSE;
  __try{
    if(g_pWinsockHook)
    {

      bool bUseOriginalFuncData = false;
      if(!shared_proxy_enabled) bUseOriginalFuncData = true;

      if (bUseOriginalFuncData)
      {
          g_pWinsockHook->ConnectEx(s, name, namelen, lpSendBuffer, dwSendDataLength, lpdwBytesSent, lpOverlapped);
      }
      else
      {
        SOCKADDR_IN stSvrAddr = {0};
        stSvrAddr.sin_family = AF_INET;
        stSvrAddr.sin_port = htons(shared_proxy_port);
        stSvrAddr.sin_addr.s_addr = inet_addr(shared_proxy_ip);

        g_pWinsockHook->ConnectEx(s,(SOCKADDR*)&stSvrAddr/*name*/, sizeof(SOCKADDR)/*namelen*/  , lpSendBuffer, dwSendDataLength, lpdwBytesSent, lpOverlapped);
      }

      if (SOCKET_ERROR == ret)
      {
          OutputDebugLog("ret==SOCKET_ERROR");
      }
    }
  }__except(1){
      OutputDebugLog("AmhHook_ConnectEx Failed");
  }
  OutputDebugLog("AmhHook_ConnectEx End");
  return ret;
}

SOCKET WSAAPI AmhHook_WSASocketW(int af, int type, int protocol,
                  LPWSAPROTOCOL_INFOW lpProtocolInfo, GROUP g, DWORD dwFlags)
{
  SOCKET ret = SOCKET_ERROR;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->WSASocketW(af, type, protocol, lpProtocolInfo, g, dwFlags);

  }__except(1){
      OutputDebugLog("AmhHook_WSASocketW Failed");
  }
  return ret;
}

int WSAAPI AmhHook_closesocket(SOCKET s)
{
  int ret = SOCKET_ERROR;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->closesocket(s);

  }__except(1){
      OutputDebugLog("AmhHook_closesocket Failed");
  }
  return ret;
}

int WSAAPI AmhHook_recv(SOCKET s, char FAR * buf, int len, int flags)
{
  int ret = SOCKET_ERROR;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->recv(s, buf, len, flags);

  }__except(1){
      OutputDebugLog("AmhHook_recv Failed");
  }
  return ret;
}

int WSAAPI AmhHook_select(int nfds, fd_set FAR * readfds, fd_set FAR * writefds, fd_set FAR * exceptfds, const struct timeval FAR * timeout)
{
  int ret = SOCKET_ERROR;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->select(nfds, readfds, writefds, exceptfds, timeout);

  }__except(1){
      OutputDebugLog("AmhHook_select Failed");
  }
  return ret;
}

int WSAAPI AmhHook_send(SOCKET s, const char FAR * buf, int len, int flags)
{
  int ret = SOCKET_ERROR;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->send(s, buf, len, flags);

  }__except(1){
      OutputDebugLog("AmhHook_send Failed");
  }
  return ret;
}

int WSAAPI AmhHook_getaddrinfo(PCSTR pNodeName, PCSTR pServiceName,
                            const ADDRINFOA * pHints, PADDRINFOA * ppResult)
{
  int ret = WSAEINVAL;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->getaddrinfo(pNodeName, pServiceName,
                             (ADDRINFOA *)pHints, ppResult);

  }__except(1){
      OutputDebugLog("AmhHook_getaddrinfo Failed");
  }
  return ret;
}

int WSAAPI AmhHook_GetAddrInfoW(PCWSTR pNodeName, PCWSTR pServiceName,
                             const ADDRINFOW * pHints, PADDRINFOW * ppResult)
{
  int ret = WSAEINVAL;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->GetAddrInfoW(pNodeName, pServiceName,
                              (ADDRINFOW *)pHints, ppResult);

  }__except(1){
      OutputDebugLog("AmhHook_GetAddrInfoW Failed");
  }
  return ret;
}

struct hostent * WSAAPI AmhHook_gethostbyname(const char * name)
{
  struct hostent * ret = NULL;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->gethostbyname(name);

  }__except(1){
      OutputDebugLog("AmhHook_gethostbyname Failed");
  }
  return ret;
}

int WSAAPI AmhHook_WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                        LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,
                        LPWSAOVERLAPPED lpOverlapped,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
  int ret = SOCKET_ERROR;
  __try{

    if (g_pWinsockHook)
      ret = g_pWinsockHook->WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags, lpOverlapped, lpCompletionRoutine);

  }__except(1){
      OutputDebugLog("AmhHook_WSARecv Failed");
  }
  return ret;
}

int WSAAPI AmhHook_WSASend(SOCKET s, LPWSABUF lpBuffers,
                           DWORD dwBufferCount,
              LPDWORD lpNumberOfBytesSent, DWORD dwFlags,
              LPWSAOVERLAPPED lpOverlapped,
              LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
  int ret = SOCKET_ERROR;
  __try{

    if (g_pWinsockHook)
      ret = g_pWinsockHook->WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent,dwFlags, lpOverlapped, lpCompletionRoutine);
  }__except(1){
    OutputDebugLog("AmhHook_WSASend Failed");
  }
  return ret;
}

BOOL WSAAPI AmhHook_WSAGetOverlappedResult(SOCKET s,
                                  LPWSAOVERLAPPED lpOverlapped,
              LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags)
{
  BOOL ret = FALSE;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->WSAGetOverlappedResult(s, lpOverlapped, lpcbTransfer, fWait,  lpdwFlags);

  }__except(1){
      OutputDebugLog("AmhHook_WSAGetOverlappedResult Failed");
  }
  return ret;
}

int WSAAPI AmhHook_WSAEventSelect(SOCKET s, WSAEVENT hEventObject,
                                long lNetworkEvents)
{
  int ret = SOCKET_ERROR;
  __try{
    if (g_pWinsockHook)
    ret = g_pWinsockHook->WSAEventSelect(s, hEventObject, lNetworkEvents);

  }__except(1){
      OutputDebugLog("AmhHook_WSAEventSelect Failed");
  }
  return ret;
}

int WSAAPI AmhHook_WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEventObject,
                            LPWSANETWORKEVENTS lpNetworkEvents)
{
  int ret = SOCKET_ERROR;
  __try{

    if (g_pWinsockHook)
      ret = g_pWinsockHook->WSAEnumNetworkEvents(s, hEventObject, lpNetworkEvents);
  }__except(1){
      OutputDebugLog("AmhHook_WSAEnumNetworkEvents Failed");
  }
  return ret;
}

int WSAAPI AmhHook_GetAddrInfoExA(PCSTR pName, PCSTR pServiceName,
    DWORD dwNameSpace,
    LPGUID lpNspId, const ADDRINFOEXA *hints, PADDRINFOEXA *ppResult,
    struct timeval *timeout, LPOVERLAPPED lpOverlapped,
    LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine,
    LPHANDLE lpNameHandle)
{
  int ret = EAI_FAIL;
  __try{

    if (g_pWinsockHook)
      ret = g_pWinsockHook->GetAddrInfoExA(pName, pServiceName, dwNameSpace, lpNspId, (ADDRINFOEXA *)hints, ppResult, timeout, lpOverlapped, lpCompletionRoutine, lpNameHandle);
  }__except(1){
      OutputDebugLog("AmhHook_GetAddrInfoExA Failed");
  }
  return ret;
}

int WSAAPI AmhHook_GetAddrInfoExW(PCWSTR pName, PCWSTR pServiceName, DWORD dwNameSpace,
    LPGUID lpNspId, const ADDRINFOEXW *hints, PADDRINFOEXW *ppResult,
    struct timeval *timeout, LPOVERLAPPED lpOverlapped,
    LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine,
    LPHANDLE lpHandle)
{
  int ret = EAI_FAIL;
  __try{

    if (g_pWinsockHook)
      ret = g_pWinsockHook->GetAddrInfoExW(pName, pServiceName, dwNameSpace, lpNspId,
        (ADDRINFOEXW *)hints, ppResult, timeout, lpOverlapped,
        lpCompletionRoutine, lpHandle);
  }__except(1){
      OutputDebugLog("AmhHook_GetAddrInfoExW Failed");
  }
  return ret;
}

PTP_IO WINAPI AmhHook_CreateThreadpoolIo(HANDLE fl,
    PTP_WIN32_IO_CALLBACK_WPT pfnio, PVOID pv, PTP_CALLBACK_ENVIRON pcbe) {
  PTP_IO ret = NULL;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->CreateThreadpoolIo(fl, pfnio, pv, pcbe, false);
  }__except(1){
      OutputDebugLog("AmhHook_CreateThreadpoolIo Failed");
  }
  return ret;
}

PTP_IO WINAPI AmhHook_CreateThreadpoolIo_base(HANDLE fl,
    PTP_WIN32_IO_CALLBACK_WPT pfnio, PVOID pv, PTP_CALLBACK_ENVIRON pcbe) {
  PTP_IO ret = NULL;
  __try{
    if (g_pWinsockHook)
      ret = g_pWinsockHook->CreateThreadpoolIo(fl, pfnio, pv, pcbe, true);
  }__except(1){
      OutputDebugLog("AmhHook_CreateThreadpoolIo_base Failed");
  }
  return ret;
}

void WINAPI AmhHook_CloseThreadpoolIo(PTP_IO pio) {
  __try{
    if (g_pWinsockHook)
      g_pWinsockHook->CloseThreadpoolIo(pio, false);
  }__except(1){
      OutputDebugLog("AmhHook_CloseThreadpoolIo Failed");
  }
}

void WINAPI AmhHook_CloseThreadpoolIo_base(PTP_IO pio) {
  __try{
    if (g_pWinsockHook)
      g_pWinsockHook->CloseThreadpoolIo(pio, true);
  }__except(1){
      OutputDebugLog("AmhHook_CloseThreadpoolIo_base Failed");
  }
}

void WINAPI AmhHook_StartThreadpoolIo(PTP_IO pio) {
  __try{
    if (g_pWinsockHook)
      g_pWinsockHook->StartThreadpoolIo(pio, false);
  }__except(1){
      OutputDebugLog("AmhHook_StartThreadpoolIo Failed");
  }
}

void WINAPI AmhHook_StartThreadpoolIo_base(PTP_IO pio)
{
  __try{
  if (g_pWinsockHook)
    g_pWinsockHook->StartThreadpoolIo(pio, true);
}__except(1){
    OutputDebugLog("AmhHook_StartThreadpoolIo_base Failed");
}

}

int WSAAPI AmhHook_WSAIoctl(SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer, DWORD cbInBuffer, LPVOID lpvOutBuffer, DWORD cbOutBuffer,
  LPDWORD lpcbBytesReturned, LPWSAOVERLAPPED lpOverlapped,
  LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
  int ret = SOCKET_ERROR;
  __try{
    if (g_pWinsockHook)
    ret = g_pWinsockHook->WSAIoctl(s, dwIoControlCode, lpvInBuffer, cbInBuffer, lpvOutBuffer, cbOutBuffer, lpcbBytesReturned, lpOverlapped, lpCompletionRoutine);

  }__except(1){
      OutputDebugLog("AmhHook_WSAIoctl Failed");
  }
  return ret;
}

/******************************************************************************
*******************************************************************************
**                                                                           **
**                            CWSHook Class                                   **
**                                                                           **
*******************************************************************************
******************************************************************************/

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
CWinsockHook::CWinsockHook(
#ifdef ENABLE_WPTRACK
  TrackDns& dns, TrackSockets& sockets, TestState& test_state
#endif
  ):
  _getaddrinfo(NULL)
    ,_hook(NULL)
#ifdef ENABLE_WPTRACK
  , _dns(dns)
  , _sockets(sockets)
  , _test_state(test_state)
#endif

  {

#ifdef ENABLE_WPTRACK
  _recv_buffers.InitHashTable(257);
  _send_buffers.InitHashTable(257);
  _send_buffer_original_length.InitHashTable(257);
#endif
  _threadpool_callbacks.InitHashTable(257);
  _threadpool_sockets.InitHashTable(257);
  _connecting.InitHashTable(257);
  _connectex_functions.InitHashTable(257);
  InitializeCriticalSection(&cs);
  //Init();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void CWinsockHook::Init()
{
  if (g_pWinsockHook || _hook)
    return;

  _hook = new NCodeHookIA32();
    g_pWinsockHook = this;

    // install the code hooks
  _WSASocketW = _hook->createHookByName("ws2_32.dll", "WSASocketW",
                                      AmhHook_WSASocketW);
  _closesocket = _hook->createHookByName("ws2_32.dll", "closesocket",
                                       AmhHook_closesocket);
  _connect = _hook->createHookByName("ws2_32.dll", "connect", AmhHook_connect);
  _recv = _hook->createHookByName("ws2_32.dll", "recv", AmhHook_recv);
  _send = _hook->createHookByName("ws2_32.dll", "send", AmhHook_send);
  _select = _hook->createHookByName("ws2_32.dll", "select", AmhHook_select);
  _GetAddrInfoW = _hook->createHookByName("ws2_32.dll", "GetAddrInfoW",
                                        AmhHook_GetAddrInfoW);
  _gethostbyname = _hook->createHookByName("ws2_32.dll", "gethostbyname",
                                         AmhHook_gethostbyname);
  _GetAddrInfoExA = _hook->createHookByName("ws2_32.dll", "GetAddrInfoExA",
                                          AmhHook_GetAddrInfoExA);
  _GetAddrInfoExW = _hook->createHookByName("ws2_32.dll", "GetAddrInfoExW",
                                          AmhHook_GetAddrInfoExW);
  _WSARecv = _hook->createHookByName("ws2_32.dll", "WSARecv", AmhHook_WSARecv);
  _WSASend = _hook->createHookByName("ws2_32.dll", "WSASend", AmhHook_WSASend);
  _WSAGetOverlappedResult = _hook->createHookByName("ws2_32.dll",
      "WSAGetOverlappedResult", AmhHook_WSAGetOverlappedResult);
  _WSAEventSelect = _hook->createHookByName("ws2_32.dll", "WSAEventSelect",
                                          AmhHook_WSAEventSelect);
  _WSAEnumNetworkEvents = _hook->createHookByName("ws2_32.dll",
      "WSAEnumNetworkEvents", AmhHook_WSAEnumNetworkEvents);
  _CreateThreadpoolIo = _hook->createHookByName("kernel32.dll",
      "CreateThreadpoolIo", AmhHook_CreateThreadpoolIo);
  _CreateThreadpoolIo_base = _hook->createHookByName("kernelbase.dll",
      "CreateThreadpoolIo", AmhHook_CreateThreadpoolIo_base);
  _CloseThreadpoolIo = _hook->createHookByName("kernelbase.dll",
      "CloseThreadpoolIo", AmhHook_CloseThreadpoolIo);
  _CloseThreadpoolIo_base = _hook->createHookByName("kernel32.dll",
      "CloseThreadpoolIo", AmhHook_CloseThreadpoolIo_base);
  _StartThreadpoolIo = _hook->createHookByName("kernelbase.dll",
      "StartThreadpoolIo", AmhHook_StartThreadpoolIo);
  _StartThreadpoolIo_base = _hook->createHookByName("kernel32.dll",
      "StartThreadpoolIo", AmhHook_StartThreadpoolIo_base);
  _WSAIoctl = _hook->createHookByName("ws2_32.dll", "WSAIoctl", AmhHook_WSAIoctl);

  // only hook the A version if the W version wasn't present (XP SP1 or below)
  if (!_GetAddrInfoW)
    _getaddrinfo = _hook->createHookByName("ws2_32.dll", "getaddrinfo",
                                         AmhHook_getaddrinfo);

}

void CWinsockHook::Destroy()
{

#ifdef ENABLE_WPTRACE
  EnterCriticalSection(&cs);
  _send_buffers.RemoveAll();
  _send_buffer_original_length.RemoveAll();
  LeaveCriticalSection(&cs);
#endif

    if (_hook)
    {
     delete _hook;  // remove all the hooks
    _hook = NULL;
    }
}
/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
CWinsockHook::~CWinsockHook(void)
{
  if( g_pWinsockHook == this )
  {
    g_pWinsockHook = NULL;
        Destroy();
  }

#ifdef ENABLE_WPTRACE
  EnterCriticalSection(&cs);
  _send_buffers.RemoveAll();
  _send_buffer_original_length.RemoveAll();
  LeaveCriticalSection(&cs);
#endif
  DeleteCriticalSection(&cs);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
SOCKET CWinsockHook::WSASocketW(int af, int type, int protocol,
                  LPWSAPROTOCOL_INFOW lpProtocolInfo, GROUP g, DWORD dwFlags) {
  SOCKET ret = INVALID_SOCKET;
#ifdef ENABLE_WPTRACE
  _sockets.ResetSslFd();
#endif
  if (_WSASocketW) {
    ret = _WSASocketW(af, type, protocol, lpProtocolInfo, g, dwFlags);
#ifdef ENABLE_WPTRACE
    if( ret != INVALID_SOCKET && !_test_state._exit )
      _sockets.Create(ret);
#endif
  }
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - WSASocketW, socket created"), ret);
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWinsockHook::closesocket(SOCKET s) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - closesocket"), s);
#endif
  int ret = SOCKET_ERROR;
#ifdef ENABLE_WPTRACE
  _sockets.ResetSslFd();
#endif
  EnterCriticalSection(&cs);
  _connecting.RemoveKey(s);
  LeaveCriticalSection(&cs);
#ifdef ENABLE_WPTRACE
  if (!_test_state._exit)
    _sockets.Close(s);
#endif
  if (_closesocket)
    ret = _closesocket(s);
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWinsockHook::connect(IN SOCKET s, const struct sockaddr FAR * name,
                                                              IN int namelen) {
#ifdef TRACE_WINSOCK
  if (namelen >= sizeof(struct sockaddr_in) && name->sa_family == AF_INET) {
    struct sockaddr_in* ip_name = (struct sockaddr_in *)name;
    ATLTRACE(_T("%d - connect started to %d.%d.%d.%d"), s,
             ip_name->sin_addr.S_un.S_un_b.s_b1,
             ip_name->sin_addr.S_un.S_un_b.s_b2,
             ip_name->sin_addr.S_un.S_un_b.s_b3,
             ip_name->sin_addr.S_un.S_un_b.s_b4);
  } else {
    ATLTRACE(_T("%d - connect started, unsupported protocol"), s);
  }
#endif
  int ret = SOCKET_ERROR;
#ifdef ENABLE_WPTRACE
  _sockets.ResetSslFd();
  if (!_test_state._exit)
    _sockets.Connect(s, name, namelen);
#endif
  if (_connect)
    ret = _connect(s, name, namelen);
  if (!ret) {
#ifdef ENABLE_WPTRACE
    _sockets.Connected(s);
#endif
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - connected synchronously"), s);
#endif
  }
  else if(ret == SOCKET_ERROR && WSAGetLastError() == WSAEWOULDBLOCK)
  {
    EnterCriticalSection(&cs);
    _connecting.SetAt(s,s);
    LeaveCriticalSection(&cs);
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - async connect started"), s);
#endif
  }
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
BOOL CWinsockHook::ConnectEx(SOCKET s, const struct sockaddr FAR *name, int namelen,
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
#ifdef ENABLE_WPTRACE
  _sockets.ResetSslFd();
  if (!_test_state._exit)
    _sockets.Connect(s, name, namelen);
#endif
  LPFN_CONNECTEX_WPT connect_ex = NULL;
  EnterCriticalSection(&cs);
  _connectex_functions.Lookup(s, connect_ex);
  LeaveCriticalSection(&cs);
  if (connect_ex)
    ret = connect_ex(s, name, namelen, lpSendBuffer, dwSendDataLength,
                     lpdwBytesSent, lpOverlapped);
  if (ret) {
#ifdef ENABLE_WPTRACE
    _sockets.Connected(s);
#endif
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
int  CWinsockHook::recv(SOCKET s, char FAR * buf, int len, int flags) {
  int ret = SOCKET_ERROR;
  if (_recv)
    ret = _recv(s, buf, len, flags);
#ifdef ENABLE_WPTRACK
    if (!_test_state._exit) {
    if (ret == SOCKET_ERROR && len == 1) {
      _sockets.SetSslSocket(s);
    } else if (ret > 0 && !flags && buf && len) {
      DataChunk chunk(buf, ret);
      _sockets.DataIn(s, chunk, false);
    }
  }
#endif
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - recv, %d byte buffer returned %d"), s, len, ret);
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int  CWinsockHook::WSARecv(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                     LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags,
                     LPWSAOVERLAPPED lpOverlapped,
                     LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
  int ret = SOCKET_ERROR;
  if (_WSARecv)
    ret = _WSARecv(s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, lpFlags,
                                            lpOverlapped, lpCompletionRoutine);
#ifdef ENABLE_WPTRACK
  if (!_test_state._exit && lpBuffers && dwBufferCount) {
    if (ret == 0 && lpNumberOfBytesRecvd && *lpNumberOfBytesRecvd) {
      DWORD num_bytes = *lpNumberOfBytesRecvd;
      for (DWORD i = 0; i < dwBufferCount && num_bytes > 0; ++i) {
        DWORD buffer_size = min(lpBuffers[i].len, num_bytes);
        if (buffer_size) {
          if (lpBuffers[i].buf) {
            DataChunk chunk(lpBuffers[i].buf, buffer_size);
            _sockets.DataIn(s, chunk, false);
          }
          num_bytes -= buffer_size;
        }
      }
    } else if (ret == SOCKET_ERROR && lpOverlapped) {
      WsaBuffTracker buff(lpBuffers, dwBufferCount);
      _recv_buffers.SetAt(lpOverlapped, buff);
    }
  }
  _sockets.ResetSslFd();
#endif
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - WSARecv returned %d"), s, ret);
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWinsockHook::send(SOCKET s, const char FAR * buf, int len, int flags)
{
  int ret = SOCKET_ERROR;
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - send %d bytes"), s, len);
#endif
#ifdef ENABLE_WPTRACK
  if (_send) {
    _sockets.ClaimSslFd(s);
    DataChunk chunk(buf, len);
    int original_len = len;
    if (len && !_test_state._exit) {
      _sockets.ModifyDataOut(s, chunk, false);
      _sockets.DataOut(s, chunk, false);
    }
    ret = _send(s, chunk.GetData(), chunk.GetLength(), flags);
    ret = original_len;
  }
  _sockets.ResetSslFd();
#else
   if (_send)
   {
    ret = _send(s, buf, len, flags);
  }
#endif

   return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWinsockHook::WSASend(SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
              LPDWORD lpNumberOfBytesSent, DWORD dwFlags,
              LPWSAOVERLAPPED lpOverlapped,
              LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
  int ret = SOCKET_ERROR;
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - WSASend %d buffers"), s, dwBufferCount);
#endif
#ifdef ENABLE_WPTRACK
  if (_WSASend) {

    _sockets.ClaimSslFd(s);
    bool is_modified = 0;
    unsigned original_len = 0;
    DataChunk chunk;
    if (!_test_state._exit) {
      for (DWORD i = 0; i < dwBufferCount; i++) {
        original_len += lpBuffers[i].len;
      }
      // Concatenate all the buffers together.
      LPSTR data = chunk.AllocateLength(original_len);
      for (DWORD i = 0; i < dwBufferCount; i++) {
        DWORD buffer_len = lpBuffers[i].len;
        if (buffer_len && lpBuffers[i].buf) {
          memcpy(data, lpBuffers[i].buf, buffer_len);
        }
        data += buffer_len;
      }
      is_modified = _sockets.ModifyDataOut(s, chunk, false);
      _sockets.DataOut(s, chunk, false);
    }
    if (is_modified) {
      WSABUF out;
      out.buf = (char *)chunk.GetData();
      out.len = chunk.GetLength();
      ret = _WSASend(s, &out, 1, lpNumberOfBytesSent, dwFlags, lpOverlapped,
                     lpCompletionRoutine);
      // Respond with the number of bytes the sending app was expecting
      // to be written.  It can get confused if you write more data than
      // they provided.
      if (!ret) {
        if (lpNumberOfBytesSent && *lpNumberOfBytesSent == out.len) {
          *lpNumberOfBytesSent = original_len;
        }
      } else if (WSAGetLastError() == WSA_IO_PENDING) {
        _send_buffers.SetAt(lpOverlapped, chunk);
      }
    } else {
      ret = _WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent,
                     dwFlags, lpOverlapped, lpCompletionRoutine);
    }
  }
  _sockets.ResetSslFd();
#else
  if (_WSASend) {
    ret = _WSASend(s, lpBuffers, dwBufferCount, lpNumberOfBytesSent,
                     dwFlags, lpOverlapped, lpCompletionRoutine);
  }

#endif
  return ret;
}

/*-----------------------------------------------------------------------------
  Check for completion of an async connect
-----------------------------------------------------------------------------*/
int CWinsockHook::select(int nfds, fd_set FAR * readfds, fd_set FAR * writefds,
              fd_set FAR * exceptfds, const struct timeval FAR * timeout) {
  int ret = SOCKET_ERROR;
#ifdef ENABLE_WPTRACK

  _sockets.ResetSslFd();
#else
#endif
  if (_select)
    ret = _select(nfds, readfds, writefds, exceptfds, timeout);

    if (ret > 0 && writefds && writefds->fd_count && !_connecting.IsEmpty()) {
    EnterCriticalSection(&cs);
    for (u_int i = 0; i < writefds->fd_count; i++) {
      SOCKET s = writefds->fd_array[i];
      if (_connecting.RemoveKey(s))
      {
#ifdef ENABLE_WPTRACK
        _sockets.Connected(s);
#else
#endif
      }
    }
    LeaveCriticalSection(&cs);
  }
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("select returned %d"), ret);
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int  CWinsockHook::getaddrinfo(PCSTR pNodeName, PCSTR pServiceName,
                         ADDRINFOA * pHints, PADDRINFOA * ppResult) {
  int ret = WSAEINVAL;
  CString name = CA2T(pNodeName);
#ifdef ENABLE_WPTRACK

  _sockets.ResetSslFd();
  void * context = NULL;
  if (!_test_state._exit)
    context = _dns.LookupStart(name);
  if (pHints)
    pHints->ai_flags |= AI_CANONNAME;
#else
#endif
  if (_getaddrinfo)
    ret = _getaddrinfo(CT2A((LPCTSTR)name), pServiceName, pHints, ppResult);

#ifdef ENABLE_WPTRACK

  if (!ret && !_test_state._exit) {
    PADDRINFOA addr = *ppResult;
    int count = 0;
    while (addr && count < 100) {
      count++;
      if (addr->ai_canonname)
        _dns.LookupAlias(name, (LPCTSTR)CA2T(addr->ai_canonname));
      if (addr->ai_addrlen >= sizeof(struct sockaddr_in) &&
          addr->ai_family == AF_INET) {
        struct sockaddr_in * ipName = (struct sockaddr_in *)addr->ai_addr;
        _dns.LookupAddress(context, ipName->sin_addr.S_un.S_addr);
      }
      addr = addr->ai_next;
    }
  }

  if (context && !_test_state._exit)
    _dns.LookupDone(context, ret);

#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int  CWinsockHook::GetAddrInfoW(PCWSTR pNodeName, PCWSTR pServiceName,
                          ADDRINFOW * pHints, PADDRINFOW * ppResult) {
  int ret = WSAEINVAL;
  CString name = CW2T(pNodeName);
#ifdef ENABLE_WPTRACK

  _sockets.ResetSslFd();
  void * context = NULL;
  if (!_test_state._exit)
      context = _dns.LookupStart(name);
  if (pHints)
    pHints->ai_flags |= AI_CANONNAME;
#else
#endif
  if (_GetAddrInfoW)
    ret = _GetAddrInfoW(CT2W((LPCWSTR)name), pServiceName, pHints, ppResult);

 #ifdef ENABLE_WPTRACK
 if (!ret && !_test_state._exit) {
    PADDRINFOW addr = (PADDRINFOW)*ppResult;
    int count = 0;
    while (addr && count < 100) {
      count++;
      if (addr->ai_canonname)
        _dns.LookupAlias(name, addr->ai_canonname);
      if (addr->ai_addrlen >= sizeof(struct sockaddr_in) &&
          addr->ai_family == AF_INET) {
        struct sockaddr_in * ipName = (struct sockaddr_in *)addr->ai_addr;
        _dns.LookupAddress(context, ipName->sin_addr.S_un.S_addr);
      }
      addr = addr->ai_next;
    }
  }

  if (context)
    _dns.LookupDone(context, ret);
#else
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
  Firefox falls back to gethostbyname if ipv6 isn't detected on the machine
-----------------------------------------------------------------------------*/
struct hostent * CWinsockHook::gethostbyname(const char * pNodeName) {
  struct hostent * ret = NULL;
  CString name = CA2T(pNodeName);
#ifdef ENABLE_WPTRACK

  _sockets.ResetSslFd();
  void * context = NULL;
  if (!_test_state._exit)
      context = _dns.LookupStart(name);
#else
#endif
  if (_gethostbyname)
    ret = _gethostbyname((LPCSTR)CT2A(name));
#ifdef ENABLE_WPTRACK

  if (ret && !_test_state._exit) {
    if (ret->h_name)
      _dns.LookupAlias(name, (LPCTSTR)CA2T(ret->h_name));
    char ** alias = ret->h_aliases;
    int count = 0;
    while (*alias && count < 100) {
      count++;
      _dns.LookupAlias(name, (LPCTSTR)CA2T(*alias));
      alias++;
    }
    for (int i = 0; ret->h_addr_list[i] != 0; i++)
      _dns.LookupAddress(context, *(u_long *)ret->h_addr_list[i]);
  }

  int err = WSAHOST_NOT_FOUND;
  if (ret)
    err = 0;
  if (context)
    _dns.LookupDone(context, err);
#else
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void CWinsockHook::ProcessOverlappedIo(SOCKET s, LPOVERLAPPED lpOverlapped,
                                  LPDWORD lpNumberOfBytesTransferred) {
#ifdef ENABLE_WPTRACK
  WsaBuffTracker buff;
  // handle a receive
  if (_recv_buffers.Lookup(lpOverlapped, buff)) {
    DWORD bytes = *lpNumberOfBytesTransferred;
    for (DWORD i = 0; i < buff._buffer_count && bytes; i++) {
      DWORD data_bytes = min(bytes, buff._buffers[i].len);
      if (data_bytes && buff._buffers[i].buf) {
        DataChunk chunk(buff._buffers[i].buf, data_bytes);
        _sockets.DataIn(s, chunk, false);
        bytes -= data_bytes;
      }
    }
  }
  // handle a send (where we modified the outbound headers)
  DataChunk chunk;
  if (_send_buffers.Lookup(lpOverlapped, chunk)) {
    _send_buffers.RemoveKey(lpOverlapped);
    if (lpNumberOfBytesTransferred) {
      DWORD len = *lpNumberOfBytesTransferred;
      if (_send_buffer_original_length.Lookup(lpOverlapped, len)) {
        _send_buffer_original_length.RemoveKey(lpOverlapped);
        *lpNumberOfBytesTransferred = len;
      }
    }
  }
#else
#endif
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
BOOL CWinsockHook::WSAGetOverlappedResult(SOCKET s, LPWSAOVERLAPPED lpOverlapped,
              LPDWORD lpcbTransfer, BOOL fWait, LPDWORD lpdwFlags) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - WSAGetOverlappedResult"), s);
#endif
  BOOL ret = false;
#ifdef ENABLE_WPTRACK
  _sockets.ResetSslFd();
#else
#endif
  if (_WSAGetOverlappedResult)
    ret = _WSAGetOverlappedResult(s, lpOverlapped, lpcbTransfer, fWait,
                                  lpdwFlags);
#ifdef ENABLE_WPTRACK

  if (ret && lpcbTransfer && !_test_state._exit)
    ProcessOverlappedIo(s, lpOverlapped, lpcbTransfer);
#else
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWinsockHook::WSAEventSelect(SOCKET s, WSAEVENT hEventObject,
                                long lNetworkEvents) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - WSAEventSelect"), s);
#endif
  int ret = SOCKET_ERROR;
#ifdef ENABLE_WPTRACK
  _sockets.ResetSslFd();
#else
#endif
  if (_WSAEventSelect)
    ret = _WSAEventSelect(s, hEventObject, lNetworkEvents);
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWinsockHook::WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEventObject,
                            LPWSANETWORKEVENTS lpNetworkEvents) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("%d - WSAEnumNetworkEvents"), s);
#endif
  int ret = SOCKET_ERROR;
#ifdef ENABLE_WPTRACK
  _sockets.ResetSslFd();
#else
#endif
  if (_WSAEnumNetworkEvents)
    ret = _WSAEnumNetworkEvents(s, hEventObject, lpNetworkEvents);
#ifdef ENABLE_WPTRACK

  if (!ret && !_test_state._exit &&
      lpNetworkEvents && lpNetworkEvents->lNetworkEvents & FD_CONNECT)
    _sockets.Connected(s);
#else
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWinsockHook::GetAddrInfoExA(PCSTR pName, PCSTR pServiceName, DWORD dwNameSpace,
    LPGUID lpNspId, ADDRINFOEXA *hints, PADDRINFOEXA *ppResult,
    struct timeval *timeout, LPOVERLAPPED lpOverlapped,
    LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine,
    LPHANDLE lpNameHandle) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("GetAddrInfoExA - %S"), pName);
#endif
  int ret = EAI_FAIL;
  CString name = (LPCWSTR)CA2W(pName);
#ifdef ENABLE_WPTRACK
  _sockets.ResetSslFd();

  void * context = NULL;

  if (!_test_state._exit)
      context = _dns.LookupStart(name);
  if (hints)
    hints->ai_flags |= AI_CANONNAME;
#else
#endif
  if (_GetAddrInfoExA)
    ret = _GetAddrInfoExA((LPCSTR)CW2A(name), pServiceName, dwNameSpace,
        lpNspId, hints, ppResult, timeout, lpOverlapped, lpCompletionRoutine,
        lpNameHandle);
#ifdef ENABLE_WPTRACK

  int err = WSAHOST_NOT_FOUND;
  if (ret == NO_ERROR && ppResult && *ppResult) {
    PADDRINFOEXA addr = *ppResult;
    int count = 0;
    while (addr && count < 100) {
      count++;
      if (addr->ai_canonname)
        _dns.LookupAlias(name, (LPCTSTR)CA2T(addr->ai_canonname));
      if (addr->ai_addrlen >= sizeof(struct sockaddr_in) &&
          addr->ai_family == AF_INET) {
        struct sockaddr_in * ipName = (struct sockaddr_in *)addr->ai_addr;
        _dns.LookupAddress(context, ipName->sin_addr.S_un.S_addr);
      }
      addr = addr->ai_next;
    }
    err = 0;
  }
  if (context)
    _dns.LookupDone(context, err);
#else
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int CWinsockHook::GetAddrInfoExW(PCWSTR pName, PCWSTR pServiceName, DWORD dwNameSpace,
    LPGUID lpNspId, ADDRINFOEXW *hints, PADDRINFOEXW *ppResult,
    struct timeval *timeout, LPOVERLAPPED lpOverlapped,
    LPLOOKUPSERVICE_COMPLETION_ROUTINE lpCompletionRoutine,
    LPHANDLE lpHandle) {
#ifdef TRACE_WINSOCK
  ATLTRACE(_T("GetAddrInfoExW - %s, overlapped = 0x%p, completion = 0x%p"),
      pName, lpOverlapped, lpCompletionRoutine);
#endif
  int ret = EAI_FAIL;
  CString name = pName;
#ifdef ENABLE_WPTRACK
  _sockets.ResetSslFd();
  void * context = NULL;

  if (!_test_state._exit)
      context = _dns.LookupStart(name);
  if (hints)
    hints->ai_flags |= AI_CANONNAME;

#else
#endif
  if (_GetAddrInfoExW)
    ret = _GetAddrInfoExW(LPCWSTR(name), pServiceName, dwNameSpace, lpNspId,
        hints, ppResult, timeout, lpOverlapped, lpCompletionRoutine,
        lpHandle);

#ifdef ENABLE_WPTRACK

  int err = WSAHOST_NOT_FOUND;
  if (ret == NO_ERROR && ppResult && *ppResult) {
    PADDRINFOEXW addr = *ppResult;
    int count = 0;
    while (addr && count < 100) {
      count++;
      if (addr->ai_canonname)
        _dns.LookupAlias(name, addr->ai_canonname);
      if (addr->ai_addrlen >= sizeof(struct sockaddr_in) &&
          addr->ai_family == AF_INET) {
        struct sockaddr_in * ipName = (struct sockaddr_in *)addr->ai_addr;
        _dns.LookupAddress(context, ipName->sin_addr.S_un.S_addr);
      }
      addr = addr->ai_next;
    }
    err = 0;
  }
  if (context)
    _dns.LookupDone(context, err);

#else
#endif
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
VOID WINAPI ThreadpoolCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context,
    PVOID Overlapped, ULONG IoResult, ULONG_PTR NumberOfBytesTransferred,
    PTP_IO Io) {
  if (g_pWinsockHook)
    g_pWinsockHook->ThreadpoolCallback(Instance, Context, Overlapped, IoResult,
                              NumberOfBytesTransferred, Io);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void CWinsockHook::ThreadpoolCallback(PTP_CALLBACK_INSTANCE Instance, PVOID Context,
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
#ifdef ENABLE_WPTRACK

  if (s != INVALID_SOCKET && _connecting.RemoveKey(s)) {
    _sockets.Connected(s);
  }
  if (s != INVALID_SOCKET &&
      Overlapped &&
      NumberOfBytesTransferred &&
      !_test_state._exit) {
    DWORD bytes = NumberOfBytesTransferred;
    ProcessOverlappedIo(s, (LPOVERLAPPED)Overlapped, &bytes);
    NumberOfBytesTransferred = bytes;
  }
#else
#endif
  if (callback)
    callback(Instance, Context, Overlapped, IoResult, NumberOfBytesTransferred,
             Io);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
PTP_IO CWinsockHook::CreateThreadpoolIo(HANDLE fl,
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
void CWinsockHook::CloseThreadpoolIo(PTP_IO pio, bool kernelBase) {
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
void CWinsockHook::StartThreadpoolIo(PTP_IO pio, bool kernelBase) {
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
int CWinsockHook::WSAIoctl(SOCKET s, DWORD dwIoControlCode, LPVOID lpvInBuffer,
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
      LPFN_CONNECTEX_WPT connect_ex_hook = AmhHook_ConnectEx;
      memcpy(lpvOutBuffer, &connect_ex_hook, sizeof(LPFN_CONNECTEX_WPT));
    }
  }
  return ret;
}
