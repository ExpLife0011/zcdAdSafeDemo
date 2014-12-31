#include "stdafx.h"
#include "Windows.h"
#include "tlhelp32.h"



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
     
void enableProcessPrivilege()
{
    EnableDebugPrivilege(SE_DEBUG_NAME);
}

int KGRemoteInjectLibrary(UINT processPID,LPCTSTR dllPath)
{
  HMODULE hKernel32 = ::GetModuleHandle(_T("Kernel32"));
  // 1. 在远程进程中为szLibPath 分配内存
  UINT uProcID = processPID;
  LPCTSTR lpcszDllPath = dllPath;
  //打开进程
  HANDLE hProcess;
  hProcess= ::OpenProcess(PROCESS_ALL_ACCESS,
                FALSE,uProcID);
  if(!hProcess)
  {
    TRACE("打开目标进程失败\n");
    CloseHandle(hProcess);
    return 3;
  }

  int allocSize=(lstrlen(lpcszDllPath)+1)*sizeof(TCHAR);
  //申请内存存放以DLL文件路径名
  TCHAR *pDllFileNameRemote=(TCHAR*)VirtualAllocEx(hProcess,NULL,allocSize,MEM_COMMIT,PAGE_READWRITE);
  //写DLL文件路径名到远程进程
  if(!pDllFileNameRemote)
  {
    TRACE("申请目标进程内存失败\n");
    CloseHandle(hProcess);
    return 4;
  }
  if(!WriteProcessMemory(hProcess,pDllFileNameRemote,(PVOID)lpcszDllPath, allocSize, NULL))
  {
    TRACE("写目标进程内存失败\n");
    VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
    CloseHandle(hProcess);
    return 5;
  }

  //创建远程线程，卸载远程DLL
  HANDLE hThreadRemote =NULL;
  hThreadRemote = CreateRemoteThread(hProcess,NULL,0,
    (LPTHREAD_START_ROUTINE) ::GetProcAddress( hKernel32,"LoadLibraryW" )
    ,(LPVOID)pDllFileNameRemote,0,NULL);
  if(!hThreadRemote)
  {
    TRACE("创建远程线程运行LoadLibrary失败\n");
    DWORD dw = GetLastError();
    VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
    CloseHandle(hThreadRemote);
    CloseHandle(hProcess);
    return 8;
  }

  WaitForSingleObject(hThreadRemote,INFINITE);

  DWORD FreeLibInfo;
  GetExitCodeThread(hThreadRemote,&FreeLibInfo);
  if(!FreeLibInfo)
  {
    TRACE("LoadLibrary失败\n");
    VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
    CloseHandle(hThreadRemote);
    CloseHandle(hProcess);
    return 9;
  }
  TRACE("卸载DLL成功\n");  OutputDebugStringA("卸载DLL成功\n");
  VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
  CloseHandle(hThreadRemote);
  CloseHandle(hProcess);
  return 0;
}

