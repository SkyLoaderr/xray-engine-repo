// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ide2.h"

#include "MainFrame.h"
#include "ScintillaView.h"
#include "LuaDoc.h"
#include "LuaView.h"

#include "../xrGame/script_debugger_messages.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_COMMAND_EX(ID_VIEW_OUTPUT, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OUTPUT, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_WORKSPACE, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WORKSPACE, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_CALLSTACK, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CALLSTACK, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_LOCALS, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOCALS, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_WATCHES, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WATCHES, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_THREADS, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THREADS, OnUpdateControlBarMenu)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_FILE_OPENPROJECT, OnFileOpenproject)
	ON_COMMAND(ID_FILE_SAVEPROJECT, OnFileSaveproject)
	ON_COMMAND(ID_FILE_SAVEPROJECTAS, OnFileSaveprojectas)
	ON_COMMAND(ID_PROJECT_ADD_FILES, OnProjectAddFiles)
	ON_WM_CLOSE()
	ON_COMMAND(ID_DEBUG_GO, OnDebugGo)
	ON_COMMAND(ID_DEBUG_STEPINTO, OnDebugStepinto)
	ON_COMMAND(ID_DEBUG_STEPOVER, OnDebugStepover)
	ON_COMMAND(ID_DEBUG_STEPOUT, OnDebugStepout)
	ON_COMMAND(ID_DEBUG_RUNTOCURSOR, OnDebugRuntocursor)
	ON_COMMAND(ID_DEBUG_STOPDEBUGGING, OnDebugStopdebugging)
	ON_COMMAND(ID_DEBUG_DEBUGBREAK, OnDebugBreak)

	ON_COMMAND(ID_FILE_NEWPROJECT, OnFileNewproject)
	ON_COMMAND(ID_FILE_CLOSEPROJECT, OnFileCloseproject)

	ON_UPDATE_COMMAND_UI(ID_DEBUG_STEPINTO, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STEPOVER, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STEPOUT, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_GO, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_RUNTOCURSOR, OnUpdateDebugMenu)
	ON_UPDATE_COMMAND_UI(ID_DEBUG_STOPDEBUGGING, OnUpdateDebugMenu)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_needAnswer = FALSE;
	m_pMailSlotThread = NULL;
	m_mailSlot = CreateMailSlotByName(IDE_MAIL_SLOT);
	m_hAccelNoProject = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_NO_PROJECT));
	m_hAccelDebug = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_DEBUG));
	m_hAccelDebugBreak = ::LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_ACCEL_DEBUG_BREAK));
}

CMainFrame::~CMainFrame()
{
	if(m_pMailSlotThread!=NULL)
 		delete m_pMailSlotThread;

	CloseHandle(m_mailSlot);

	::DestroyAcceleratorTable(m_hAccelNoProject);
	::DestroyAcceleratorTable(m_hAccelDebug);
	::DestroyAcceleratorTable(m_hAccelDebugBreak);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndMDIClient.SubclassWindow (m_hWndMDIClient)) 
	{
              TRACE ("Failed to subclass MDI client window\n");
              return (-1);
    }            

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	InitDockingWindows();

	LoadBarState(_T("Build"));


 	m_pMailSlotThread = AfxBeginThread(StartListener, this,0,0,CREATE_SUSPENDED);
 	if(m_pMailSlotThread!=NULL)
 	{
 		m_pMailSlotThread->m_bAutoDelete=false;
 		m_pMailSlotThread->ResumeThread();
 	}
	
	return 0;
}


BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


