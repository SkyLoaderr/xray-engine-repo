// ProjectFile.cpp: implementation of the CProjectFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ide2.h"
#include "ProjectFile.h"

#include "LuaEditor.h"
#include "MainFrame.h"
#include "ScintillaView.h"
#include "LuaView.h"

//#include "../xrGame/mslotutils.h"
#include "/Projects/xray/xrGame/mslotutils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void CProjectFile::FillBreakPoints(CMailSlotMsg* msg)
{
	if( m_breakPoints.GetSize() ){
		CString fName = GetName(GetName());
		msg->w_string(fName.GetBuffer());
		msg->w_int(m_breakPoints.GetSize());

		POSITION pos = m_breakPoints.GetStartPosition();
		int nLine, nTemp;
		while (pos != NULL)
		{
			m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
			msg->w_int(nLine);
		}
	}

}

CProjectFile::CProjectFile()
{
	RemoveAllDebugLines();
	RemoveAllBreakPoints();
	m_bBreakPointsSaved = FALSE;
	SetLuaView(NULL);
}

CProjectFile::~CProjectFile()
{
	if( GetLuaView() )
		GetLuaView()->SetProjectFile(NULL);
}

void CProjectFile::RemoveAllDebugLines()
{
	m_nMinDebugLine = 2147483647;
	m_nMaxDebugLine = 0;

	m_debugLines.RemoveAll();
}

void CProjectFile::AddDebugLine(int nLine)
{
	m_debugLines[nLine] = 1;
	if ( nLine<m_nMinDebugLine )
		m_nMinDebugLine = nLine;
	if ( nLine>m_nMaxDebugLine )
		m_nMaxDebugLine = nLine;
}

void CProjectFile::RemoveAllBreakPoints()
{
	m_nMinBreakPoint = 2147483647;
	m_nMaxBreakPoint = 0;

	m_breakPoints.RemoveAll();

	CProject* pProject = g_mainFrame->GetProject();
	pProject->SetModifiedFlag(TRUE);
}

void CProjectFile::AddBreakPoint(int nLine)
{
	m_breakPoints[nLine] = 1;
	if ( nLine<m_nMinBreakPoint)
		m_nMinBreakPoint = nLine;
	if ( nLine>m_nMaxBreakPoint )
		m_nMaxBreakPoint = nLine;

	CProject* pProject = g_mainFrame->GetProject();
	pProject->SetModifiedFlag(TRUE);
}

void CProjectFile::RemoveBreakPoint(int nLine)
{
	m_breakPoints.RemoveKey(nLine);

	m_nMinBreakPoint = 2147483647;
	m_nMaxBreakPoint = 0;

	POSITION pos = m_breakPoints.GetStartPosition();
	int nTemp;
	while (pos != NULL)
	{
		m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
		if ( nLine<m_nMinBreakPoint)
			m_nMinBreakPoint = nLine;
		if ( nLine>m_nMaxBreakPoint )
			m_nMaxBreakPoint = nLine;
	}

	CProject* pProject = g_mainFrame->GetProject();
	pProject->SetModifiedFlag(TRUE);
}

int CProjectFile::GetNextDebugLine(int nLine)
{
	int nTemp;
	++nLine;

	while ( nLine<=m_nMaxDebugLine )
		if ( m_debugLines.Lookup(nLine, nTemp) )
			return nLine;
		else
			++nLine;

	return 0;
}

int CProjectFile::GetPreviousDebugLine(int nLine)
{
	int nTemp;
	--nLine;

	while ( nLine>=m_nMinDebugLine )
		if ( m_debugLines.Lookup(nLine, nTemp) )
			return nLine;
		else
			--nLine;

	return 0;
}

int CProjectFile::GetNearestDebugLine(int nLine)
{
	int nTemp, nNearest;
	if ( m_debugLines.Lookup(nLine, nTemp) )
		return nLine;

	if ( (nNearest=GetNextDebugLine(nLine)) > 0 )
		return nNearest;

	if ( (nNearest=GetPreviousDebugLine(nLine)) > 0 )
		return nNearest;

	return 0;
}

BOOL CProjectFile::PositionBreakPoints()
{
/*	if ( !CLuaHelper::LoadDebugLines(this) )
		return FALSE;

	BOOL bModified = FALSE;
	POSITION pos = m_breakPoints.GetStartPosition();
	int nLine, nTemp, nNearest;
	while (pos != NULL)
	{
		m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
		nNearest = GetNearestDebugLine(nLine);
		if ( nNearest == 0 )
		{
			m_breakPoints.RemoveKey(nLine);
			bModified = TRUE;
		}
		else if ( nLine != nNearest )
		{
			m_breakPoints.RemoveKey(nLine);
			m_breakPoints.SetAt(nNearest, 1);
			bModified = TRUE;
		}
	}

	return bModified;
*/
	return FALSE;
}
CString	CProjectFile::GetName(CString& str)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( str, drive, dir, fname, ext );
	return CString(fname);
}

CString CProjectFile::GetName()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	return CString(fname);
}

CString CProjectFile::GetNameExt()
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( m_strPathName, drive, dir, fname, ext );
	return CString(fname)+ext;
}

BOOL CProjectFile::HasBreakPoint(int nLine)
{
	int nTemp;
	return m_breakPoints.Lookup(nLine, nTemp);
}

void CProjectFile::SetBreakPointsIn(CLuaEditor *pEditor)
{
	m_bBreakPointsSaved = FALSE;
	pEditor->ClearAllBreakpoints();

	POSITION pos = m_breakPoints.GetStartPosition();
	int nLine, nTemp;
	while (pos != NULL)
	{
		m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
		pEditor->SetBreakpoint(nLine);
	}
	m_bBreakPointsSaved = TRUE;
}

BOOL CProjectFile::HasFile(CString strPathName)
{
 	if(!GetName(m_strPathName).CompareNoCase(GetName(strPathName)))
 		return TRUE;
 /*
 	//should actually search using the luasearch path
 	DWORD n=MAX_PATH;
 	CString sFullPath;	
 	::GetFullPathName(strPathName,n,sFullPath.GetBuffer(n),NULL);
 	sFullPath.ReleaseBuffer();
 
 	if(!m_strPathName.CompareNoCase(sFullPath))
 		return TRUE;
 	return FALSE;*/
 	return FALSE;
}


BOOL CProjectFile::Load(CArchive &ar)
{
	RemoveAllDebugLines();
	RemoveAllBreakPoints();

//	ar >> m_strRelPathName;
	ar >> m_strPathName;

/*	CProject* pProject = ((CMainFrame*)AfxGetMainWnd())->GetProject();
	m_strPathName = pProject->GetProjectDir();
	PathAppend(m_strPathName.GetBuffer(MAX_PATH), m_strRelPathName);
*/
	ar >> m_nMinBreakPoint;
	ar >> m_nMaxBreakPoint;

	int nBreakPoints;
	ar >> nBreakPoints;

	for ( int i=0; i<nBreakPoints; ++i )
	{
		int nLine;
		ar >> nLine;

		m_breakPoints[nLine] = 1;
	}

	return TRUE;
}

BOOL CProjectFile::Save(CArchive &ar)
{
//	ar << m_strRelPathName;
	ar << m_strPathName;
	ar << m_nMinBreakPoint;
	ar << m_nMaxBreakPoint;

	int nBreakPoints = m_breakPoints.GetCount();
	ar << nBreakPoints;

	POSITION pos = m_breakPoints.GetStartPosition();
	int nLine, nTemp;
	while (pos != NULL)
	{
		m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
		ar << nLine;
	}

	return TRUE;
}
