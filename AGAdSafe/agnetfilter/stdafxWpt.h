#undef _WIN32_WINNT
#define _WIN32_WINNT  0x0600
#include <Ws2tcpip.h>
#undef _WIN32_WINNT
#define _WIN32_WINNT  0x0501
#include <Wincrypt.h>

#include <shlobj.h>
#include <atlstr.h>
#include <atlcoll.h>
//#include "../wptdriver/util.h"