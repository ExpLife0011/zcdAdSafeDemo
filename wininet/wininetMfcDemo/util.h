#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#include <atlbase.h>
#include <atlstr.h>


#define USE_WINHTTP //Comment this line to user wininet.

#ifdef USE_WINHTTP
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#else
#include <wininet.h>
#pragma comment(lib, "wininet.lib")
#endif
#define BUF_SIZE (4096)

// CrackedUrl

class CrackedUrl 
{
	int m_scheme;
	CStringW m_host;
	int m_port;
	CStringW m_path;
public:
	CrackedUrl(LPCWSTR url)
	{
		URL_COMPONENTS uc = { 0};
		uc.dwStructSize = sizeof(uc);

		const DWORD BUF_LEN = 256;
		WCHAR host[BUF_LEN];
		uc.lpszHostName = host;
		uc.dwHostNameLength = BUF_LEN;

		WCHAR path[BUF_LEN];
		uc.lpszUrlPath = path;
		uc.dwUrlPathLength = BUF_LEN;

		WCHAR extra[BUF_LEN];
		uc.lpszExtraInfo = extra;
		uc.dwExtraInfoLength = BUF_LEN;

#ifdef USE_WINHTTP
		if (!WinHttpCrackUrl(url, 0, ICU_ESCAPE, &uc)) 
		{
			printf("Error:WinHttpCrackUrl failed!\n");
		}
#else
		if (!InternetCrackUrl(url, 0, ICU_ESCAPE, &uc)) 
		{
			printf("Error:InternetCrackUrl failed!\n");
		}
#endif
		m_scheme = uc.nScheme;
		m_host = host;
		m_port = uc.nPort;
		m_path = path;
	}

	int GetScheme() const
	{

		return m_scheme;
	}

	LPCWSTR GetHostName() const
	{
		return m_host;
	}

	int GetPort() const
	{
		return m_port;
	}

	LPCWSTR GetPath() const
	{
		return m_path;
	}

	static CStringA UrlEncode(const char* p)
	{
		if (p == 0) 
		{
			return CStringA();
		}
		CStringA buf;

		for (;;) 
		{
			int ch = (BYTE) (*(p++));
			if (ch == '\0') 
			{
				break;
			}

			if (isalnum(ch) || ch == '_' || ch == '-' || ch == '.')
			{
				buf += (char)ch;
			}
			else if (ch == ' ') 
			{
				buf += '+';
			}
			else 
			{
				char c[16];
				wsprintfA(c, "%%%02X", ch);
				buf += c;
			}
		}

		return buf;
	}
};

// CrackedUrl

