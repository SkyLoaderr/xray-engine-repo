// CopyFolderDlgProp.cpp : implementation file
//

#include "stdafx.h"
#include "xrUpdate.h"
#include "CopyFolderDlgProp.h"
#include "folderdialog.h"
#include "upd_task.h"
#include "pathdialog.h"


// CCopyFolderDlgProp dialog

IMPLEMENT_DYNAMIC(CCopyFolderDlgProp, CDialog)
CCopyFolderDlgProp::CCopyFolderDlgProp(LPCTSTR lpszTemplateName, CWnd* pParent /*=NULL*/)
	: CDialog(lpszTemplateName, pParent)
	, m_src_folder(_T(""))
	, m_target_folder(_T(""))
{
}


CCopyFolderDlgProp::~CCopyFolderDlgProp()
{
}

void CCopyFolderDlgProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SRC_FOLDER_2, m_src_folder);
	DDX_Text(pDX, IDC_EDIT_DST_FOLDER_2, m_target_folder);

	if(pDX->m_bSaveAndValidate){
		m_task->set_source_folder(m_src_folder.GetBuffer());
		m_task->set_target_folder(m_target_folder.GetBuffer());
	}
}


BEGIN_MESSAGE_MAP(CCopyFolderDlgProp, CDialog)
	ON_EN_CHANGE(IDC_EDIT_SRC_FOLDER_2, OnEnChangeEditSrcFolder2)
	ON_EN_CHANGE(IDC_EDIT_DST_FOLDER_2, OnEnChangeEditDstFolder2)
	ON_BN_CLICKED(IDC_BTN_SEL_SRC_2, OnBnClickedBtnSelSrc2)
	ON_BN_CLICKED(IDC_BTN_SEL_DST_2, OnBnClickedBtnSelDst2)
END_MESSAGE_MAP()


void CCopyFolderDlgProp::init_(CTask* t)
{
	m_task = dynamic_cast<CTaskCopyFolder*>(t);
	m_src_folder	= m_task->source_folder();
	m_src_folder.MakeLower();
	m_target_folder	= m_task->target_folder();
	m_target_folder.MakeLower();
	UpdateData(FALSE);
}


void CCopyFolderDlgProp::OnEnChangeEditSrcFolder2()
{
	UpdateData(TRUE);
}

void CCopyFolderDlgProp::OnEnChangeEditDstFolder2()
{
	UpdateData(TRUE);
}

void CCopyFolderDlgProp::OnBnClickedBtnSelSrc2()
{
	CFolderDialog fd;

	if(fd.DoModal()==IDOK){
		m_src_folder = fd.GetPathName();
	}
	UpdateData(FALSE);
	UpdateData(TRUE);
}

void CCopyFolderDlgProp::OnBnClickedBtnSelDst2()
{
	CFolderDialog fd;

	if(fd.DoModal()==IDOK){
		m_target_folder = fd.GetPathName();
	}
	UpdateData(FALSE);
	UpdateData(TRUE);
}
