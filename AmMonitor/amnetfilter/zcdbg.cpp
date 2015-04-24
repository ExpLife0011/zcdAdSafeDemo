#include "stdafx.h"
#include "zcdbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>

namespace zcdbg
{

void OutputDebugLogA(LPCSTR cstr)
{
  OutputDebugStringA(cstr);
  return;
}

void OutputDebugLogW(LPCWSTR cstr)
{
  OutputDebugStringW(cstr);
  return;
}

}
//

void DbgWriteFileLogA(LPCSTR cstr)
{
   // chrome下，会出现183错误：创建已存在的文件。故此函数直接返回。
  char szFileFullPath[MAX_PATH*sizeof(char)];
  ::GetModuleFileNameA(NULL, szFileFullPath, MAX_PATH*sizeof(char));

  std::string s = szFileFullPath;
  int len = strlen(szFileFullPath);
  char *cfilename = strrchr(szFileFullPath, '\\');
  //char szFileName[128]='\0';
  //strcpy(szFileName, szFileFullPath+begin);
  char log[MAX_PATH];
  memset(log, 0, MAX_PATH*sizeof(char));
  lstrcpyA(log, cfilename+1);
  int loglen = strlen(log);
  memset(log+loglen-3, 'l', sizeof(char));
  memset(log+loglen-2, 'o', sizeof(char));
  memset(log+loglen-1, 'g', sizeof(char));

  char prefix[MAX_PATH]; memset(prefix, 0, MAX_PATH);
  lstrcpyA(prefix, "e:\\temp\\logs\\");
  char *logpath = lstrcatA(prefix, log);

  char curlog[MAX_PATH] ={0};
  lstrcpyA(curlog, szFileFullPath);
  lstrcatA(curlog,".log");
  logpath = curlog;
  //
  struct tm *local;
  time_t t;
  t=time(NULL);
  local = localtime(&t);

  //
  FILE *fp = fopen(logpath, "a+");
  if (fp)
  {
    fprintf(fp, "%2d:%2d:%2d\t%s\n", local->tm_hour, local->tm_min, local->tm_sec, cstr);
    fclose(fp);
    fp = NULL;
  }

}
