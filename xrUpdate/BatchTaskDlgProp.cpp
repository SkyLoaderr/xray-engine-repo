// BatchTaskDlgProp.cpp : implementation file
//

#include "stdafx.h"
#include "xrUpdate.h"
#include "BatchTaskDlgProp.h"
#include "upd_task.h"
#include "folderdialog.h"
#include ".\batchtaskdlgprop.h"


// CBatchTaskDlgProp dialog

IMPLEMENT_DYNAMIC(CBatchTaskDlgProp, CDialog)
CBatchTaskDlgProp::CBatchTaskDlgProp(LPCTSTR lpszTemplateName, CWnd* pParent)
	: CDialog(lpszTemplateName, pParent)
	, m_params(_T(""))
	, m_wrk_folder(_T(""))
{
}

CBatchTaskDlgProp::~CBatchTaskDlgProp()
{
}

void CBatchTaskDlgProp::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CMD, m_params);
	DDX_Control(pDX, IDC_LIST_FILES_3, m_files_list_box);
	DDX_Text(pDX, IDC_EDIT_WRK_FOLDER, m_wrk_folder);

	if(pDX->m_bSaveAndValidate){//TRUE== controls->members
		m_task->set_params(m_params.GetBuffer());
		m_task->set_wrk_folder(m_wrk_folder.GetBuffer());
	}
}


BEGIN_MESSAGE_MAP(CBatchTaskDlgProp, CDialog)
	ON_EN_CHANGE(IDC_EDIT_CMD, OnEnChangeEditCmd)
	ON_BN_CLICKED(IDC_BTN_ADD_FILE_3, OnBnClickedBtnAddFile3)
	ON_BN_CLICKED(IDC_BTN_REMOVE_FILE_3, OnBnClickedBtnRemoveFile3)
	ON_BN_CLICKED(IDC_BTN_REMOVE_ALL_FILES_3, OnBnClickedBtnRemoveAllFiles3)
	ON_BN_CLICKED(IDC_BTN_SEL_WRK_FOLDER_5, OnBnClickedBtnSelWrkFolder)
	ON_EN_CHANGE(IDC_EDIT_WRK_FOLDER, OnEnChangeEditWrkFolder)
END_MESSAGE_MAP()


void CBatchTaskDlgProp::init_(CTask* t)
{
	m_task = dynamic_cast<CTaskBatchExecute*>(t);
	m_params		= m_task->get_params();
	m_wrk_folder	= m_task->get_wrk_folder();

	UpdateData(FALSE);
	redrawFileList();
}


void CBatchTaskDlgProp::OnEnChangeEditCmd()
{
	UpdateData(TRUE);
}

void CBatchTaskDlgProp::OnBnClickedBtnAddFile3()
{
	CFileDialog fd(TRUE,0,0,OFN_ALLOWMULTISELECT,0,0,0);
	TCHAR FileNameList[8192]= {0};
	fd.m_ofn.lpstrFile = FileNameList;
	fd.m_ofn.nMaxFile=8192;

	if (fd.DoModal()==IDOK){
		POSITION pos = fd.GetStartPosition();
		while(pos){
			CString fname = fd.GetNextPathName(pos);
			m_task->file_list()->push_back(fname.GetBuffer());
		}
		redrawFileList();
	}
}

void CBatchTaskDlgProp::OnBnClickedBtnRemoveFile3()
{
	CFileNamesArray new_fl;
	for(int i=0;	i<m_files_list_box.GetCount();++i ){
		if(!m_files_list_box.GetSel(i))
			new_fl.push_back(m_task->file_list()->at(i));
		else
			Msg("Removed file %s index=%d from task %s",m_task->file_list()->at(i).c_str(), i, m_task->name());
	};

	m_task->file_list()->clear();
	*(m_task->file_list()) = new_fl;
	redrawFileList();
}

void CBatchTaskDlgProp::OnBnClickedBtnRemoveAllFiles3()
{
	m_files_list_box.ResetContent();
	m_task->file_list()->clear();
}

void CBatchTaskDlgProp::redrawFileList()
{
	m_files_list_box.ResetContent();
	CFileNamesArray* fl = m_task->file_list();
	CFileNamesArray::iterator it = fl->begin();
	for(;it!=fl->end();++it){
		m_files_list_box.AddString((*it).c_str());
	}
}

void CBatchTaskDlgProp::OnBnClickedBtnSelWrkFolder()
{
	CFolderDialog fd;
	if(fd.DoModal()==IDOK){
		m_wrk_folder = fd.GetPathName();
	}

	UpdateData(FALSE);
	UpdateData(TRUE);
}

void CBatchTaskDlgProp::OnEnChangeEditWrkFolder()
{
	UpdateData(TRUE);
}
