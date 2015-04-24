#include <tchar.h>
#include <Windows.h>

//Handle of current process
HANDLE g_hProc;

//Backup of orignal code of target api
BYTE g_aBackup[6];
BYTE g_aOpcode[6];

//Critical section, prevent concurrency of calling the monitor
CRITICAL_SECTION g_cs;

//Base address of target API in DWORD
DWORD g_dwApiFunc = (DWORD)GetTickCount;


__inline BOOL readOriginalFunction(void)
{
// Modify the heading 6 bytes opcode in target API to jmp instruction,
// the jmp instruction will lead the EIP to our fake function
BOOL ret = ReadProcessMemory(g_hProc, LPVOID(g_dwApiFunc), LPVOID(g_aBackup), sizeof(g_aBackup)/ sizeof(g_aBackup[0]), NULL);
return ret;
}
//Hook the target API
__inline BOOL setHookFunction(void)
{
    // Modify the heading 6 bytes opcode in target API to jmp instruction,
    // the jmp instruction will lead the EIP to our fake function
    BOOL ret = WriteProcessMemory(g_hProc, LPVOID(g_dwApiFunc),
        LPVOID(g_aOpcode), sizeof(g_aOpcode) / sizeof(g_aOpcode[0]), NULL);
    return ret;
}

//Unhook the target API
__inline BOOL setUnhookFunction(void)
{
    // Restore the heading 6 bytes opcode of target API.
    BOOL ret = WriteProcessMemory(g_hProc, LPVOID(g_dwApiFunc),
        LPVOID(g_aBackup), sizeof(g_aOpcode) / sizeof(g_aOpcode[0]), NULL);
    return ret;
}

//Pre-declare
BOOL UninstallMonitor(void);

//Monitor Function
DWORD WINAPI funcNewGetTickCount(DWORD dwErr)
{
    //Thread safety
    EnterCriticalSection(&g_cs);

    //Restore the original API before calling it
    setUnhookFunction();
    DWORD dw = GetTickCount();
    setHookFunction();

    //You can do anything here, and you can call the UninstallMonitor
    //when you want to leave.

    //Thread safety
    LeaveCriticalSection(&g_cs);
    return dw;
}

//Install Monitor
BOOL InstallMonitor(void)
{
    //Get handle of current process
    g_hProc = GetCurrentProcess();

    g_aOpcode[0] = 0xE9; //JMP Procudure
    *(DWORD*)(&g_aOpcode[1]) = (DWORD)funcNewGetTickCount - g_dwApiFunc - 5;

    InitializeCriticalSection(&g_cs);

    //Start monitor
    return setHookFunction();
}

BOOL UninstallMonitor(void)
{
    //Release monitor
    if (!setUnhookFunction())
        return FALSE;

    DeleteCriticalSection(&g_cs);

    CloseHandle(g_hProc);

    //Synchronize to main application, release semaphore to free injector
    HANDLE hSema = OpenSemaphore(EVENT_ALL_ACCESS, FALSE, _T("Global\\InjHack"));
    if (hSema == NULL)
        return FALSE;
    return ReleaseSemaphore(hSema, 1, (LPLONG)g_hProc);
}

BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hInstDll);

        InstallMonitor();
        break;

    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
