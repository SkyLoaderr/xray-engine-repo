#include "stdafx.h"
#include "script_debugger.h"
#include "script_lua_helper.h"


CScriptDebugger* CScriptDebugger::m_pDebugger = NULL;

LRESULT CScriptDebugger::_SendMessage(u32 message, WPARAM wParam, LPARAM lParam)
{
	if ( message >= _DMSG_FIRST_MSG && message <= _DMSG_LAST_MSG )
		return DebugMessage(message, wParam, lParam);

	return 0;
//	return CMDIFrameWnd::WindowProc(message, wParam, lParam);
}

LRESULT CScriptDebugger::DebugMessage(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch( nMsg )
	{
	case DMSG_WRITE_DEBUG:
//		GetOutputWnd()->GetOutput(COutputWnd::outputDebug)->Write((const char*)wParam);
		break;
	case DMSG_HAS_BREAKPOINT:
//		return GetProject()->HasBreakPoint((const char*)wParam, (int)lParam);
		break;
	case DMSG_GOTO_FILELINE:
//		GotoFileLine((const char*)wParam, (int)lParam);
		break;
	case DMSG_DEBUG_START:
//		SetMode(modeDebug);
		break;
	case DMSG_DEBUG_BREAK:
//		SetMode(modeDebugBreak);
		break;
	case DMSG_DEBUG_END:
//		SetMode(modeBuild);
		break;
	case DMSG_CLEAR_STACKTRACE:
//		m_wndCallStack.Clear();
		break;
	case DMSG_ADD_STACKTRACE:
/*		m_wndCallStack.Add(((StackTrace*)wParam)->szDesc, 
			((StackTrace*)wParam)->szFile, 
			((StackTrace*)wParam)->nLine);*/
		break;
	case DMSG_GOTO_STACKTRACE_LEVEL:
//		m_wndCallStack.GotoStackTraceLevel(wParam);
		break;
	case DMSG_GET_STACKTRACE_LEVEL:
//		return m_wndCallStack.GetLevel();
		break;
	case DMSG_CLEAR_LOCALVARIABLES:
//		m_wndLocals.RemoveAll();
		break;
	case DMSG_ADD_LOCALVARIABLE:
/*		m_wndLocals.AddVariable(((Variable*)wParam)->szName, 
			((Variable*)wParam)->szType, 
			((Variable*)wParam)->szValue);*/
		break;
	case DMSG_REDRAW_WATCHES:
//		m_wndWatches.Redraw();
		break;
	}

	return 0;
}



CScriptDebugger::CScriptDebugger()
{
	m_pDebugger = this;
//	m_pThread = NULL;
}

CScriptDebugger::~CScriptDebugger()
{
	if(!xr_waitableThread::join(1000))
		xr_waitableThread::kill();
/*	if(m_pThread!=NULL)
		delete m_pThread;
*/
}

lua_State* CScriptDebugger::GetLuaState() 
{ 
	return m_lua.GetState(); 
};

BOOL CScriptDebugger::Prepare(lua_State* l)
{
/*
	m_hWndMainFrame = AfxGetMainWnd()->GetSafeHwnd();

	COutputWnd* pBar = ((CMainFrame*)AfxGetMainWnd())->GetOutputWnd();
	pBar->SetActiveOutput(COutputWnd::outputDebug);

	CScintillaView* pTab = pBar->GetOutput(COutputWnd::outputDebug);
	pTab->Clear();

	CProject *pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	if ( pProject->PositionBreakPoints() )
		AfxMessageBox("One or more breakpoints are not positioned on valid lines. These breakpoints have been moved to the next valid line.", MB_OK);
*/
	m_lua.PrepareDebugger(l);

	m_nMode = DMOD_NONE;

	return TRUE;
}

