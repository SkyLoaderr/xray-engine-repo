//----------------------------------------------------
// file: UI_BottomBar.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MenuTemplate.rh"


//----------------------------------------------------


#define _EXTRACT_UI()\
	_UI=(XRayEditorUI*)GetWindowLong(GetParent(hw),GWL_USERDATA);

static BOOL CALLBACK WP_BottomBar( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){

	XRayEditorUI *_UI;
	RECT _RC;

	switch( msg ){

	case WM_INITDIALOG:
		break;

	case WM_CLOSE:
		break;

	case WM_ERASEBKGND:
		GetClientRect( hw, &_RC );
		_UI=(XRayEditorUI*)GetWindowLong(GetParent(hw),GWL_USERDATA);
		_UI->GoldFill( (HDC)wp, &_RC );
		break;

	case WM_DRAWITEM:

		_EXTRACT_UI();

		switch( (UINT) wp ){

		case IDC_OP_WIRE:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_WireMode );
			break;
		case IDC_OP_TEXTURE:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_TexturesEnabled );
			break;
		case IDC_OP_GRID:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_GridEnabled );
			break;
		case IDC_OP_PIVOT:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_FltPivots );
			break;
		case IDC_OP_SOUNDS:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_FltSounds );
			break;
		case IDC_OP_LIGHTS:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_FltLights );
			break;

		default:
			_UI->DrawGoldText( (LPDRAWITEMSTRUCT) lp );
			break; }

		break;

	case WM_COMMAND:
		
		_EXTRACT_UI();

		switch( LOWORD(wp) ){

		case IDC_OP_WIRE:
			_UI->m_WireMode = !_UI->m_WireMode;
			InvalidateRect( GetDlgItem(hw,LOWORD(wp)), 0, 0 );
			break;
		case IDC_OP_TEXTURE:
			_UI->m_TexturesEnabled = !_UI->m_TexturesEnabled;
			InvalidateRect( GetDlgItem(hw,LOWORD(wp)), 0, 0 );
			break;
		case IDC_OP_GRID:
			_UI->m_GridEnabled = !_UI->m_GridEnabled;
			InvalidateRect( GetDlgItem(hw,LOWORD(wp)), 0, 0 );
			break;
		case IDC_OP_PIVOT:
			_UI->m_FltPivots = !_UI->m_FltPivots;
			InvalidateRect( GetDlgItem(hw,LOWORD(wp)), 0, 0 );
			break;
		case IDC_OP_SOUNDS:
			_UI->m_FltSounds = !_UI->m_FltSounds;
			InvalidateRect( GetDlgItem(hw,LOWORD(wp)), 0, 0 );
			break;
		case IDC_OP_LIGHTS:
			_UI->m_FltLights = !_UI->m_FltLights;
			InvalidateRect( GetDlgItem(hw,LOWORD(wp)), 0, 0 );
			break;
		}

		break;

	default:
		return FALSE; }

	return TRUE;
}

//----------------------------------------------------

bool XRayEditorUI::CreateBottomBar(){

	m_BottomBar = CreateDialog( m_Instance,
		MAKEINTRESOURCE( IDD_BOTTOM_BAR ),
		m_Window, WP_BottomBar );

	if( m_BottomBar == 0 )
		return false;

	return true;
}

void XRayEditorUI::DestroyBottomBar(){
	DestroyWindow( m_BottomBar );
	m_BottomBar = 0;
}

//----------------------------------------------------
