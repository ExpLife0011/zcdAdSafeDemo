#pragma once


#include <mswsock.h>
#include "ncodehook/NCodeHookInstantiation.h"


class CMSWSockHook
{
public:
  CMSWSockHook(void);
  virtual ~CMSWSockHook(void);
  void Init(void);
  void Destroy(void);
  
  HINTERNET WSPStartup(LPCWSTR lpszAgent, DWORD dwAccessType, LPCWSTR lpszProxy, LPCWSTR lpszProxyBypass, DWORD dwFlags);

private:
  CRITICAL_SECTION cs;
  NCodeHookIA32  hook;


  LPWINHTTPOPENW _WinHttpOpenW;
};

