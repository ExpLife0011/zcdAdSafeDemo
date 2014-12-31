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
#include "hook_ws2.h"
#include "util.h"

#pragma comment(lib, "ws2_32.lib")
#define INCL_WINSOCK_API_TYPEDEFS 1
//#include <windows.h>
#include <WinSock2.h>
//#include <ws2tcpip.h>

static CWs2Hook * g_pWsHook = NULL;

/******************************************************************************
*******************************************************************************
**                                                        **
**                          Stub Functions                   **
**                                                        **
*******************************************************************************
******************************************************************************/


int WSAAPI AmhConnectHook(IN SOCKET s, const struct sockaddr FAR * name, IN int namelen)
{
  WriteAGLog("connect_AGHook Begin");
  int ret = SOCKET_ERROR;
  __try{
    if(g_pWsHook)
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

        ret = g_pWsHook->connect(s, name, namelen);
      }
      else
      {
        //ret =  g_pWsHook->connect(s, name, namelen);
        //unsigned long ul = 0;
        //ioctlsocket (s, FIONBIO, (unsigned long*)&ul);
        ret =  g_pWsHook->connect(s, (SOCKADDR*)&stSvrAddrIn/*name*/, sizeof(SOCKADDR)/*namelen*/);
      }

      if (SOCKET_ERROR == ret)
      {
        //DWORD dw = GetLastError();
        //LPVOID lpMsgBuf;
//     FormatMessageA(
//      FORMAT_MESSAGE_ALLOCATE_BUFFER |
//      FORMAT_MESSAGE_FROM_SYSTEM |
//      FORMAT_MESSAGE_IGNORE_INSERTS,
//      NULL,
//      GetLastError(),
//      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
//      (LPSTR) &lpMsgBuf,
//      0,
//      NULL
//      );
        //WriteAGLog((LPCSTR)lpMsgBuf);
        WriteAGLog("ret==SOCKET_ERROR");
      }
    }
  }__except(1){
    WriteAGLog("connect_AGHook Failed");
  }
  WriteAGLog("connect_AGHook End");
  return ret;
}


///////////////////////////////////



////////////
CWs2Hook::CWs2Hook()

{
  InitializeCriticalSection(&cs);

}

CWs2Hook::~CWs2Hook()
{
  if (g_pWsHook == this)
  {
    g_pWsHook = NULL;
    //hook.removeHook(_connect);
    _connect = NULL;
  }

  DeleteCriticalSection(&cs);
}
void CWs2Hook::Init()
{
  if (!g_pWsHook)
    g_pWsHook = this;

  // install the code hooks
  _connect = hook.createHookByName("ws2_32.dll", "connect", AmhConnectHook);

}

int  CWs2Hook::connect(IN SOCKET s, const struct sockaddr FAR * name, IN int namelen)
{
  int ret = SOCKET_ERROR;

  if (_connect)
  {
    ret = _connect(s, name, namelen);
  }

  return ret;
}

