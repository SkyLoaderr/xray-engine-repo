// xrUpdateView.cpp : implementation of the CxrUpdateView class
//

#include "stdafx.h"
#include "xrUpdate.h"

#include "xrUpdateDoc.h"
#include "xrUpdateView.h"
#include "upd_task.h"
#include ".\xrupdateview.h"
#include "CopyTaskPropDlg.h"
#include "CopyFolderDlgProp.h"
#include "SectionQueryDlg.h"
#include "ExecAppTaskDlgProp.h"
#include "BatchTaskDlgProp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CTreeCtrl* g_tree_ctrl = NULL;

void updateTreeItemName(HTREEITEM itm,CTask* t)
{
	if(!g_tree_ctrl)
		return;

	g_tree_ctrl->SetItemText( itm, t->name() );
}
// CxrUpdateView

IMPLEMENT_DYNCREATE(CxrUpdateView, CFormView)

BEGIN_MESSAGE_MAP(CxrUpdateView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButtonAdd)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BTN_RUN, OnBnClickedBtnRun)


	ON_COMMAND(ID_COPYFILESTASK_S,  OnAddCopyFilesTask)
	ON_COMMAND(ID_COPYFILESTASK_COPYFOLDERTASK,  OnAddCopyFolderTask)
	ON_COMMAND(ID_COPYFILESTASK_EXECUTEPROCESS,  OnAddExecuteTask)
	ON_COMMAND(ID_COPYFILESTASK_BATCHEXECUTE,  OnAddBatchExecuteTask)


	ON_EN_CHANGE(IDC_EDIT_TASK_NAME, OnEnChangeEditTaskName)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnNMClickTree1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButtonCopyTask)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButtonMoveUp)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButtonMoveDown)
END_MESSAGE_MAP()

// CxrUpdateView construction/destruction

CxrUpdateView::CxrUpdateView()
	: CFormView(CxrUpdateView::IDD)
	, m_task_name(_T(""))
	, m_task_proirity(_T(""))
	, m_task_type_static(_T(""))
{
	m_active_task = NULL;
	b_initialized = FALSE;
	// TODO: add construction code here
	m_root = NULL;
	m_cur_prop_wnd = NULL;
	m_copy_files_dlg = xr_new<CCopyFilesTaskPropDlg>(MAKEINTRESOURCE(IDD_COPY_TASK_DLG),this);
	m_copy_folder_dlg = xr_new<CCopyFolderDlgProp>(MAKEINTRESOURCE(IDD_DIALOG_CPY_FOLDER),this);
	m_exec_process_dlg = xr_new<CExecAppTaskDlgProp>(MAKEINTRESOURCE(IDD_DIALOG_EXEC_PROCESS),this);
	m_batch_process_dlg = xr_new<CBatchTaskDlgProp>(MAKEINTRESOURCE(IDD_BATCH_TASK),this);
}
CxrUpdateView::~CxrUpdateView()
{
	g_tree_ctrl = NULL;
}

void CxrUpdateView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree_ctrl);
	DDX_Control(pDX, IDC_BUTTON2, m_fake_btn);
	DDX_Text(pDX, IDC_EDIT_TASK_NAME, m_task_name);
	DDX_Control(pDX, IDC_EDIT_TASK_NAME, m_task_name_edt);
	DDX_Text(pDX, IDC_STATIC_TASK_PRIORITY, m_task_proirity);
	DDX_Text(pDX, IDC_STATIC_TASK_TYPE2, m_task_type_static);
}

BOOL CxrUpdateView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs


	return CFormView::PreCreateWindow(cs);
}

void CxrUpdateView::CreateRoot()
{
	TV_INSERTSTRUCT itm;
	itm.hParent = NULL;
	itm.hInsertAfter = TVI_LAST;
	itm.item.pszText = "root";
	itm.item.lParam = NULL;
	itm.item.mask = /*TVIF_IMAGE | TVIF_SELECTEDIMAGE |*/ TVIF_TEXT;
	m_root = m_tree_ctrl.InsertItem(&itm);
}

void CxrUpdateView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	if(!b_initialized){
		m_copy_files_dlg->Create(MAKEINTRESOURCE(IDD_COPY_TASK_DLG),this);
		m_copy_folder_dlg->Create(MAKEINTRESOURCE(IDD_DIALOG_CPY_FOLDER),this);
		m_exec_process_dlg->Create(MAKEINTRESOURCE(IDD_DIALOG_EXEC_PROCESS),this);
		m_batch_process_dlg->Create(MAKEINTRESOURCE(IDD_BATCH_TASK),this);

		m_task_name_edt.EnableWindow(FALSE);
	}
	g_tree_ctrl = &m_tree_ctrl;
	b_initialized = TRUE;
}