int KGRemoteEjectLibrary(UINT processPID,LPCTSTR dllPath)
{
  UINT uProcID = processPID;
  LPCTSTR lpcszDllPath = dllPath;
  //打开进程
  HANDLE hProcess;
  hProcess= ::OpenProcess(PROCESS_ALL_ACCESS,
//                PROCESS_CREATE_THREAD |
//                PROCESS_VM_OPERATION |
//                PROCESS_VM_WRITE ,
                FALSE,uProcID);
  if(!hProcess)
  {
    TRACE("打开目标进程失败\n");
    CloseHandle(hProcess);
    return 3;
  }

  int allocSize=(lstrlen(lpcszDllPath)+1)*sizeof(TCHAR);
  //申请内存存放以DLL文件路径名
  TCHAR *pDllFileNameRemote=(TCHAR*)VirtualAllocEx(hProcess,NULL,allocSize,MEM_COMMIT,PAGE_READWRITE);
  //写DLL文件路径名到远程进程
  if(!pDllFileNameRemote)
  {
    TRACE("申请目标进程内存失败\n");
    CloseHandle(hProcess);
    return 4;
  }
  if(!WriteProcessMemory(hProcess,pDllFileNameRemote,(PVOID)lpcszDllPath, allocSize, NULL))
  {
    TRACE("写目标进程内存失败\n");
    VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
    CloseHandle(hProcess);
    return 5;
  }

  while(1)
  {
    HMODULE hKernel32 = ::GetModuleHandle(_T("Kernel32.exe"));
    //创建远程线程,GetModuleHandle获取刚刚注入的DLL的句柄
    HANDLE hThreadRemote=CreateRemoteThread(hProcess,NULL,0,
    (PTHREAD_START_ROUTINE)GetModuleHandleW
  //(PTHREAD_START_ROUTINE)::GetProcAddress(hKernel32,"GetModuleHandleW")
      ,pDllFileNameRemote,0,NULL);
    if(!hThreadRemote)
    {
      TRACE("创建目标线程运行GetModuleHandle失败\n"); OutputDebugStringA("创建目标线程运行GetModuleHandle失败\n");
      VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
      CloseHandle(hThreadRemote);
      CloseHandle(hProcess);
      return 6;
    }

    WaitForSingleObject(hThreadRemote,INFINITE);
    DWORD hDllRemote;
    GetExitCodeThread(hThreadRemote,&hDllRemote);
    if(!hDllRemote)
    {
      TRACE("未加载DLL\n"); OutputDebugStringA("未加载DLL\n");//卸载结束！！
      VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
      CloseHandle(hThreadRemote);
      CloseHandle(hProcess);
      return 7;
    }
    CloseHandle(hThreadRemote);
    hThreadRemote=NULL;


    //创建远程线程，卸载远程DLL
    hThreadRemote = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)FreeLibrary,(LPVOID)hDllRemote,0,NULL);
    if(!hThreadRemote)
    {
      TRACE("创建远程线程运行FreeLibrary失败\n");
      VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
      CloseHandle(hThreadRemote);
      CloseHandle(hProcess);
      return 8;
    }
    WaitForSingleObject(hThreadRemote,INFINITE);

    DWORD FreeLibInfo;
    GetExitCodeThread(hThreadRemote,&FreeLibInfo);
    if(!FreeLibInfo)
    {
      TRACE("FreeLibrary失败\n");
      VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
      CloseHandle(hThreadRemote);
      CloseHandle(hProcess);
      return 9;
    }
    TRACE("卸载DLL成功\n");  OutputDebugStringA("卸载DLL成功\n");
    VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
    CloseHandle(hThreadRemote);
    CloseHandle(hProcess);
    break;
  }
  return 0;
}



DWORD KGEnumProcEjectLibrary(LPCTSTR lpcszPath)
{
  //快照进程信息
  HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  PROCESSENTRY32 lppe;
  lppe.dwSize=sizeof(PROCESSENTRY32);
  //获得第一个进程的信息
  BOOL bOK = Process32FirstW(hSnapshot,&lppe);
  while(bOK)
  {
//   lppe.th32ProcessID;
    TRACE( lppe.szExeFile);TRACE("\n");
    OutputDebugStringW(lppe.szExeFile);OutputDebugStringA("\n");
    KGRemoteEjectLibrary(lppe.th32ProcessID,lpcszPath);
   bOK = Process32Next(hSnapshot,&lppe);
  }
  CloseHandle(hSnapshot);

  return 0;

}


//#define BROWSER_PROCESS _T("iexplore.exe")
//#define BROWSER_PROCESS _T("chrome.exe")
#define BROWSER_PROCESS _T("HookDialogDemo.exe")

DWORD KGEnumProcInjectLibraryIE(LPCTSTR lpcszPath)
{
  enableProcessPrivilege();
  //快照进程信息
  HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  PROCESSENTRY32 lppe;
  lppe.dwSize=sizeof(PROCESSENTRY32);
  //获得第一个进程的信息
  BOOL bOK = Process32FirstW(hSnapshot,&lppe);
  while(bOK)
  {
//   lppe.th32ProcessID;
    if( 0==lstrcmpi(lppe.szExeFile,BROWSER_PROCESS))
    {
      TRACE( lppe.szExeFile);TRACE("\n");
      OutputDebugStringW(lppe.szExeFile);OutputDebugStringA("\n");
      KGRemoteInjectLibrary(lppe.th32ProcessID,lpcszPath);
    }
    bOK = Process32Next(hSnapshot,&lppe);
  }
  CloseHandle(hSnapshot);

  return 0;

}

DWORD KGEnumProcEjectLibraryIE(LPCTSTR lpcszPath)
{
  //快照进程信息
  HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  PROCESSENTRY32 lppe;
  lppe.dwSize=sizeof(PROCESSENTRY32);
  //获得第一个进程的信息
  BOOL bOK = Process32FirstW(hSnapshot,&lppe);
  while(bOK)
  {
//   lppe.th32ProcessID;
    if( 0==lstrcmpi(lppe.szExeFile,BROWSER_PROCESS))
    {
      TRACE( lppe.szExeFile);TRACE("\n");
      OutputDebugStringW(lppe.szExeFile);OutputDebugStringA("\n");
      KGRemoteEjectLibrary(lppe.th32ProcessID,lpcszPath);
    }
    bOK = Process32Next(hSnapshot,&lppe);
  }
  CloseHandle(hSnapshot);

  return 0;

}
