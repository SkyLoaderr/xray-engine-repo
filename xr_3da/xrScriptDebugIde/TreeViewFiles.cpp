// TreeViewFiles.cpp : implementation file
//

#include "stdafx.h"
#include "ide2.h"
#include "TreeViewFiles.h"

#include "MainFrame.h"
#include "LuaDoc.h"
#include "LuaView.h"
#include "SSConnection.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles
//#define working_folder "$/xrStalker/Scripts/"
CString str_WhoCheckedOut(IVSSItemPtr vss_item);


IMPLEMENT_DYNCREATE(CTreeViewFiles, CTreeView)

CTreeViewFiles::CTreeViewFiles()
{
	working_folder  = 	AfxGetApp()->GetProfileString("options","sSafeFolder", "$/xrStalker/Scripts/" );
}

CTreeViewFiles::~CTreeViewFiles()
{
}


BEGIN_MESSAGE_MAP(CTreeViewFiles, CTreeView)
	//{{AFX_MSG_MAP(CTreeViewFiles)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDED, OnItemexpanded)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(NM_CLICK, OnLclick)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)

	//}}AFX_MSG_MAP
	ON_COMMAND(ID_PROJECT_ADD_FILES,  OnProjectAddFiles)
	ON_COMMAND(ID_PROJECT_RUNPROJECT, OnRunApplication)
	ON_COMMAND(ID_PROJECT_DEBUGGING,  OnDebuggingOptions)

	ON_COMMAND(ID_VSS_CHECKIN,			OnVSSCheckIn)
	ON_COMMAND(ID_VSS_CHECKOUT,			OnVSSCheckOut)
	ON_COMMAND(ID_VSS_UNDOCHECKOUT,		OnVSSUndoCheckOut)
	ON_COMMAND(ID_VSS_DIFFERENCE,		OnVSSDifference)


	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles drawing

void CTreeViewFiles::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles diagnostics

#ifdef _DEBUG
void CTreeViewFiles::AssertValid() const
{
	CTreeView::AssertValid();
}

void CTreeViewFiles::Dump(CDumpContext& dc) const
{
	CTreeView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTreeViewFiles message handlers

int CTreeViewFiles::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pTree = &GetTreeCtrl();

	m_images.Create (IDB_IL_FILE, 16, 1, RGB(0,255,0));
	int cc = m_images.GetImageCount();
	m_pTree->SetImageList (&m_images, TVSIL_NORMAL);

	return 0;
}

void CTreeViewFiles::RemoveAll()
{
	m_pTree->DeleteAllItems();
}

void CTreeViewFiles::AddRoot(CString strProject)
{
	CString strLabel;
	strLabel.Format("Project '%s'", strProject);

	TV_INSERTSTRUCT root;
	root.hParent = NULL;
	root.hInsertAfter = TVI_SORT;
	root.item.iImage = 1;
	root.item.iSelectedImage = 1;
	root.item.pszText = strLabel.GetBuffer(0);
	root.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	m_hRoot = m_pTree->InsertItem(&root);

	TV_INSERTSTRUCT files;
	files.hParent = m_hRoot;
	files.hInsertAfter = TVI_SORT;
	files.item.iImage = 2;
	files.item.iSelectedImage = 2;
	files.item.pszText = "Lua scripts";
	files.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	m_hFilesFolder = m_pTree->InsertItem(&files);
}

HTREEITEM CTreeViewFiles::AddProjectFile(CString strName, long lParam)
{
	TV_INSERTSTRUCT file;
	file.hParent = m_hFilesFolder;
	file.hInsertAfter = TVI_LAST;
	file.item.iImage = 4;
	file.item.iSelectedImage = 4;
	file.item.pszText = strName.GetBuffer(0);
	file.item.lParam = lParam;
	file.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	
	return m_pTree->InsertItem(&file);
}

BOOL CTreeViewFiles::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT;
	
	return CTreeView::PreCreateWindow(cs);
}