BOOL CMainFrame::InitDockingWindows()
{
	SetInitialSize(125,125,225,225);

	if (!m_wndWorkspace.Create(this, ID_VIEW_WORKSPACE,
		_T("Workspace"), CSize(200,100), CBRS_LEFT))
	{
		TRACE0("Failed to create dialog bar m_wndWorkspace\n");
		return -1;		// fail to create
	}
	
	if (!m_wndOutput.Create(this, ID_VIEW_OUTPUT,
		_T("Output"), CSize(300,100), CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialog bar m_wndOutput\n");
		return -1;		// fail to create
	}

	if (!m_wndCallStack.Create(this, ID_VIEW_CALLSTACK,
		_T("Call Stack"), CSize(300,100), CBRS_RIGHT))
	{
		TRACE0("Failed to create dialog bar m_wndCallStack\n");
		return -1;		// fail to create
	}

	if (!m_wndLocals.Create(this, ID_VIEW_LOCALS,
		_T("Local Variables"), CSize(200,100), CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialog bar m_wndLocals\n");
		return -1;		// fail to create
	}

	if (!m_wndWatches.Create(this, ID_VIEW_WATCHES,
		_T("Local Variables"), CSize(200,100), CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialog bar m_wndWatches\n");
		return -1;		// fail to create
	}

	if (!m_wndThreads.Create(this, ID_VIEW_THREADS,
		_T("Script Threads"), CSize(200,100), CBRS_BOTTOM))
	{
		TRACE0("Failed to create dialog bar m_wndThreads\n");
		return -1;		// fail to create
	}
	

	m_wndWorkspace.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndOutput.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndCallStack.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndLocals.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndWatches.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);
	m_wndThreads.EnableDockingOnSizeBar(CBRS_ALIGN_ANY);

	EnableDockingSizeBar(CBRS_ALIGN_ANY);
	DockSizeBar(&m_wndWorkspace);
	DockSizeBar(&m_wndOutput);
	DockSizeBar(&m_wndCallStack);
	DockSizeBar(&m_wndLocals);
	DockSizeBar(&m_wndWatches);
	DockSizeBar(&m_wndThreads);

	return TRUE;
}

void CMainFrame::OnFileOpenproject() 
{
	if ( GetProject()->Load() )
	{
		m_wndWorkspace.Enable(TRUE);
		OnUpdateFrameTitle(TRUE);
	}
}

void CMainFrame::OnFileSaveproject() 
{
	GetProject()->Save();
}

void CMainFrame::OnFileSaveprojectas() 
{
	GetProject()->SaveAs();
}

void CMainFrame::OnProjectAddFiles() 
{
	GetProject()->AddFiles();
}


void CMainFrame::OnClose() 
{
	if ( m_nAppMode!=modeNoProject )
		GetProject()->SaveModified();
	
	CMDIFrameWnd::OnClose();
}



CLuaView* CMainFrame::GetActiveView()
{
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)AfxGetApp()->m_pMainWnd;
	CMDIChildWnd *pChild = (CMDIChildWnd *)pFrame->GetActiveFrame();
	if ( !pChild )
		return NULL;

	CView *pView = pChild->GetActiveView();
	if ( pView && pView->IsKindOf(RUNTIME_CLASS(CLuaView)) )
		return (CLuaView*)pView;

	return NULL;
}



LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if ( message >= _DMSG_FIRST_MSG && message <= _DMSG_LAST_MSG )
		return DebugMessage(message, wParam, lParam);
	
	return CMDIFrameWnd::WindowProc(message, wParam, lParam);
}

LRESULT CMainFrame::DebugMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch( nMsg )
	{
	case DMSG_NEW_CONNECTION:
		m_wndCallStack.Clear();
		m_wndLocals.RemoveAll();
		GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Clear();
		GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write("Script debugger connected...\n");
		break;
	case DMSG_NEED_ANSWER:
		m_needAnswer = TRUE;
		break;
	case DMSG_WRITE_DEBUG:
		GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write((const char*)wParam);
		GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write("\n");
		break;
	case DMSG_HAS_BREAKPOINT:
		return GetProject()->HasBreakPoint((const char*)wParam, (int)lParam);
	case DMSG_GOTO_FILELINE:
		GotoFileLine((const char*)wParam, (int)lParam);
		break;
	case DMSG_DEBUG_START:
		SetMode(modeDebug);
		break;
	case DMSG_DEBUG_BREAK:
	case DMSG_SHOW_IDE:
		SetMode(modeDebugBreak);
		break;
	case DMSG_DEBUG_END:
		SetMode(modeIdle);
		break;
	case DMSG_CLEAR_STACKTRACE:
		m_wndCallStack.Clear();
		break;
	case DMSG_ADD_STACKTRACE:
		m_wndCallStack.Add(((StackTrace*)wParam)->szDesc, 
			((StackTrace*)wParam)->szFile, 
			((StackTrace*)wParam)->nLine);
		break;
	case DMSG_GOTO_STACKTRACE_LEVEL:
		m_wndCallStack.GotoStackTraceLevel(wParam);
		break;
	case DMSG_GET_STACKTRACE_LEVEL:
		return m_wndCallStack.GetLevel();
	case DMSG_CLEAR_LOCALVARIABLES:
		m_wndLocals.RemoveAll();
		break;
	case DMSG_ADD_LOCALVARIABLE:
		m_wndLocals.AddVariable(((Variable*)wParam)->szName, 
			((Variable*)wParam)->szType, 
			((Variable*)wParam)->szValue);
		break;
	case DMSG_REDRAW_WATCHES:
		m_wndWatches.Redraw();
		break;
	case DMSG_GET_BREAKPOINTS:
		SendBreakPoints();
		break;
	}

	return 0;
}

