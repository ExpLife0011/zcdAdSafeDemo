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
//#include "shared_mem.h"

#pragma comment(lib, "ws2_32.lib")
#define INCL_WINSOCK_API_TYPEDEFS 1
#include <WinSock2.h>


CWs2Hook * g_pWsHook = NULL;



/******************************************************************************
*******************************************************************************
**                                                        **
**                          Stub Functions                   **
**                                                        **
*******************************************************************************
******************************************************************************/


static int WSAAPI  AmhConnectHook(IN SOCKET s, const struct sockaddr FAR * name, IN int namelen)
{

  WriteAGLog("connect_AGHook Begin");
  int ret = SOCKET_ERROR;
//if (g_pWsHook)
//  ret = g_pWsHook->connect(s, name, namelen);
//  return ret;

  __try{
    if(g_pWsHook)
    {
      WriteAGLog("...");
      BOOL bMark = FALSE;
      if (name!=NULL)
      {
          WriteAGLog(".1A");
        sockaddr_in sin;
        memcpy(&sin, &name, sizeof(sin));

        WriteAGLog(".1B");
        if(sin.sin_port==htons(9222))
        {
          bMark = TRUE;
        }
      }
      WriteAGLog(".1C");
      SOCKADDR_IN stSvrAddrIn = {0};
      stSvrAddrIn.sin_family = AF_INET;
      stSvrAddrIn.sin_port = htons(8888);
      stSvrAddrIn.sin_addr.s_addr = inet_addr("127.0.0.1");
      //if(!shared_proxy_enabled) bMark = TRUE;
      WriteAGLog(".1F");
      if (bMark)
      {
          WriteAGLog(".2A");
        ret = g_pWsHook->connect(s, name, namelen);
      }
      else
      {
          WriteAGLog(".2B");
        ret =  g_pWsHook->connect(s, (SOCKADDR*)&stSvrAddrIn/*name*/, sizeof(SOCKADDR)/*namelen*/);
      }

      if (SOCKET_ERROR == ret)
      {
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
  :hook_(0),_connect(0)

{
  WriteAGLog("CWs2Hook::CWs2Hook");
  InitializeCriticalSection(&cs);

}

CWs2Hook::~CWs2Hook()
{
WriteAGLog("CWs2Hook::~CWs2Hook");
if (g_pWsHook == this)
  {
    g_pWsHook = NULL;
    //hook.removeHook(_connect);
    Destroy();
    //_connect = NULL;
  }

  DeleteCriticalSection(&cs);
}
void CWs2Hook::Destroy(void)
{
WriteAGLog("CWs2Hook::Destroy");
if( hook_)
    {
  delete hook_;
  hook_ = NULL;
  
    }
    _connect = NULL;
}

void CWs2Hook::Init()
{
WriteAGLog("CWs2Hook::Init");
if (g_pWsHook || hook_)
    return;


  hook_ = new NCodeHookIA32();
  g_pWsHook = this;
  WriteAGLog("CWs2Hook::createHookByName");
  // install the code hooks
  _connect = hook_->createHookByName("ws2_32.dll", "connect", AmhConnectHook);

  CString str;
  LPBYTE p = (LPBYTE)AmhConnectHook;
  str.Format(_T("Addr[%x][%x %x %x %x %x %x]"),AmhConnectHook,p[0],p[1],p[2],p[3],p[4],p[5]);
  ::OutputDebugString(str);
   p = (LPBYTE)_connect;
  str.Format(_T("Addr[%x][%x %x %x %x %x %x]"),_connect,p[0],p[1],p[2],p[3],p[4],p[5]);
  ::OutputDebugString(str);

}

int CWs2Hook::connect(__in SOCKET s,
__in_bcount(namelen) const struct sockaddr FAR * name,
__in int namelen
)
//IN SOCKET s, const struct sockaddr FAR * name, IN int namelen)
{
  int ret = SOCKET_ERROR;
  WriteAGLog(" CWs2Hook::connect.Begin");
  if (_connect)
  {
     WriteAGLog(" CWs2Hook::connect.1");
    ret = _connect(s, name, namelen);
   WriteAGLog(" CWs2Hook::connect.2");
  }
  WriteAGLog(" CWs2Hook::connect.END");
  return ret;
}

