#include "stdafx.h"

#include "script_CallStack.h"
#include "script_debugger.h"

CScriptCallStack::CScriptCallStack()
{}

CScriptCallStack::~CScriptCallStack()
{}

/*
int CCallStack::OnSci(CScintillaView* pView, SCNotification* pNotify)
{
	CLuaEditor* pEditor = ((CScintillaView*)GetView(0))->GetEditor();

	CPoint pt;
	int nLine;
	CString strLine;
	switch (pNotify->nmhdr.code)
	{
	case SCN_DOUBLECLICK:
		GetCursorPos(&pt);
		pEditor->ScreenToClient(&pt);
		nLine = pEditor->LineFromPoint(pt);
		GotoStackTraceLevel(nLine-1);
		break;
	};

	return 0;
}
*/

void CScriptCallStack::Clear()
{
	m_nCurrentLevel = -1;
	m_lines.clear();
	m_files.clear();
}

void CScriptCallStack::Add(const char *szDesc, const char *szFile, int nLine)
{
//	((CScintillaView*)GetView(0))->Write(CString(szDesc)+"\n");

/*	string_path sp;
	strcat (sp, szFile);
*/
	SPath	sp;
	m_files.push_back(sp);
	strcat(m_files.back().path, szFile );
	m_lines.push_back(nLine);
}

void CScriptCallStack::GotoStackTraceLevel(int nLevel)
{
	if ( nLevel<0 || (u32)nLevel >= m_files.size() )
		return;

	m_nCurrentLevel = nLevel;

//	CLuaEditor* pEditor = ((CScintillaView*)GetView(0))->GetEditor();
//	pEditor->SetStackTraceLevel(nLevel);

	//open document in editor
//	((CMainFrame*)AfxGetMainWnd())->GotoFileLine(m_files[nLevel], m_lines[nLevel]);	

//	((CMainFrame*)AfxGetMainWnd())->GetDebugger()->StackLevelChanged();
	CScriptDebugger::GetDebugger()->StackLevelChanged();
}


