// ide2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ide2.h"

#include "MainFrame.h"
#include "LuaFrame.h"
#include "LuaDoc.h"
#include "LuaView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMainFrame* g_mainFrame = NULL;

/////////////////////////////////////////////////////////////////////////////
// CIdeApp

BEGIN_MESSAGE_MAP(CIdeApp, CWinApp)
	//{{AFX_MSG_MAP(CIdeApp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdeApp construction

CIdeApp::CIdeApp()
{
	m_hScintilla = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIdeApp object

CIdeApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CIdeApp initialization

BOOL CIdeApp::InitInstance()
{
	m_hScintilla = LoadLibrary("SciLexer.DLL");
	if ( !m_hScintilla )
	{
		AfxMessageBox("Can't load Scintilla dll 'SciLexer.dll'",
			MB_OK|MB_ICONERROR);
		return FALSE;
	}

	if(CheckExisting(IDE_MAIL_SLOT))
		return FALSE;

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("xrScriptDebugIde"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	m_pLuaTemplate = new CMultiDocTemplate(
		IDR_IDE2TYPE,
		RUNTIME_CLASS(CLuaDoc),
		RUNTIME_CLASS(CLuaFrame), // custom MDI child frame
		RUNTIME_CLASS(CLuaView));
	AddDocTemplate(m_pLuaTemplate);

	// create main MDI Frame window
	 g_mainFrame = new CMainFrame;

	m_pMainWnd = g_mainFrame;

	if (!g_mainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	g_mainFrame->SetMode(CMainFrame::modeNoProject);
	g_mainFrame->OpenDefaultProject();

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
//	if (!ProcessShellCommand(cmdInfo))
//		return FALSE;

	// must be here because load frame of child view takes accel table from parent
	// and we want it to be null then
	// The main window has been initialized, so show and update it.
	g_mainFrame->ShowWindow(m_nCmdShow);
	g_mainFrame->UpdateWindow();
	
	LoadIcon(IDR_MAINFRAME);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CIdeApp message handlers


int CIdeApp::ExitInstance() 
{
	if ( m_hScintilla )
		FreeLibrary(m_hScintilla);		

	return CWinApp::ExitInstance();
}

CLuaView* CIdeApp::FindProjectFilesView(CProjectFile *pPF)
{
	POSITION pos = m_pLuaTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CLuaDoc* pDoc = (CLuaDoc*)m_pLuaTemplate->GetNextDoc(pos);
		if ( pDoc->GetProjectFile() == pPF )
			return pDoc->GetView();
	}

	return NULL;
}

CLuaView* CIdeApp::LoadProjectFilesView(CProjectFile *pPF)
{
	CLuaDoc* pDoc = (CLuaDoc*)m_pLuaTemplate->OpenDocumentFile(pPF->GetPathName(),TRUE);
	if ( pDoc )
		return pDoc->GetView();
	else
		return NULL;
}

CProjectFile * g_pPF = NULL;
CLuaView* CIdeApp::OpenProjectFilesView(CProjectFile *pPF, int nLine)
{
	g_pPF = pPF;
	CLuaView* pView = FindProjectFilesView(pPF);
	if ( !pView )
		pView = LoadProjectFilesView(pPF);

	if ( pView )
	{
		pView->Activate();

		if ( nLine>=0 )
			pView->GetEditor()->GotoLine(nLine);
	}

	return pView;
}

BOOL CIdeApp::SaveModifiedDocuments()
{
	BOOL bModified = FALSE;

	POSITION pos = m_pLuaTemplate->GetFirstDocPosition();
	while (pos != NULL)
	{
		CLuaDoc* pDoc = (CLuaDoc*)m_pLuaTemplate->GetNextDoc(pos);
		if ( !pDoc->IsInProject() )
			continue;

		if ( pDoc->IsModified() )
		{
			pDoc->DoFileSave();
			bModified = TRUE;

			// writing new lines changes breakpoint positions
			pDoc->GetView()->GetEditor()->SetBreakPointsIn(pDoc->GetProjectFile());
		}
	}

	return bModified;
}
