// xrUpdate.h : main header file for the xrUpdate application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CxrUpdateApp:
// See xrUpdate.cpp for the implementation of this class
//
class CLogDlg;
class CxrUpdateApp : public CWinApp
{
public:
	CxrUpdateApp();
	~CxrUpdateApp();


// Overrides
public:
	CLogDlg*	m_log_dlg;
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CxrUpdateApp theApp;
extern void	__stdcall log_cb_fn (LPCSTR string);
extern CLogDlg*	g_log_dlg;