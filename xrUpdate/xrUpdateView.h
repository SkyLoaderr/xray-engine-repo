// xrUpdateView.h : interface of the CxrUpdateView class
//


#pragma once
#include "afxcmn.h"
#include "afxwin.h"

class CxrUpdateDoc;
class CCopyFilesTaskPropDlg;
class CCopyFolderDlgProp;
class CExecAppTaskDlgProp;
class CBatchTaskDlgProp;

class CxrUpdateView : public CFormView
{
protected: // create from serialization only
	CxrUpdateView();
	DECLARE_DYNCREATE(CxrUpdateView)

public:
	enum{ IDD = IDD_XRUPDATE_FORM };

// Attributes
public:
	CxrUpdateDoc* GetDocument() const;

// Operations
public:

// Overrides
	public:
virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
			void SortItems(HTREEITEM itm);
			void moveItem(BOOL b);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CxrUpdateView();

protected:
	CTask*					m_active_task;
	BOOL				b_initialized;
	CWnd*				m_cur_prop_wnd;
	CCopyFilesTaskPropDlg*	m_copy_files_dlg;
	CCopyFolderDlgProp*		m_copy_folder_dlg;
	CExecAppTaskDlgProp*	m_exec_process_dlg;
	CBatchTaskDlgProp*		m_batch_process_dlg;

void CreateRoot();
HTREEITEM FillTaskTree(CTask*, HTREEITEM parent=NULL);
BOOL ShowPropDlg(CTask*);
void CheckChildren(HTREEITEM itm, BOOL b);
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnAddCopyFilesTask();
	afx_msg void OnAddCopyFolderTask();
	afx_msg void OnAddExecuteTask();
	afx_msg void OnAddBatchExecuteTask();
	
	void TryAddNewTask(int t);
public:
	HTREEITEM m_root;
	CTreeCtrl m_tree_ctrl;
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_fake_btn;
	afx_msg void OnBnClickedBtnRun();
	afx_msg void OnEnChangeEditTaskName();
	CString m_task_name;
	CEdit m_task_name_edt;
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonRemove();
	afx_msg void OnBnClickedButtonCopyTask();
	afx_msg void OnBnClickedButtonMoveUp();
	afx_msg void OnBnClickedButtonMoveDown();
	CString m_task_proirity;
	CString m_task_type_static;
};

inline CxrUpdateDoc* CxrUpdateView::GetDocument() const
   { return reinterpret_cast<CxrUpdateDoc*>(m_pDocument); }
