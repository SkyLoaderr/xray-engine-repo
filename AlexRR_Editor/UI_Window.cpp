//----------------------------------------------------
// file: UI_Window.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"


//----------------------------------------------------


static LRESULT CALLBACK WP_Main( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){

	CREATESTRUCT *_Cs;
	XRayEditorUI *_UI;

	switch( msg ){

	case WM_CREATE:
		_Cs = (LPCREATESTRUCT)lp;
		SetWindowLong(hw,GWL_USERDATA,(LONG)_Cs->lpCreateParams);
		break;

	case WM_SIZE:
	case WM_MOVE:
		_UI=(XRayEditorUI*)GetWindowLong(hw,GWL_USERDATA);
		_UI->AlignWindows();
		break;

	case WM_COMMAND:
		_UI=(XRayEditorUI*)GetWindowLong(hw,GWL_USERDATA);
		_UI->AccelCommand( LOWORD(wp) );
		break;

	case WM_PAINT:
		ValidateRect(hw,0);
		break;
	case WM_ERASEBKGND:
		break;

	case WM_CLOSE:
		//PostQuitMessage(0);
		break;

	default:
		return DefWindowProc( hw, msg, wp, lp ); }

	return 0l;
}

//----------------------------------------------------


bool XRayEditorUI::CreateMainWindow(){

	WNDCLASS wc;
	memset( &wc, 0, sizeof(wc) );
	wc.hCursor = LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW));
	wc.hIcon = LoadIcon(0,MAKEINTRESOURCE(IDI_QUESTION));
	wc.hInstance = m_Instance;
	wc.lpfnWndProc = WP_Main;
	wc.lpszClassName = "mainclassname";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	if( 0==RegisterClass( &wc ) )
		return false;

	m_Window = CreateWindow( 
		"mainclassname",
		"XRay Map Editor (Gold Edition)",
		//WS_OVERLAPPED,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,CW_USEDEFAULT,
		CW_USEDEFAULT,CW_USEDEFAULT,
		0, 0, m_Instance, this );

	if( m_Window==0 )
		return false;

	SetWindowLong( m_Window, GWL_STYLE, WS_DLGFRAME|WS_CLIPCHILDREN );
	ShowWindow( m_Window, SW_MAXIMIZE );
	//ShowWindow( m_Window, SW_SHOW );
	UpdateWindow( m_Window );

	return true;
}

void XRayEditorUI::DestroyMainWindow(){
	DestroyWindow( m_Window );
	m_Window = 0;
}

//----------------------------------------------------
