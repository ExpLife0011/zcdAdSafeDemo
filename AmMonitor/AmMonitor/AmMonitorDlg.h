
// AmMonitorDlg.h : ͷ�ļ�
//

#pragma once


// CAmMonitorDlg �Ի���
class CAmMonitorDlg : public CDialogEx
{
// ����
public:
	CAmMonitorDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_AMMONITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
