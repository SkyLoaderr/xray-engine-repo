// xrUpdateView.cpp : implementation of the CxrUpdateView class
//

#include "stdafx.h"
#include "xrUpdate.h"

#include "xrUpdateDoc.h"
#include "xrUpdateView.h"
#include "upd_task.h"
#include "CopyTaskPropDlg.h"
#include "CopyFolderDlgProp.h"
#include "SectionQueryDlg.h"
#include "ExecAppTaskDlgProp.h"
#include "BatchTaskDlgProp.h"
#include ".\xrupdateview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CTreeCtrl* g_tree_ctrl = NULL;
extern CxrUpdateView* g_view  = NULL;

void updateTreeItemName(HTREEITEM itm,CTask* t)
{
	if(!g_tree_ctrl)
		return;

	g_tree_ctrl->SetItemText( itm, t->name() );
}

void updateTaskTree()
{
	if(!g_tree_ctrl||!g_view)
		return;
	
	g_view->updateCheckedItems();
}
void RunTask()
{
	if(!g_tree_ctrl||!g_view)
		return;
	
	g_view->OnBnClickedBtnRun();
}
// CxrUpdateView

#define UWM_TV_CHECKBOX    WM_APP   // the custom message we post to ourself
class CDlgOrWhatever
{
  // ...
  CTreeCtrl    m_tree;
  afx_msg LRESULT OnTvCheckbox(WPARAM wp, LPARAM lp); // our message handler for the posted message
};


IMPLEMENT_DYNCREATE(CxrUpdateView, CFormView)

BEGIN_MESSAGE_MAP(CxrUpdateView, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButtonAdd)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BTN_RUN, OnBnClickedBtnRun)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()

	ON_MESSAGE(UWM_TV_CHECKBOX, OnTvCheckbox)

	ON_COMMAND(ID_COPYFILESTASK_S,  OnAddCopyFilesTask)
	ON_COMMAND(ID_COPYFILESTASK_COPYFOLDERTASK,  OnAddCopyFolderTask)
	ON_COMMAND(ID_COPYFILESTASK_EXECUTEPROCESS,  OnAddExecuteTask)
	ON_COMMAND(ID_COPYFILESTASK_BATCHEXECUTE,  OnAddBatchExecuteTask)
	ON_COMMAND(ID_COPYFILESTASK_ROOT,           OnAddRootTask)
	ON_MESSAGE(ADD_LOG_MSG, OnAddLogMsg)

	ON_EN_CHANGE(IDC_EDIT_TASK_NAME, OnEnChangeEditTaskName)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, OnNMClickTree1)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButtonRemove)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButtonCopyTask)
	ON_BN_CLICKED(IDC_BUTTON5, OnBnClickedButtonMoveUp)
	ON_BN_CLICKED(IDC_BUTTON6, OnBnClickedButtonMoveDown)
	ON_BN_CLICKED(IDC_UPD_BTN, OnBnClickedUpdBtn)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE1, OnTvnBegindragTree1)
END_MESSAGE_MAP()

// CxrUpdateView construction/destruction

LRESULT	 CxrUpdateView::OnAddLogMsg(WPARAM wp, LPARAM lp)
{
	log_cb_fn((LPCSTR)wp );
	return 0;
}

CxrUpdateView::CxrUpdateView()
	: CFormView(CxrUpdateView::IDD)
	, m_task_name(_T(""))
	, m_task_proirity(_T(""))
	, m_task_type_static(_T(""))
{
	m_active_task = NULL;
	b_initialized = FALSE;
	// TODO: add construction code here
	m_root			= NULL;
	m_cur_prop_wnd	= NULL;
	m_bLDragging	= FALSE;
	m_copy_files_dlg = xr_new<CCopyFilesTaskPropDlg>(MAKEINTRESOURCE(IDD_COPY_TASK_DLG),this);
	m_copy_folder_dlg = xr_new<CCopyFolderDlgProp>(MAKEINTRESOURCE(IDD_DIALOG_CPY_FOLDER),this);
	m_exec_process_dlg = xr_new<CExecAppTaskDlgProp>(MAKEINTRESOURCE(IDD_DIALOG_EXEC_PROCESS),this);
	m_batch_process_dlg = xr_new<CBatchTaskDlgProp>(MAKEINTRESOURCE(IDD_BATCH_TASK),this);
	g_view = this;
}
CxrUpdateView::~CxrUpdateView()
{
	g_tree_ctrl = NULL;
	g_view		= NULL;
	
	m_copy_files_dlg->DestroyWindow();
	xr_delete(m_copy_files_dlg);

	m_copy_folder_dlg->DestroyWindow();
	xr_delete(m_copy_folder_dlg);

	m_exec_process_dlg->DestroyWindow();
	xr_delete(m_exec_process_dlg);

	m_batch_process_dlg->DestroyWindow();
	xr_delete(m_batch_process_dlg);
}
int CxrUpdateView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	return 0;
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

		m_images.Create (IDR_MAINFRAME, 16, 1, RGB(0,255,0));
		int cc = m_images.GetImageCount();
		m_tree_ctrl.SetImageList (&m_images, TVSIL_NORMAL);
	}
	g_tree_ctrl = &m_tree_ctrl;
	b_initialized = TRUE;

	g_app_wnd = GetSafeHwnd();
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

    m_tree_ctrl.SetRedraw(FALSE);
	if (m_tree_ctrl.GetCount() > 0)
		 if (!m_tree_ctrl.DeleteAllItems())
                        {
                            m_tree_ctrl.SetRedraw(TRUE); // for safety
                        }


	FillTaskTree(t,TVI_ROOT);

	m_tree_ctrl.SetRedraw(TRUE);

}

