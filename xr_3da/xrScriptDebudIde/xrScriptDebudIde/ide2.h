// ide2.h : main header file for the IDE2 application
//

#if !defined(AFX_IDE2_H__A3FA6447_9B87_4B84_A3F1_D73F185A1AAC__INCLUDED_)
#define AFX_IDE2_H__A3FA6447_9B87_4B84_A3F1_D73F185A1AAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


class CLuaView;
class CLuaDoc;
class CProjectFile;

class CIdeApp : public CWinApp
{
public:
	BOOL		SaveModifiedDocuments();
	CLuaView*	OpenProjectFilesView(CProjectFile* pPF, int nLine=-1);
	CLuaView*	LoadProjectFilesView(CProjectFile* pPF);
	CLuaView*	FindProjectFilesView(CProjectFile* pPF);

	CIdeApp();

	CMultiDocTemplate* m_pLuaTemplate;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIdeApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CIdeApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	HMODULE m_hScintilla;
};

extern CIdeApp theApp;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDE2_H__A3FA6447_9B87_4B84_A3F1_D73F185A1AAC__INCLUDED_)
