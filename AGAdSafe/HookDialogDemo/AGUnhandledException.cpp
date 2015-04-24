#include "stdafx.h"
#include "AGUnhandledException.h"
#include <DbgHelp.h>
#pragma comment (lib, "dbghelp.lib")

AGUnhandledException::AGUnhandledException()
{

}

AGUnhandledException::~AGUnhandledException()
{

}
void WINAPI AGUnhandledException::clearcache()
{
  CString strANGDir = _T("D:\\work");
  CString strDumpFile = strANGDir + _T("\\appcrash.dmp");
  ::DeleteFile(strDumpFile);
}

LONG WINAPI AGUnhandledException::callback_func(
    __in struct _EXCEPTION_POINTERS *pExceptionInfo
)

{
  CString strANGDir = _T("D:\\work");
  CString strDumpFile = strANGDir + _T("\\appcrash.dmp");

  HANDLE hFile = ::CreateFile(  strDumpFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if( hFile != INVALID_HANDLE_VALUE)
  {
    MINIDUMP_EXCEPTION_INFORMATION einfo;
    einfo.ThreadId = ::GetCurrentThreadId();
    einfo.ExceptionPointers = pExceptionInfo;
    einfo.ClientPointers = FALSE;

    ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, MiniDumpNormal, &einfo, NULL, NULL);
    ::CloseHandle(hFile);
  }
  return EXCEPTION_EXECUTE_HANDLER;
}