void CMainFrame::GotoFileLine(const char *szFile, int nLine)
{
	CProjectFile* pPF = GetProject()->GetProjectFile(szFile);

	if ( pPF )
		theApp.OpenProjectFilesView(pPF, nLine);
}

void CMainFrame::SetMode(int nMode)
{
	m_nAppMode = nMode;
	switch ( nMode )
	{
	case modeNoProject:
		m_hAccelTable = m_hAccelNoProject;
		m_wndCallStack.Clear();
		break;
	case modeIdle:
		m_hAccelTable = m_hAccelDebug;
		m_wndCallStack.Clear();
		break;
	case modeDebug:
		m_hAccelTable = m_hAccelDebug;
		m_wndCallStack.Clear();
		break;
	case modeDebugBreak:
		m_hAccelTable = m_hAccelDebugBreak;
		break;
	}

	m_wndMDIClient.ResetMenu();
	DrawMenuBar();

	OnUpdateFrameTitle(TRUE);
}

void CMainFrame::OnUpdateDebugMenu(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_needAnswer);
	if(pCmdUI->m_nID==ID_DEBUG_DEBUGBREAK)
		pCmdUI->Enable(!m_needAnswer);
}	

void CMainFrame::OnDebugGo() 
{
	if (!m_needAnswer)return;

	CMailSlotMsg msg;
	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_GO);
	SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);

	m_needAnswer = FALSE;
}


void CMainFrame::OnDebugStepinto() 
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_STEP_INTO);
	SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);
	
	m_needAnswer = FALSE;
}

void CMainFrame::OnDebugStepover() 
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_STEP_OVER);
	SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);

	m_needAnswer = FALSE;
}

void CMainFrame::OnDebugStepout() 
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_STEP_OUT);
	SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);

	m_needAnswer = FALSE;
}

void CMainFrame::OnDebugRuntocursor() 
{
	GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write("Not implemented yet...\n");
	SendBreakPoints();
	return;
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_DEBUG_RUN_TO_CURSOR);
	SendMailslotMessage(DEBUGGER_MAIL_SLOT, msg);

	m_needAnswer = FALSE;
}

void CMainFrame::OnDebugBreak()
{
	if (m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_DEBUG_BREAK);
	if( CheckExisting (DEBUGGER_MAIL_SLOT) ){
		SendMailslotMessage(DEBUGGER_MAIL_SLOT, msg);
		GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write("Initiated debug break...\n");
	}else
		GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write("Debugger not present...\n");

}


void CMainFrame::OnDebugStopdebugging() 
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int(DMSG_STOP_DEBUGGING);
	SendMailslotMessage(DEBUGGER_MAIL_SLOT, msg);

	m_needAnswer = FALSE;
}



void CMainFrame::OpenDefaultProject()
{
	CString sFileName;
	
	DWORD				sz_user	= 64;
	char				UserName[64];

	CString sAppName;
	CString sFullFileName;

	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	GetUserName			(UserName,&sz_user);

	AfxGetModuleShortFileName(AfxGetInstanceHandle(),sAppName);
	_splitpath( sAppName, drive, dir, fname, ext );

	sFullFileName.Format("%s%s%s_scr_dbg.lpr",drive,dir,UserName);

	BOOL bFileExist = FALSE;

  WIN32_FIND_DATA FindFileData;
  HANDLE hFind;

  hFind = FindFirstFile(sFullFileName, &FindFileData);

  if (hFind == INVALID_HANDLE_VALUE) {
	bFileExist = FALSE;
  } else {
    bFileExist = TRUE;
    FindClose(hFind);
  };

	
	if ( !bFileExist )
	{
		GetProject()->New(sFullFileName);
		SetMode(modeIdle);
		m_wndWorkspace.Enable(TRUE);
		OnUpdateFrameTitle(TRUE);
	}else{
		GetProject()->Load(sFullFileName);
		SetMode(modeIdle);
		m_wndWorkspace.Enable(TRUE);
		OnUpdateFrameTitle(TRUE);
	}
}

