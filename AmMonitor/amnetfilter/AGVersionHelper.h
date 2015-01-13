
#pragma once
#include <stdint.h>

class AGVersionHelper
{
public:
  AGVersionHelper();
  ~AGVersionHelper();

  CString GetUpdateAppPath(CString updAppName);
  
  BOOL GetFileVersion(LPCTSTR strFile);
  CString GetFileVersionCString();
protected:
  DWORD mdwFileVersionMS;
  DWORD mdwFileVersionLS;
protected:

};

