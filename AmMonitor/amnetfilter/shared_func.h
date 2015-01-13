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

#pragma once

#ifdef WPTDRIVER
#define SHARED_EXPFUNC __declspec( dllimport )
#define SHARED_API 
#else
#define SHARED_EXPFUNC __declspec( dllexport )
#define SHARED_API 
#endif

extern "C" {
SHARED_EXPFUNC void SHARED_API SetProxyEnabled(bool enable);

SHARED_EXPFUNC void SHARED_API SetResultsFileBase(const WCHAR * file_base);
SHARED_EXPFUNC void SHARED_API SetTestTimeout(DWORD timeout);
SHARED_EXPFUNC void SHARED_API SetClearedCache(bool cleared_cache);
SHARED_EXPFUNC bool SHARED_API GetClearedCache();
SHARED_EXPFUNC void SHARED_API SetCurrentRun(DWORD run);
SHARED_EXPFUNC void SHARED_API SetDebugLevel(int level, const WCHAR * log_file);
SHARED_EXPFUNC int  SHARED_API GetCPUUtilization();
SHARED_EXPFUNC void SHARED_API SetCPUUtilization(int utilization);
SHARED_EXPFUNC void SHARED_API SetHasGPU(bool has_gpu);
SHARED_EXPFUNC void SHARED_API ResetTestResult();
SHARED_EXPFUNC int  SHARED_API GetTestResult();
SHARED_EXPFUNC void SHARED_API SetBrowserExe(const WCHAR * exe);
SHARED_EXPFUNC DWORD SHARED_API GetBrowserProcessId();
}
