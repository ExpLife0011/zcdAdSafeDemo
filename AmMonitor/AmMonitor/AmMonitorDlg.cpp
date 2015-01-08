
// AmMonitorDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AmMonitor.h"
#include "AmMonitorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAmMonitorDlg �Ի���




CAmMonitorDlg::CAmMonitorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAmMonitorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAmMonitorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAmMonitorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_BN_CLICKED(IDC_BUTTON_LOAD, &CAmMonitorDlg::OnBnClickedButtonLoad)
  ON_BN_CLICKED(IDC_BUTTON_FREE, &CAmMonitorDlg::OnBnClickedButtonFree)
  ON_BN_CLICKED(IDC_BUTTON_HOOK, &CAmMonitorDlg::OnBnClickedButtonHook)
  ON_BN_CLICKED(IDC_BUTTON_UNHOOK, &CAmMonitorDlg::OnBnClickedButtonUnhook)
  ON_BN_CLICKED(IDC_BUTTON_INSTALL, &CAmMonitorDlg::OnBnClickedButtonInstall)
  ON_BN_CLICKED(IDC_BUTTON_UNINSTALL, &CAmMonitorDlg::OnBnClickedButtonUninstall)
  ON_BN_CLICKED(IDC_BUTTON_REMOTEI, &CAmMonitorDlg::OnBnClickedButtonRemotei)
END_MESSAGE_MAP()


// CAmMonitorDlg ��Ϣ�������

BOOL CAmMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CAmMonitorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAmMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAmMonitorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#include "KGRemote.h"
HINSTANCE g_hFilterInst=NULL;
//CString ROOTDIR =_T("");
#ifdef _DEBUG
 CString ROOTDIR =_T("D:\\work\\work_zcdadsafedemo.wc2\\AmMonitor\\Debug\\");
#else
 CString ROOTDIR =_T("D:\\work\\work_zcdadsafedemo.wc2\\AmMonitor\\Release\\");
#endif
void CAmMonitorDlg::OnBnClickedButtonLoad()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
 ::LoadLibrary(_T("ws2_32.dll"));
  CString str =ROOTDIR+_T("amnetfilter.dll");
  g_hFilterInst = ::LoadLibrary(str);
}


void CAmMonitorDlg::OnBnClickedButtonFree()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  if(g_hFilterInst)
  {
  FreeLibrary(g_hFilterInst);
  g_hFilterInst = NULL;
  }
  
}
typedef int (WINAPI *INSTALL)(HWND handle,HWND host);
typedef int (WINAPI *UNINSTALL)();
 HWND hNotepad =NULL;
 //CString findWindClassName = _T("");
 LPCTSTR findWindowTitle=NULL;
 LPCTSTR findWindClassName = _T("Chrome_WidgetWin_1");
 //CString findWindowTitle=_T("HookDialogDemo");
 
 void CAmMonitorDlg::OnBnClickedButtonHook()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  INSTALL func = NULL;
  func=(INSTALL)::GetProcAddress (g_hFilterInst,"_ZcnInstallCallwndHook@8");
  if(func)
  {
    hNotepad =  ::FindWindow(findWindClassName,findWindowTitle);
      //func(m_hWnd,m_hWnd);
      func(hNotepad,m_hWnd);
  }
  //PostThreadMessage(::GetWindowThreadProcessId(hNotepad,NULL)
  // , WM_USER+00,(LPARAM)GetCurrentThreadId(),0);
}


void CAmMonitorDlg::OnBnClickedButtonUnhook()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
    UNINSTALL g_loadhook =(UNINSTALL)::GetProcAddress (g_hFilterInst,"_ZcnUnInstallCallwndHook@0");
    if(g_loadhook)
    {
        g_loadhook();
    }
}

void CAmMonitorDlg::OnBnClickedButtonInstall()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  hNotepad =  ::FindWindow(findWindClassName,findWindowTitle);
  UINT WM_HOOKEX = ::RegisterWindowMessage(_T("WM_AMMONITOR_RK" ));
    ::SendMessage(hNotepad,WM_HOOKEX,1,1);
    //::PostMessage(hNotepad,WM_HOOKEX,1,1);
  //DWORD idProc = 0;
  //DWORD idTarget = ::GetWindowThreadProcessId(hNotepad,&idProc);

  //PostThreadMessage(idProc,WM_HOOKEX,0,1);
  //DWORD dw = GetLastError();

}


void CAmMonitorDlg::OnBnClickedButtonUninstall()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  hNotepad =  ::FindWindow(findWindClassName,findWindowTitle);
  UINT WM_HOOKEX = ::RegisterWindowMessage(_T("WM_AMMONITOR_RK" ));
    ::SendMessage(hNotepad,WM_HOOKEX,1,0);
    //::PostMessage(HWND_BROADCAST,WM_HOOKEX,1,1);
  //DWORD idProc = 0;
  //DWORD idTarget = ::GetWindowThreadProcessId(hNotepad,&idProc);

  //PostThreadMessage(idProc,WM_HOOKEX,0,0);
}


void CAmMonitorDlg::OnBnClickedButtonRemotei()
{
  // TODO: �ڴ���ӿؼ�֪ͨ����������
  CString str =ROOTDIR+_T("amnetfilter.dll");
  KGEnumProcInjectLibraryIE(str);
}
