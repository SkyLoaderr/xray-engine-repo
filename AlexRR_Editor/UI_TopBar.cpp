//----------------------------------------------------
// file: UI_TopBar.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MenuTemplate.rh"


//----------------------------------------------------

#define _EXTRACT_UI()\
	_UI=(XRayEditorUI*)GetWindowLong(GetParent(hw),GWL_USERDATA);

static BOOL CALLBACK WP_TopBar( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){

	XRayEditorUI *_UI;
	RECT _RC;

	switch( msg ){

	case WM_INITDIALOG:
		break;

	case WM_CLOSE:
		break;

	case WM_ERASEBKGND:
		GetClientRect( hw, &_RC );
		_EXTRACT_UI();
		_UI->GoldFill( (HDC)wp, &_RC );
		break;

	case WM_DRAWITEM:

		_EXTRACT_UI();

		switch( (UINT) wp ){

		case IDC_HIDE_EDITOR:
		case IDC_EXIT_EDITOR:
		case IDC_TOGGLE_CONSOLE:
			_UI->DrawButton( (LPDRAWITEMSTRUCT) lp );
			break;

		case IDC_VP_ZOOM:
		case IDC_VP_MOVE:
		case IDC_VP_ROTATE:
			_UI->DrawButton( (LPDRAWITEMSTRUCT) lp );
			break;

		case IDC_VIEW_TYPE_MAP:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Viewport == VIEWPORT_MAP );
			break;
		case IDC_VIEW_TYPE_USER:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Viewport == VIEWPORT_USER );
			break;
		case IDC_VIEW_TYPE_OBJECT:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Viewport == VIEWPORT_OBJECT );
			break;

		default:
			_UI->DrawGoldText( (LPDRAWITEMSTRUCT) lp );
			break; }

		break;

	case WM_COMMAND:

		_EXTRACT_UI();

		switch( LOWORD(wp) ){

		case IDC_TOGGLE_CONSOLE:
			_UI->Command( COMMAND_CONSOLE );
			break;
		case IDC_HIDE_EDITOR:
			_UI->Command( COMMAND_HIDE );
			break;
		case IDC_EXIT_EDITOR:
			_UI->Command( COMMAND_EXIT );
			break;

		case IDC_VIEW_TYPE_MAP:
			_UI->ChangeViewport( VIEWPORT_MAP );
			break;
		case IDC_VIEW_TYPE_USER:
			_UI->ChangeViewport( VIEWPORT_USER );
			break;
		case IDC_VIEW_TYPE_OBJECT:
			_UI->ChangeViewport( VIEWPORT_OBJECT );
			break;
		}

		break;


	default:
		return FALSE; }

	return TRUE;
}

//----------------------------------------------------

bool XRayEditorUI::CreateTopBar(){

	m_TopBar = CreateDialog( m_Instance,
		MAKEINTRESOURCE( IDD_TOP_BAR ),
		m_Window, WP_TopBar );

	m_ZoomControl.Create( GetDlgItem(m_TopBar,IDC_VP_ZOOM) );
	m_MoveControl.Create( GetDlgItem(m_TopBar,IDC_VP_MOVE) );
	m_RotateControl.Create( GetDlgItem(m_TopBar,IDC_VP_ROTATE) );

	if( m_TopBar == 0 )
		return false;

	return true;
}

void XRayEditorUI::DestroyTopBar(){

	m_ZoomControl.Restore();
	m_MoveControl.Restore();
	m_RotateControl.Restore();

	DestroyWindow( m_TopBar );
	m_TopBar = 0;
}

//----------------------------------------------------
