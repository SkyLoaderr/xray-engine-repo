// CopyTaskPropDlg.cpp : implementation file
//

#include "stdafx.h"
#include "xrUpdate.h"
#include "CopyTaskPropDlg.h"
#include "upd_task.h"
#include "pathdialog.h"
#include "folderdialog.h"

// CCopyTaskPropDlg dialog

IMPLEMENT_DYNAMIC(CCopyFilesTaskPropDlg, CDialog)
CCopyFilesTaskPropDlg::CCopyFilesTaskPropDlg(LPCTSTR lpszTemplateName, CWnd* pParent /*=NULL*/)
	: CDialog(lpszTemplateName, pParent)
	, m_dest_folder(_T(""))
	, m_task(NULL)
{
}

CCopyFilesTaskPropDlg::~CCopyFilesTaskPropDlg()
{
}

void CCopyFilesTaskPropDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILES, m_files_list_box);
	DDX_Text(pDX, IDC_EDIT_DST_FOLDER, m_dest_folder);

	if(pDX->m_bSaveAndValidate){//TRUE== controls->members
		m_task->set_target_folder(m_dest_folder.GetBuffer());
	}
}


BEGIN_MESSAGE_MAP(CCopyFilesTaskPropDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_SEL_DST, OnBnClickedBtnSelDst)
	ON_BN_CLICKED(IDC_BTN_ADD_FILE, OnBnClickedBtnAddFile)
	ON_BN_CLICKED(IDC_BTN_REMOVE_FILE, OnBnClickedBtnRemoveFile)
	ON_BN_CLICKED(IDC_BTN_REMOVE_ALL_FILES, OnBnClickedBtnRemoveAllFiles)
	ON_EN_CHANGE(IDC_EDIT_DST_FOLDER, OnEnChangeEditDstFolder)
END_MESSAGE_MAP()


// CCopyTaskPropDlg message handlers

void CCopyFilesTaskPropDlg::init_(CTask* t)
{
	m_task = dynamic_cast<CTaskCopyFiles*>(t);
	m_dest_folder	= m_task->target_folder();
	m_dest_folder.MakeLower();
	UpdateData(FALSE);
	redrawFileList();
}


void CCopyFilesTaskPropDlg::OnBnClickedBtnSelDst()
{

	CFolderDialog fd;

	if(fd.DoModal()==IDOK){
		m_dest_folder = fd.GetPathName();
	}
	UpdateData(FALSE);
	UpdateData(TRUE);
}

void CCopyFilesTaskPropDlg::OnBnClickedBtnAddFile()
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

void CCopyFilesTaskPropDlg::OnBnClickedBtnRemoveFile()
{
	CFileNamesArray new_fl;
	int c = m_task->file_list()->size();
	for(int i=0;	i<m_files_list_box.GetCount();++i ){
		if(!m_files_list_box.GetSel(i))
			new_fl.push_back(m_task->file_list()->at(i));
		else
			Msg("Removed file %s index=%d from task %s",*m_task->file_list()->at(i), i, m_task->name());
	};
	c = m_task->file_list()->size();
	m_task->file_list()->clear();
	c = m_task->file_list()->size();
	*(m_task->file_list()) = new_fl;
	c = m_task->file_list()->size();
	redrawFileList();
}

void CCopyFilesTaskPropDlg::OnBnClickedBtnRemoveAllFiles()
{
	m_files_list_box.ResetContent();
	m_task->file_list()->clear();
}

void CCopyFilesTaskPropDlg::redrawFileList()
{
	m_files_list_box.ResetContent();
	CFileNamesArray* fl = m_task->file_list();
	int c = m_task->file_list()->size();
	CFileNamesArray::iterator it = fl->begin();
	for(;it!=fl->end();++it){
		m_files_list_box.AddString(*(*it));
	}
	int cc = m_files_list_box.GetCount();

}

void CCopyFilesTaskPropDlg::OnEnChangeEditDstFolder()
{
	UpdateData(TRUE);
}

