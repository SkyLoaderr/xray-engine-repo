//----------------------------------------------------
// file: UI_DialogSelLandscape.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Library.h"
#include "Texture.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

static void SDlg_AddFolder(
	TexFolder& folder,
	HWND htree,
	HTREEITEM parent,
	ETexture *_Current )
{

	list<TexFolder*>::iterator _I0;
	list<TexFolder*>::iterator _I0e;
	list<ETexture*>::iterator _T0;
	list<ETexture*>::iterator _T0e;

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

		SDlg_AddFolder( **_I0, htree, item, _Current );
	}
		
	_T0 = folder.m_Items.begin();
	_T0e = folder.m_Items.end();

	for(;_T0!=_T0e;_T0++){

		sprintf( desc, "%s [%dx%d]",
			(*_T0)->name(),
			(*_T0)->width(), (*_T0)->height() );
		
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

static BOOL CALLBACK SelectLandscapeDlgProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){

	LPDRAWITEMSTRUCT ds;
	HIMAGELIST imagelist;
	ETexture **_T;
	ETexture *_pT;
	HWND hw2;
	TVITEM tvi;
	HTREEITEM item;
	LPNMHDR notifyheader;
	RECT rcdraw;

	switch( msg ){

	case WM_INITDIALOG:

		SetWindowLong(hw,GWL_USERDATA,lp);
		_T = (ETexture **)GetWindowLong(hw,GWL_USERDATA);

		hw2 = GetDlgItem( hw, IDC_TEXTREE );

		imagelist = ImageList_LoadBitmap( UI.inst(), MAKEINTRESOURCE(IDB_TEXSELECTOR_IML),20,0, RGB(255,255,255) );
		TreeView_SetImageList( hw2, imagelist, TVSIL_NORMAL );

		SDlg_AddFolder( Lib.m_TexFolders, hw2, TVI_ROOT, (*_T) );

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
			_T = (ETexture **)GetWindowLong(hw,GWL_USERDATA);
			hw2 = GetDlgItem( hw, IDC_TEXTREE );

			if( (item = TreeView_GetSelection( hw2 )) ){
				memset( &tvi, 0, sizeof(tvi) );
				tvi.mask = TVIF_PARAM;
				tvi.hItem = item;
				if( TreeView_GetItem( hw2, &tvi ) ){
					if( tvi.lParam ){
						_pT = (ETexture*) tvi.lParam;
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


	case WM_DRAWITEM:
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
		break;

	default:
		return FALSE;
	}
	return TRUE;
}


//----------------------------------------------------

ETexture *XRayEditorUI::SelectLandscape(){

	ETexture *_T = Lib.l();
	if(IDOK == DialogBoxParam(inst(),
		MAKEINTRESOURCE(IDD_SDLG_SELECTLANDSCAPE),
		wnd(), SelectLandscapeDlgProc, (LPARAM)&_T ) ){
			return _T;
	}
	return 0;
}

//----------------------------------------------------

