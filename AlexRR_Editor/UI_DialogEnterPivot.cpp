//----------------------------------------------------
// file: UI_DialogEnterPivot.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "Scene.h"
#include "MenuTemplate.rh"

//----------------------------------------------------

static BOOL CALLBACK EnterPivotDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){
	XRayEditorUI *_UI;
	switch( msg ){
	case WM_INITDIALOG:
		SetWindowLong( hw, GWL_USERDATA, lp );
		_UI=(XRayEditorUI*)GetWindowLong( hw, GWL_USERDATA );
		_UI->SetDlgItemFloat( hw, IDC_PIVOT_X, _UI->pivot().x );
		_UI->SetDlgItemFloat( hw, IDC_PIVOT_Y, _UI->pivot().y );
		_UI->SetDlgItemFloat( hw, IDC_PIVOT_Z, _UI->pivot().z );
		break;
	case WM_COMMAND:
		switch( LOWORD(wp) ){
		case IDCANCEL:
			EndDialog(hw,IDCANCEL);
			break;
		case IDOK:
			_UI=(XRayEditorUI*)GetWindowLong( hw, GWL_USERDATA );
			_UI->pivot().x = _UI->GetDlgItemFloat( hw, IDC_PIVOT_X );
			_UI->pivot().y = _UI->GetDlgItemFloat( hw, IDC_PIVOT_Y );
			_UI->pivot().z = _UI->GetDlgItemFloat( hw, IDC_PIVOT_Z );
			EndDialog(hw,IDOK);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hw,IDCANCEL);
		break;
	default:
		return FALSE;
	}
	return FALSE;
}

void XRayEditorUI::EnterPivotDialog(){
	DialogBoxParam( inst(), MAKEINTRESOURCE( IDD_ENTER_PIVOT ),
		wnd(), EnterPivotDialogProc, (LPARAM)this );
}

//----------------------------------------------------

