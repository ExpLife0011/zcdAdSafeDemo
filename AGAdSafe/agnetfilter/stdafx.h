// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
#define INCL_WINSOCK_API_TYPEDEFS 1
// Windows ͷ�ļ�:
#include <windows.h>



// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
//#include <wininet.h>
//#include <winhttp.h>

#include <tchar.h>


#undef _WIN32_WINNT
#define _WIN32_WINNT  0x0600
#include <Ws2tcpip.h>
#undef _WIN32_WINNT
#define _WIN32_WINNT  0x0501
#include <Wincrypt.h>

#include <shlobj.h>
#include <atlstr.h>
#include <atlcoll.h>
//#include "../wptdriver/util.