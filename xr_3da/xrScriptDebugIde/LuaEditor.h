#if !defined(AFX_LUAEDITOR_H__C002AF8D_DFEF_4577_A494_10F2B3DF2458__INCLUDED_)
#define AFX_LUAEDITOR_H__C002AF8D_DFEF_4577_A494_10F2B3DF2458__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LuaEditor.h : header file
//

#include "Platform.h"
#include "scintilla.h"

/////////////////////////////////////////////////////////////////////////////
// CLuaEditor window

class CProjectFile;
class CLuaView;

class CLuaEditor : public CWnd
{
// Construction
public:
	CLuaEditor();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLuaEditor)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetLuaLexer();
	BOOL PreparePrint(CDC* pDC, CPrintInfo* pInfo);
	void PrintPage(CDC* pDC, CPrintInfo* pInfo);
	void EndPrint(CDC* pDC, CPrintInfo* pInfo);
	void SetStackTraceLevel(int nLevel);
	void SetCallStackMargins();
	int GetCurrentLine();
	void GotoLastLine();
	CString GetLine(int nLine);
	void ClearAll();
	void AddText(const char* szText);
	void SetReadOnly(BOOL bReadOnly);
	void SetEditorMargins();
	void GrabFocus();
	void Copy();
	void Paste();
	BOOL CanPaste();
	void Clear();
	void Cut();
	BOOL CanCutOrClear();
	void SelectAll();
	void Redo();
	BOOL CanRedo();
	void Undo();
	BOOL CanUndo();
	void SetBreakPointsIn(CProjectFile* pPF);
	void GotoLine(int nLine);
	void SetBreakpoint(int nLine);
	void ClearAllBreakpoints();
	BOOL Save(CFile* pFile);
	BOOL Load(CFile* pFile);
	BOOL ToggleBreakpoint(int nLine);
	int LineFromPoint(CPoint& pt);
	BOOL Create(CWnd* pParentWnd, UINT nCtrlID);
	virtual ~CLuaEditor();
//find 
	CString GetSelText();
	long	GetSelectionStart(BOOL bDirect=TRUE);
	long	GetSelectionEnd(BOOL bDirect=TRUE);
	int		GetSelText(char* text, BOOL bDirect = TRUE);
	int		GetLength(BOOL bDirect = TRUE);
	void	HideSelection(BOOL normal, BOOL bDirect = TRUE);
	void	ReplaceSel(const char* text, BOOL bDirect = TRUE);
	long	FindText(int flags, TextToFind* ft, BOOL bDirect = TRUE);
	void	SetSel(long start, long end, BOOL bDirect = TRUE);
	int		PointXFromPosition(long pos, BOOL bDirect = TRUE);
	int		PointYFromPosition(long pos, BOOL bDirect = TRUE);

// thread safe functions
	//void tsAddText(const char *szMsg) {	AddText(szMsg); };
	//void tsSetReadOnly(BOOL bReadOnly) { SetReadOnly(bReadOnly); };

	void Print(bool showDialog);
	CharacterRange GetSelection();
	void GotoLineDlg();
	void FindText(CString str);
	CLuaView*	m_lua_view;
protected:
	int (*m_fnScintilla)(void*,int,int,int);
	void *m_ptrScintilla;
	CUIntArray	m_pages;
	BOOL m_bShowCalltips;
	CString m_strCallTip;

	int Sci(int nCmd, int wParam=0, int lParam=0);

	// Generated message map functions
protected:
	//{{AFX_MSG(CLuaEditor)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LUAEDITOR_H__C002AF8D_DFEF_4577_A494_10F2B3DF2458__INCLUDED_)
