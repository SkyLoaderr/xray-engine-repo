//----------------------------------------------------
// file: SceneClassEditor.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "SceneClassEditor.h"

//----------------------------------------------------

BOOL CALLBACK ClassEditorDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){

	SceneClassEditor *editor;

	switch( msg ){

	case WM_INITDIALOG:
		SetWindowLong( hw, GWL_USERDATA, lp );
		editor=(SceneClassEditor*)GetWindowLong( hw, GWL_USERDATA );
		editor->m_DialogWindow = hw;
		editor->DlgInit( hw );
		break;

	case WM_COMMAND:
		editor=(SceneClassEditor*)GetWindowLong( hw, GWL_USERDATA );
		editor->Command( wp, lp );
		break;

	case WM_NOTIFY:
		editor=(SceneClassEditor*)GetWindowLong( hw, GWL_USERDATA );
		editor->Notify( wp, lp );
		break;

	case WM_DRAWITEM:
		editor=(SceneClassEditor*)GetWindowLong( hw, GWL_USERDATA );
		editor->DrawItem( wp, lp );
		break;

	case WM_CLOSE:
		EndDialog( hw, IDCANCEL );
		break;

	default:
		return FALSE; }

	return TRUE;
}


DWORD CALLBACK ClassEditorThread( LPVOID param ){
	SceneClassEditor *editor = (SceneClassEditor *)param;
	
	DialogBoxParam( editor->m_Instance,
		MAKEINTRESOURCE(editor->m_ResID),
		editor->m_ParentWindow,
		ClassEditorDialogProc,
		(LPARAM)param );

	editor->m_Objects.clear();
	editor->m_DialogWindow = 0;

	return 0;
}

//----------------------------------------------------

void SceneClassEditor::StopThread(){

	DWORD exitcode = 0;
	if( GetExitCodeThread(m_Thread,&exitcode) ){
		if( exitcode == STILL_ACTIVE ){
			_ASSERTE( m_DialogWindow );
			PostMessage( m_DialogWindow, WM_CLOSE, 0, 0 );
			while( exitcode == STILL_ACTIVE )
				if( !GetExitCodeThread(m_Thread,&exitcode) ){
					NConsole.print( "Error: can't complete thread" );
					break;
				}
		}
	} else {
		NConsole.print( "Error: can't complete thread" );
	}

	CloseHandle( m_Thread );
	m_Thread = 0;
}

void SceneClassEditor::MakeThread(){

	m_Thread = CreateThread( 0,0,
		ClassEditorThread, this, 0, &m_ThreadID );

	_ASSERTE( m_Thread );
	_ASSERTE( m_ThreadID );
}

bool SceneClassEditor::Create( HINSTANCE instance, HWND parentwindow, ObjectList& objectset ){

	if( m_Thread )
		StopThread();

	m_Instance = instance;
	m_ParentWindow = parentwindow;

	m_Objects.clear();
	m_Objects.merge( objectset );

	MakeThread();

	return true;
}

bool SceneClassEditor::CancelEditor( ){
	if( m_Thread ){
		StopThread();
		return true; }
	return true;
}

//----------------------------------------------------


//----------------------------------------------------


