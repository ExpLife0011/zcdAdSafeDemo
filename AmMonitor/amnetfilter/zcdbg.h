#pragma once
#include <time.h>
#include <Windows.h>

#define WriteAGLog zcdbg::OutputDebugLogA
#define OutputDebugLog zcdbg::OutputDebugLogA

namespace zcdbg
{

void OutputDebugLogA(LPCSTR cstr);
void OutputDebugLogW(LPCWSTR cstr);

}
