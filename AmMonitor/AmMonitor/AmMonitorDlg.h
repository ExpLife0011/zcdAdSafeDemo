
// AmMonitorDlg.h : 头文件
//

#pragma once


// CAmMonitorDlg 对话框
class CAmMonitorDlg : public CDialogEx
{
// 构造
public:
	CAmMonitorDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_AMMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnBnClickedButtonTrigger();
  afx_msg void OnBnClickedButtonLoad();
  afx_msg void OnBnClickedButtonFree();
  afx_msg void OnBnClickedButtonHook();
  afx_msg void OnBnClickedButtonUnhook();
  afx_msg void OnBnClickedButtonInstall();
  afx_msg void OnBnClickedButtonUninstall();
  afx_msg void OnBnClickedButtonRemotei();
  afx_msg void OnBnClickedButtonInstallIe();
  afx_msg void OnBnClickedButtonUninstallIe();
};
