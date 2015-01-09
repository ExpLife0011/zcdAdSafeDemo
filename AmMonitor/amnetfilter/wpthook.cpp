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

// wpthook.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
//#include "shared_mem.h"
#include "wpthook.h"

//#define WriteAGLog(x) 

//#include "hook_winsock.cc"
//#include "hook_wininet.cc"
//#include "hook_ws2.cc"
//#include "hook_winhttp.cc"
WptHook * global_hook = NULL;
extern HINSTANCE global_dll_handle;


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
WptHook::WptHook(void):
  ws_hook_(0)
#ifdef _WINHTTP_ENABLE_
    ,winhttp_hook_(0)
#endif
    ,wininet_hook_(0),winsock_hook_(0)
    ,mModWs2(0),mModWinhttp(0),mModWinInet(0)
{
  ws_hook_ = new CWs2Hook;
  //wininet_hook_ = new CWinInetHook;
  //winsock_hook_ = new CWinsockHook;
 // winhttp_hook_ = new CWinHttpHook;

  //mModWs2 = LoadLibraryA("ws2_32.dll"); 
  //mModWinhttp = LoadLibraryA("winhttp.dll"); 
  //mModWinInet = LoadLibraryA("wininet.dll"); 
  //Init();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
WptHook::~WptHook(void)
{
    if( ws_hook_)
    {
        delete ws_hook_;
        ws_hook_ = NULL;
    }
    if( wininet_hook_)
    {
        delete wininet_hook_;
        wininet_hook_ = NULL;
    }
    if( winsock_hook_)
    {
        delete winsock_hook_;
        winsock_hook_ = NULL;
    }
#ifdef _WINHTTP_ENABLE_
    if( winhttp_hook_)
    {
        delete winhttp_hook_;
        winhttp_hook_ = NULL;
    }
#endif


    if(mModWs2) FreeLibrary(mModWs2);
    if(mModWinhttp) FreeLibrary(mModWinhttp);
    if(mModWinInet) FreeLibrary(mModWinInet);

}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void WptHook::Init()
{
    if(ws_hook_) ws_hook_->Init();
    if(wininet_hook_) wininet_hook_->Init();
    if(winsock_hook_) winsock_hook_->Init();
#ifdef _WINHTTP_ENABLE_ 
    if(winhttp_hook_) winhttp_hook_->Init();
#endif
}

void WptHook::Destroy()
{
    if(ws_hook_) ws_hook_->Destroy();
    if(wininet_hook_) wininet_hook_->Destroy();
    if(winsock_hook_) winsock_hook_->Destroy();
#ifdef _WINHTTP_ENABLE_
    if(winhttp_hook_) winhttp_hook_->Destroy();
#endif
}