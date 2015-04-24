#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
using namespace std;

void WriteAGLog(LPCSTR cstr)
{
  OutputDebugStringA(cstr);
	return;	// chrome下，会出现183错误：创建已存在的文件。故此函数直接返回。
	char szFileFullPath[MAX_PATH*sizeof(char)];
	::GetModuleFileNameA(NULL, szFileFullPath, MAX_PATH*sizeof(char));

	string s = szFileFullPath;
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


// DLL_Injection.cpp : 定义 DLL 应用程序的导出函数。
//
/* 
#include "stdafx.h"
//#include "DLL_Injection.h"
#include <process.h>
#include <list>
#include <vector>
#include <TlHelp32.h>
std::list<DLL> Dll_List;
 
extern DWORD EventSize;
 
//提升进程权限
bool EnableDebugPrivilege(const LPCTSTR name)
{
     HANDLE token;
     TOKEN_PRIVILEGES tp;
    //打开进程令牌环
     if(!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&token))
   {
      return false;
   }
   //获得进程本地唯一ID
    LUID luid;
    if(!LookupPrivilegeValue(NULL,name,&luid))
    {
         return false;
    }
    tp.PrivilegeCount=1;
    tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
    tp.Privileges[0].Luid=luid;
    //调整进程权限
    if(!AdjustTokenPrivileges(token,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
    {
        return false;
    }
    return true;
}
     
bool LoadDllToProcess(TCHAR *Path,DWORD Id)
{
    EnableDebugPrivilege(SE_DEBUG_NAME);
    HANDLE Process=OpenProcess(PROCESS_ALL_ACCESS,FALSE,Id);
    if(Process==NULL)
    {
        return false;
    }
    else
    {
        size_t Size=wcslen(Path)*sizeof(TCHAR)+1;
        BYTE *Param=(BYTE *)VirtualAllocEx(Process,NULL,Size,MEM_COMMIT,PAGE_READWRITE);
        if(Param)
        {
            if(WriteProcessMemory(Process,Param,Path,Size,NULL))
            {
                VirtualProtectEx(Process,Param,Size,PAGE_READONLY,NULL);
                if(CreateRemoteThread(Process,NULL,0,(LPTHREAD_START_ROUTINE)LoadLibrary,
                                     Param,NULL,NULL)==NULL)
                {
                    return false;
                }
            }
            else
            {
                CloseHandle(Process);
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

*/