
// AmMonitor.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CAmMonitorApp:
// �йش����ʵ�֣������ AmMonitor.cpp
//

class CAmMonitorApp : public CWinApp
{
public:
	CAmMonitorApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CAmMonitorApp theApp;