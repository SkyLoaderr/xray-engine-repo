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
			void SortItems													(HTREEITEM itm);
			void moveItem													(BOOL b);
	virtual void DoDataExchange												(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate											(); // called first time after construct

// Implementation

//dragging
	CImageList		m_images;
	CImageList*		m_pDragImage;	//содержит список изображений используемый  во  время переноса
	BOOL			m_bLDragging;
	HTREEITEM		m_hitemDrag,m_hitemDrop;

public:
	virtual ~CxrUpdateView();

protected:
	HTREEITEM				m_root;
	CTreeCtrl				m_tree_ctrl;
	CButton					m_fake_btn;
	CString					m_task_name;
	CEdit					m_task_name_edt;
	CString					m_task_proirity;
	CString					m_task_type_static;


	CTask*					m_active_task;
	BOOL					b_initialized;
	CWnd*					m_cur_prop_wnd;
	CCopyFilesTaskPropDlg*	m_copy_files_dlg;
	CCopyFolderDlgProp*		m_copy_folder_dlg;
	CExecAppTaskDlgProp*	m_exec_process_dlg;
	CBatchTaskDlgProp*		m_batch_process_dlg;

	void					CreateRoot											();
	HTREEITEM				FillTaskTree										(CTask*, HTREEITEM parent=NULL);
	BOOL					ShowPropDlg											(CTask*);
	void					CheckChildren										(HTREEITEM itm, BOOL b);
	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
protected:
	virtual void			OnUpdate											(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void			OnAddRootTask										();
	afx_msg void			OnAddCopyFilesTask									();
	afx_msg void			OnAddCopyFolderTask									();
	afx_msg void			OnAddExecuteTask									();
	afx_msg void			OnAddBatchExecuteTask								();
	void					TryAddNewTask										(int t);

	HTREEITEM				CopyBranch											( HTREEITEM htiBranch, HTREEITEM htiNewParent, 
																					HTREEITEM htiAfter = TVI_LAST );
	HTREEITEM				CopyItem											( HTREEITEM hItem, HTREEITEM htiNewParent, 
																				HTREEITEM htiAfter = TVI_LAST );

public:
	afx_msg void			OnBnClickedButtonAdd								();
	afx_msg void			OnTvnSelchangedTree1								(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void			OnBnClickedBtnRun									();
	afx_msg void			OnEnChangeEditTaskName								();
	afx_msg void			OnNMClickTree1										(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void			OnBnClickedButtonRemove								();
	afx_msg void			OnBnClickedButtonCopyTask							();
	afx_msg void			OnBnClickedButtonMoveUp								();
	afx_msg void			OnBnClickedButtonMoveDown							();
	afx_msg void			OnBnClickedUpdBtn									();

	LRESULT					OnTvCheckbox										(WPARAM wp, LPARAM lp);
	LRESULT					OnAddLogMsg											(WPARAM wp, LPARAM lp);
	void					updateCheckedItems									();
	void					RedrawChecks										(CTask* t);
	void					SetCheckItem										(HTREEITEM itm, BOOL b);
	afx_msg void OnTvnBegindragTree1(NMHDR *pNMHDR, LRESULT *pResult);
	virtual void OnMouseMove(UINT nFlags, CPoint point);
	virtual void OnLButtonUp(UINT nFlags, CPoint point);

};

inline CxrUpdateDoc* CxrUpdateView::GetDocument() const
   { return reinterpret_cast<CxrUpdateDoc*>(m_pDocument); }
extern void updateTaskTree();
extern void RunTask();
extern CxrUpdateView* g_view;