// CxrUpdateView diagnostics


void deleteItemRecurs(CTreeCtrl* tree, HTREEITEM itm, BOOL del_this)
{
	while(HTREEITEM i = tree->GetNextItem(itm,TVGN_CHILD)){
		deleteItemRecurs(tree,i,TRUE);
	}
	if(del_this)
		tree->DeleteItem(itm);
}

void CxrUpdateView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CTask* t = GetDocument()->m_task;
	if(!t)
		return;
/*
	if(!m_root)
		CreateRoot();
*/

/*
	while(HTREEITEM i = m_tree_ctrl.GetChildItem(m_root))
		m_tree_ctrl.DeleteItem(i);
*/

//	FillTaskTree(t,m_root);

//	m_tree_ctrl.DeleteAllItems();

//	deleteItemRecurs(&m_tree_ctrl, TVI_ROOT, FALSE);


    m_tree_ctrl.SetRedraw(FALSE);
	if (m_tree_ctrl.GetCount() > 0)
		 if (!m_tree_ctrl.DeleteAllItems())
                        {
                            m_tree_ctrl.SetRedraw(TRUE); // for safety
                        }


	FillTaskTree(t,TVI_ROOT);

	m_tree_ctrl.SetRedraw(TRUE);
//	GetDocument()->SetModifiedFlag(TRUE);
}

//8-067-441-52-07 саша гулова
HTREEITEM CxrUpdateView::FillTaskTree(CTask* t, HTREEITEM parent)
{
    m_tree_ctrl.SetRedraw(FALSE);
	TV_INSERTSTRUCT itm;
	itm.hParent = parent;
	itm.hInsertAfter = TVI_LAST;
	itm.item.iImage = 1;
	itm.item.iSelectedImage = 1;
	string128 name;
	strcpy(name,t->name());
	itm.item.pszText = name;
	itm.item.lParam = (LPARAM)t;
	itm.item.mask = /*TVIF_IMAGE | TVIF_SELECTEDIMAGE |*/ TVIF_TEXT;
	HTREEITEM tree_itm = m_tree_ctrl.InsertItem(&itm);
	m_tree_ctrl.SetItemData(tree_itm,(LPARAM)t);
	t->m_tree_itm = tree_itm;
	m_tree_ctrl.SetCheck(tree_itm, t->is_enabled());
	BOOL b = m_tree_ctrl.GetCheck(tree_itm);
	for(u32 idx=0; idx<t->sub_task_count();++idx){
		FillTaskTree(t->get_sub_task(idx),tree_itm);
	}

	SortItems(tree_itm);
	return tree_itm;

	m_tree_ctrl.SetRedraw(TRUE);
}



void CxrUpdateView::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CTask* t = (CTask*)m_tree_ctrl.GetItemData( pNMTreeView->itemNew.hItem );
	if(t){
		m_active_task = t;
		ShowPropDlg(t);
	}else{
		m_active_task = NULL;
		if(m_cur_prop_wnd)
			m_cur_prop_wnd->ShowWindow(SW_HIDE);
	}

	*pResult = 0;
}

