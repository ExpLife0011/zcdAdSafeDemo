

//提升进程权限
bool EnableDebugPrivilege(const LPCTSTR name);

     
void enableProcessPrivilege();
;
int KGRemoteInjectLibrary(UINT processPID,LPCTSTR dllPath);


int KGRemoteEjectLibrary(UINT processPID,LPCTSTR dllPath);


DWORD KGEnumProcEjectLibrary(LPCTSTR lpcszPath);


DWORD KGEnumProcInjectLibraryIE(LPCTSTR lpcszPath);
DWORD KGEnumProcEjectLibraryIE(LPCTSTR lpcszPath);
HWND GetMainWindow(DWORD pid) ;
DWORD KGEnumProcSendMessageIE(   __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
