// Project.cpp: implementation of the CProject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "Project.h"

#include "WorkspaceWnd.h"
#include "TreeViewFiles.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "MainFrame.h"
#include "ProjectProperties.h"
#include "ScintillaView.h"
#include "ProjectNew.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProject::CProject()
{
	SetModifiedFlag(FALSE);
}

CProject::~CProject()
{
	RemoveProjectFiles();
}

//--------------------------------------------------------------------------------------------------
//- file and directory functions
//--------------------------------------------------------------------------------------------------

CString CProject::GetName()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	return CString(fname);
}

CString CProject::GetNameExt()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	return CString(fname)+ext;
}



//--------------------------------------------------------------------------------------------------
//- project files functions
//--------------------------------------------------------------------------------------------------

void CProject::RedrawFilesTree()
{
	CWorkspaceWnd* pWorkspace = g_mainFrame->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	pTree->RemoveAll();

	pTree->AddRoot(GetName());

	int nFiles = m_files.GetSize();
	for ( int i=0; i<nFiles; ++i )
		pTree->AddProjectFile(m_files[i]->GetNameExt(), (long)m_files[i]);
}


CProjectFile* CProject::GetProjectFile(CString strPathName)
{
	int nSize = m_files.GetSize();
	for ( int i=0; i<nSize; ++i )
		if ( m_files[i]->HasFile(strPathName) )
			return m_files[i];

	AddFile(strPathName, FALSE);
	nSize = m_files.GetSize();
	for ( int i=0; i<nSize; ++i )
		if ( m_files[i]->HasFile(strPathName) )
			return m_files[i];

	return NULL;
}

void CProject::AddFiles()
{
	CFileDialog fd(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_ALLOWMULTISELECT, 
		"Lua files (*.lua)|*.lua|All files (*.*)|*.*||", g_mainFrame);

	if ( fd.DoModal()==IDOK )
	{
		POSITION pos = fd.GetStartPosition();
		while (pos)
		{
			CString strPathName = fd.GetNextPathName(pos);
			AddFile(strPathName);
		}
	}

}

void CProject::AddFile(CString strPathName, BOOL bFindExisting)
{
 	CProjectFile* pPF;
	if ( (bFindExisting)&&(pPF=GetProjectFile(strPathName)) != NULL )
		return;

	pPF = new CProjectFile;
	pPF->SetPathName(strPathName);

	AddFile(pPF);
}

void CProject::AddFile(CProjectFile* pPF)
{
	m_files.Add(pPF);

	CWorkspaceWnd* pWorkspace = g_mainFrame->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	pTree->AddProjectFile(pPF->GetNameExt(), (long)pPF);

	SetModifiedFlag(TRUE);
}

void CProject::RemoveFile(CProjectFile *pPF)
{
	int nSize = m_files.GetSize();
	for ( int i=0; i<nSize; ++i )
		if ( m_files[i] == pPF )
		{
			m_files.RemoveAt(i);
			break;
		}

	SetModifiedFlag(TRUE);
}

void CProject::RemoveProjectFiles()
{
	int nSize = m_files.GetSize();
	for ( int i=0; i<nSize; ++i )
	{
		CLuaView *pView = theApp.FindProjectFilesView(m_files[i]);
		if ( pView )
			pView->CloseFrame();
	}

	for ( i=0; i<nSize; ++i )
		delete m_files[i];

	m_files.RemoveAll();
}


//--------------------------------------------------------------------------------------------------
//- project new/save/load/close functions
//--------------------------------------------------------------------------------------------------

void CProject::NewProject()
{
	AfxMessageBox("ERROR :)");
	GetCurrentDirectory(MAX_PATH, m_strPathName.GetBuffer(MAX_PATH));
	m_strPathName.ReleaseBuffer();
	m_strPathName += "\\project1.lpr";
	RedrawFilesTree();
	SetModifiedFlag(FALSE);
}

BOOL CProject::New(CString sFileName)
{
	m_strPathName = sFileName;

	RedrawFilesTree();

	Save();
	SetModifiedFlag(FALSE);

	return TRUE;
};

BOOL CProject::New()
{
	SaveModified();

	CProjectNew pn;
	if ( pn.DoModal()!=IDOK )
		return FALSE;

	m_strPathName = pn.GetProjectPathName();
	return New(m_strPathName);
/*
	RedrawFilesTree();

	pn.CreateByType(this);

	Save();
	SetModifiedFlag(FALSE);

	return TRUE;
*/
}

