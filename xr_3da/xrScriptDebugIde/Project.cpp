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
#include "ScintillaView.h"
#include "DebugDialog.h"
#include "NewProjectDlg.h"

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

/*
CString CProject::GetNameExt()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	return CString(fname)+ext;
}
*/


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

	pTree->ExpandFiles();
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
		"Script files (*.script)|*.script|All files (*.*)|*.*||", g_mainFrame);

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
			delete m_files.GetAt(i);
			m_files.RemoveAt(i);
			break;
		}

	SetModifiedFlag(TRUE);
//	RedrawFilesTree();
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
/*
void CProject::NewProject()
{
	AfxMessageBox("ERROR :)");
	GetCurrentDirectory(MAX_PATH, m_strPathName.GetBuffer(MAX_PATH));
	m_strPathName.ReleaseBuffer();
	m_strPathName += "\\project1.lpr";
	RedrawFilesTree();
	SetModifiedFlag(FALSE);
}
*/
/*
BOOL CProject::New(CString sFileName)
{
	m_strPathName = sFileName;

	RedrawFilesTree();

	Save();
	SetModifiedFlag(FALSE);

	return TRUE;
};
*/
BOOL CProject::New()
{
	SaveModified();
	Close();
	DWORD				sz_user	= 64;
	char				UserName[64];

	GetUserName			(UserName,&sz_user);
	

	CNewProjectDlg dlg;
//	dlg.m_project_name.Format("%s_scr_dbg",UserName);
	CString s;
	s.Format("%s_scr_dbg",UserName);
	dlg.SetFileName(s);
	if ( dlg.DoModal()!=IDOK ){
		return FALSE;
	}
/*	if(dlg.m_project_name.Find(".lpr")!=dlg.m_project_name.GetLength()-4)
		m_strName = dlg.m_project_name + ".lpr";
*/
	m_strName = dlg.GetProjectName();

	RedrawFilesTree();

	Save();
	SetModifiedFlag(FALSE);

	return TRUE;

}

BOOL CProject::Close()
{
	SaveModified();
	SetName("");

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
	if( !fin.Open(sFileName,CFile::modeRead) )
	{
		CString msg;
		msg.Format("Unable to open project file %s", sFileName);
		AfxMessageBox(msg);
		return FALSE;
	}


	m_strName = sFileName;

	CArchive ar(&fin, CArchive::load);

	RemoveProjectFiles();
	
	BOOL bResult = Load(ar);

	SetModifiedFlag(FALSE);

	RedrawFilesTree();

	return bResult;
}

BOOL CProject::Load(CArchive &ar)
{
	int version;
	ar >> version;
	if( (version > PROJ_VERSION) || (version < 0x03)){
		AfxMessageBox("Project file is corrupted. Re-create project.");
		return FALSE;
	}

	long nFiles;
	ar >> nFiles;

	for ( int i=0; i<nFiles; ++i )
	{
		CProjectFile *pPF = new CProjectFile;
		pPF->Load(ar);

		AddFile(pPF);
	}

	try{
		ar >> m_command;
		ar >> m_arguments;
		ar >> m_working_dir;
	}
	catch( ... )
	{
		//fake.
		//new version of file saving :)
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
	return Save( GetName() );
}

BOOL CProject::Save(CString strPathName)
{
	CFile fout;
	if ( !fout.Open(strPathName, CFile::modeCreate|CFile::modeWrite) )
	{
		AfxMessageBox("Unable to open project file");
		return FALSE;
	}

	m_strName = strPathName;

	CArchive ar(&fout, CArchive::store);

	BOOL bResult = Save(ar);

	SetModifiedFlag(FALSE);

	return bResult;
}

BOOL CProject::Save(CArchive &ar)
{
	int version = PROJ_VERSION;
	ar << version;

	long nFiles = m_files.GetSize();
	ar << nFiles;

	for ( int i=0; i<nFiles; ++i )
		m_files[i]->Save(ar);

	ar << m_command;
	ar << m_arguments;
	ar << m_working_dir;

	return TRUE;
}

BOOL CProject::SaveAs()
{
	CFileDialog fd(FALSE, "lpr", GetName(), OFN_PATHMUSTEXIST, 
		"Project files (*.lpr)", g_mainFrame);
//		"Project files (*.lpr)|*.lpr|All files (*.*)|*.*||", g_mainFrame);

	if ( fd.DoModal()!=IDOK )
		return FALSE;

	SetName (fd.GetPathName() );

	return Save();
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
/*
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
}*/

void CProject::FillBreakPoints(CMailSlotMsg* msg)
{
	if(!m_files.GetSize()){
		msg->w_int(0);
		return;
	};
		
	int nCnt = 0;
	CProjectFile* pf;
	for(int i=0;i<m_files.GetSize();++i)
	{
		pf = m_files[i];
		if( pf->HasBreakPoint() )
			++nCnt;
	};

	msg->w_int(nCnt);
	for(int i=0;i<m_files.GetSize();++i)
	{
		pf = m_files[i];
		if( pf->HasBreakPoint() )
			pf->FillBreakPoints(msg);
	};
}

BOOL CProject::HasBreakPoint(const char *szFile, int nLine)
{
	CProjectFile *pPF = GetProjectFile(szFile);
	if ( pPF==NULL )
		return FALSE;

	return pPF->HasBreakPoint(nLine);
}

void CProject::OnRunApplication()
{
	if( m_command.IsEmpty() )
	{
		AfxMessageBox("Debugging params not defined");
		OnDebugOptions();
		return;
	}
	
	CString strCmdLine;
	strCmdLine.Format("%s%s",m_working_dir.GetBuffer(),m_command.GetBuffer() );

	CString curDir;
	if(!m_working_dir.IsEmpty()){
		GetCurrentDirectoryA(2048,curDir.GetBuffer(2048));
		SetCurrentDirectoryA(m_working_dir.GetBuffer());
	};
	spawnl(_P_NOWAIT, m_command.GetBuffer(), (m_arguments.IsEmpty())?" ":m_arguments.GetBuffer());
	if(!m_working_dir.IsEmpty())
		SetCurrentDirectoryA(curDir.GetBuffer());
}

void CProject::OnDebugOptions()
{
	CDebugDialog d;

	d.m_command			= m_command;
	d.m_arguments		= m_arguments;
	d.m_working_dir		= m_working_dir;

	if ( d.DoModal()!=IDOK )
		return ;

	m_command			= d.m_command;
	m_arguments			= d.m_arguments;
	m_working_dir		= d.m_working_dir;

	SetModifiedFlag(TRUE);
}

void CProject::SetModifiedFlag(BOOL bModified) 
{ 
	m_bModified = bModified; 
};



/*
void CProjectNew::OnProjectSelloc() 
{
	UpdateData(TRUE);

	BROWSEINFO bi;
	TCHAR szDir[MAX_PATH];
	LPITEMIDLIST pidl;
	LPMALLOC pMalloc;

	if (SUCCEEDED(SHGetMalloc(&pMalloc))) 
	{
		ZeroMemory(&bi,sizeof(bi));
		bi.hwndOwner = NULL;
		bi.pszDisplayName = 0;
		bi.pidlRoot = 0;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
		bi.lpfn = BrowseCallbackProc;

		pidl = SHBrowseForFolder(&bi);
		if (pidl) 
		{
			if (SHGetPathFromIDList(pidl,szDir)) 
			{
				m_strProjectDir = szDir;
				UpdateData(FALSE);
			}

			// In C++: pMalloc->Free(pidl); pMalloc->Release();
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}	
}


*/