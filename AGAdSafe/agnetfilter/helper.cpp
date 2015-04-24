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

  if (!lstrcmpi(exe, shared_browser_exe)) {
    LPTSTR cmdline = GetCommandLine();
    if (!lstrcmpi(exe, _T("chrome.exe"))) {
      if (_tcsstr(cmdline, _T("http://127.0.0.1:8888/blank.html")))
        ok = true;
    } else if (!lstrcmpi(exe, _T("firefox.exe"))) {
      if (_tcsstr(cmdline, _T("http://127.0.0.1:8888/blank.html")))
        ok = true;
    } else if (!lstrcmpi(exe, _T("iexplore.exe"))) {
      ok = true;
    }
  } else if (!lstrcmpi(_T("safari.exe"), shared_browser_exe) &&
             !lstrcmpi(exe, _T("WebKit2WebProcess.exe"))) {
      ok = true;
  }

  return ok;
}