void CMainFrame::OnFileNewproject() 
{
	if ( GetProject()->New() )
	{
		SetMode(modeIdle);
		m_wndWorkspace.Enable(TRUE);
		OnUpdateFrameTitle(TRUE);
	}
}

void CMainFrame::OnFileCloseproject() 
{
	if ( GetProject()->Close() )
	{
		SetMode(modeNoProject);
		m_wndWorkspace.Enable(FALSE);
		OnUpdateFrameTitle(TRUE);
	}
}

void CMainFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
	if ((GetStyle() & FWS_ADDTOTITLE) == 0)
		return;     // leave it alone!

	CLuaView* pView = GetActiveView();
	if ( pView && bAddToTitle )
		UpdateFrameTitleForDocument(pView->GetDocument()->GetTitle());
	else
		UpdateFrameTitleForDocument(NULL);
}

void CMainFrame::UpdateFrameTitleForDocument(LPCTSTR lpszDocName)
{
	// copy first part of title loaded at time of frame creation
	TCHAR szText[256+_MAX_PATH];

	if (GetStyle() & FWS_PREFIXTITLE)
	{
		szText[0] = '\0';   // start with nothing

		// get name of currently active view
		if (lpszDocName != NULL)
		{
			lstrcpy(szText, lpszDocName);
			// add current window # if needed
			if (m_nWindow > 0)
				wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);
			lstrcat(szText, _T(" - "));
		}
		lstrcat(szText, m_strTitle);
	}
	else
	{
		// get name of currently active view
		if ( m_nAppMode!=modeNoProject )
		{
			lstrcpy(szText, m_project.GetName());
			lstrcat(szText, _T(" - "));
//			lstrcat(szText, m_strTitle);
		}else
			lstrcpy(szText, m_strTitle);

/*		if ( m_nAppMode==modeDebug )
			lstrcat(szText, _T(" [run] "));
		else if ( m_nAppMode==modeDebugBreak )
			lstrcat(szText, _T(" [break] "));
*/
		if(m_needAnswer)
			lstrcat(szText, _T(" [DEBUG BREAK] "));
		else
			lstrcat(szText, _T(" [NOT ACTIVE] "));


		if (lpszDocName != NULL)
		{
			lstrcat(szText, _T(" - ["));
			lstrcat(szText, lpszDocName);
			// add current window # if needed
			if (m_nWindow > 0)
				wsprintf(szText + lstrlen(szText), _T(":%d"), m_nWindow);
			lstrcat(szText, _T("]"));
		}
	}

	// set title if changed, but don't remove completely
	// Note: will be excessive for MDI Frame with maximized child
	AfxSetWindowText(m_hWnd, szText);
}

void CMainFrame::StackLevelChanged(int nLine)
{
	if (!m_needAnswer)return;
	CMailSlotMsg msg;
	msg.w_int(DMSG_SHOW_IDE);
	msg.w_int (DMSG_GOTO_STACKTRACE_LEVEL);
	SendMailslotMessage(DEBUGGER_MAIL_SLOT, msg);
	m_needAnswer = FALSE;
}

