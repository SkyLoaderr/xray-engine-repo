// LuaView.cpp : implementation of the CLuaView class
//

#include "stdafx.h"
#include "ide2.h"

#include "LuaDoc.h"
#include "LuaView.h"
#include "ProjectFile.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLuaView

IMPLEMENT_DYNCREATE(CLuaView, CView)

BEGIN_MESSAGE_MAP(CLuaView, CView)
	//{{AFX_MSG_MAP(CLuaView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_SELECT_ALL, OnEditSelectAll)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_COMMAND(ID_TOGGLE_BREAKPOINT, OnToggleBreakpoint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLuaView construction/destruction

CLuaView::CLuaView()
{
	// TODO: add construction code here

}

CLuaView::~CLuaView()
{
}

BOOL CLuaView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CLuaView drawing

void CLuaView::OnDraw(CDC* pDC)
{
	CLuaDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CLuaView diagnostics

#ifdef _DEBUG
void CLuaView::AssertValid() const
{
	CView::AssertValid();
}

void CLuaView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLuaDoc* CLuaView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLuaDoc)));
	return (CLuaDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLuaView message handlers

int CLuaView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if ( !m_editor.Create(this, ID_EDIT_CTRL) )
		return -1;
	
	m_editor.SetEditorMargins();
	m_editor.SetLuaLexer();

	return 0;
}

void CLuaView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	if ( ::IsWindow(m_editor.m_hWnd) )
		m_editor.SetWindowPos(NULL, 0, 0, cx, cy, 0);
}

BOOL CLuaView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR *lpnmhdr = (LPNMHDR) lParam; 

	if (lpnmhdr->hwndFrom == m_editor.m_hWnd)
	{
		*pResult = OnSci((SCNotification*)lParam);
		return TRUE;
	}
	
	return CView::OnNotify(wParam, lParam, pResult);
}

int CLuaView::OnSci(SCNotification* pNotify)
{
	CPoint pt;
	int nLine;
	switch (pNotify->nmhdr.code)
	{
		case SCN_MARGINCLICK:
			if ( GetDocument()->IsInProject() )
			{
				GetCursorPos(&pt);
				ScreenToClient(&pt);
				nLine = m_editor.LineFromPoint(pt);
				
				ToggleBreakPoint(nLine);
			}
		break;

		case SCI_SETSAVEPOINT:
		case SCN_SAVEPOINTREACHED:
			GetDocument()->SetModifiedFlag(FALSE);
		break;

		case SCN_SAVEPOINTLEFT:
			GetDocument()->SetModifiedFlag(TRUE);
		break;
	}

	return TRUE;
}

void CLuaView::OnEditCopy() 
{
	// TODO: Add your command handler code here
	m_editor.Copy();
}

void CLuaView::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanCutOrClear());	
}

void CLuaView::OnEditCut() 
{
	m_editor.Cut();
}

void CLuaView::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanCutOrClear());	
}

void CLuaView::OnEditClear() 
{
	m_editor.Clear();
}

void CLuaView::OnUpdateEditClear(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanCutOrClear());	
}

void CLuaView::OnEditPaste() 
{
	m_editor.Paste();
}

void CLuaView::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanPaste());
}

void CLuaView::OnEditUndo() 
{
	m_editor.Undo();
}

void CLuaView::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_editor.CanUndo());
}

void CLuaView::OnEditRedo() 
{
	m_editor.Redo();
}

void CLuaView::OnUpdateEditRedo(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_editor.CanRedo());	
}

void CLuaView::OnEditSelectAll() 
{
	m_editor.SelectAll();	
}


void CLuaView::Activate()
{
	CFrameWnd* pFrame = GetParentFrame();

	if (pFrame != NULL)
		pFrame->ActivateFrame();
	else
		TRACE0("Error: Can not find a frame for document to activate.\n");

	CFrameWnd* pAppFrame;
	if (pFrame != (pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd))
	{
		ASSERT_KINDOF(CFrameWnd, pAppFrame);
		pAppFrame->ActivateFrame();
	}

	GetEditor()->GrabFocus();
}


void CLuaView::CloseFrame()
{
	CFrameWnd* pFrame = GetParentFrame();

	if (pFrame != NULL)
		pFrame->SendMessage(WM_CLOSE);
}

void CLuaView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	// use main frame dynamic accel table
	GetParentFrame()->m_hAccelTable = NULL;
}

void CLuaView::ToggleBreakPoint(int nLine)
{
	CMainFrame* pFrame = g_mainFrame;
/*	if ( pFrame->GetMode() == CMainFrame::modeDebug ||
		pFrame->GetMode() == CMainFrame::modeDebugBreak )
	{
		nLine = GetDocument()->GetProjectFile()->GetNearestDebugLine(nLine);
		if ( nLine==0 )
			return;
	}*/

	if ( m_editor.ToggleBreakpoint(nLine) )
		GetDocument()->GetProjectFile()->AddBreakPoint(nLine);
	else
		GetDocument()->GetProjectFile()->RemoveBreakPoint(nLine);
}

void CLuaView::OnToggleBreakpoint() 
{
	if ( GetDocument()&&GetDocument()->IsInProject() )
		ToggleBreakPoint(m_editor.GetCurrentLine());
}
