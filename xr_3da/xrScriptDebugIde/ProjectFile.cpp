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
//		CString fName = GetName(GetName());
		CString fName = GetName();
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
	m_pluaview = NULL;
	Change_status(vss_unknown);

	RemoveAllDebugLines();
	RemoveAllBreakPoints();
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

CString CProjectFile::GetName()
{
	return GetFileName(m_strPathName);
}

CString CProjectFile::GetFileName(CString& str )
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath( str, drive, dir, fname, ext );
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
	pEditor->ClearAllBreakpoints();

	POSITION pos = m_breakPoints.GetStartPosition();
	int nLine, nTemp;
	while (pos != NULL)
	{
		m_breakPoints.GetNextAssoc( pos, nLine, nTemp );
		pEditor->SetBreakpoint(nLine);
	}
}
/*
BOOL CProjectFile::HasFile(CString strPathName)
{
 	if(!GetName(m_strPathName).CompareNoCase(GetName(strPathName)))
 		return TRUE;
 
	return FALSE;
}*/

BOOL CProjectFile::Is(CString& strPathName)
{
 	if(!GetName().CompareNoCase(GetFileName(strPathName)))
 		return TRUE;
 
	return FALSE;
}

BOOL CProjectFile::Load(CArchive &ar)
{
	RemoveAllDebugLines();
	RemoveAllBreakPoints();

	ar >> m_strPathName;

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
	UpdateSS_status();
	return TRUE;
}

BOOL CProjectFile::Save(CArchive &ar)
{
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

bool CProjectFile::SaveFile()
{
	if(GetLuaView())
		return GetLuaView()->_save();

	return true;
}

void CProjectFile::ReloadFile()
{
	if(!GetLuaView())
		return;

	CFile fin;
	if ( fin.Open(GetPathName(), CFile::modeRead) )
		GetLuaView()->GetEditor()->Load(&fin);
}

void CProjectFile::Check_view ()
{
	if(GetLuaView())
		GetLuaView()->GetEditor()->SetReadOnly(m_ssStatus==vss_not_checked_out);
}

void CProjectFile::Change_status(EVSSStatus st)
{
	m_ssStatus = st;
	Check_view();
}


EVSSStatus CProjectFile::GetSS_status	()
{
	UpdateSS_status();
	return m_ssStatus;
}

void CProjectFile::UpdateSS_status()
{
	if(!theApp.m_ssConnection.b_IsConnected()){
		Change_status(vss_unknown);
		return;
	}

	CString str = GetNameExt();

	IVSSItemPtr vssItem;
	CString str_;
	str_.Format("%s%s",getWorkingFolder(),str);
	CComBSTR file_name = str_;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);
	if(vssItem==NULL){
		Change_status(vss_no_ss);
		return;
	}

	long stat;
	HRESULT hr;
	CString str_temp;
	if((hr = vssItem->get_IsCheckedOut(&stat)) == S_OK){
		if(stat == VSSFILE_NOTCHECKEDOUT){
			Change_status(vss_not_checked_out);
		}else if(stat == VSSFILE_CHECKEDOUT){
			Change_status(vss_checked_out);
		}else if(stat == VSSFILE_CHECKEDOUT_ME){
			Change_status(vss_checked_out_me);
		}
	}
}
void CProjectFile::SS_check_in	()
{
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str = GetNameExt();

	IVSSItemPtr vssItem;
	CComBSTR file_name = getWorkingFolder()+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_comment("no comment");
	CComBSTR bstr_localSpec;
	if(!SaveFile() )
		return;

	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->Checkin(bstr_comment, bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		UpdateSS_status();
	}
}
void CProjectFile::SS_check_out	()
{
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str = GetNameExt();

	IVSSItemPtr vssItem;
	CComBSTR file_name = getWorkingFolder()+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_comment("no comment");
	CComBSTR bstr_localSpec;

	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->Checkout(bstr_comment, bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		ReloadFile();
		UpdateSS_status();
	}

}
void CProjectFile::SS_undo_check_out ()
{
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str = GetNameExt();

	IVSSItemPtr vssItem;
	CComBSTR file_name = getWorkingFolder()+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_localSpec;
	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->UndoCheckout(bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		ReloadFile();
		UpdateSS_status();
	}

}
void CProjectFile::SS_difference ()
{
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str = GetNameExt();

	IVSSItemPtr vssItem;
	CComBSTR file_name = getWorkingFolder()+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_localSpec;
	vssItem->get_LocalSpec(&bstr_localSpec);
	VARIANT_BOOL res;
	vssItem->get_IsDifferent(bstr_localSpec, &res);
	if (b_DisplayAnyError())
		return;

	if(res==0){
		AfxMessageBox("The files are identical");
		return;
	}else{
		CComBSTR bstr_localSpec_ss = bstr_localSpec;//+".ssver";
		bstr_localSpec_ss.Append(".ssver");
		vssItem->Get(&bstr_localSpec_ss,0);
		if (b_DisplayAnyError())
			return;

		CString f1,f2;
		f1 = CW2A(bstr_localSpec);
		f2 = CW2A(bstr_localSpec_ss);
		CFileStatus fs;
		CFile::GetStatus(f2,fs);
		fs.m_attribute &= ~(CFile::readOnly);
		CFile::SetStatus(f2,fs);
		CString cmd = ((CIdeApp*)AfxGetApp())->m_comparerCmd;
		if(cmd.GetLength()==0){
			AfxMessageBox("File comparer executable not defined");
			return;
		}

		CString args;
		args.Format(((CIdeApp*)AfxGetApp())->m_comparerFormat, f1, f2);
		spawnl(_P_NOWAIT, cmd.GetBuffer(), args.GetBuffer());
	}

}
void CProjectFile::SS_get_latest ()
{
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str = GetNameExt();

	IVSSItemPtr vssItem;
	CComBSTR file_name = getWorkingFolder()+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);

	CComBSTR bstr_localSpec;
	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->Get(&bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		ReloadFile();
		UpdateSS_status();
	}

}
const CString&	CProjectFile::getWorkingFolder()
{
	return g_mainFrame->GetProject()->m_ss_working_folder;
}