BOOL CScriptDebugger::Start()
{
	if( !xr_waitableThread::join(5))
		xr_waitableThread::kill();
	
	xr_waitableThread::start();
/*
	if(m_pThread!=NULL)
		delete m_pThread;
	m_pThread = AfxBeginThread(StartDebugger, this,0,0,CREATE_SUSPENDED);
	if(m_pThread!=NULL)
	{
		m_pThread->m_bAutoDelete=false;
		m_pThread->ResumeThread();
	}

	return m_pThread!=NULL;
*/
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// thread functions
//////////////////////////////////////////////////////////////////////////////////////////////

void CScriptDebugger::run_w()
{
	StartDebugger();
};

UINT CScriptDebugger::StartDebugger()
{
	m_nLine = 0;
	m_nLevel = 0;

//	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_START, 0, 0);
	_SendMessage(DMSG_DEBUG_START, 0, 0);

	int nRet = m_lua.StartDebugger();
	EndThread();

	return nRet;
}
/*
UINT CScriptDebugger::StartDebugger( LPVOID pParam )
{	
	return ((CScriptDebugger*)pParam)->StartDebugger();
}*/


void CScriptDebugger::Write(const char* szMsg)
{
//	::SendMessage(m_hWndMainFrame, DMSG_WRITE_DEBUG, (WPARAM)szMsg, 0);
	_SendMessage(DMSG_WRITE_DEBUG, (WPARAM)szMsg, 0);
}

void CScriptDebugger::LineHook(const char *szFile, int nLine)
{
	if ( m_nMode == DMOD_STOP )
		EndThread();

	if (
//		::SendMessage(m_hWndMainFrame, DMSG_HAS_BREAKPOINT, (WPARAM)szFile, (LPARAM)nLine) ||
		_SendMessage(DMSG_HAS_BREAKPOINT, (WPARAM)szFile, (LPARAM)nLine) ||
		m_nMode==DMOD_STEP_INTO || 
		m_nMode==DMOD_BREAK ||
		(m_nMode==DMOD_STEP_OVER && m_nLevel<=0) || 
		(m_nMode==DMOD_STEP_OUT && m_nLevel<0) ||
		(m_nMode==DMOD_RUN_TO_CURSOR && 
			xr_strcmp(m_strPathName,szFile) &&
	//		m_strPathName.CompareNoCase(szFile)==0 && 
			m_nLine==nLine) )
	{
		DebugBreak(szFile, nLine);		
	}
}

void CScriptDebugger::FunctionHook(const char *szFile, int nLine, BOOL bCall)
{
	if ( m_nMode == DMOD_STOP )
		EndThread();

	m_nLevel += (bCall?1:-1);
}

void CScriptDebugger::DebugBreak(const char *szFile, int nLine)
{
	m_nMode = DMOD_NONE;

//	::SendMessage(m_hWndMainFrame, DMSG_GOTO_FILELINE, (WPARAM)szFile, (LPARAM)nLine);
	_SendMessage(DMSG_GOTO_FILELINE, (WPARAM)szFile, (LPARAM)nLine);
	m_lua.DrawStackTrace();
	m_lua.DrawGlobalVariables();
//	::SendMessage(m_hWndMainFrame, DMSG_REDRAW_WATCHES, 0, 0);
	_SendMessage(DMSG_REDRAW_WATCHES, 0, 0);
//	::SendMessage(m_hWndMainFrame, DMSG_GOTO_STACKTRACE_LEVEL, 0, 0);
	_SendMessage(DMSG_GOTO_STACKTRACE_LEVEL, 0, 0);

//	m_event.ResetEvent();
	m_mutex.lock();

//	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_BREAK, 0, 0);
	_SendMessage(DMSG_DEBUG_BREAK, 0, 0);
//	CSingleLock lock(&m_event, TRUE);
	xr_sync sync(m_mutex);

	if ( m_nMode == DMOD_STOP )
		EndThread();
}


void CScriptDebugger::Go()
{
//	m_event.SetEvent();
	m_mutex.unlock();
//	::SendMessage(m_hWndMainFrame, DMSG_DEBUG_START, 0, 0);
	_SendMessage(DMSG_DEBUG_START, 0, 0);
}


void CScriptDebugger::StepInto()
{
	m_nMode = DMOD_STEP_INTO;
	Go();
}

void CScriptDebugger::StepOver()
{
	m_nMode = DMOD_STEP_OVER;
//	m_strPathName = m_lua.GetSource();
	m_nLevel = 0;
	Go();
}

void CScriptDebugger::StepOut()
{
	m_nMode = DMOD_STEP_OUT;
//	m_strPathName = m_lua.GetSource();
	m_nLevel = 0;
	Go();
}
/*
void CScriptDebugger::RunToCursor()
{
	CLuaView* pView = ((CMainFrame*)AfxGetMainWnd())->GetActiveView();
	CLuaDoc* pDoc = pView->GetDocument();

	if ( !pDoc->IsInProject() )
		return;

	m_nMode = DMOD_RUN_TO_CURSOR;

	CProjectFile* pPF = pDoc->GetProjectFile();
	m_strPathName = pPF->GetPathName();

	int nLine = pView->GetEditor()->GetCurrentLine();
	m_nLine = pPF->GetNearestDebugLine(nLine);

	Go();
}
*/
void CScriptDebugger::Stop()
{
	m_nMode = DMOD_STOP;
	Go();

	if( !xr_waitableThread::join(5000) )
		xr_waitableThread::kill();
/*
	MSG msg;
	while ( WaitForSingleObject (m_pThread->m_hThread, 1)==WAIT_TIMEOUT )
		if ( ::PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE) )
			AfxGetThread()->PumpMessage ();

	delete m_pThread;
	m_pThread=NULL;
*/
}

void CScriptDebugger::Break()
{
	m_nMode = DMOD_BREAK;
}

void CScriptDebugger::ClearStackTrace()
{
//	::SendMessage(m_hWndMainFrame, DMSG_CLEAR_STACKTRACE, 0, 0);
	_SendMessage(DMSG_CLEAR_STACKTRACE, 0, 0);
}

void CScriptDebugger::AddStackTrace(const char* szDesc, const char* szFile, int nLine)
{
	StackTrace st;
	st.szDesc = szDesc;
	st.szFile = szFile;
	st.nLine = nLine;

//	::SendMessage(m_hWndMainFrame, DMSG_ADD_STACKTRACE, (WPARAM)&st, 0);
	_SendMessage(DMSG_ADD_STACKTRACE, (WPARAM)&st, 0);
}

int CScriptDebugger::GetStackTraceLevel()
{
//	return ::SendMessage(m_hWndMainFrame, DMSG_GET_STACKTRACE_LEVEL, 0, 0);
	return _SendMessage(DMSG_GET_STACKTRACE_LEVEL, 0, 0);
}

void CScriptDebugger::StackLevelChanged()
{
	m_lua.DrawLocalVariables();
}

void CScriptDebugger::ClearLocalVariables()
{
//	::SendMessage(m_hWndMainFrame, DMSG_CLEAR_LOCALVARIABLES, 0, 0);
	_SendMessage(DMSG_CLEAR_LOCALVARIABLES, 0, 0);
}

void CScriptDebugger::AddLocalVariable(const char *name, const char *type, const char *value)
{
	Variable var;
	var.szName = name;
	var.szType = type;
	var.szValue = value;

//	::SendMessage(m_hWndMainFrame, DMSG_ADD_LOCALVARIABLE, (WPARAM)&var, 0);
	_SendMessage(DMSG_ADD_LOCALVARIABLE, (WPARAM)&var, 0);
}

void CScriptDebugger::ClearGlobalVariables()
{
//	::SendMessage(m_hWndMainFrame, DMSG_CLEAR_GLOBALVARIABLES, 0, 0);
	_SendMessage(DMSG_CLEAR_GLOBALVARIABLES, 0, 0);
}

void CScriptDebugger::AddGlobalVariable(const char *name, const char *type, const char *value)
{
	Variable var;
	var.szName = name;
	var.szType = type;
	var.szValue = value;

//	::SendMessage(m_hWndMainFrame, DMSG_ADD_GLOBALVARIABLE, (WPARAM)&var, 0);
	_SendMessage(DMSG_ADD_GLOBALVARIABLE, (WPARAM)&var, 0);
}

BOOL CScriptDebugger::GetCalltip(const char *szWord, char *szCalltip)
{
	return m_lua.GetCalltip(szWord, szCalltip);
}


void CScriptDebugger::EndThread()
{
//	FreeConsole();
//	::SendMessage(m_pDebugger->m_hWndMainFrame, DMSG_DEBUG_END, 0, 0);
	_SendMessage(DMSG_DEBUG_END, 0, 0);
	m_pDebugger->Write("The program has exited...\n");
	m_pDebugger->m_lua.StopDebugger();
	if( !xr_waitableThread::join(1000) )
		xr_waitableThread::kill();

//	AfxEndThread(0);
}


//CString CScriptDebugger::Eval(CString strCode)
void CScriptDebugger::Eval(const char* strCode, char* res)
{
	string1024 strCodeFull;
	const char * r = "return  ";
	strconcat(strCodeFull,r,strCode);
//	strCode = "return " + strCode;
	
//	m_lua.Eval(strCode.GetBuffer(0), strRet.GetBuffer(256));
	m_lua.Eval(strCodeFull, res);
//	strRet.ReleaseBuffer();
}

/*
void CScriptDebugger::Execute()
{
	CString strInterpreter = "F:\\a ja tak\\lubie cie bardzo\\lua.exe"; //theApp.GetModuleDir() + "\\" + "lua.exe";
	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();

	_spawnl( _P_NOWAIT, strInterpreter, strInterpreter, "\"" + pProject->GetDebugPathNameExt() + "\"", NULL );
}
*/