void CxrUpdateView::ShowPropDlg(CTask* t)
{
	m_task_name_edt.EnableWindow(TRUE);
	m_task_name=t->name();
	m_task_type_static = *typeToStr(t->type());
	m_task_proirity.Format("Task proirity: %d",t->m_priority);
	UpdateData(FALSE);

	RECT r_place;
	RECT r_btn;
	RECT r_view;
	m_fake_btn.GetWindowRect(&r_btn);
	GetWindowRect(&r_view);

	r_place.left	= r_btn.left - r_view.left;
	r_place.right	= r_place.left+(r_btn.right-r_btn.left);
	r_place.top		= 100;
	r_place.bottom	= r_view.bottom-r_view.top-10;

	if(t->type()==eTaskCopyFiles){
		m_copy_files_dlg->init_(t);
		if(m_cur_prop_wnd!=m_copy_files_dlg){
			if(m_cur_prop_wnd)
				m_cur_prop_wnd->ShowWindow(SW_HIDE);

			m_copy_files_dlg->SetParent( this );
			m_copy_files_dlg->ShowWindow(SW_SHOW);
			m_copy_files_dlg->MoveWindow(&r_place);
			m_cur_prop_wnd = m_copy_files_dlg;
		}
	}

	if(t->type()==eTaskCopyFolder){
		m_copy_folder_dlg->init_(t);
		if(m_cur_prop_wnd!=m_copy_folder_dlg){
			if(m_cur_prop_wnd)
				m_cur_prop_wnd->ShowWindow(SW_HIDE);

			m_copy_folder_dlg->SetParent( this );
			m_copy_folder_dlg->ShowWindow(SW_SHOW);
			m_copy_folder_dlg->MoveWindow(&r_place);
			m_cur_prop_wnd = m_copy_folder_dlg;
		}
	}

	if(t->type()==eTaskRunExecutable){
		m_exec_process_dlg->init_(t);
		if(m_cur_prop_wnd!=m_exec_process_dlg){
			if(m_cur_prop_wnd)
				m_cur_prop_wnd->ShowWindow(SW_HIDE);

			m_exec_process_dlg->SetParent( this );
			m_exec_process_dlg->ShowWindow(SW_SHOW);
			m_exec_process_dlg->MoveWindow(&r_place);
			m_cur_prop_wnd = m_exec_process_dlg;
		}
	}

	if(t->type()==eTaskBatchExecute){
		m_batch_process_dlg->init_(t);
		if(m_cur_prop_wnd!=m_batch_process_dlg){
			if(m_cur_prop_wnd)
				m_cur_prop_wnd->ShowWindow(SW_HIDE);

			m_batch_process_dlg->SetParent( this );
			m_batch_process_dlg->ShowWindow(SW_SHOW);
			m_batch_process_dlg->MoveWindow(&r_place);
			m_cur_prop_wnd = m_batch_process_dlg;
		}
	}


}

void CxrUpdateView::OnBnClickedBtnRun()
{
	AfxGetApp()->DoWaitCursor(1); // 0 => restore, 1=> begin, -1=> end

	CTask* t = GetDocument()->m_task;
	t->run();

	AfxGetApp()->DoWaitCursor(-1); // 0 => restore, 1=> begin, -1=> end
}

void CxrUpdateView::OnBnClickedButtonAdd()
{
	HMENU hMenu = NULL;
		hMenu = LoadMenu(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDR_MENU_ADD_TASK));

		if ( !hMenu )
			return;

		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (!hSubMenu) 
		{
			DestroyMenu(hMenu);
			return;
		}

		POINT mouse;
		GetCursorPos(&mouse);
		::SetForegroundWindow(m_hWnd);	
		::TrackPopupMenu(hSubMenu, 0, mouse.x, mouse.y, 0, m_hWnd, NULL);

		DestroyMenu(hMenu);
}

void CxrUpdateView::OnAddCopyFilesTask()
{
	TryAddNewTask(eTaskCopyFiles);
}

void CxrUpdateView::OnAddCopyFolderTask()
{
	TryAddNewTask(eTaskCopyFolder);
}

void CxrUpdateView::OnAddExecuteTask()
{
	TryAddNewTask(eTaskRunExecutable);
}
void CxrUpdateView::OnAddBatchExecuteTask()
{
	TryAddNewTask(eTaskBatchExecute);
}

void CxrUpdateView::TryAddNewTask(int t)
{
	HTREEITEM hItem = m_tree_ctrl.GetSelectedItem();
	CTask* t_parent = (CTask*)m_tree_ctrl.GetItemData(hItem);
	if(!t_parent)
		return;

	string128 new_name;
	strconcat(new_name, "new_",*typeToStr((ETaskType)t) );

	CTask* task_new = CTaskFacrory::create_task( (ETaskType)t );

	task_new->set_name(new_name);
	t_parent->add_sub_task(task_new);
	HTREEITEM itm = FillTaskTree(task_new, hItem);
	m_tree_ctrl.SelectItem(itm);
}

void CxrUpdateView::OnEnChangeEditTaskName()
{
	UpdateData(TRUE);
	m_active_task->set_name(m_task_name.GetBuffer());
}


void CxrUpdateView::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	CPoint pt;
	GetCursorPos(&pt);
	m_tree_ctrl.ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM itm = m_tree_ctrl.HitTest(pt, &nFlags);
	if(!itm)
		return;
	CTask* t = (CTask*)m_tree_ctrl.GetItemData( itm );
	
	if(!t)
		return;

