#pragma once
#include "afxwin.h"


// CBatchTaskDlgProp dialog
class CTask;
class CTaskBatchExecute;

class CBatchTaskDlgProp : public CDialog
{
	DECLARE_DYNAMIC(CBatchTaskDlgProp)

public:
	CBatchTaskDlgProp(LPCTSTR lpszTemplateName, CWnd* pParent = NULL);   // standard constructor
	virtual ~CBatchTaskDlgProp();

// Dialog Data
	enum { IDD = IDD_BATCH_TASK };

protected:
	CTaskBatchExecute*			m_task;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void redrawFileList();

	DECLARE_MESSAGE_MAP()
public:
	void	init_(CTask*);
	afx_msg void OnEnChangeEditCmd();
	CString m_params;
	afx_msg void OnBnClickedBtnAddFile3();
	afx_msg void OnBnClickedBtnRemoveFile3();
	afx_msg void OnBnClickedBtnRemoveAllFiles3();
	CListBox m_files_list_box;
	afx_msg void OnBnClickedBtnSelWrkFolder();
	CString m_wrk_folder;
	afx_msg void OnEnChangeEditWrkFolder();
};