void CTreeViewFiles::OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	if ( pNMTreeView->itemNew.hItem == m_hFilesFolder )
	{
		if ( pNMTreeView->action==TVE_EXPAND )
			m_pTree->SetItemImage(pNMTreeView->itemNew.hItem, 3, 3);
		else if ( pNMTreeView->action==TVE_COLLAPSE )
			m_pTree->SetItemImage(pNMTreeView->itemNew.hItem, 2, 2);
	}

	if ( pNMTreeView->itemNew.hItem == m_hFilesFolder && pNMTreeView->action==TVE_EXPAND )
	{
			HTREEITEM child =  m_pTree->GetChildItem(pNMTreeView->itemNew.hItem);
						
			while(child){
				VSSUpdateStatus(child);

				child = m_pTree->GetNextSiblingItem(child);
			}
	}
	*pResult = 0;

}

void CTreeViewFiles::OnLclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CPoint pt;
	GetCursorPos(&pt);
	m_pTree->ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM item = m_pTree->HitTest(pt, &nFlags);

	if(m_pTree->GetParentItem(item) == m_hFilesFolder )
		VSSUpdateStatus(item);
}

void CTreeViewFiles::OnRclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	HTREEITEM itm = m_pTree->GetSelectedItem();
	long stat = VSSGetStatus(itm);

	// TODO: Add your control notification handler code here
	CPoint pt;
	GetCursorPos(&pt);
	m_pTree->ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM item = m_pTree->HitTest(pt, &nFlags);
	
	HMENU hMenu = NULL;
	if ( item == m_hRoot ){
		hMenu = LoadMenu(theApp.m_hInstance, MAKEINTRESOURCE(IDR_PROJECT_MENU));

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

	if(m_pTree->GetParentItem(item) == m_hFilesFolder ){
		hMenu = LoadMenu(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MENU1));
		if ( !hMenu )
			return;

		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (!hSubMenu) 
		{
			DestroyMenu(hMenu);
			return;
		}

	CMenu mnu;
	mnu.Attach(hSubMenu);
	HTREEITEM itm = m_pTree->GetSelectedItem();
	long stat = VSSGetStatus(itm);
	UINT mnu_itm;
	mnu_itm = mnu.GetMenuItemID(0);
	mnu.EnableMenuItem(mnu_itm,(stat==VSSFILE_CHECKEDOUT_ME)?MF_ENABLED : MF_GRAYED);


	mnu_itm = mnu.GetMenuItemID(1);
	mnu.EnableMenuItem(mnu_itm,((stat==VSSFILE_NOTCHECKEDOUT)||(stat==VSSFILE_CHECKEDOUT))?MF_ENABLED : MF_GRAYED );
	mnu_itm = mnu.GetMenuItemID(2);
	mnu.EnableMenuItem(mnu_itm,(stat==VSSFILE_CHECKEDOUT_ME)?MF_ENABLED : MF_GRAYED);
	mnu_itm = mnu.GetMenuItemID(3);
	mnu.EnableMenuItem(mnu_itm,(stat != -1)?MF_ENABLED : MF_GRAYED);

		POINT mouse;
		GetCursorPos(&mouse);
		::SetForegroundWindow(m_hWnd);	
//		::TrackPopupMenu(hSubMenu, 0, mouse.x, mouse.y, 0, m_hWnd, NULL);
		mnu.TrackPopupMenu(/*hSubMenu,*/ 0, mouse.x, mouse.y, this, NULL);
		
		DestroyMenu(hMenu);

	}
	*pResult = 0;
}

void CTreeViewFiles::OnProjectAddFiles() 
{
	CProject* pProject = g_mainFrame->GetProject();
	pProject->AddFiles();
}

void CTreeViewFiles::OnRunApplication() 
{
	CProject* pProject = g_mainFrame->GetProject();
	pProject->OnRunApplication();
}