//    MSG         msg_;
	
/*	while(PeekMessage( &msg_, m_tree_ctrl.GetSafeHwnd(), 0U, 0U, PM_REMOVE )){
              TranslateMessage	( &msg_ );
              DispatchMessage	( &msg_ );
		}
*/
	BOOL b = m_tree_ctrl.GetCheck(itm);
	*pResult = 0;

/*
void CTreeappView::OnLButtonDown(UINT nFlags, CPoint point) 
{
     CTreeView::OnLButtonDown(nFlags, point);

    HTREEITEM hItem = GetTreeCtrl().HitTest(point);

    if (!hItem)
        return;

    bool bCheck = GetTreeCtrl().GetCheck(hItem) != 0;
}
*/

}

void CxrUpdateView::CheckChildren(HTREEITEM itm, BOOL b)
{
	m_tree_ctrl.SetCheck(itm,b);//self
	CTask* t = (CTask*)m_tree_ctrl.GetItemData( itm );
	if(t)
		t->set_enabled(b);
	
	HTREEITEM itm_ch;
		itm_ch = m_tree_ctrl.GetChildItem(itm);
		while(itm_ch){
			CheckChildren(itm_ch,b);
			itm_ch = m_tree_ctrl.GetNextSiblingItem(itm_ch);
		}
}


void CxrUpdateView::OnBnClickedButtonRemove()
{
	HTREEITEM itm = m_tree_ctrl.GetSelectedItem();
	if(!itm)
		return;
	CTask* t = (CTask*)m_tree_ctrl.GetItemData(itm);
	if(!t)
		return;
	CString msg;
	msg.Format("Remove task %s ?",t->name());
	if(AfxMessageBox(msg,MB_YESNO)!=IDYES)
		return;

	CTask* parent = t->parent();
	if(parent)
		parent->remove_sub_task(t);
	xr_delete(t);
	m_tree_ctrl.DeleteItem(itm);
}

void CxrUpdateView::OnBnClickedButtonCopyTask()
{
	HTREEITEM itm = m_tree_ctrl.GetSelectedItem();
	if(!itm)
		return;

	HTREEITEM itm_parent = m_tree_ctrl.GetParentItem(itm);
	if(!itm_parent)
		return;

	CTask* t = (CTask*)m_tree_ctrl.GetItemData(itm);
	if(!t)
		return;

	if( !t->parent() )
		return;

	CTask* t_new = t->copy();
	t->parent()->add_sub_task(t_new);
	FillTaskTree(t_new,itm_parent);
}



void CxrUpdateView::OnBnClickedButtonMoveUp()
{
	moveItem(TRUE);
}

void CxrUpdateView::OnBnClickedButtonMoveDown()
{
	moveItem(FALSE);
}

void CxrUpdateView::moveItem(BOOL b) //true==Up
{
	HTREEITEM itm = m_tree_ctrl.GetSelectedItem();
	if(!itm)
		return;

	HTREEITEM itm_parent = m_tree_ctrl.GetParentItem(itm);
	if(!itm_parent)
		return;

	CTask* t = (CTask*)m_tree_ctrl.GetItemData(itm);
	if(!t)
		return;

	if( !t->parent() )
		return;
	if(b)//up
	{
		if(t->m_priority>0)
			t->m_priority--; //up
	}else
		t->m_priority++; //down

	t->parent()->sort_sub_tasks();
	SortItems(t->parent()->m_tree_itm);
	ShowPropDlg(t);
}

static int CALLBACK 
MyCompareProc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
   CTask*    t1 = (CTask*)lParam1;
   CTask*    t2 = (CTask*)lParam2;

   if(t1->m_priority == t2->m_priority)
	   return 0;
   if(t1->m_priority < t2->m_priority)
	   return -1;
   if(t1->m_priority > t2->m_priority)
	   return 1;
   
   return 0;
}

void CxrUpdateView::SortItems(HTREEITEM itm)
{
	TVSORTCB tvs;
	CTask* t = GetDocument()->m_task;
	if(!t)
		return;
//   tvs.hParent = t->m_tree_itm;
	tvs.hParent = itm;
//   tvs.hParent = TVI_ROOT;
   tvs.lpfnCompare = MyCompareProc;
   tvs.lParam = (LPARAM)&m_tree_ctrl;

	m_tree_ctrl.SortChildrenCB(&tvs);
}