void CMainFrame::TranslateMsg( CMailSlotMsg& msg )
{
	int nType;
	msg.r_int(nType);
	
	switch(nType) {
	case DMSG_GET_BREAKPOINTS:
			SendMessage(DMSG_GET_BREAKPOINTS,0,0);
		break;
	case DMSG_NEW_CONNECTION:
			SendMessage(DMSG_NEW_CONNECTION,0,0);
		break;

	case DMSG_SHOW_IDE:
			SendMessage(SW_SHOW,0,0);
			SendMessage(DMSG_NEED_ANSWER,0,0);
			SendMessage(DMSG_SHOW_IDE,0,0);
		break;

	case DMSG_WRITE_DEBUG:{
			char str[2048];str[0]=0;
			msg.r_string(str);
//			GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write(str);
			SendMessage(DMSG_WRITE_DEBUG,(WPARAM)(&str),0);
		}break;

	case DMSG_HAS_BREAKPOINT:{
	//		msg.w_string((char*)wParam);
	//		msg.r_int((int)lParam);
			char str[2048];str[0]=0;
			int i;
			msg.r_string(str);
			msg.r_int(i);
			
//			int res = GetProject()->HasBreakPoint(str, i);
			int res = SendMessage(DMSG_HAS_BREAKPOINT,(WPARAM)str,(LPARAM)i);
			msg.Reset();
			msg.w_int(DMSG_HAS_BREAKPOINT);
			msg.w_int(res);
			SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);
			if(res)
			{	
				char outstr[2048];outstr[0]=0;
				sprintf(outstr,"breakPoint at %s line %d\n",str,i);
				SendMessage(DMSG_WRITE_DEBUG,(WPARAM)outstr,0);
			};
		}break;

	case DMSG_GOTO_FILELINE:{
	//		msg.w_string((char*)wParam);
	//		msg.w_int((int)lParam);
			char str[2048];str[0]=0;
			int i;
			msg.r_string(str);
			msg.r_int(i);
//			GotoFileLine(str, i);
			SendMessage (DMSG_GOTO_FILELINE,(WPARAM)str,(LPARAM)i);
//			GotoFileLine((const char*)wParam, (int)lParam);
		}break;

	case DMSG_CLEAR_STACKTRACE:{
//			m_wndCallStack.Clear();
			SendMessage(DMSG_CLEAR_STACKTRACE,0,0);
		}break;

	case DMSG_ADD_STACKTRACE:{
//			msg.w_buff((StackTrace*)wParam, sizeof(StackTrace) );
			StackTrace st;
			msg.r_buff((void*)(&st),sizeof(StackTrace));
//			m_wndCallStack.Add(st.szDesc, st.szFile, st.nLine);
			SendMessage(DMSG_ADD_STACKTRACE,(WPARAM)(&st),0);
		}break;

	case DMSG_CLEAR_LOCALVARIABLES:{
//			m_wndLocals.RemoveAll();
			SendMessage(DMSG_CLEAR_LOCALVARIABLES,0,0);
		}break;

	case DMSG_ADD_LOCALVARIABLE:{
//		msg.w_buff(wParam,sizeof(Variable));
			Variable v;
			msg.r_buff(&v,sizeof(Variable));
//			m_wndLocals.AddVariable(v.szName, v.szType, v.szValue);
			SendMessage(DMSG_ADD_LOCALVARIABLE,(WPARAM)(&v),0);
		}break;

	case DMSG_GET_STACKTRACE_LEVEL:{
			msg.Reset();
			msg.w_int(DMSG_GET_STACKTRACE_LEVEL);
			
			int level = SendMessage(DMSG_GET_STACKTRACE_LEVEL,0,0);
			msg.w_int(level);
			SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);
		}break;
	default:
		break;
	}
}
void CMainFrame::SendBreakPoints()
{
	CMailSlotMsg msg;
	msg.w_int(DMSG_GET_BREAKPOINTS);
	GetProject()->FillBreakPoints(&msg);
	if( CheckExisting(DEBUGGER_MAIL_SLOT) )
		SendMailslotMessage(DEBUGGER_MAIL_SLOT,msg);
}

UINT CMainFrame::StartListener( LPVOID pParam )
{
	return ((CMainFrame*)pParam)->StartListener();
}

UINT CMainFrame::StartListener()
{
	CMailSlotMsg msg;
	while(true){
	if( CheckMailslotMessage(m_mailSlot, msg) )
		TranslateMsg(msg);
		Sleep(10);
	};

}


BOOL CMainFrame::ErrorStringToFileLine(CString strError, CString &strPathName, int &nLine)
{
	CString strFileLine = strError;
	if ( strError.Left(4)=="luac" )
	strFileLine = strError.Mid(6);

	int nPos1 = strFileLine.Find(':',2);
	if ( nPos1==-1 )
	return FALSE;
	int nPos2 = strFileLine.Find(':', nPos1+1);
	if ( nPos2==-1 )
	return FALSE;

	CString strNum = strFileLine.Mid(nPos1+1, nPos2-nPos1-1);
	nLine = atoi(strNum);
	strPathName = strFileLine.Left(nPos1);

	return TRUE;
}