void CTreeViewFiles::OnDebuggingOptions() 
{
	CProject* pProject = g_mainFrame->GetProject();
	pProject->OnDebugOptions();
}

void CTreeViewFiles::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	CPoint pt;
	GetCursorPos(&pt);
	m_pTree->ScreenToClient(&pt);
	UINT nFlags;
	HTREEITEM hItem = m_pTree->HitTest(pt, &nFlags);
	if(!hItem)
		return;

	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
	if ( pPF )
	{
		pPF->m_bBreakPointsSaved = FALSE;
		CLuaView* pView = theApp.OpenProjectFilesView(pPF);
		if(pView){
			pPF->m_lua_view = pView;
			pView->Activate();
			VSSUpdateStatus(hItem);
		}
	}

	*pResult = 0;
}

void CTreeViewFiles::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	if ( hItem && pTVKeyDown->wVKey == VK_DELETE )
	{
		CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(hItem);
		if ( pPF )
		{
			m_pTree->DeleteItem(hItem);
			CProject* pProject = g_mainFrame->GetProject();
			pProject->RemoveFile(pPF);
		}
	}

	*pResult = 0;
}

void CTreeViewFiles::ExpandFiles()
{
	GetTreeCtrl().Expand(m_hRoot, TVE_EXPAND );
}


CString str_WhoCheckedOut(IVSSItemPtr vss_item)
{
	CString str_user(_T("None"));
	IVSSCheckoutsPtr checkOuts;
	vss_item->get_Checkouts(&checkOuts);

	IVSSCheckoutPtr vss_checkOutItem;
	checkOuts->get_Item(_variant_t(1L), &vss_checkOutItem);
	CComBSTR str_userName;
	vss_checkOutItem->get_Username(&str_userName);

	long l_count;
	checkOuts->get_Count(&l_count);
	if (l_count == 1)
	{
		CW2A pszA( str_userName );
		str_user.Format(_T("%s"), pszA);
	}
	else if (l_count > 1)
	{
		CW2A pszA( str_userName );
		str_user.Format(_T("%s et.al."), pszA);
	}
	return str_user;
}

void CTreeViewFiles::VSSUpdateStatus(HTREEITEM itm)
{
	long stat = VSSGetStatus(itm);
		switch (stat)
		{
		
		case -1:{
			m_pTree->SetItemImage(itm, 4, 4);
			DWORD_PTR pp = m_pTree->GetItemData(itm);
			if(pp){
				CProjectFile* pf = (CProjectFile*)pp;
				if(pf->m_lua_view)
					pf->m_lua_view->GetEditor()->SetReadOnly(stat!=-1);
			}
		}break;

		case VSSFILE_NOTCHECKEDOUT:{
			
			m_pTree->SetItemImage(itm, 5, 5);
			DWORD_PTR pp = m_pTree->GetItemData(itm);
			if(pp){
				CProjectFile* pf = (CProjectFile*)pp;
				if(pf->m_lua_view)
					pf->m_lua_view->GetEditor()->SetReadOnly(stat!=-1);
			}

			}break;
		case VSSFILE_CHECKEDOUT:
			break;
		case VSSFILE_CHECKEDOUT_ME :{
			m_pTree->SetItemImage(itm, 6, 6);
			DWORD_PTR pp = m_pTree->GetItemData(itm);
			if(pp){
				CProjectFile* pf = (CProjectFile*)pp;
				if(pf->m_lua_view)
					pf->m_lua_view->GetEditor()->SetReadOnly(FALSE);
			}

			}break;
		default:
			break;
		}
}

long CTreeViewFiles::VSSGetStatus(HTREEITEM itm)
{
	if(m_pTree->GetParentItem(itm)!=m_hFilesFolder)
		return -1;
	if(!theApp.m_ssConnection.b_IsConnected())
		return -1;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CString str_;
	str_.Format("%s%s",working_folder,str);
	CComBSTR file_name = str_;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);
	if(vssItem==NULL)
		return -1;
	long stat;
	HRESULT hr;
	CString str_temp;
	if((hr = vssItem->get_IsCheckedOut(&stat)) == S_OK){
		return stat;
	}
	return -1;
}
void CTreeViewFiles::OnVSSCheckIn(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();

	Save(hItem);
	VSSCheckIn(hItem);
	VSSUpdateStatus(hItem);
}