BOOL CProject::Close()
{
	SaveModified();

	m_strPathName = "";

	RemoveProjectFiles();

	CWorkspaceWnd* pWorkspace = g_mainFrame->GetWorkspaceWnd();
	CTreeViewFiles* pTree = pWorkspace->GetTreeViewFiles();
	pTree->RemoveAll();

	return TRUE;
}

BOOL CProject::Load()
{
	SaveModified();

	CFileDialog fd(TRUE, NULL, NULL, OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST, 
		"Project files (*.lpr)|*.lpr|All files (*.*)|*.*||", g_mainFrame);

	if ( fd.DoModal()!=IDOK )
		return FALSE;

	return Load(fd.GetPathName());
/*
	CFile fin;
	if ( !fin.Open(fd.GetPathName(), CFile::modeRead) )
	{
		AfxMessageBox("Unable to open project file");
		return FALSE;
	}

	m_strPathName = fd.GetPathName();

	CArchive ar(&fin, CArchive::load);

	RemoveProjectFiles();
	
	BOOL bResult = Load(ar);

	SetModifiedFlag(FALSE);

	RedrawFilesTree();

	return bResult;
*/
}

BOOL CProject::Load(CString sFileName)
{
	CFile fin;
	if ( !fin.Open(sFileName, CFile::modeRead) )
	{
		AfxMessageBox("Unable to open project file");
		return FALSE;
	}

	m_strPathName = sFileName;

	CArchive ar(&fin, CArchive::load);

	RemoveProjectFiles();
	
	BOOL bResult = Load(ar);

	SetModifiedFlag(FALSE);

	RedrawFilesTree();

	return bResult;
}

BOOL CProject::Load(CArchive &ar)
{
	ar >> m_strPathName;

	long nFiles;
	ar >> nFiles;

	for ( int i=0; i<nFiles; ++i )
	{
		CProjectFile *pPF = new CProjectFile;
		pPF->Load(ar);

		AddFile(pPF);
	}

	return TRUE;
}

BOOL CProject::Save()
{
/*
	CString sFileName;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	AfxGetModuleShortFileName(AfxGetInstanceHandle(),sFileName);
	_splitpath( sFileName, drive, dir, fname, ext );

	sFileName.Format("%s%s%s",drive,dir,m_strPathName);
*/
	return Save(m_strPathName);
}

BOOL CProject::Save(CString strPathName)
{
	CFile fout;
	if ( !fout.Open(strPathName, CFile::modeCreate|CFile::modeWrite) )
	{
		AfxMessageBox("Unable to open project file");
		return FALSE;
	}

	CArchive ar(&fout, CArchive::store);

	BOOL bResult = Save(ar);

	SetModifiedFlag(FALSE);

	return bResult;
}

BOOL CProject::Save(CArchive &ar)
{
	ar << m_strPathName;

	long nFiles = m_files.GetSize();
	ar << nFiles;

	for ( int i=0; i<nFiles; ++i )
		m_files[i]->Save(ar);

	return TRUE;
}

BOOL CProject::SaveAs()
{
	CFileDialog fd(FALSE, "lpr", m_strPathName, OFN_PATHMUSTEXIST, 
		"Project files (*.lpr)|*.lpr|All files (*.*)|*.*||", g_mainFrame);

	if ( fd.DoModal()!=IDOK )
		return FALSE;

	m_strPathName = fd.GetPathName();

	BOOL bResult = Save(m_strPathName);

	return bResult;
}


void CProject::SaveModified()
{
	if ( m_bModified )
	{
		if ( AfxMessageBox("Your project has changed. Do you want to save it?", MB_YESNO)==IDYES )
			Save();
	}
}

//--------------------------------------------------------------------------------------------------
//- compile/build functions
//--------------------------------------------------------------------------------------------------

BOOL CProject::PositionBreakPoints()
{
	BOOL bModified = FALSE;
	int nFiles = m_files.GetSize();
	for ( int i=0; i<nFiles; ++i )
	{
		if ( m_files[i]->PositionBreakPoints() )
		{
			bModified = TRUE;
			CLuaView* pView = theApp.FindProjectFilesView(m_files[i]);
			if ( pView )
				m_files[i]->SetBreakPointsIn(pView->GetEditor());
		}
	}

	if ( bModified )
		SetModifiedFlag(TRUE);

	return bModified;
}

CString CProject::GetProjectDir()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	if ( dir[strlen(dir)-1]=='\\' )
		dir[strlen(dir)-1] = '\0';
	return CString(drive)+dir;	
}

BOOL CProject::HasBreakPoint(const char *szFile, int nLine)
{
	CProjectFile *pPF = GetProjectFile(szFile);
	if ( pPF==NULL )
		return FALSE;

	return pPF->HasBreakPoint(nLine);
}

