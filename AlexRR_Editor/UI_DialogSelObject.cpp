//----------------------------------------------------
// file: UI_DialogSelObject.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Library.h"
#include "SceneObject.h"
#include "Texture.h"
#include "SObject2.h"
#include "SceneClassList.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

static void SODlg_AddFolder(
	ObjFolder& folder,
	HWND htree,
	HTREEITEM parent,
	SceneObject *_Current )
{

	list<ObjFolder*>::iterator _I0;
	list<ObjFolder*>::iterator _I0e;
	list<SceneObject*>::iterator _T0;
	list<SceneObject*>::iterator _T0e;

	char desc[1024];
	TVINSERTSTRUCT is;

	_I0 = folder.m_Subs.begin();
	_I0e = folder.m_Subs.end();

	for(;_I0!=_I0e;_I0++){

		memset( &is, 0, sizeof(is) );
		is.hParent = parent;
		is.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		is.item.pszText = (*_I0)->m_Name;
		is.item.iImage = 0;
		is.item.iSelectedImage = 1;
		HTREEITEM item = TreeView_InsertItem( htree, &is );

		SODlg_AddFolder( **_I0, htree, item, _Current );
	}
		
	_T0 = folder.m_Items.begin();
	_T0e = folder.m_Items.end();

	for(;_T0!=_T0e;_T0++){

		if( (*_T0)->ClassID()==OBJCLASS_SOBJECT2 ){
			SObject2 *sobj = (SObject2 *)(*_T0);
			sprintf( desc, "%s (%s)", sobj->GetName(), sobj->GetClassName() );
		} else {
			sprintf( desc, "%s (?)", (*_T0)->GetName() );
		}
		
		memset( &is, 0, sizeof(is) );
		is.hParent = parent;
		is.item.mask = TVIF_TEXT|TVIF_PARAM|TVIF_IMAGE|TVIF_SELECTEDIMAGE;
		is.item.lParam = (LPARAM) (*_T0);
		is.item.pszText = desc;
		is.item.iImage = 2;
		is.item.iSelectedImage = 3;
		HTREEITEM titem = TreeView_InsertItem( htree, &is );

		if( (*_T0) == _Current )
			TreeView_SelectItem( htree, titem );
	}
}

static BOOL CALLBACK SelectSObjectDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){

	//LPDRAWITEMSTRUCT ds;
	HIMAGELIST imagelist;
	SceneObject **_T;
	SceneObject *_pT;
	HWND hw2;
	TVITEM tvi;
	HTREEITEM item;
	LPNMHDR notifyheader;
	//RECT rcdraw;

	switch( msg ){

	case WM_INITDIALOG:

		SetWindowLong(hw,GWL_USERDATA,lp);
		_T = (SceneObject **)GetWindowLong(hw,GWL_USERDATA);

		hw2 = GetDlgItem( hw, IDC_TEXTREE );

		imagelist = ImageList_LoadBitmap( UI.inst(), MAKEINTRESOURCE(IDB_TEXSELECTOR_IML),20,0, RGB(255,255,255) );
		TreeView_SetImageList( hw2, imagelist, TVSIL_NORMAL );

		SODlg_AddFolder( Lib.m_ObjFolders, hw2, TVI_ROOT, (*_T) );

		break;

	case WM_DESTROY:
		hw2 = GetDlgItem( hw, IDC_TEXTREE );
		imagelist = TreeView_GetImageList( hw2, TVSIL_NORMAL );
		TreeView_SetImageList( hw2, 0, TVSIL_NORMAL );
		ImageList_Destroy( imagelist );
		break;

	case WM_CLOSE:
		EndDialog(hw,IDCANCEL);
		break;

	case WM_COMMAND:
		switch( LOWORD(wp) ){
		case IDCANCEL:
			SendMessage(hw,WM_CLOSE,0,0);
			break;

		case IDOK:
			_T = (SceneObject **)GetWindowLong(hw,GWL_USERDATA);
			hw2 = GetDlgItem( hw, IDC_TEXTREE );

			if( (item = TreeView_GetSelection( hw2 )) ){
				memset( &tvi, 0, sizeof(tvi) );
				tvi.mask = TVIF_PARAM;
				tvi.hItem = item;
				if( TreeView_GetItem( hw2, &tvi ) ){
					if( tvi.lParam ){
						_pT = (SceneObject*) tvi.lParam;
						(*_T) = _pT;
						EndDialog( hw, IDOK );
					}
				}
			}
			break;
		}
		break;

	case WM_NOTIFY:
		if( IDC_TEXTREE == (UINT)wp ){
			notifyheader = (LPNMHDR)lp;
			if( notifyheader->code == TVN_SELCHANGED ){
				InvalidateRect( GetDlgItem( hw, IDC_TEXPREVIEW ), 0, TRUE );
			} else if( notifyheader->code == NM_DBLCLK ){
				SendMessage( hw, WM_COMMAND, IDOK, 0 );
			}
		}
		break;


	/*case WM_DRAWITEM:
		if( IDC_TEXPREVIEW == (UINT)wp ){
			hw2 = GetDlgItem( hw, IDC_TEXTREE );
			item = TreeView_GetSelection( hw2 );
			memset( &tvi, 0, sizeof(tvi) );
			tvi.mask = TVIF_PARAM;
			tvi.hItem = item;
			if( TreeView_GetItem( hw2, &tvi ) ){
				if( tvi.lParam ){
					_pT = (ETexture*) tvi.lParam;
					ds = (LPDRAWITEMSTRUCT)lp;
					rcdraw.left = (ds->rcItem.right - _pT->width()) / 2;
					rcdraw.top = (ds->rcItem.bottom - _pT->height()) / 2;
					rcdraw.right = rcdraw.left + _pT->width();
					rcdraw.bottom = rcdraw.top + _pT->height();
					_pT->thumb( ds->hDC, & rcdraw );
				}
			}
		}
		break;*/

	default:
		return FALSE;
	}
	return TRUE;
}


//----------------------------------------------------

SceneObject *XRayEditorUI::SelectSObject(){

	SceneObject *_T = Lib.o();
	if(IDOK == DialogBoxParam(inst(),
		MAKEINTRESOURCE(IDD_SDLG_SELECTLANDSCAPE),
		wnd(), SelectSObjectDlgProc, (LPARAM)&_T ) ){
			return _T;
	}
	return 0;
}

//----------------------------------------------------

