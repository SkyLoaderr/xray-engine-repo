// MainFrame.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "WorkspaceWnd.h"
#include "OutputWnd.h"
#include "CallStack.h"
#include "Project.h"
#include "MDIClientWnd.h"
#include "VariablesBar.h"
#include "WatchBar.h"
#include "ScriptThreadsBar.h"

#define R_ASSERT(x) ASSERT(x)
#include "../xrGame/mslotutils.h"

class CMainFrame : public CCJMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

// Attributes
public:
	enum {
		modeNoProject,
		modeIdle,
		modeDebug,
		modeDebugBreak
	} appMode;

protected:
	CWorkspaceWnd			m_wndWorkspace;
	COutputWnd				m_wndOutput;
	CCallStack				m_wndCallStack;
	CProject				m_project;
	CMDIClientWnd			m_wndMDIClient;
	CVariablesBar			m_wndLocals;
	CWatchBar				m_wndWatches;
	CScriptThreadsBar		m_wndThreads;
	HACCEL					m_hAccelDebug, m_hAccelDebugBreak, m_hAccelNoProject;
	int						m_nAppMode;
	BOOL					m_needAnswer;

	static UINT		StartListener( LPVOID pParam );	
	UINT			StartListener();	
	void			TranslateMsg( CMailSlotMsg& msg );
	CWinThread*		m_pMailSlotThread;
	HANDLE			m_mailSlot;

	BOOL			checkExistingFolder(CString str);
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	//}}AFX_VIRTUAL

// Implementation
public:
	void			EvalWatch(CString watch, int iItem);
	void			OpenVarTable(char* varName);
	void			ThreadChanged(int nThreadID);
	void			StackLevelChanged(int nLevel);
	BOOL			ErrorStringToFileLine(CString strError, CString &strPathName, int &nLine);
	void			SetMode(int nMode);
	int				GetMode() { return m_nAppMode; };
	void			GotoFileLine(const char* szFile, int nLine);
	LRESULT			DebugMessage(UINT nMsg, WPARAM wParam, LPARAM lParam);
	CLuaView*		GetActiveView();
	COutputWnd*		GetOutputWnd() { return &m_wndOutput; };
	CProject*		GetProject() { return &m_project; };
	CWorkspaceWnd*	GetWorkspaceWnd() { return &m_wndWorkspace; };
	CCallStack*		GetCallStack() { return &m_wndCallStack; };
	BOOL			InitDockingWindows();
	void			UpdateFrameTitleForDocument(LPCTSTR lpszDocName);
	virtual			~CMainFrame();
	void			SendBreakPoints();
	void			OpenDefaultProject();

#ifdef _DEBUG
	virtual void	AssertValid() const;
	virtual void	Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	CStatusBar		m_wndStatusBar;
	CToolBar		m_wndToolBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnFileOpenproject();
	afx_msg void OnFileSaveproject();
	afx_msg void OnFileSaveprojectas();
	afx_msg void OnProjectAddFiles();
	afx_msg void OnRunApplication();
	afx_msg void OnDebuggingOptions();
	afx_msg void OnClose();
	afx_msg void OnDebugGo();
	afx_msg void OnDebugStepinto();
	afx_msg void OnDebugStepover();
	afx_msg void OnDebugStepout();
	afx_msg void OnDebugRuntocursor();
	afx_msg void OnDebugStopdebugging();
	afx_msg void OnFileNewproject();
	afx_msg void OnFileCloseproject();
	afx_msg void OnDebugBreak();
	afx_msg void OnToolsOptions();

	afx_msg void OnUpdateDebugMenu(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolsOptions(CCmdUI* pCmdUI);

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

extern CMainFrame*	g_mainFrame;
