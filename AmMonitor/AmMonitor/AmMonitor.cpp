
// AmMonitor.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "AmMonitor.h"
#include "AmMonitorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define INCL_WINSOCK_API_TYPEDEFS 1
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
//#include <ws2tcpip.h>

int WSAAPI  AmhConnectHook(IN SOCKET s, const struct sockaddr FAR * name, IN int namelen)
{
  int ret = SOCKET_ERROR;
        sockaddr_in sin;
        memcpy(&sin, &name, sizeof(sin));

      SOCKADDR_IN stSvrAddrIn = {0};
      stSvrAddrIn.sin_family = AF_INET;
      stSvrAddrIn.sin_port = htons(8888);
      stSvrAddrIn.sin_addr.s_addr = inet_addr("127.0.0.1");
     
  return ret;
}

// CAmMonitorApp

BEGIN_MESSAGE_MAP(CAmMonitorApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CAmMonitorApp ����

CAmMonitorApp::CAmMonitorApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CAmMonitorApp ����

CAmMonitorApp theApp;


// CAmMonitorApp ��ʼ��

BOOL CAmMonitorApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CAmMonitorDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