void CTreeViewFiles::OnVSSCheckOut(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	VSSCheckOut(hItem);
	VSSUpdateStatus(hItem);
}

void CTreeViewFiles::OnVSSUndoCheckOut(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	VSSUndoCheckOut(hItem);
	VSSUpdateStatus(hItem);

}
void CTreeViewFiles::OnVSSDifference(){
	HTREEITEM hItem = m_pTree->GetSelectedItem();
	VSSCheckIn(hItem);
}


void CTreeViewFiles::VSSCheckIn(HTREEITEM itm){
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CComBSTR file_name = working_folder+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);
	long stat;

	CComBSTR bstr_comment("no comment");
	CComBSTR bstr_localSpec;
/*	long flags =	VSSFLAG_CMPCHKSUM|VSSFLAG_DELNO|VSSFLAG_FORCEDIRYES|
					VSSFLAG_KEEPNO|VSSFLAG_RECURSNO|VSSFLAG_UPDUNCH;
	*/
	Save(itm);
	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->Checkin(bstr_comment, bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		VSSUpdateStatus(itm);
	}

}

void CTreeViewFiles::VSSCheckOut(HTREEITEM itm){
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CComBSTR file_name = working_folder+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);
	long stat;

	CComBSTR bstr_comment("no comment");
	CComBSTR bstr_localSpec;
/*	long flags =	VSSFLAG_CMPCHKSUM|VSSFLAG_DELNO|VSSFLAG_FORCEDIRYES|
					VSSFLAG_KEEPNO|VSSFLAG_RECURSNO|VSSFLAG_UPDUNCH;
	*/
	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->Checkout(bstr_comment, bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		VSSUpdateStatus(itm);
		Reload(itm);
	}
}

void CTreeViewFiles::VSSUndoCheckOut(HTREEITEM itm){
	if(!theApp.m_ssConnection.b_IsConnected())
		return;

	CString str;
	str = m_pTree->GetItemText(itm);

	IVSSItemPtr vssItem;
	CComBSTR file_name = working_folder+str;
	theApp.m_ssConnection.p_GetSourcesafeDatabase()->get_VSSItem(file_name, FALSE, &vssItem);
	long stat;

	CComBSTR bstr_comment("no comment");
	CComBSTR bstr_localSpec;
/*	long flags =	VSSFLAG_CMPCHKSUM|VSSFLAG_DELNO|VSSFLAG_FORCEDIRYES|
					VSSFLAG_KEEPNO|VSSFLAG_RECURSNO|VSSFLAG_UPDUNCH;
	*/
	vssItem->get_LocalSpec(&bstr_localSpec);
	vssItem->UndoCheckout(bstr_localSpec, 0);
	if (!b_DisplayAnyError())
	{
		VSSUpdateStatus(itm);
		Reload(itm);
	}
}

void CTreeViewFiles::VSSDifferences(HTREEITEM itm){
}

void  CTreeViewFiles::Save(HTREEITEM itm){
	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(itm);
	if ( pPF )
	{
		if(pPF->m_lua_view)
			pPF->m_lua_view->_save();
	}

}

void  CTreeViewFiles::Reload(HTREEITEM itm){

	CProjectFile* pPF = (CProjectFile*)m_pTree->GetItemData(itm);
	if ( pPF )
	{
		pPF->m_bBreakPointsSaved = FALSE;
		CLuaView* pView = theApp.OpenProjectFilesView(pPF);
		CFile fin;
		if ( fin.Open(pPF->GetPathName(), CFile::modeRead) )
			pView->GetEditor()->Load(&fin);
	}
}