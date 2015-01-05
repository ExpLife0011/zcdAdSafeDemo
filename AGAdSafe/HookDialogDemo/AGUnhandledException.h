
#pragma once

class AGUnhandledException
{
public:
  AGUnhandledException();
  ~AGUnhandledException();

  static void WINAPI clearcache();
  static LONG WINAPI callback_func(
      __in struct _EXCEPTION_POINTERS *ExceptionInfo
  );
};

