#pragma once
#include "afxwin.h"


// CCopyFilesTaskPropDlg dialog
class CTask;
class CTaskCopyFiles;

class CCopyFilesTaskPropDlg : public CDialog
{
	DECLARE_DYNAMIC(CCopyFilesTaskPropDlg)

public:
	CCopyFilesTaskPropDlg(LPCTSTR lpszTemplateName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CCopyFilesTaskPropDlg();

// Dialog Data
	enum { IDD = IDD_COPY_TASK_DLG };

protected:
	CTaskCopyFiles*			m_task;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void		redrawFileList();
	DECLARE_MESSAGE_MAP()
public:
	void	init_(CTask*);
	CListBox m_files_list_box;
	CString m_dest_folder;
	afx_msg void OnBnClickedBtnSelDst();
	afx_msg void OnBnClickedBtnAddFile();
	afx_msg void OnBnClickedBtnRemoveFile();
	afx_msg void OnBnClickedBtnRemoveAllFiles();
	afx_msg void OnEnChangeEditDstFolder();
};
