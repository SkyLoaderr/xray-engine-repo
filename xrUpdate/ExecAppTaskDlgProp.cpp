// ExecAppTaskDlgProp.cpp : implementation file
//

#include "stdafx.h"
#include "xrUpdate.h"
#include "ExecAppTaskDlgProp.h"
#include "upd_task.h"
#include "folderdialog.h"


// CExecAppTaskDlgProp dialog

IMPLEMENT_DYNAMIC(CExecAppTaskDlgProp, CDialog)
CExecAppTaskDlgProp::CExecAppTaskDlgProp(LPCTSTR lpszTemplateName, CWnd* pParent /*=NULL*/)
	: CDialog(lpszTemplateName, pParent)
	, m_app_name(_T(""))
	, m_params(_T(""))
	, m_wrk_folder(_T(""))
{
}

CExecAppTaskDlgProp::~CExecAppTaskDlgProp()
{
}

void CExecAppTaskDlgProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_APP_NAME, m_app_name);
	DDX_Text(pDX, IDC_EDIT_APP_PARAMS, m_params);
	DDX_Text(pDX, IDC_EDIT_WRK_FOLDER, m_wrk_folder);

	if(pDX->m_bSaveAndValidate){
		m_task->set_app_name(m_app_name.GetBuffer());
		m_task->set_params(m_params.GetBuffer());
		m_task->set_wrk_folder(m_wrk_folder.GetBuffer());
	}
}


BEGIN_MESSAGE_MAP(CExecAppTaskDlgProp, CDialog)
	ON_EN_CHANGE(IDC_EDIT_APP_NAME, OnEnChangeEditAppName)
	ON_EN_CHANGE(IDC_EDIT_APP_PARAMS, OnEnChangeEditAppParams)
	ON_EN_CHANGE(IDC_EDIT_WRK_FOLDER, OnEnChangeEditWrkFolder)
	ON_BN_CLICKED(IDC_BTN_SEL_APP, OnBnClickedBtnSelApp)
	ON_BN_CLICKED(IDC_BTN_SEL_WRK_FOLDER, OnBnClickedBtnSelWrkFolder)
END_MESSAGE_MAP()

void CExecAppTaskDlgProp::init_(CTask* t)
{
	m_task = dynamic_cast<CTaskExecute*>(t);

	m_app_name		= m_task->get_app_name();
	m_params		= m_task->get_params();
	m_wrk_folder	= m_task->get_wrk_folder();
	m_app_name.MakeLower();
	m_params.MakeLower();
	m_wrk_folder.MakeLower();

	UpdateData(FALSE);
}

void CExecAppTaskDlgProp::OnEnChangeEditAppName()
{
	UpdateData(TRUE);
}

void CExecAppTaskDlgProp::OnEnChangeEditAppParams()
{
	UpdateData(TRUE);
}

void CExecAppTaskDlgProp::OnEnChangeEditWrkFolder()
{
	UpdateData(TRUE);
}

void CExecAppTaskDlgProp::OnBnClickedBtnSelApp()
{
	CFileDialog fd(TRUE);
	if (fd.DoModal()==IDOK){
		m_app_name = fd.GetPathName();
	}

	UpdateData(FALSE);
	UpdateData(TRUE);
}

void CExecAppTaskDlgProp::OnBnClickedBtnSelWrkFolder()
{
	CFolderDialog fd;
	if(fd.DoModal()==IDOK){
		m_wrk_folder = fd.GetPathName();
	}

	UpdateData(FALSE);
	UpdateData(TRUE);
}
