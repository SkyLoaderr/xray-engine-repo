// xrUpdateDoc.cpp : implementation of the CxrUpdateDoc class
//

#include "stdafx.h"
#include "xrUpdate.h"

#include "xrUpdateDoc.h"
#include "upd_task.h"
#include "xrUpdateView.h"
#include "MainFrm.h"
#include ".\xrupdatedoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CxrUpdateDoc

IMPLEMENT_DYNCREATE(CxrUpdateDoc, CDocument)

BEGIN_MESSAGE_MAP(CxrUpdateDoc, CDocument)
END_MESSAGE_MAP()


// CxrUpdateDoc construction/destruction

CxrUpdateDoc::CxrUpdateDoc()
{
	// TODO: add one-time construction code here
	m_task = NULL;
}

CxrUpdateDoc::~CxrUpdateDoc()
{
}

BOOL CxrUpdateDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	m_task =	CTaskFacrory::create_task(eTaskRoot);
	m_task->set_name("new_task");
	return TRUE;

/*	
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
*/
}




// CxrUpdateDoc serialization

void CxrUpdateDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{// TODO: add storing code here
	}
	else
	{// TODO: add loading code here
	}
}


void CxrUpdateDoc::OnCloseDocument()
{
	if(m_task)
		xr_delete(m_task);
	CDocument::OnCloseDocument();
}

BOOL CxrUpdateDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if(m_task)
		xr_delete(m_task);

	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	CInifile Ini(lpszPathName,TRUE,TRUE,FALSE);
	m_task = CTaskFacrory::create_task(Ini,"main");
	m_task->load(Ini,"main");

	return TRUE;
}

BOOL CxrUpdateDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	CDocument::OnSaveDocument(lpszPathName);

	CInifile Ini(lpszPathName,FALSE,FALSE,TRUE);
	m_task->save(Ini,"main");
	return TRUE;
}

BOOL CxrUpdateDoc::SaveModified()
{
	return CDocument::SaveModified();
}
