// LuaDoc.cpp : implementation of the CLuaDoc class
//

#include "stdafx.h"
#include "ide2.h"

#include "LuaDoc.h"

#include "LuaView.h"
#include "MainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLuaDoc

IMPLEMENT_DYNCREATE(CLuaDoc, CDocument)

BEGIN_MESSAGE_MAP(CLuaDoc, CDocument)
	//{{AFX_MSG_MAP(CLuaDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLuaDoc construction/destruction

CLuaDoc::CLuaDoc()
{
	// TODO: add one-time construction code here

}

CLuaDoc::~CLuaDoc()
{
}

BOOL CLuaDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CLuaDoc serialization

void CLuaDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		CLuaEditor* pEditor = GetView()->GetEditor();
		pEditor->Save(ar.GetFile());
	}
	else
	{
		// TODO: add loading code here
		CLuaEditor* pEditor = GetView()->GetEditor();
		pEditor->Load(ar.GetFile());
	}
}

/////////////////////////////////////////////////////////////////////////////
// CLuaDoc diagnostics

#ifdef _DEBUG
void CLuaDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLuaDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLuaDoc commands

CLuaView* CLuaDoc::GetView()
{
	POSITION pos = GetFirstViewPosition();
	if (pos != NULL)
		return (CLuaView*)GetNextView(pos); // get first one

	return NULL;
}

BOOL CLuaDoc::IsInProject()
{
	return GetProjectFile()!=NULL;
}

CProjectFile* CLuaDoc::GetProjectFile()
{
	CMainFrame* pFrame = g_mainFrame;
	return pFrame->GetProject()->GetProjectFile(GetPathName());
}

BOOL CLuaDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	CMainFrame* pFrame = g_mainFrame;
	CProjectFile* pPF = pFrame->GetProject()->GetProjectFile(lpszPathName);
	if ( pPF )
		pPF->SetBreakPointsIn(GetView()->GetEditor());
	
	return TRUE;
}
