
// myLspUi.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CmyLspUiApp:
// �йش����ʵ�֣������ myLspUi.cpp
//

class CmyLspUiApp : public CWinApp
{
public:
	CmyLspUiApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CmyLspUiApp theApp;