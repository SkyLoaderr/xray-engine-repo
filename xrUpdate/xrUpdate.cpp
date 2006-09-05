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


#define ON_MESSAGE__(message, memberFxn) \
	{ message, 0, 0, 0, AfxSig_lwl, \
		(AFX_PMSG)(static_cast< LRESULT (CCmdTarget::*)(WPARAM, LPARAM) > \
		(memberFxn)) },

BEGIN_MESSAGE_MAP(CxrUpdateApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)

END_MESSAGE_MAP()

	

// CxrUpdateApp construction
//extern CLogDlg*	g_log_dlg = NULL;
extern HWND g_app_wnd =NULL;

//typedef void	( * LogCallback)	(LPCSTR string);

void	/* __stdcall*/ log_cb_fn (LPCSTR string)
{
	DWORD a;
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);  
	BOOL ss = WriteConsole(hStdout,string,xr_strlen(string),&a,NULL);
	ss = WriteConsole(hStdout,"\n",1,&a,NULL);

/*	if(!g_log_dlg)
		return;
	g_log_dlg->m_list_box.AddString(string);
	g_log_dlg->ShowWindow(SW_SHOW);
	g_log_dlg->UpdateWindow();
*/
}

void test ();
CxrUpdateApp::CxrUpdateApp()
{
	test	();
}

CxrUpdateApp::~CxrUpdateApp()
{
	FlushLog();
	Core._destroy();
//	g_log_dlg = NULL;
//	m_log_dlg->DestroyWindow();
//	xr_delete(m_log_dlg);
	FreeConsole();

	if( m_pReadThread )
	{
		TerminateThread( m_pReadThread->m_hThread,0 );
//		delete m_pReadThread;
	}

}

CxrUpdateApp theApp;

// CxrUpdateApp initialization

BOOL CxrUpdateApp::InitInstance()
{
	CWinApp::InitInstance();

//	g_log_dlg = m_log_dlg;
//	m_log_dlg->Create( MAKEINTRESOURCE(IDD_LOG_DIALOG) , NULL);
//	m_log_dlg->ShowWindow(SW_SHOW);

	BOOL r = AllocConsole();
	_VerifyPath("x:\\updater");
	SetCurrentDirectoryA("x:\\updater");

//	CreateShellRedirect();
	Core._initialize("xrUpdate",log_cb_fn,FALSE);
	FS._initialize(CLocatorAPI::flTargetFolderOnly,"x:\\updater");

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
	ProcessShellCommand(cmdInfo);

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	updateTaskTree();
	
	if(strstr( strlwr(m_lpCmdLine), "-run"))
		RunTask();

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


void test ()
{
	return;
	HANDLE h = CreateFile(	"c:\\test.txt",
							0,
							0,
							NULL,
							OPEN_EXISTING,
							FILE_ATTRIBUTE_READONLY | FILE_FLAG_NO_BUFFERING,
							NULL);
	if(h==INVALID_HANDLE_VALUE){
		// file not found 
	}else
	{
		// file found
	}
}
