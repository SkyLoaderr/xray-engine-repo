//----------------------------------------------------
// file: TB_AddLandscape.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Scene.h"
#include "Library.h"
#include "Texture.h"
#include "MSC_AddLandscape.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

TB_AddLandscape::TB_AddLandscape()
	:XRayEditorUIToolBox()
{
}

TB_AddLandscape::~TB_AddLandscape(){
}

//----------------------------------------------------

void TB_AddLandscape::Command( WPARAM wp, LPARAM lp ){

	ETexture *t = 0;

	switch( LOWORD( wp ) ){

	case IDC_LBRUSHSTYLE:
		t = UI.SelectLandscape();
		if( t ){
			Lib.lset( t );
			InvalidateRect( GetDlgItem(m_ChildDialog,IDC_LBRUSHSTYLE), 0, TRUE );
		}
		break;

	case IDC_BSIZE_1:
		MSC_AddLandscape.m_BrushSize = 1;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_3:
		MSC_AddLandscape.m_BrushSize = 3;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_5:
		MSC_AddLandscape.m_BrushSize = 5;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_7:
		MSC_AddLandscape.m_BrushSize = 7;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_9:
		MSC_AddLandscape.m_BrushSize = 9;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_11:
		MSC_AddLandscape.m_BrushSize = 11;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_13:
		MSC_AddLandscape.m_BrushSize = 13;
		UpdateBSButtons( m_ChildDialog );
		break;
	case IDC_BSIZE_15:
		MSC_AddLandscape.m_BrushSize = 15;
		UpdateBSButtons( m_ChildDialog );
		break;

	}
}

void TB_AddLandscape::DrawItem( WPARAM wp, LPARAM lp ){
	switch( (UINT) wp ){

	case IDC_LBRUSHSTYLE:
		if( Lib.l() ){
			LPDRAWITEMSTRUCT ds = (LPDRAWITEMSTRUCT) lp;
			Lib.l()->stretchthumb( ds->hDC, &ds->rcItem );
		} else{
			LPDRAWITEMSTRUCT ds = (LPDRAWITEMSTRUCT) lp;
			FillRect( ds->hDC, &ds->rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH) );
		}
		break;

	case IDC_BSIZE_1:
		UI.DrawCheck( (LPDRAWITEMSTRUCT)lp, MSC_AddLandscape.m_BrushSize == 1 );
		break;
	case IDC_BSIZE_3:
		UI.DrawCheck( (LPDRAWITEMSTRUCT)lp, MSC_AddLandscape.m_BrushSize == 3 );
		break;
	case IDC_BSIZE_5:
		UI.DrawCheck( (LPDRAWITEMSTRUCT)lp, MSC_AddLandscape.m_BrushSize == 5 );
		break;
	case IDC_BSIZE_7:
		UI.DrawCheck( (LPDRAWITEMSTRUCT)lp, MSC_AddLandscape.m_BrushSize == 7 );
		break;
	case IDC_BSIZE_9:
		UI.DrawCheck( (LPDRAWITEMSTRUCT)lp, MSC_AddLandscape.m_BrushSize == 9 );
		break;
	case IDC_BSIZE_11:
		UI.DrawCheck( (LPDRAWITEMSTRUCT)lp, MSC_AddLandscape.m_BrushSize == 11 );
		break;
	case IDC_BSIZE_13:
		UI.DrawCheck( (LPDRAWITEMSTRUCT)lp, MSC_AddLandscape.m_BrushSize == 13 );
		break;
	case IDC_BSIZE_15:
		UI.DrawCheck( (LPDRAWITEMSTRUCT)lp, MSC_AddLandscape.m_BrushSize == 15 );
		break;

	}
}

void TB_AddLandscape::Init( HWND hw ){
	UpdateBSButtons( hw );
}

//----------------------------------------------------

void TB_AddLandscape::UpdateBSButtons( HWND hw ){
	InvalidateRect( GetDlgItem(hw,IDC_BSIZE_1),0,0);
	InvalidateRect( GetDlgItem(hw,IDC_BSIZE_3),0,0);
	InvalidateRect( GetDlgItem(hw,IDC_BSIZE_5),0,0);
	InvalidateRect( GetDlgItem(hw,IDC_BSIZE_7),0,0);
	InvalidateRect( GetDlgItem(hw,IDC_BSIZE_9),0,0);
	InvalidateRect( GetDlgItem(hw,IDC_BSIZE_11),0,0);
	InvalidateRect( GetDlgItem(hw,IDC_BSIZE_13),0,0);
	InvalidateRect( GetDlgItem(hw,IDC_BSIZE_15),0,0);
}

