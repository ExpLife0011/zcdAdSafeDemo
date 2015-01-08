#include "stdafx.h"
#include "helper.h"
#include "shared_mem.h"
/*-----------------------------------------------------------------------------
    See if this is the browser process we're actually interested in
    instrumenting (for browsers that have multi-process architectures)
-----------------------------------------------------------------------------*/
bool IsCorrectBrowserProcess(LPCTSTR exe)
{
  bool ok = false;


    if (!lstrcmpi(exe, _T("chrome.exe"))) {
        ok = true;
    } else if (!lstrcmpi(exe, _T("firefox.exe"))) {
        ok = true;
    } else if (!lstrcmpi(exe, _T("iexplore.exe"))) {
      ok = true;
    } else if (!lstrcmpi(exe, shared_browser_exe))
  {
  } 

  return ok;
}
