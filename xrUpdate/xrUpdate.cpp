// xrUpdate.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "xrUpdate.h"
#include "MainFrm.h"

#include "xrUpdateDoc.h"
#include "xrUpdateView.h"
#include "LogDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CxrUpdateApp

BEGIN_MESSAGE_MAP(CxrUpdateApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CxrUpdateApp construction
extern CLogDlg*	g_log_dlg = NULL;
void	__stdcall log_cb_fn (LPCSTR string)
{
	if(!g_log_dlg)
		return;
	g_log_dlg->m_list_box.InsertString(0,string);
	g_log_dlg->ShowWindow(SW_SHOW);
}

CxrUpdateApp::CxrUpdateApp()
{
	m_log_dlg = xr_new<CLogDlg>( MAKEINTRESOURCE(IDD_LOG_DIALOG));

}

CxrUpdateApp::~CxrUpdateApp()
{
	Core._destroy();
	g_log_dlg = NULL;
	xr_delete(m_log_dlg);
}

// The one and only CxrUpdateApp object

CxrUpdateApp theApp;

// CxrUpdateApp initialization

BOOL CxrUpdateApp::InitInstance()
{
	CWinApp::InitInstance();

	g_log_dlg = m_log_dlg;
	m_log_dlg->Create( MAKEINTRESOURCE(IDD_LOG_DIALOG) , NULL);
	m_log_dlg->ShowWindow(SW_SHOW);

	Core._initialize("xrUpdate",log_cb_fn,FALSE);
	FS._initialize(CLocatorAPI::flTargetFolderOnly,"c:\\upd");

	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CxrUpdateDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CxrUpdateView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();


	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// App command to run the dialog
void CxrUpdateApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CxrUpdateApp message handlers

