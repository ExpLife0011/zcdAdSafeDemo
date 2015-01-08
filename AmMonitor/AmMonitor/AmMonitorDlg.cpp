
// AmMonitorDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "AmMonitor.h"
#include "AmMonitorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CAmMonitorDlg 对话框




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


// CAmMonitorDlg 消息处理程序

BOOL CAmMonitorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAmMonitorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
  // TODO: 在此添加控件通知处理程序代码
 ::LoadLibrary(_T("ws2_32.dll"));
  CString str =ROOTDIR+_T("amnetfilter.dll");
  g_hFilterInst = ::LoadLibrary(str);
}


void CAmMonitorDlg::OnBnClickedButtonFree()
{
  // TODO: 在此添加控件通知处理程序代码
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
  // TODO: 在此添加控件通知处理程序代码
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
  // TODO: 在此添加控件通知处理程序代码
    UNINSTALL g_loadhook =(UNINSTALL)::GetProcAddress (g_hFilterInst,"_ZcnUnInstallCallwndHook@0");
    if(g_loadhook)
    {
        g_loadhook();
    }
}

void CAmMonitorDlg::OnBnClickedButtonInstall()
{
  // TODO: 在此添加控件通知处理程序代码
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
  // TODO: 在此添加控件通知处理程序代码
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
  // TODO: 在此添加控件通知处理程序代码
  CString str =ROOTDIR+_T("amnetfilter.dll");
  KGEnumProcInjectLibraryIE(str);
}
