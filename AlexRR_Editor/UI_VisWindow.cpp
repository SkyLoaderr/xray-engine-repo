//----------------------------------------------------
// file: UI_VisWindow.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"


//----------------------------------------------------

#define _EXTRACT_UI()\
	_UI=(XRayEditorUI*)GetWindowLong(hw,GWL_USERDATA);

static LRESULT CALLBACK WP_VisWindow( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){

	XRayEditorUI *_UI;
	CREATESTRUCT *_Cs;

	switch( msg ){

	case WM_CREATE:
		_Cs = (LPCREATESTRUCT)lp;
		SetWindowLong(hw,GWL_USERDATA,(LONG)_Cs->lpCreateParams);
		break;

	case WM_PAINT:
		ValidateRect(hw,0);
		break;
	case WM_ERASEBKGND:
		break;

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		_EXTRACT_UI();
		_UI->MouseStart( msg==WM_RBUTTONDOWN );
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		_EXTRACT_UI();
		_UI->MouseEnd();
		break;
	case WM_MOUSEMOVE:
		_EXTRACT_UI();
		_UI->MouseProcess();
		break;

	case WM_CLOSE:
		break;

	default:
		return DefWindowProc( hw, msg, wp, lp ); }

	return 0l;
}

//----------------------------------------------------

bool XRayEditorUI::CreateVisWindow(){

	WNDCLASS wc;
	memset( &wc, 0, sizeof(wc) );
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH );
	wc.hCursor = LoadCursor(0,MAKEINTRESOURCE(IDC_ARROW));
	wc.hInstance = m_Instance;
	wc.lpfnWndProc = WP_VisWindow;
	wc.lpszClassName = "visclassname";
	wc.style = CS_HREDRAW | CS_VREDRAW;
	if( 0==RegisterClass( &wc ) )
		return false;

	m_VisWindow = CreateWindow( 
		"visclassname","",
		WS_CHILD | WS_BORDER,
		CW_USEDEFAULT,CW_USEDEFAULT,
		CW_USEDEFAULT,CW_USEDEFAULT,
		m_Window, 0, m_Instance, this );

	if( m_VisWindow==0 )
		return false;

	ShowWindow( m_VisWindow, SW_SHOW );
	UpdateWindow( m_VisWindow );

	return true;
}

void XRayEditorUI::DestroyVisWindow(){
	DestroyWindow( m_VisWindow );
	m_VisWindow = 0;
}

//----------------------------------------------------

