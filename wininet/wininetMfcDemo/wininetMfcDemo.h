
// wininetMfcDemo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CwininetMfcDemoApp:
// �йش����ʵ�֣������ wininetMfcDemo.cpp
//

class CwininetMfcDemoApp : public CWinAppEx
{
public:
	CwininetMfcDemoApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CwininetMfcDemoApp theApp;