//8-067-441-52-07 саша гулова
HTREEITEM CxrUpdateView::FillTaskTree(CTask* t, HTREEITEM parent)
{
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

}



void CxrUpdateView::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CTask* t = (CTask*)m_tree_ctrl.GetItemData( pNMTreeView->itemNew.hItem );
	BOOL r=FALSE;
	if(t){
		m_active_task = t;
		r = ShowPropDlg(t);
	}else
		m_active_task = NULL;

	if(!r){
		if(m_cur_prop_wnd){
			m_cur_prop_wnd->ShowWindow(SW_HIDE);
			m_cur_prop_wnd=NULL;
		}
	}

	*pResult = 0;
}
//067-238-17-17 pasha mob
BOOL CxrUpdateView::ShowPropDlg(CTask* t)
{
	m_task_name_edt.EnableWindow(TRUE);
	m_task_name=t->name();
	m_task_type_static.Format("Task type = %s", *typeToStr(t->type()) );
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
	return TRUE;
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
	return TRUE;
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
	return TRUE;
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
	return TRUE;
	}

	return FALSE;
}

void CxrUpdateView::OnBnClickedBtnRun()
{
	AfxGetApp()->DoWaitCursor(1); // 0 => restore, 1=> begin, -1=> end

	CTask* t = GetDocument()->m_task;
	t->exec();

	AfxGetApp()->DoWaitCursor(-1); // 0 => restore, 1=> begin, -1=> end
	AfxMessageBox("Done");
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

void CxrUpdateView::OnAddRootTask ()
{
	TryAddNewTask(eTaskRoot);
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

	CTask* t_new = t->copy(TRUE);
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
   tvs.hParent = itm;
   tvs.lpfnCompare = MyCompareProc;
   tvs.lParam = (LPARAM)&m_tree_ctrl;

	m_tree_ctrl.SortChildrenCB(&tvs);
}

void CxrUpdateView::OnBnClickedUpdBtn()
{
	HTREEITEM itm = m_tree_ctrl.GetSelectedItem();
	if(!itm)
		return;
	CTask* t = (CTask*)m_tree_ctrl.GetItemData(itm);
	if(!t)
		return;
	BOOL b = m_tree_ctrl.GetCheck(itm);
	CheckChildren(itm,b);
}


LRESULT CxrUpdateView::OnTvCheckbox(WPARAM wp, LPARAM lp)
{
  HTREEITEM hitem = (HTREEITEM) lp;

	CTask* t = (CTask*)m_tree_ctrl.GetItemData(hitem);
	if(!t)
		return 0;

	BOOL b = m_tree_ctrl.GetCheck(hitem);
	CheckChildren(hitem,b);
	m_tree_ctrl.SelectItem(hitem);

  return 0;
}


void CxrUpdateView::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
  DWORD dw = GetMessagePos();                   // retrieve mouse cursor position when msg was sent
  CPoint p(GET_X_LPARAM(dw), GET_Y_LPARAM(dw)); // ..and put into point structure
  m_tree_ctrl.ScreenToClient(&p);                    // make coords local to tree client area

  UINT htFlags = 0;
  HTREEITEM it = m_tree_ctrl.HitTest(p, &htFlags);   // See where the click was on

  if (it != NULL && ( htFlags & TVHT_ONITEMSTATEICON)) {   
    // the check box was hit - we just post a message for later processing
    PostMessage(UWM_TV_CHECKBOX, pNMHDR->idFrom, (LPARAM) it);
  }

}

//////////////////dragding routines
void CxrUpdateView::OnTvnBegindragTree1(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	m_hitemDrag = pNMTreeView->itemNew.hItem;
	m_hitemDrop = NULL;

	m_pDragImage = m_tree_ctrl.CreateDragImage(m_hitemDrag);  // get the image list for dragging
	// CreateDragImage() возвращает  NULL если нет списка изображаний
	// связанного с деревом
	if( !m_pDragImage )
		return;

	m_bLDragging = TRUE;
	m_pDragImage->BeginDrag(0, CPoint(-15,-15));
	POINT pt = pNMTreeView->ptDrag;
	ClientToScreen( &pt );
	m_pDragImage->DragEnter(NULL, pt);
	SetCapture();

}

