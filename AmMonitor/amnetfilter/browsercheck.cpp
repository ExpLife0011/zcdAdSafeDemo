#include "stdafx.h"
#include "browsercheck.h"





int BrowserCheck::chekcPlatformType()
{
  typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
  LPFN_ISWOW64PROCESS fnIsWow64Process;
  BOOL bIsWow64 = FALSE;
  fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(L"kernel32"),"IsWow64Process");
  if (NULL != fnIsWow64Process)
  {
    fnIsWow64Process(::GetCurrentProcess(),&bIsWow64);
  }
  return bIsWow64;
}
