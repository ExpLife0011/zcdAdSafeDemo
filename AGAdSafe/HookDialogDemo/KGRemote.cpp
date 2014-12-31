#include "stdafx.h"
#include "Windows.h"
#include "tlhelp32.h"



//��������Ȩ��
bool EnableDebugPrivilege(const LPCTSTR name)
{
     HANDLE token;
     TOKEN_PRIVILEGES tp;
    //�򿪽������ƻ�
     if(!OpenProcessToken(GetCurrentProcess(),
        TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&token))
   {
      return false;
   }
   //��ý��̱���ΨһID
    LUID luid;
    if(!LookupPrivilegeValue(NULL,name,&luid))
    {
         return false;
    }
    tp.PrivilegeCount=1;
    tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
    tp.Privileges[0].Luid=luid;
    //��������Ȩ��
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
  // 1. ��Զ�̽�����ΪszLibPath �����ڴ�
  UINT uProcID = processPID;
  LPCTSTR lpcszDllPath = dllPath;
  //�򿪽���
  HANDLE hProcess;
  hProcess= ::OpenProcess(PROCESS_ALL_ACCESS,
                FALSE,uProcID);
  if(!hProcess)
  {
    TRACE("��Ŀ�����ʧ��\n");
    CloseHandle(hProcess);
    return 3;
  }

  int allocSize=(lstrlen(lpcszDllPath)+1)*sizeof(TCHAR);
  //�����ڴ�����DLL�ļ�·����
  TCHAR *pDllFileNameRemote=(TCHAR*)VirtualAllocEx(hProcess,NULL,allocSize,MEM_COMMIT,PAGE_READWRITE);
  //дDLL�ļ�·������Զ�̽���
  if(!pDllFileNameRemote)
  {
    TRACE("����Ŀ������ڴ�ʧ��\n");
    CloseHandle(hProcess);
    return 4;
  }
  if(!WriteProcessMemory(hProcess,pDllFileNameRemote,(PVOID)lpcszDllPath, allocSize, NULL))
  {
    TRACE("дĿ������ڴ�ʧ��\n");
    VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
    CloseHandle(hProcess);
    return 5;
  }

  //����Զ���̣߳�ж��Զ��DLL
  HANDLE hThreadRemote =NULL;
  hThreadRemote = CreateRemoteThread(hProcess,NULL,0,
    (LPTHREAD_START_ROUTINE) ::GetProcAddress( hKernel32,"LoadLibraryW" )
    ,(LPVOID)pDllFileNameRemote,0,NULL);
  if(!hThreadRemote)
  {
    TRACE("����Զ���߳�����LoadLibraryʧ��\n");
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
    TRACE("LoadLibraryʧ��\n");
    VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
    CloseHandle(hThreadRemote);
    CloseHandle(hProcess);
    return 9;
  }
  TRACE("ж��DLL�ɹ�\n");  OutputDebugStringA("ж��DLL�ɹ�\n");
  VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
  CloseHandle(hThreadRemote);
  CloseHandle(hProcess);
  return 0;
}

int KGRemoteEjectLibrary(UINT processPID,LPCTSTR dllPath)
{
  UINT uProcID = processPID;
  LPCTSTR lpcszDllPath = dllPath;
  //�򿪽���
  HANDLE hProcess;
  hProcess= ::OpenProcess(PROCESS_ALL_ACCESS,
//                PROCESS_CREATE_THREAD |
//                PROCESS_VM_OPERATION |
//                PROCESS_VM_WRITE ,
                FALSE,uProcID);
  if(!hProcess)
  {
    TRACE("��Ŀ�����ʧ��\n");
    CloseHandle(hProcess);
    return 3;
  }

  int allocSize=(lstrlen(lpcszDllPath)+1)*sizeof(TCHAR);
  //�����ڴ�����DLL�ļ�·����
  TCHAR *pDllFileNameRemote=(TCHAR*)VirtualAllocEx(hProcess,NULL,allocSize,MEM_COMMIT,PAGE_READWRITE);
  //дDLL�ļ�·������Զ�̽���
  if(!pDllFileNameRemote)
  {
    TRACE("����Ŀ������ڴ�ʧ��\n");
    CloseHandle(hProcess);
    return 4;
  }
  if(!WriteProcessMemory(hProcess,pDllFileNameRemote,(PVOID)lpcszDllPath, allocSize, NULL))
  {
    TRACE("дĿ������ڴ�ʧ��\n");
    VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
    CloseHandle(hProcess);
    return 5;
  }

  while(1)
  {
    HMODULE hKernel32 = ::GetModuleHandle(_T("Kernel32.exe"));
    //����Զ���߳�,GetModuleHandle��ȡ�ո�ע���DLL�ľ��
    HANDLE hThreadRemote=CreateRemoteThread(hProcess,NULL,0,
    (PTHREAD_START_ROUTINE)GetModuleHandleW
  //(PTHREAD_START_ROUTINE)::GetProcAddress(hKernel32,"GetModuleHandleW")
      ,pDllFileNameRemote,0,NULL);
    if(!hThreadRemote)
    {
      TRACE("����Ŀ���߳�����GetModuleHandleʧ��\n"); OutputDebugStringA("����Ŀ���߳�����GetModuleHandleʧ��\n");
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
      TRACE("δ����DLL\n"); OutputDebugStringA("δ����DLL\n");//ж�ؽ�������
      VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
      CloseHandle(hThreadRemote);
      CloseHandle(hProcess);
      return 7;
    }
    CloseHandle(hThreadRemote);
    hThreadRemote=NULL;


    //����Զ���̣߳�ж��Զ��DLL
    hThreadRemote = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)FreeLibrary,(LPVOID)hDllRemote,0,NULL);
    if(!hThreadRemote)
    {
      TRACE("����Զ���߳�����FreeLibraryʧ��\n");
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
      TRACE("FreeLibraryʧ��\n");
      VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
      CloseHandle(hThreadRemote);
      CloseHandle(hProcess);
      return 9;
    }
    TRACE("ж��DLL�ɹ�\n");  OutputDebugStringA("ж��DLL�ɹ�\n");
    VirtualFreeEx(hProcess,pDllFileNameRemote,allocSize,MEM_RELEASE);
    CloseHandle(hThreadRemote);
    CloseHandle(hProcess);
    break;
  }
  return 0;
}



DWORD KGEnumProcEjectLibrary(LPCTSTR lpcszPath)
{
  //���ս�����Ϣ
  HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  PROCESSENTRY32 lppe;
  lppe.dwSize=sizeof(PROCESSENTRY32);
  //��õ�һ�����̵���Ϣ
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
  //���ս�����Ϣ
  HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  PROCESSENTRY32 lppe;
  lppe.dwSize=sizeof(PROCESSENTRY32);
  //��õ�һ�����̵���Ϣ
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
  //���ս�����Ϣ
  HANDLE hSnapshot=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
  PROCESSENTRY32 lppe;
  lppe.dwSize=sizeof(PROCESSENTRY32);
  //��õ�һ�����̵���Ϣ
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