void CxrUpdateView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CFormView::OnLButtonUp(nFlags, point);

	if (m_bLDragging)
	{
		m_bLDragging = FALSE;
		CImageList::DragLeave(this);
		CImageList::EndDrag();
		ReleaseCapture();

		delete m_pDragImage;

		// Удаляем подсветку цели
		m_tree_ctrl.SelectDropTarget(NULL);

		if( m_hitemDrag == m_hitemDrop )
			return;

		HTREEITEM htiParent = m_hitemDrop;
		while( (htiParent = m_tree_ctrl.GetParentItem( htiParent )) != NULL )
		{
			if( htiParent == m_hitemDrag ) return;
		}

		m_tree_ctrl.Expand( m_hitemDrop, TVE_EXPAND ) ;

		HTREEITEM htiNew = CopyBranch( m_hitemDrag, m_hitemDrop, TVI_LAST );

		m_tree_ctrl.DeleteItem(m_hitemDrag);
		m_tree_ctrl.SelectItem( htiNew );

		CTask* t = (CTask*)m_tree_ctrl.GetItemData(htiNew);
		CTask* t_parent = (CTask*)m_tree_ctrl.GetItemData(m_hitemDrop);
		t->reparent(t_parent);
		BOOL b = m_tree_ctrl.GetCheck(m_hitemDrop);
		CheckChildren(m_hitemDrop,b);
		

//		CProject* pProject = g_mainFrame->GetProject();
//		pProject->SetModifiedFlag(TRUE);

	}

}
void CxrUpdateView::OnMouseMove(UINT nFlags, CPoint point) 
{
	HTREEITEM	hitem;
	UINT		flags;

	if (m_bLDragging)
	{
		POINT pt = point;
		ClientToScreen( &pt );
		CImageList::DragMove(pt);
		if ((hitem = m_tree_ctrl.HitTest(point, &flags)) != NULL)
		{
			CImageList::DragShowNolock(FALSE);
			m_tree_ctrl.SelectDropTarget(hitem);
			m_hitemDrop = hitem;
			CImageList::DragShowNolock(TRUE);
		}
	}
	
	CFormView::OnMouseMove(nFlags, point);
}
HTREEITEM CxrUpdateView::CopyBranch( HTREEITEM htiBranch, HTREEITEM htiNewParent, 
                                                HTREEITEM htiAfter /*= TVI_LAST*/ )
{
        HTREEITEM hChild;

        HTREEITEM hNewItem = CopyItem( htiBranch, htiNewParent, htiAfter );
        hChild = m_tree_ctrl.GetChildItem(htiBranch);
        while( hChild != NULL)
        {
                // рекурсивно переносим все элементы
                CopyBranch(hChild, hNewItem);  
                hChild = m_tree_ctrl.GetNextSiblingItem( hChild );
        }
        return hNewItem;
}

HTREEITEM CxrUpdateView::CopyItem( HTREEITEM hItem, HTREEITEM htiNewParent, 
									HTREEITEM htiAfter /*= TVI_LAST*/ )
{
 TV_INSERTSTRUCT tvstruct;
 HTREEITEM hNewItem;
 CString sText;

 // берем информацию источника
 tvstruct.item.hItem = hItem;
 tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
 m_tree_ctrl.GetItem(&tvstruct.item);
 sText = m_tree_ctrl.GetItemText( hItem );

 tvstruct.item.cchTextMax = sText.GetLength();
 tvstruct.item.pszText = sText.LockBuffer();

 // Вставляем элемент в заданное место
 tvstruct.hParent = htiNewParent;
 tvstruct.hInsertAfter = htiAfter;
 tvstruct.item.mask = TVIF_IMAGE| TVIF_SELECTEDIMAGE | TVIF_TEXT;
 hNewItem = m_tree_ctrl.InsertItem(&tvstruct);sText.ReleaseBuffer();

 // Теперь копируем данные и состояние элемента
 m_tree_ctrl.SetItemData( hNewItem, m_tree_ctrl.GetItemData(hItem ));
 m_tree_ctrl.SetItemState( hNewItem, m_tree_ctrl.GetItemState(hItem, TVIS_STATEIMAGEMASK ),TVIS_STATEIMAGEMASK );

 // Вызываем виртуальную функцию для дальнейшей обработки наследованного класса
// OnItemCopied( hItem, hNewItem);
 return hNewItem;
}

void CxrUpdateView::SetCheckItem(HTREEITEM itm, BOOL b)
{
	m_tree_ctrl.SetCheck(itm,b);//self
}
void CxrUpdateView::RedrawChecks (CTask* t)
{
	if(!t)
		return;

	u32	 cnt = t->sub_task_count		();
	for(u32 i=0; i<cnt; ++i)
		RedrawChecks( t->get_sub_task(i) );

	SetCheckItem(t->m_tree_itm, t->is_enabled());
}

void CxrUpdateView::updateCheckedItems ()
{
	CTask* t = GetDocument()->m_task;
	if(!t)
		return;
	
	RedrawChecks(t);
}
