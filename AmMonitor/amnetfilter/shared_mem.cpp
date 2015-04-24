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

#include "stdafx.h"
#include "shared_mem.h"
#include "shared_func.h"

#pragma once


//-------------------------------------------------------------
// shared data 
// Notice: seen by both: the instance of "HookInjEx.dll" mapped
//   into "explorer.exe" as well as by the instance
//   of "HookInjEx.dll" mapped into our "HookInjEx.exe"
#pragma data_seg (".SharedDataName")

UINT WM_AMMONITOR = 0;
const TCHAR GUID_HOOKMSG[_MAX_PATH] = {_T("WM_AMMONITOR_RK" )};
UINT WM_AMMONITORRET = 0;
const TCHAR GUID_HOOKMSGRET[_MAX_PATH] = {_T("WM_AMMONITOR_RET" )};

//所有进程都要访问，且hook一次就行
HHOOK global_hCallwndHook = NULL;
HHOOK g_hKbHook = NULL;

WCHAR shared_browser_exe[MAX_PATH] = {NULL};
DWORD shared_browser_process_id = 0;
const TCHAR shared_app_exe[] = _T("AmMonitor.dll");

bool shared_proxy_enabled = true;
char shared_proxy_ip[MAX_PATH] = "127.0.0.1";
int shared_proxy_port = 8888;
int shared_client_port = 80;
char shared_proxy_inetA[_MAX_PATH] = "http://127.0.0.1:8888";
WCHAR shared_proxy_inetW[_MAX_PATH] = L"http://127.0.0.1:8888";

HHOOK shared_hook_handle = 0;
WCHAR shared_results_file_base[MAX_PATH] = {NULL};
DWORD shared_test_timeout = 120000;
bool  shared_cleared_cache = false;
DWORD shared_current_run = 0;
WCHAR shared_log_file[MAX_PATH] = {NULL};
int   shared_debug_level = 0;
int   shared_cpu_utilization = 0;
bool  shared_has_gpu = false;
int   shared_result = -1;
#pragma data_seg ()

#pragma comment(linker,"/SECTION:.SharedDataName,RWS")

void SHARED_API SetProxyEnabled(bool enable)
{
  shared_proxy_enabled = enable;
}


/*-----------------------------------------------------------------------------
  Set the base file name to use for results files
-----------------------------------------------------------------------------*/
void SHARED_API SetResultsFileBase(const WCHAR * file_base) {
  lstrcpyW(shared_results_file_base, file_base);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void SHARED_API SetTestTimeout(DWORD timeout) {
  shared_test_timeout = timeout;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void SHARED_API SetClearedCache(bool cleared_cache) {
  shared_cleared_cache = cleared_cache;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
bool SHARED_API GetClearedCache() {
  return shared_cleared_cache;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void SHARED_API SetCurrentRun(DWORD run) {
  shared_current_run = run;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void SHARED_API SetHasGPU(bool has_gpu) {
  shared_has_gpu = has_gpu;
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void SHARED_API SetDebugLevel(int level, const WCHAR * log_file) {
  shared_debug_level = level;
  lstrcpyW(shared_log_file, log_file);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int SHARED_API GetCPUUtilization() {
  return shared_cpu_utilization;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void SHARED_API SetCPUUtilization(int utilization) {
  shared_cpu_utilization = utilization;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void SHARED_API ResetTestResult() {
  shared_result = -1;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
int SHARED_API GetTestResult() {
  return shared_result;
}

/*-----------------------------------------------------------------------------
  Set the exe name for the browser we are currently using
-----------------------------------------------------------------------------*/
void SHARED_API SetBrowserExe(const WCHAR * exe) {
  if (exe)
    lstrcpyW(shared_browser_exe, exe);
  else
    lstrcpyW(shared_browser_exe, L"");
  shared_browser_process_id = 0;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
DWORD SHARED_API GetBrowserProcessId() {
  return shared_browser_process_id;
}
