//----------------------------------------------------
// file: TB_Main.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "TB_Main.h"
#include "UI_Main.h"
#include "CustomControls.h"

//----------------------------------------------------

static BOOL CALLBACK ToolBoxDialogProc( HWND hw, UINT msg, WPARAM wp, LPARAM lp ){
	
	XRayEditorUIToolBox *bar;
	HDC hdc;
	RECT rect;
	HBRUSH brush;

	switch( msg ) {

	case WM_INITDIALOG:
		SetWindowLong( hw, GWL_USERDATA, lp );
		bar=(XRayEditorUIToolBox*)lp;
		bar->Init( hw );
		break;

	case WM_COMMAND:
		bar=(XRayEditorUIToolBox*)GetWindowLong( hw, GWL_USERDATA );
		bar->Command( wp, lp );
		break;

	case WM_DRAWITEM:
		bar=(XRayEditorUIToolBox*)GetWindowLong( hw, GWL_USERDATA );
		bar->DrawItem( wp, lp );
		break;
		
	case WM_ERASEBKGND:
		GetClientRect(hw,&rect);
		hdc = GetDC( hw );
		brush = CreateSolidBrush(g_BackColor.colorref());
		FillRect(hdc,&rect,brush);
		DeleteObject(brush);
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

//----------------------------------------------------

void XRayEditorUIToolBox::Create( HINSTANCE _Instance, HWND _Window, int _ChildRes ){
	m_Instance = _Instance;
	m_Window = _Window;
	m_ChildDialog = CreateDialogParam( m_Instance, 
		MAKEINTRESOURCE(_ChildRes),
		m_Window, ToolBoxDialogProc, (LPARAM)this );
	
	RECT rect;
	GetClientRect( m_Window, &rect );
	SetWindowPos(m_ChildDialog,0,
		0,0,rect.right, rect.bottom,
		SWP_NOZORDER );

	ShowWindow( m_ChildDialog, SW_SHOW );
	UpdateWindow( m_ChildDialog );
}

void XRayEditorUIToolBox::Restore(){
	DestroyWindow( m_ChildDialog );
	m_ChildDialog = 0;
}

XRayEditorUIToolBox::XRayEditorUIToolBox(){
	m_Window = 0;
	m_ChildDialog = 0;
}

XRayEditorUIToolBox::~XRayEditorUIToolBox(){
	_ASSERTE( m_ChildDialog==0 );
}


//----------------------------------------------------

void XRayEditorUIToolBox::EraseBack( HDC hdc, RECT *rect ){

	BYTE _R = 127;
	BYTE _G = 127;
	BYTE _B = 0;

	COLORREF maincolor = RGB(_R,_G,_B);
	COLORREF downcolor0 = RGB(_R*2,_G*2,_B*2);
	COLORREF upcolor0 = RGB(_R/2,_G/2,_B/2);

	HBRUSH hbr = CreateSolidBrush(maincolor);
	FillRect( hdc, rect, hbr );
	DeleteObject( hbr );

	HPEN hp0_up = CreatePen( PS_SOLID, 1, upcolor0 );
	HPEN hp0_down = CreatePen( PS_SOLID, 1, downcolor0 );

	MoveToEx( hdc,rect->left,rect->bottom-1,0);

	hp0_up = (HPEN)SelectObject( hdc, hp0_up );
	LineTo( hdc,rect->left,rect->top);
	LineTo( hdc,rect->right-1,rect->top);
	hp0_up = (HPEN)SelectObject( hdc, hp0_up );

	hp0_down = (HPEN)SelectObject( hdc, hp0_down );
	LineTo( hdc,rect->right-1,rect->bottom-1);
	LineTo( hdc,rect->left,rect->bottom-1);
	hp0_down = (HPEN)SelectObject( hdc, hp0_down );

	DeleteObject( hp0_up );
	DeleteObject( hp0_down );
}

//----------------------------------------------------