HINTERNET OpenSession(LPCWSTR userAgent = 0)
{
#ifdef USE_WINHTTP
	return WinHttpOpen(userAgent, NULL/*WINHTTP_ACCESS_TYPE_NAMED_PROXY*//*NULL*/, NULL/*_T("http://127.0.0.1:8888")*//*NULL*/, NULL, NULL);;
#else
	return InternetOpen(userAgent, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
#endif
}

HINTERNET Connect(HINTERNET hSession, LPCWSTR serverAddr, int portNo)
{
#ifdef USE_WINHTTP
	return WinHttpConnect(hSession, serverAddr, (INTERNET_PORT) portNo, 0);
#else
	return InternetConnect(hSession, serverAddr, portNo, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
#endif
}

HINTERNET OpenRequest(HINTERNET hConnect, LPCWSTR verb, LPCWSTR objectName, int scheme)
{
	DWORD flags = 0;
#ifdef USE_WINHTTP
	if (scheme == INTERNET_SCHEME_HTTPS) {
		flags |= WINHTTP_FLAG_SECURE;
	}

	return WinHttpOpenRequest(hConnect, verb, objectName, NULL, NULL, NULL, flags);

#else
	if (scheme == INTERNET_SCHEME_HTTPS) {
		flags |= INTERNET_FLAG_SECURE;
	}

	return HttpOpenRequest(hConnect, verb, objectName, NULL, NULL, NULL, flags, 0);
#endif
}

BOOL AddRequestHeaders(HINTERNET hRequest, LPCWSTR header)
{
	SIZE_T len = lstrlenW(header);
#ifdef USE_WINHTTP
	return WinHttpAddRequestHeaders(hRequest, header, DWORD(len), WINHTTP_ADDREQ_FLAG_ADD);
#else
	return HttpAddRequestHeaders(hRequest, header, DWORD(len), HTTP_ADDREQ_FLAG_ADD);
#endif
}

BOOL SendRequest(HINTERNET hRequest, const void* body, DWORD size)
{
#ifdef USE_WINHTTP
	return WinHttpSendRequest(hRequest, 0, 0, const_cast<void*>(body), size, size, 0);
#else
	return HttpSendRequest(hRequest, 0, 0, const_cast<void*>(body), size);
#endif
}

BOOL EndRequest(HINTERNET hRequest)
{
#ifdef USE_WINHTTP
	return WinHttpReceiveResponse(hRequest, 0);
#else
	// if you use HttpSendRequestEx to send request then use HttpEndRequest in here!

	return TRUE;
#endif
}

BOOL QueryInfo(HINTERNET hRequest, int queryId, char* szBuf, DWORD* pdwSize)
{
#ifdef USE_WINHTTP
	return WinHttpQueryHeaders(hRequest, (DWORD) queryId, 0, szBuf, pdwSize, 0);
#else
	return HttpQueryInfo(hRequest, queryId, szBuf, pdwSize, 0);
#endif
}

BOOL ReadData(HINTERNET hRequest, void* buffer, DWORD length, DWORD* cbRead)
{
#ifdef USE_WINHTTP
	return WinHttpReadData(hRequest, buffer, length, cbRead);
#else
	return InternetReadFile(hRequest, buffer, length, cbRead);
#endif
}

void CloseInternetHandle(HINTERNET hInternet)
{
	if (hInternet)
	{
#ifdef USE_WINHTTP
		WinHttpCloseHandle(hInternet);
#else
		InternetCloseHandle(hInternet);
#endif
	}
}

int GetUrl()
{
	HINTERNET hSession = 0;
	HINTERNET hConnect = 0;
	HINTERNET hRequest = 0;
	CStringW strHeader(L"Content-type: application/x-www-form-urlencoded\r\n");

	// Test data

	//CrackedUrl crackedUrl(L"http://www.easy-creator.net/test2/add.asp");
	CrackedUrl crackedUrl(L"http://www.baidu.com/");
	CStringA strPostData("value1=10&value2=14");

	// Open session.

	hSession = OpenSession(L"HttpPost by l_zhaohui@163.com");
	if (hSession == NULL) {
		printf("Error:Open session!\n");
		return -1;
	}

	HINTERNET thSession = OpenSession();
	if (thSession == NULL)
	{
		return -1;
	}

	// Connect.

	hConnect = Connect(hSession, crackedUrl.GetHostName(), crackedUrl.GetPort());
	if (hConnect == NULL) {
		printf("Error:Connect failed!\n");
		return -1;
	}

	// Open request.
	//hRequest = OpenRequest(hConnect, L"POST", crackedUrl.GetPath(), crackedUrl.GetScheme());
	hRequest = OpenRequest(hConnect, L"GET", crackedUrl.GetPath(), crackedUrl.GetScheme()); 
	if (hRequest == NULL) {
		printf("Error:OpenRequest failed!\n");
		return -1;
	}

	// Add request header.

	if (!AddRequestHeaders(hRequest, strHeader)) {
		printf("Error:AddRequestHeaders failed!\n");
		return -1;
	}

	// Send post data.

	if (!SendRequest(hRequest, (const char*)strPostData, strPostData.GetLength())) {
		printf("Error:SendRequest failed!\n");
		return -1;
	}

	// End request

	if (!EndRequest(hRequest)) {
		printf("Error:EndRequest failed!\n");
		return -1;
	}

	char szBuf[BUF_SIZE+1];
	DWORD dwSize = 0;
	szBuf[0] = 0;

	// Query header info.

#ifdef USE_WINHTTP
	int contextLengthId = WINHTTP_QUERY_CONTENT_LENGTH;
	int statusCodeId = WINHTTP_QUERY_STATUS_CODE;
	int statusTextId = WINHTTP_QUERY_STATUS_TEXT;
#else
	int contextLengthId = HTTP_QUERY_CONTENT_LENGTH;
	int statusCodeId = HTTP_QUERY_STATUS_CODE;
	int statusTextId = HTTP_QUERY_STATUS_TEXT;
#endif
	dwSize = BUF_SIZE;
	if (QueryInfo(hRequest, contextLengthId, szBuf, &dwSize)) {
		szBuf[dwSize] = 0;
		printf("Content length:[%s]\n", szBuf);
	}

	dwSize = BUF_SIZE;
	if (QueryInfo(hRequest, statusCodeId, szBuf, &dwSize)) {
		szBuf[dwSize] = 0;
		printf("Status code:[%s]\n", szBuf);
	}

	dwSize = BUF_SIZE;
	if (QueryInfo(hRequest, statusTextId, szBuf, &dwSize)) {
		szBuf[dwSize] = 0;
		printf("Status text:[%s]\n", szBuf);
	}

	// read data.

	for (;;) {
		dwSize = BUF_SIZE;
		if (ReadData(hRequest, szBuf, dwSize, &dwSize) == FALSE) {
			break;
		}

		if (dwSize <= 0) {
			break;
		}

		szBuf[dwSize] = 0;
		//printf("%s\n", szBuf); //Output value = value1 + value2

	}

	CloseInternetHandle(hRequest);
	CloseInternetHandle(hConnect);
	CloseInternetHandle(hSession);

	return 0;
}