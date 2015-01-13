#include "stdafx.h"
#include "AGVersionHelper.h"

#pragma comment (lib, "Version.lib")

AGVersionHelper::AGVersionHelper()
{
}
AGVersionHelper::~AGVersionHelper()
{
}

BOOL AGVersionHelper::GetFileVersion(LPCTSTR strFile)
{
  mdwFileVersionMS = 0;
  mdwFileVersionLS = 0;

  TCHAR szVersionBuffer[8192] = _T("");
  DWORD dwVerSize;
  DWORD dwHandle;

  dwVerSize = GetFileVersionInfoSize(strFile, &dwHandle);
  if (dwVerSize == 0)  return FALSE;

  if (GetFileVersionInfo(strFile, 0, dwVerSize, szVersionBuffer))
  {
    VS_FIXEDFILEINFO * pInfo;
    unsigned int nInfoLen;

    if (VerQueryValue(szVersionBuffer, _T("\\"), (void**)&pInfo, &nInfoLen))
    {
      mdwFileVersionMS = pInfo->dwFileVersionMS;
      mdwFileVersionLS = pInfo->dwFileVersionLS;

      return TRUE;
    }
  }

  return TRUE;
}


