//----------------------------------------------------
// file: UI_LeftBar.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MenuTemplate.rh"


//----------------------------------------------------


#define _EXTRACT_UI()\
	_UI=(XRayEditorUI*)GetWindowLong(GetParent(hw),GWL_USERDATA);


static BOOL CALLBACK WP_LeftBar( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){

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

		case IDC_CLEAR_SCENE:
		case IDC_LOAD_SCENE:
		case IDC_SAVE_SCENE:
		case IDC_SAVEAS_SCENE:
		case IDC_START_BUILD:
		case IDC_BUILD_OPTIONS:
		case IDC_UPDATE_ALL:
		case IDC_MENU:
		case IDC_CUT:
		case IDC_COPY:
		case IDC_PASTE:
		case IDC_UNDO:
		case IDC_REDO:
			_UI->DrawButton( (LPDRAWITEMSTRUCT) lp );
			break;

		case IDC_TARGET_LAND:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Target == TARGET_LANDSCAPE );
			break;
		case IDC_TARGET_OBJ:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Target == TARGET_OBJECT );
			break;
		case IDC_TARGET_LIGHT:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Target == TARGET_LIGHT );
			break;
		case IDC_TARGET_PIVOT:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Target == TARGET_PIVOT );
			break;
		case IDC_TARGET_SNDPLANE:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Target == TARGET_SNDPLANE );
			break;
		case IDC_TARGET_SOUND:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Target == TARGET_SOUND );
			break;
		case IDC_TARGET_NUM_PIVOT:
			_UI->DrawButton( (LPDRAWITEMSTRUCT) lp );
			break;

		case IDC_ACTION_SELECT:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Action == ACTION_SELECT );
			break;
		case IDC_ACTION_ADD:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Action == ACTION_ADD );
			break;
		case IDC_ACTION_MOVE:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Action == ACTION_MOVE );
			break;
		case IDC_ACTION_ROTATE:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Action == ACTION_ROTATE );
			break;
		case IDC_ACTION_SCALE:
			_UI->DrawCheck( (LPDRAWITEMSTRUCT) lp, _UI->m_Action == ACTION_SCALE );
			break;

		case IDC_SUBPLACE:
			_UI->GoldFill(((LPDRAWITEMSTRUCT)lp)->hDC,
				& ((LPDRAWITEMSTRUCT)lp)->rcItem );
			break;

		default:
			_UI->DrawGoldText( (LPDRAWITEMSTRUCT) lp );
			break; }
		break;

	case WM_COMMAND:
		
		_EXTRACT_UI();

		switch( LOWORD(wp) ){

		case IDC_TARGET_LAND:
			_UI->ChangeTarget( TARGET_LANDSCAPE );
			break;
		case IDC_TARGET_OBJ:
			_UI->ChangeTarget( TARGET_OBJECT );
			break;
		case IDC_TARGET_LIGHT:
			_UI->ChangeTarget( TARGET_LIGHT );
			break;
		case IDC_TARGET_PIVOT:
			_UI->ChangeTarget( TARGET_PIVOT );
			break;
		case IDC_TARGET_SNDPLANE:
			_UI->ChangeTarget( TARGET_SNDPLANE );
			break;
		case IDC_TARGET_SOUND:
			_UI->ChangeTarget( TARGET_SOUND );
			break;

		case IDC_TARGET_NUM_PIVOT:
			_UI->Command( COMMAND_ENTER_PIVOT );
			break;

		case IDC_ACTION_SELECT:
			_UI->ChangeAction( ACTION_SELECT );
			break;
		case IDC_ACTION_ADD:
			_UI->ChangeAction( ACTION_ADD );
			break;
		case IDC_ACTION_MOVE:
			_UI->ChangeAction( ACTION_MOVE );
			break;
		case IDC_ACTION_ROTATE:
			_UI->ChangeAction( ACTION_ROTATE );
			break;
		case IDC_ACTION_SCALE:
			_UI->ChangeAction( ACTION_SCALE );
			break;

		case IDC_CLEAR_SCENE:
			_UI->Command( COMMAND_CLEAR );
			break;
		case IDC_LOAD_SCENE:
			_UI->Command( COMMAND_LOAD );
			break;
		case IDC_SAVE_SCENE:
			_UI->Command( COMMAND_SAVE );
			break;
		case IDC_SAVEAS_SCENE:
			_UI->Command( COMMAND_SAVEAS );
			break;

		case IDC_START_BUILD:
			_UI->Command( COMMAND_BUILD );
			break;
		case IDC_BUILD_OPTIONS:
			_UI->Command( COMMAND_OPTIONS );
			break;

		case IDC_UPDATE_ALL:
			_UI->Command( COMMAND_UPDATE_ALL );
			break;
		case IDC_MENU:
			_UI->Command( COMMAND_MENU );
			break;

		case IDC_CUT:
			_UI->Command( COMMAND_CUT );
			break;
		case IDC_COPY:
			_UI->Command( COMMAND_COPY );
			break;
		case IDC_PASTE:
			_UI->Command( COMMAND_PASTE );
			break;
		case IDC_UNDO:
			_UI->Command( COMMAND_UNDO );
			break;
		case IDC_REDO:
			_UI->Command( COMMAND_REDO );
			break;
		}
		break;

	default:
		return FALSE; }

	return TRUE;
}

//----------------------------------------------------

bool XRayEditorUI::CreateLeftBar(){

	m_LeftBar = CreateDialog( m_Instance,
		MAKEINTRESOURCE( IDD_LEFT_BAR ),
		m_Window, WP_LeftBar );

	if( m_LeftBar == 0 )
		return false;

	m_LeftSubPlace = GetDlgItem( m_LeftBar, IDC_SUBPLACE );

	if( m_LeftSubPlace == 0 )
		return false;

	return true;
}

void XRayEditorUI::DestroyLeftBar(){
	DestroyWindow( m_LeftBar );
	m_LeftBar = 0;
}

//----------------------------------------------------
