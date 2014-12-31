#pragma once


BOOL InstallCallwndHook();
BOOL UninstallCallwndHook();
BOOL InstallKbHook();
BOOL UninstallKbHook();

void installGeHookDll();
void uninstallGeHookDll();